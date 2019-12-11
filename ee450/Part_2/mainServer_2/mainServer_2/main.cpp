//
//  main.cpp
//  mainServer_2
//
//  Created by LJZ's macbook pro on 10/25/19.
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
#define MAINPORT "22947"
#define DBPORT "25947"
#define CALCPORT "24947"

typedef struct
{
    int Socket = -1;
    struct addrinfo * pointer = NULL;
} socket_addrinfo;


socket_addrinfo createSocket(const char * ip, const char * portnum)
{
    socket_addrinfo result;
    
    struct addrinfo hints, *servinfo, *p;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;

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
        if(setsockopt(result.Socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1)       // allowing the program to reuse the port
        {
            perror("server: setsockopt error");
            exit(1);
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

void dbServer(socket_addrinfo server, int * message, float * ans)
{
    return;
}

int main()
{
    while (1)
    {
        kill_zombie_process();
        socket_addrinfo db_server = createSocket(HOSTIP, DBPORT);
        socket_addrinfo calc_server = createSocket(HOSTIP, CALCPORT);
        
        
        printf("Please input link ID and file size.\n");
        int ID, size;
        scanf("%d %d", &ID, &size);
        printf("Link %d, file size %d MB.\n", ID, size);
        int message[4] = {0};
        float ans[3]={0};
        
        
        
        struct sockaddr_storage their_addr;
        socklen_t addr_len;
        addr_len = sizeof(their_addr);
        message[0] = ID;

        int numbytes = 0;
        if((numbytes = sendto(db_server.Socket, message, sizeof(message), 0, db_server.pointer->ai_addr, db_server.pointer->ai_addrlen)) == -1)
        {
            perror("sendto");
            exit(1);
        }
        printf("message length: %d\n", numbytes);
        printf("Send Link %d to database server.\n", message[0]);
        
        addr_len = sizeof(their_addr);
        if((numbytes = recvfrom(db_server.Socket, ans, sizeof(ans), 0, (struct sockaddr*)&their_addr, &addr_len)) == -1)
        {
            perror("recvfrom");
            exit(1);
        }
        printf("message length: %d\n", numbytes);
        if(ans[0] < 0)
        {
            printf("Receive no match found.\n");
            continue;
        }
        for (int i=0; i<numbytes/4; i++)
        {
            printf("ans: %f\n", ans[i]);
        }
        printf("Receive link capacity %.0f Mbps, link length %.0f km, and propagation velocity %.0f km/s.\n",
                ans[0], ans[1], ans[2]);
        
        
        float info[4];
        info[0] = float(size);
        for (int i=1; i<4; i++)
        {
            info[i] = ans[i-1];
        }

        if((numbytes = sendto(calc_server.Socket, info, sizeof(info), 0, calc_server.pointer->ai_addr, calc_server.pointer->ai_addrlen)) == -1)
        {
            perror("sendto");
            exit(1);
        }
        printf("message length: %d\n", numbytes);
        printf("Send information to calculation server.\n");
        
        struct sockaddr_storage other_addr;
        socklen_t addrlen = sizeof(other_addr);
        if((numbytes = recvfrom(calc_server.Socket, ans, sizeof(ans), 0, (struct sockaddr*)&other_addr, &addrlen)) == -1)
        {
            perror("recvfrom");
            exit(1);
        }
        printf("message length: %d\n", numbytes);
        printf("Receive transmission delay %.2f ms, propagation delay %.2f ms, and total delay %.2f ms.\n",
                ans[0], ans[1], ans[2]);
    }
    
    return 0;
}
