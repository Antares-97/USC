//
//  main.cpp
//  calcServer_2
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
#define CALCPORT "24947"


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
    Calculation Server:
    Returns the computed results of transportation delay and propagation delay.
    Creates and binds a UDP socket to its port number "24947"
    Communicates with sendto() and recvfrom()
 */
int main()
{
    int sockfd;
    struct addrinfo hints, *servinfo, *p;
    int rv;
    int numbytes;
    struct sockaddr_storage their_addr;
    socklen_t addr_len;
    
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE;
    
    /**
        Creates and binds a UDP to its port
        port num = 24947
     */
    if((rv = getaddrinfo(HOSTIP, CALCPORT, &hints, &servinfo)) != 0)        // port num = 24947
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }
    
    for(p = servinfo; p != NULL; p = p->ai_next)
    {
        if((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
        {
            perror("socket create fail");
            continue;
        }
        if(bind(sockfd, p->ai_addr, p->ai_addrlen) == -1)       // creats and bind a socket
        {
            close(sockfd);
            perror("fail to bind");
            continue;
        }
        break;
    }
    if(p == NULL)
    {
        fprintf(stderr, "fail to create");
        return 2;
    }
    freeaddrinfo(servinfo);
    printf("The Calculation Server is up and running.\n");
    kill_zombie_process();
    
    
    /**
        Main loop
        Waiting for new query from the main server
        With the given information of the link and the file size
        Compute the transportation delay & propagation delay
        Return the results as floats
     */
    while (true)        // main loop
    {
        addr_len = sizeof(their_addr);
        int num[4];
        if((numbytes = recvfrom(sockfd, num, sizeof(num), 0, (struct sockaddr*)&their_addr, &addr_len)) == -1)      // Awaits Message from all message senders. information stores in array "num"
        {
            perror("recvfrom");
            exit(1);
        }
        printf("Receive request from Main Server.\n");

        float ans[3];
        ans[0] = 1000.0 * (8.0 * num[0]) / num[1];      // transportation delay : Tt = size/C
        ans[1] = 1000.0 * num[2] / float(num[3]);       // propagation delay : Tp L/Vp
        ans[2] = ans[0] + ans[1];                       // total delay : Tt + Tp

        if((numbytes = sendto(sockfd, ans, sizeof(ans), 0, (struct sockaddr*)&their_addr, sizeof(their_addr))) == -1)       // The sender's information is stored in "their_addr", send back reply
        {
            perror("sendto:");
            exit(1);
        }

        printf("Send transmission delay %.2f ms, propagation delay %.2f ms, and total delay %.2f ms.\n",
        ans[0], ans[1], ans[2]);
    }
    close(sockfd);          // close the connection
    return 0;
}

