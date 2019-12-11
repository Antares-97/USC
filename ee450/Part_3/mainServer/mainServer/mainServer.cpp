//
//  main.cpp
//  mainServer
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
#define CLIENTPORT "21947"
#define MAINPORT "22947"
#define DBPORT "23947"
#define CALCPORT "24947"
#define MONITORPORT "25947"
#define BACKLOG 10                          // length of the waiting queue in the listen() process



/*
    Main Server: Use UDP socket to communicate
    Take two inputs from the keyboard: ID & size
    Send ID to database Server, query for corresponding link's information
    Send size and link's information to calculation server, gets computed results
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


int listener(const char * IP, const char * PORTNUM)
{
    int status;
    struct addrinfo hints, *info;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;            // regardless of IPV4 or IPV6
    hints.ai_socktype = SOCK_STREAM;        // use TCP connection
    hints.ai_flags = AI_PASSIVE;            // getaddrinfo() will assign the address of the localhost to socket structures
    
    if((status = getaddrinfo(IP, PORTNUM, &hints, &info)) != 0)
    {
        fprintf(stderr, "getaddrinfo error %s\n", gai_strerror(status));
        exit(1);
    }
    
    int listener = buildBindSocket(info);           // create and bind socket according to hostinfo
    freeaddrinfo(info);                             // release hostinfo
    if(listen(listener, BACKLOG) == -1)             // use the bind socket to listen for connections
    {
        perror("server: listen error");
        exit(1);
    }
    kill_zombie_process();          // kill other zombie processes that is currently using this port
    return listener;
}


int main()
{
    int status, sock, rv;
    struct addrinfo hints, *info, *pointer;
    struct sockaddr_storage their_addr;
    socklen_t addr_len;
    
    int client = listener(HOSTIP, CLIENTPORT);
    int monitor = listener(HOSTIP, MONITORPORT);
    printf("The Main Server is up and running.\n");
    
    while (1)                               // main loop
    {
        /**
           take input from the keyboard: ID & size
         */
        struct sockaddr_storage client_addr;
        socklen_t client_size = sizeof(client_addr);
        
        int clientSocket = accept(client, (struct sockaddr *)&client_addr, &client_size);           // listener accepts the connection, and creates a socket to handle the connection
        if(clientSocket == -1)
        {
            perror("server: accept error");
            continue;
        }
        
        int monitorSocket = accept(monitor, (struct sockaddr *)&client_addr, &client_size);
        if(monitorSocket == -1)
        {
            perror("server: accept error");
            continue;
        }
        
        int quest[3];
        int numbytes = 0;
        if((numbytes = recv(clientSocket, quest, INET6_ADDRSTRLEN, 0)) == -1)         // server receives the message send from the client via sockfd, and saves in message, numbytes is the length of the message. recv() returns -1 when failed
        {
            perror("server: receive error");
        }
        char buf[30];
        if((numbytes = recv(monitorSocket, buf, INET6_ADDRSTRLEN, 0)) == -1)         // server receives the message send from the client via sockfd, and saves in message, numbytes is the length of the message. recv() returns -1 when failed
        {
            perror("server: receive error");
        }
        printf("Receive Link %d, file size %d MB.\n", quest[1], quest[2]);
        
        if(send(monitorSocket, quest, sizeof(quest), 0) == -1)           // server send message containing its name back to the client, send() returns -1 when failed.
        {
            perror("server: send error");
        }
        
        float ID = quest[1];
        float filesize = quest[2];
        
        float result[4] = {2,-1,-1,-1};
        
        
        
        memset(&hints, 0, sizeof(hints));
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_DGRAM;         // Here we set the socket type to be UDP
        
        if((rv = getaddrinfo(HOSTIP, DBPORT, &hints, &info)) != 0)      // connect to database server, portnum = "23947"
        {
            fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
            return 1;
        }
        for(pointer = info; pointer != NULL; pointer = pointer->ai_next)
        {
            if((sock = socket(pointer->ai_family, pointer->ai_socktype, pointer->ai_protocol)) == -1)
            {
                perror("socket create fail");
                continue;
            }
            break;
        }
        if(pointer == NULL)
        {
            fprintf(stderr, "fail to create");
            return 2;
        }
        freeaddrinfo(info);
        
        
        /**
         communicate with Database Server,
         UDP uses sendto() and recvfrom() to send / receive messages
         */
        addr_len = sizeof(their_addr);
        float message[5] = {1};
        message[1] = ID;
        if((numbytes = sendto(sock, message, sizeof(message), 0, pointer->ai_addr, pointer->ai_addrlen)) == -1)        // send nums to database server via UDP socket
        {
            perror("sendto");
            exit(1);
        }
        printf("Send Link %.0f to database server.\n", message[0]);
        
        if(send(monitorSocket, message, sizeof(message), 0) == -1)           // server send message containing its name back to the client, send() returns -1 when failed.
        {
            perror("server: send error");
        }
        
        float ans[10];
        addr_len = sizeof(their_addr);
        if((numbytes = recvfrom(sock, ans, sizeof(ans), 0, (struct sockaddr*)&their_addr, &addr_len)) == -1)       // receive message from database server, store in array ans[].
        {
            perror("recvfrom");
            exit(1);
        }
        
        if(send(monitorSocket, ans, sizeof(ans), 0) == -1)           // server send message containing its name back to the client, send() returns -1 when failed.
        {
            perror("server: send error");
        }
        
        if(ans[1] == -1)
        {
            printf("Receive no match found.\n");
            if(send(clientSocket, result, sizeof(result), 0) == -1)           // server send message containing its name back to the client, send() returns -1 when failed.
            {
                perror("server: send error");
            }
            continue;
        }
        
        printf("Receive link capacity %.0f Mbps, link length %.0f km, and propagation velocity %.0f km/s.\n",
               ans[1], ans[2], ans[3]);
        close(sock);           // close the connection with database server
        
        
        
        /**
         Build connection with calculation server,
         pass the parameters and get the computed results back
         */
        if((rv = getaddrinfo(HOSTIP, CALCPORT, &hints, &info)) != 0)        // connect to calculation server, portnum = "24947"
        {
            fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
            return 1;
        }
        for(pointer = info; pointer != NULL; pointer = pointer->ai_next)
        {
            if((sock = socket(pointer->ai_family, pointer->ai_socktype, pointer->ai_protocol)) == -1)
            {
                perror("socket create fail");
                continue;
            }
            break;
        }
        if(pointer == NULL)
        {
            fprintf(stderr, "fail to create");
            return 2;
        }
        freeaddrinfo(info);
        
        
        /**
         Communicate with calculation server
         Use sendto() and recvfrom() to exchange messages
         */
        addr_len = sizeof(their_addr);
        message[0] = 3;
        message[1] = filesize;
        for (int i=2; i<5; i++)
        {
            message[i] = ans[i-1];
        }
        if((numbytes = sendto(sock, message, sizeof(message), 0, pointer->ai_addr, pointer->ai_addrlen)) == -1)        // the information of calculation server is stored in "pointer".
        {
            perror("sendto");
            exit(1);
        }
        printf("Send information to Calculation server.\n");
        
        if(send(monitorSocket, message, sizeof(message), 0) == -1)           // server send message containing its name back to the client, send() returns -1 when failed.
        {
            perror("server: send error");
        }
        
        addr_len = sizeof(their_addr);
        if((numbytes = recvfrom(sock, result, sizeof(result), 0, (struct sockaddr*)&their_addr, &addr_len)) == -1)         // receive message from all possible senders.
        {
            perror("recvfrom");
            exit(1);
        }
        printf("Receive transmission delay %.2f ms, propagation delay %.2f ms, and total delay %.2f ms.\n",
               result[1], result[2], result[3]);
        
        if(send(monitorSocket, result, sizeof(result), 0) == -1)           // server send message containing its name back to the client, send() returns -1 when failed.
        {
            perror("server: send error");
        }
        close(sock);           // Close the connection
        
        if(send(clientSocket, result, sizeof(result), 0) == -1)           // server send message containing its name back to the client, send() returns -1 when failed.
        {
            perror("server: send error");
        }
        result[0] = 5;
        if(send(monitorSocket, result, sizeof(result), 0) == -1)           // server send message containing its name back to the client, send() returns -1 when failed.
        {
            perror("server: send error");
        }

        close(clientSocket);          // main process: listener does not need this.
    }
    return 0;
}
