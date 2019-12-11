//  EE450-TP1
//  client.cpp
//  USC JUNZHE LIU 2270250947
//
//  Created by LJZ's macbook pro on 10/1/19.
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
using namespace std;

#define HOSTIP "127.0.0.1"
#define HOSTPORT "21947"
#define BACKLOG 10


/*
    CLIENT PART
    creates a client and make connetion with the server
    creates a socket to connect and communicate with server
    send its name <NAME1> to server and receive server's name <NAME2> in exchange.
*/


/*
    sockConnect function, returns a socket descriptor with a given hostinfo
    try to create a socket which connects to the host.
    copied from Beej's-Guide
 */
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
void TCPcommunicate(int client, char * clientname)
{
    char buf[100];
    int numbytes = 0;

    if(send(client, clientname, strlen(clientname), 0) == -1)           // send <NAME1> to server
    {
        perror("client: send error");
        exit(1);
    }
    printf("The client send greetings to the server\n");
    
    if((numbytes = recv(client, buf, 100, 0)) == -1)            // receives <NAME2> from server, stores in buf, numbytes returns the length of the message.
    {
        perror("client: receive error");
        exit(1);
    }
    buf[numbytes] = '\0';
    printf("Get reply from: %s\n", buf);
    
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
    if(argc != 2)       // If the name of the client is not assigned.
    {
        exit(0);
    }
    printf("The client is up and running\n");
    
    int status;
    struct addrinfo hints;
    struct addrinfo * hostinfo;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;            // regardless of IPV4 or IPV6
    hints.ai_socktype = SOCK_STREAM;        // use TCP connection
    
    if((status = getaddrinfo(HOSTIP, HOSTPORT, &hints, &hostinfo)) != 0)        // getaddrinfo, connect to localhost(127.0.0.1), port number 21947
    {
        fprintf(stderr, "getaddrinfo error %s\n", gai_strerror(status));
        exit(2);
    }

    int client = sockConnect(hostinfo);         // creates a socket and make connection to the server
    freeaddrinfo(hostinfo);                     // release the hostinfo

    TCPcommunicate(client, argv[1]);            // communicate with server with the socket
    close(client);          // tear down the connection after communication completed
    
    return 0;
}
