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
void connectMainServer(int calc_socket)
{
    ssize_t numbytes = 0;
    struct sockaddr_storage * other_addr;
    memset(&other_addr, 0, sizeof(other_addr));
    socklen_t addr_len = sizeof(other_addr);
    int message_in[10];
    if ((numbytes = recvfrom(calc_socket, message_in, sizeof(message_in), 0, (struct sockaddr*)other_addr, &addr_len)) == -1)
    {
        perror("fail recvfrom");
        exit(1);
    }
    printf("Receive request from Main Server.\n");
    
    float ans[3];
    ans[0] = 1000.0 * (8.0 * message_in[1]) / message_in[2];
    ans[1] = 1000.0 * float(message_in[3]) / message_in[4];
    ans[2] = ans[0] + ans[1];


    if((numbytes = sendto(calc_socket, &ans, sizeof(ans), 0, (struct sockaddr*)other_addr, sizeof(other_addr))) == -1)
    {
        perror("fail sendto");
        exit(1);
    }
    printf("Send transmission delay %.2f ms, propagation delay %.2f ms, and total delay %.2f ms.\n",
            ans[0], ans[1], ans[2]);
}


int main(int argc, char * argv[])
{
    socket_addrinfo calcServer = buildBindSocket(HOSTIP, CALCPORT, true);           // create and bind socket according to hostinfo
    
//    kill_zombie_process();          // kill other zombie processes that is currently using this port
    printf("The Calculation Server is up and running.\n");
    
    while (true)            // main loop, always waiting for new connections
    {
        connectMainServer(calcServer.Socket);
    }

    return 0;
}
