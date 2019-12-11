//
//  main.cpp
//  dbServer_2
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
    
    if((rv = getaddrinfo(HOSTIP, DBPORT, &hints, &servinfo)) != 0)
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
        if(bind(sockfd, p->ai_addr, p->ai_addrlen) == -1)
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
    
    std::vector< float * > database;
    freopen("./database.txt", "r", stdin);
    float ID, C, L, Vp;
    while (scanf("%f %f %f %f", &ID, &C, &L, &Vp)!= EOF)
    {
        float * link = new float [4];
        link[0] = ID;
        link[1] = C;
        link[2] = L;
        link[3] = Vp;
        database.push_back(link);
        printf("%.2f %.2f %.2f %.2f\n", ID, C, L, Vp);
    }

    printf("The Database Server is up and running.\n");
    kill_zombie_process();
    while (true)
    {
        addr_len = sizeof(their_addr);
        int num[4];
        if((numbytes = recvfrom(sockfd, num, sizeof(num), 0, (struct sockaddr*)&their_addr, &addr_len)) == -1)
        {
            perror("recvfrom");
            exit(1);
        }
        printf("message length: %d\n", numbytes);
        float ans [3] = {-1, -1, -1};
        for(int i=0; i<database.size(); i++)
        {
            if (database[i][0] == num[0])
            {
                for (int j=0; j<3; j++)
                {
                    ans[j] = database[i][j+1];
                }
                break;
            }
        }

        if((numbytes = sendto(sockfd, ans, sizeof(ans), 0, (struct sockaddr*)&their_addr, sizeof(their_addr))) == -1)
        {
            perror("sendto:");
            exit(1);
        }
        printf("message length: %d\n", numbytes);
        if(ans[0] == -1)
        {
            printf("No match found.\n");
            continue;
        }
        printf("Receive link %d, capacity %d Mbps, link length %d km, and propagation velocity %d km/s.\n",
                num[0], int(ans[0]), int(ans[1]), int(ans[2]));
    }
    close(sockfd);
    return 0;
}

