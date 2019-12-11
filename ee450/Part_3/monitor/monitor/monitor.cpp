//
//  monitor.cpp
//  monitor
//
//  Created by LJZ's macbook pro on 12/8/19.
//  Copyright © 2019 LJZ's macbook pro. All rights reserved.
//

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>                         // used for sigemptyset
#include <iomanip>
#include <vector>


#define HOSTIP "127.0.0.1"                  // localhost IP
#define MONITORPORT "25947"
#define BACKLOG 10                          // length of the waiting queue in the listen() process


int sockConnect(struct addrinfo * hostinfo)
{
    struct addrinfo * p;
    int sockfd = -1;

    for (p = hostinfo; p != NULL; p = p->ai_next)         // Go down the linked list, and search for the first satisfying one
    {
        if((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)       // creates a socket
        {
            perror("client: socket error");
            continue;
        }
        
        if(connect(sockfd, p->ai_addr, p->ai_addrlen) == -1)            // connect the socket to the host
        {
            close(sockfd);
            perror("client: connect error");
            continue;
        }
        break;          // returns the first socket that successfully created and connected
    }
    
    if(p == NULL)       // p reaches the end of the linked list, we failed to make connection.
    {
        fprintf(stderr, "client: failed to connect\n");
        exit(1);
    }
    
    return sockfd;
}


/*
    client communicate with the server, send <NAME1> to server and receive <NAME2> back
    "int client" is the socket descriptor, clientname is <NAME1>.
 */
void TCPcommunicate(int monitor)
{
    char buf[30] = "This is monitor.";
    int numbytes = 0;
    
    if(send(monitor, buf, strlen(buf), 0) == -1)           // send <NAME1> to server
    {
        perror("monitor: send error");
        exit(1);
    }
    float message [10];
    
    if((numbytes = recv(monitor, message, sizeof(message), 0)) == -1)            // receives <NAME2> from server, stores in buf, numbytes returns the length of the message.
    {
        perror("monitor: receive error");
        exit(1);
    }
    switch (int(message[0]))
    {
        case 0:
        {
            printf("Main server receives Link %.0f and file size %.0f MB from client.\n", message[1], message[2]);
            break;
        }
        
        case 1:
        {
            printf("Main server sends Link %.0f to database server.\n", message[1]);
            break;
        }
            
        case 2:
        {
            printf("Main server receives information from database server: ");
            if(message[1] == -1)
            {
                printf("no match found.\n");
                return;
            }
            printf("link capacity %.0f Mbps, link length %.0f km, and propagation velocity %.0f km/s\n", message[1], message[2], message[3]);
            break;
        }
            
        case 3:
        {
            printf("Main server sends information to calculation server");
            break;
        }
        
        case 4:
        {
            printf("Main server receives information from calculation server");
            break;
        }
            
        case 5:
        {
            printf("Main Server sends information to client: transmission delay %.2f ms, propagation delay %.2f ms, and total delay %.2f ms.\n",
            message[1], message[2], message[3]);
            break;
        }
            
        default:
            break;
    }
    
    return;
}


int main(int argc, char * argv[])
{
    /*
       main function, uses getaddrinfo to automatically query DNS and returns the information of the host
       with IP address (127.0.0.1) and port number (21947) given
     
       argc: (ARGument Count) stores number of command-line arguments passed by the user including the name of the program.
             So if we pass a value to a program, value of argc would be 2 (one for argument and one for program name)
       argv: (ARGument Vector) is array of character pointers listing all the arguments.
             If argc is greater than zero,the array elements from argv[0] to argv[argc-1] will contain pointers to strings.
             Argv[0] is the name of the program , After that till argv[argc-1] every element is command -line arguments.
            
       Usage: ./client <NAME1> assigns <NAME1> as the name of the client. argc = 2, argv[0] = "./client", argv[1] = "<NAME1>"
     */
    printf("The Monitor is up and running\n");
    
    int status;
    struct addrinfo hints;
    struct addrinfo * hostinfo;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;            // regardless of IPV4 or IPV6
    hints.ai_socktype = SOCK_STREAM;        // use TCP connection
    
    if((status = getaddrinfo(HOSTIP, MONITORPORT, &hints, &hostinfo)) != 0)        // getaddrinfo, connect to localhost(127.0.0.1), port number 21947
    {
        fprintf(stderr, "getaddrinfo error %s\n", gai_strerror(status));
        exit(2);
    }

    int monitor = sockConnect(hostinfo);         // creates a socket and make connection to the server
    freeaddrinfo(hostinfo);                     // release the hostinfo
    while (true)
    {
        TCPcommunicate(monitor);            // communicate with server with the socket
    }
    close(monitor);          // tear down the connection after communication completed
    
    return 0;
}
