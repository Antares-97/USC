//
//  main.cpp
//  UDP_server
//
//  Created by LJZ's macbook pro on 10/24/19.
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
#define DBPORT "23947"


/*
    Database Server:
    
    Maintains a database with the information of all links
    Information extracted from "database.txt"
    Returns the corresponding information of a requested link
    Return an array of all {-1} elements if the requested link is not found
    Creates and binds a UDP socket to its port number "23947"
    Communicates with sendto() and recvfrom()
 */
int main()
{
    /**
        First part of Database server: create and bind a socket to its port numnber.
     */
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
        if(bind(sockfd, p->ai_addr, p->ai_addrlen) == -1)         // creats and bind a socket
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
    
    
    /**
        Read information from txt file, and stores in the database
     */
    std::vector< int * > database;
    freopen("./database.txt", "r", stdin);
    int ID, C, L, Vp;
    while (scanf("%d %d %d %d", &ID, &C, &L, &Vp)!= EOF)
    {
        int * link = new int [4];
        link[0] = ID;
        link[1] = C;
        link[2] = L;
        link[3] = Vp;
        database.push_back(link);
    }
    printf("The Database Server is up and running.\n");
    
    
    /**
        Main loop
        Waiting for new query from the main server
        Search the requested link in its database and return all its corresponding information
        Return an array with all -1 elements if the link is not found
     */
    while (true)
    {
        addr_len = sizeof(their_addr);
        int num[4];
        if((numbytes = recvfrom(sockfd, num, sizeof(num), 0, (struct sockaddr*)&their_addr, &addr_len)) == -1)          // receive message from all possible senders
        {
            perror("recvfrom");
            exit(1);
        }
        printf("Receive request from Main Server.\n");
        
        int ans [3] = {-1, -1, -1};         // all -1 array
        for(int i=0; i<database.size(); i++)
        {
            if (database[i][0] == num[0])       // Search in the database
            {
                for (int j=0; j<3; j++)
                    ans[j] = database[i][j+1];
                break;
            }
        }

        if((numbytes = sendto(sockfd, ans, sizeof(ans), 0, (struct sockaddr*)&their_addr, sizeof(their_addr))) == -1)           // The sender's information is stored in "their_addr", send back reply
        {
            perror("sendto:");
            exit(1);
        }
        if(ans[0] == -1)
        {
            printf("No match found.\n");
            continue;
        }
        printf("Send link %d, capacity %d Mbps, link length %d km, and propagation velocity %d km/s.\n",
                num[0], ans[0], ans[1], ans[2]);
    }
    
    close(sockfd);      // close the connection
    return 0;
}
