//
//  main.cpp
//  mainserver
//
//  Created by LJZ's macbook pro on 10/21/19.
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
#define MAINPORT "22947"                    // main server port number: 22000 + 947 (2270250947) = 22947
#define DBPORT   "23947"                    // database server port num: 23000 + 947
#define CALCPORT "24947"                    // calculate server port num: 24000 + 947


/*
    SERVER PART
    creates a chatbot server, reads a name from the input as the name of host(<NAME2>)
    connects to the localhost with IP "127.0.0.1" and the port number of "21947"
    creates and binds a socket to listen for connections
    receives messages from other clients and send back messages.
 */


/*
    buildBindSocket function
    returns a socket descriptor with a given addrinfo
    addrinfo is actually a linked list,
    build a socket according to the addrinfo and bind to the first we can
    copied from Beej's-Guide
 */

typedef struct
{
    int Socket = -1;
    struct addrinfo * pointer = NULL;
} socket_addrinfo;



socket_addrinfo buildBindSocket(const char * ip, const char * portnum, bool needbind)
{
    socket_addrinfo result;
    
    struct addrinfo hints, *servinfo, *p;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE;

    if((getaddrinfo(ip, portnum, &hints, &servinfo)) != 0)      // getaddrinfo, connect to localhost(127.0.0.1), port number 22947
    {
        fprintf(stderr, "getaddrinfo error\n");
        exit(1);
    }

    int yes = 1;
    for (p = servinfo; p != NULL; p = p->ai_next)       // loop throught the linked list, looking for the first satisfying element
    {
        if((result.Socket = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)       // creates a socket
        {
            perror("server: socket error");
            continue;
        }
        result.pointer = p;
        if(needbind)
        {
            if(setsockopt(result.Socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1)       // allowing the program to reuse the port
            {
                perror("server: setsockopt error");
                exit(1);
            }
            if(::bind(result.Socket, p->ai_addr, p->ai_addrlen) == -1)         // bind the socket to the port on our local machine
            {
                close(result.Socket);
                perror("server: bind error");
                continue;
            }
        }
        
        break;        // returns the first socket that successfully completes the above three commands.
    }
    if(p == NULL)       // p reaches the end of the linked list, we failed to bind the socket.
    {
        fprintf(stderr, "server: failed to create\n");
        exit(1);
    }
    freeaddrinfo(servinfo);
    return result;
}


/*
    Copied from Beej's Guide: Chapter 6 Client-Server Background
 */
void sigchld_handler(int s)
{
    int saved_errno = errno;
    while (waitpid(-1, NULL, WNOHANG) > 0);
    errno = saved_errno;
}


/*
    Copied from Beej's Guide: Chapter 6 Client-Server Background
    this part is responsible for reaping zombie processes that appear as the fork() child processes exit
    If we don't reap the zombie processes, then an error "Interrupted System Call".
 */
void kill_zombie_process()
{
    struct sigaction sa;
    sa.sa_handler = sigchld_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if(sigaction(SIGCHLD, &sa, NULL) == -1)
    {
        perror("sigaction error");
        exit(1);
    }
}


/*
    server listens and waits for connection, listener is the main listening socket
    once a client asks for connection, server will fork() a child process to handle it
    hostname is actually <NAME2>, the name of the server.
    mainly copied from Beej's Guide
 */
void connectServer(int main_socket, const char * PORTNUM, int * message,  float * buf)
{
    socket_addrinfo server = buildBindSocket(HOSTIP, PORTNUM, false);           // create and bind socket according to hostinfo
    ssize_t numbytes = 0;
    printf("message length: %d\n",sizeof(message));
    if((numbytes = sendto(server.Socket, &message, sizeof(message), 0, server.pointer->ai_addr, server.pointer->ai_addrlen)) == -1)
    {
        perror("fail sendto");
        exit(1);
    }
    printf("length: %d\n", numbytes);
    if(strcmp(PORTNUM, DBPORT) == 0)
    {
        printf("Send Link %d to database server.\n", message[0]);
    }
    else if (strcmp(PORTNUM, CALCPORT) == 0)
    {
        printf("Send information to calculation server.\n");
    }

    struct sockaddr_storage other_addr;
    socklen_t addr_len = sizeof(other_addr);
    if ((numbytes = recvfrom(server.Socket, buf, sizeof(buf), 0, (struct sockaddr*)&other_addr, &addr_len)) == -1)
    {
        perror("fail recvfrom");
        exit(1);
    }
    printf("length: %d\n", numbytes);
}


int main(int argc, char * argv[])
{
//    socket_addrinfo mainServer = buildBindSocket(HOSTIP, MAINPORT, true);           // create and bind socket according to hostinfo
    
//    kill_zombie_process();          // kill other zombie processes that is currently using this port
    printf("The Main Server is up and running.\n");
    
    while (true)            // main loop, always waiting for new connections
    {
        printf("Please input link ID and file size.\n");
        int ID, size;
        scanf("%d %d", &ID, &size);
        printf("Link %d, file size %d MB.\n", ID, size);
        float ans[3];
        int message[4] = {ID, -1, -1, -1};
        connectServer(0, DBPORT, message, ans);          // listen, accept and process a new connection
        
        if(ans[0] == -1)
        {
            printf("Receive no match found.\n");
            continue;
        }
        printf("Receive link capacity %d Mbps, link length %d km, and propagation velocity %d km/s.\n",
                int(ans[0]), int(ans[1]), int(ans[2]));
        message[0] = size;
        for(int i=1; i<4; i++)
            message[i] = ans[i-1];
        
        connectServer(0, CALCPORT, message, ans);
        printf("Receive transmission delay %.2f ms, propagation delay %.2f ms, and total delay %.2f ms.\n",
                ans[0], ans[1], ans[2]);
    }

    return 0;
}

