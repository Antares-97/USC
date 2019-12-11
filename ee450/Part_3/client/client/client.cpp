//
//  client.cpp
//  client
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
using namespace std;

#define HOSTIP "127.0.0.1"
#define CLIENTPORT "21947"
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
void TCPcommunicate(int client, int * quest)
{
    float result[4];
    int numbytes = 0;

    if(send(client, quest, sizeof(quest), 0) == -1)             // send <ID> and <filesize> to main server
    {
        perror("client: send error");
        exit(1);
    }
    printf("Send Link %d and file size %d MB to main server.\n", quest[0], quest[1]);
    
    if((numbytes = recv(client, result, sizeof(result), 0)) == -1)            // receives <NAME2> from server, stores in buf, numbytes returns the length of the message.
    {
        perror("client: receive error");
        exit(1);
    }
    if(result[1] == -1)
        printf("No match found.\n");
    else
        printf("Receive transmission delay %.2f ms, propagation delay %.2f ms, and total delay %.2f ms.\n",
        result[1], result[2], result[3]);
    
    return;
}


int main(int argc, char * argv[])
{
    /*
       main function, uses getaddrinfo to automatically query DNS and returns the information of the host
       with IP address (127.0.0.1) and port number (21947) given
     
       argc: (ARGument Count) stores number of command-line arguments passed by the user including the name of the program.
       argv: (ARGument Vector) is array of character pointers listing all the arguments.
             If argc is greater than zero,the array elements from argv[0] to argv[argc-1] will contain pointers to strings.
             Argv[0] is the name of the program , After that till argv[argc-1] every element is command -line arguments.
            
       Usage: ./client <ID> <filesize> assigns 2 parameters: argc = 3, argv[0] = "./client", argv[1] = "<ID>", argv[2] = "<filesize>"
     */
    if(argc != 3)       // If the name of the client is not assigned.
    {
        printf("Incorrect Input.\n");
        exit(0);
    }
    printf("The Client is up and running.\n");
    
    int quest[3] = {0, atoi(argv[1]), atoi(argv[2])};
    
    int status;
    struct addrinfo hints;
    struct addrinfo * hostinfo;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;            // regardless of IPV4 or IPV6
    hints.ai_socktype = SOCK_STREAM;        // use TCP connection
    
    if((status = getaddrinfo(HOSTIP, CLIENTPORT, &hints, &hostinfo)) != 0)        // getaddrinfo, connect to localhost(127.0.0.1), port number 21947
    {
        fprintf(stderr, "getaddrinfo error %s\n", gai_strerror(status));
        exit(2);
    }

    int client = sockConnect(hostinfo);         // creates a socket and make connection to the server
    freeaddrinfo(hostinfo);                     // release the hostinfo

    TCPcommunicate(client, quest);            // communicate with server with the socket
    close(client);          // tear down the connection after communication completed
    
    return 0;
}

