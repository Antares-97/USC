//  EE450-TP1
//  server.cpp
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
#include <signal.h>                         // used for sigemptyset

#define HOSTIP "127.0.0.1"                  // localhost IP
#define MYPORT "21947"                      // host port number: 21000 + 947 (2270250947) = 21947
#define BACKLOG 10                          // length of the waiting queue in the listen() process


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
int buildBindSocket(struct addrinfo * servinfo)
{
    struct addrinfo * p;
    int sockfd = -1;
    int yes = 1;
    for (p = servinfo; p != NULL; p = p->ai_next)       // loop throught the linked list, looking for the first satisfying element
    {
        if((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)       // creates a socket
        {
            perror("server: socket error");
            continue;
        }
        if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1)       // allowing the program to reuse the port
        {
            perror("server: setsockopt error");
            exit(1);
        }
        if(::bind(sockfd, p->ai_addr, p->ai_addrlen) == -1)         // bind the socket to the port on our local machine
        {
            close(sockfd);
            perror("server: bind error");
            continue;
        }
        break;        // returns the first socket that successfully completes the above three commands.
    }
    
    if(p == NULL)       // p reaches the end of the linked list, we failed to bind the socket.
    {
        fprintf(stderr, "server: failed to bind\n");
        exit(1);
    }
    
    return sockfd;
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
void processConnection(int listener, char * hostname)
{
    struct sockaddr_storage client_addr;
    socklen_t client_size = sizeof(client_addr);
    char message[100];
    int sockfd = accept(listener, (struct sockaddr *)&client_addr, &client_size);           // listener accepts the connection, and creates a socket to handle the connection
    if(sockfd == -1)
    {
        perror("server: accept error");
        return;
    }
    
    if(fork() == 0)         // listener fork() and produce a child process to handle the connection
    {
        close(listener);            // child process does not need the listen
        int numbytes = 0;
        if((numbytes = recv(sockfd, message, INET6_ADDRSTRLEN, 0)) == -1)         // server receives the message send from the client via sockfd, and saves in message, numbytes is the length of the message. recv() returns -1 when failed
        {
            perror("server: receive error");
        }
        message[numbytes] = '\0';
        printf("Receive greetings from: %s\n", message);
        
        if(send(sockfd, hostname, strlen(hostname), 0) == -1)           // server send message containing its name back to the client, send() returns -1 when failed.
        {
            perror("server: send error");
        }
        printf("Send greetings to: %s\n", message);
        
        close(sockfd);          // close the connection when messages are sent
        exit(0);
    }
    close(sockfd);          // main process: listener does not need this.
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
        
        Usage: ./server <NAME2>   assigns <NAME2> as the name of the server. argc = 2, argv[0] = "./server", argv[1] = "<NAME2>"
     */
    if(argc != 2)       // If the name of the server is not assigned, exit.
    {
        exit(0);
    }
    printf("The Server is up and running\n");
    
    int status;
    struct addrinfo hints, * hostinfo;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;            // regardless of IPV4 or IPV6
    hints.ai_socktype = SOCK_STREAM;        // use TCP connection
    hints.ai_flags = AI_PASSIVE;            // getaddrinfo() will assign the address of the localhost to socket structures
    
    if((status = getaddrinfo(HOSTIP, MYPORT, &hints, &hostinfo)) != 0)      // getaddrinfo, connect to localhost(127.0.0.1), port number 21947
    {
        fprintf(stderr, "getaddrinfo error %s\n", gai_strerror(status));
        exit(1);
    }
    
    int listener = buildBindSocket(hostinfo);           // create and bind socket according to hostinfo
    freeaddrinfo(hostinfo);         // release hostinfo
    if(listen(listener, BACKLOG) == -1)         // use the bind socket to listen for connections
    {
        perror("server: listen error");
        exit(1);
    }
    kill_zombie_process();          // kill other zombie processes that is currently using this port
    
    while (true)            // main loop, always waiting for new connections
    {
        processConnection(listener, argv[1]);          // listen, accept and process a new connection
    }

    return 0;
}
