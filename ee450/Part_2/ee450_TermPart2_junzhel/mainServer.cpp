//
//  main.cpp
//  UDP_client
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
#define CALCPORT "24947"


/*
    Main Server: Use UDP socket to communicate
    Take two inputs from the keyboard: ID & size
    Send ID to database Server, query for corresponding link's information
    Send size and link's information to calculation server, gets computed results
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
    
    if((rv = getaddrinfo(HOSTIP, MAINPORT, &hints, &servinfo)) != 0)
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
    printf("The Main Server is up and running.\n");
    
    while (1)                               // main loop
    {
        /**
           take input from the keyboard: ID & size
         */
        printf("Please input link ID and file size.\n");
        int ID, size;
        scanf("%d %d", &ID, &size);
        printf("Link %d, file size %d MB.\n", ID, size);
        
        
        /**
            connect to the database server, same approach to last assignment
         */
        int server_socket;
        struct addrinfo  *pointer;
        
        memset(&hints, 0, sizeof(hints));
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_DGRAM;         // Here we set the socket type to be UDP
        
        if((rv = getaddrinfo(HOSTIP, DBPORT, &hints, &servinfo)) != 0)      // connect to database server, portnum = "23947"
        {
            fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
            return 1;
        }
        for(pointer = servinfo; pointer != NULL; pointer = pointer->ai_next)
        {
            if((server_socket = socket(pointer->ai_family, pointer->ai_socktype, pointer->ai_protocol)) == -1)
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
        freeaddrinfo(servinfo);
        
        
        /**
            communicate with Database Server,
            UDP uses sendto() and recvfrom() to send / receive messages
         */
        addr_len = sizeof(their_addr);
        int num[4] = {0};
        num[0] = ID;
        if((numbytes = sendto(server_socket, num, sizeof(num), 0, pointer->ai_addr, pointer->ai_addrlen)) == -1)        // send nums to database server via UDP socket
        {
            perror("sendto");
            exit(1);
        }
        printf("Send Link %d to database server.\n", num[0]);
        
        int ans[10];
        addr_len = sizeof(their_addr);
        if((numbytes = recvfrom(server_socket, ans, sizeof(ans), 0, (struct sockaddr*)&their_addr, &addr_len)) == -1)       // receive message from database server, store in array ans[].
        {
            perror("recvfrom");
            exit(1);
        }
        if(ans[0] == -1)
        {
            printf("Receive no match found.\n");
            continue;
        }
        printf("Receive link capacity %d Mbps, link length %d km, and propagation velocity %d km/s.\n",
                int(ans[0]), int(ans[1]), int(ans[2]));
        close(server_socket);           // close the connection with database server
        
        
        /**
            Build connection with calculation server,
            pass the parameters and get the computed results back
         */
        if((rv = getaddrinfo(HOSTIP, CALCPORT, &hints, &servinfo)) != 0)        // connect to calculation server, portnum = "24947"
        {
            fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
            return 1;
        }
        for(pointer = servinfo; pointer != NULL; pointer = pointer->ai_next)
        {
            if((server_socket = socket(pointer->ai_family, pointer->ai_socktype, pointer->ai_protocol)) == -1)
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
        freeaddrinfo(servinfo);
        
        /**
            Communicate with calculation server
            Use sendto() and recvfrom() to exchange messages
         */
        addr_len = sizeof(their_addr);
        num[0] = size;
        for (int i=1; i<4; i++)
        {
            num[i] = ans[i-1];
        }
        if((numbytes = sendto(server_socket, num, sizeof(num), 0, pointer->ai_addr, pointer->ai_addrlen)) == -1)        // the information of calculation server is stored in "pointer".
        {
            perror("sendto");
            exit(1);
        }
        printf("Send information to Calculation server.\n");
        
        float result[3];
        addr_len = sizeof(their_addr);
        if((numbytes = recvfrom(server_socket, result, sizeof(result), 0, (struct sockaddr*)&their_addr, &addr_len)) == -1)         // receive message from all possible senders.
        {
            perror("recvfrom");
            exit(1);
        }
        printf("Receive transmission delay %.2f ms, propagation delay %.2f ms, and total delay %.2f ms.\n",
                result[0], result[1], result[2]);
        close(server_socket);           // Close the connection
    }
    return 0;
}
