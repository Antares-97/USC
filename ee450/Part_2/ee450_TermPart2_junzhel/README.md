# EE450 - Term Project - part 2
#### Junzhe Liu / 2270250947
#### Oct 25th

In this assignment, we simulate the UDP connections between three servers using socket programming.

## UDP socket programming introduction

We're required to use UDP socket in this program. Unlike TCP, UDP is unrealiable, connectionless and has no re-transmit mechanism. We set the socket type as UDP in the creation of the socket:
```cpp
struct addrinfo hints;
memset(&hints, 0, sizeof(hints));
hints.ai_family = AF_UNSPEC;
hints.ai_socktype = SOCK_DGRAM;   // set the socket type to be UDP
```

Unlike TCP, which needs **listen()** and **accept()** to build a handshake process before transmitting messages, UDP can send messages directly without connection. UDP sockets send messages via **sendto()** function and receive messages by **recvfrom** function. These two function differs with TCP's **send()** and **recv()** in that they require the receiver(sender)'s address in the function as one of their parameters. For a UDP client, after creating a UDP socket pointing to the server's address, it can send messages and receive reply from the socket:
```cpp
/* client: */
getaddrinfo(HOSTIP, DBPORT, &hints, &servinfo);     // get server's address
server_socket = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol)      // create a socket pointing to the server
sendto(server_socket, num, sizeof(num), 0, servinfo->ai_addr, servinfo->ai_addrlen);     //send message to the server, ervinfo->ai_addr indicates the address of the server

struct sockaddr_storage their_addr;
addr_len = sizeof(their_addr);
recvfrom(server_socket, ans, sizeof(ans), 0, (struct sockaddr*)&their_addr, &addr_len);     // Get reply from the server
```

Note that **their_addr** is actually an empty __sockaddr_storage__, but **server_socket** has been connected to the server, so the reply from the server will be send directly back to the client.

From the perspective of a server, since it has no **listen()** session, it will accept messages from all the clients, so the address of **recvfrom()** will be an empty struct:
```cpp
/* server: */
getaddrinfo(HOSTIP, DBPORT, &hints, &servinfo);     // get server's address
sockfd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol)      // create a socket pointing to the server
bind(sockfd, servinfo->ai_addr, servinfo->ai_addrlen);      // bind the socket to the server port

struct sockaddr_storage their_addr;     // An empty address
addr_len = sizeof(their_addr);
recvfrom(sockfd, num, sizeof(num), 0, (struct sockaddr*)&their_addr, &addr_len);    // receive message from all possible senders
sendto(sockfd, ans, sizeof(ans), 0, (struct sockaddr*)&their_addr, sizeof(their_addr));     // reply message
```

Notice that after **recvfrom()** some clients, the client's address will automatically stored in __their_addr__, and when the server wants to send a message back to the client, just pass the same parameter __their_addr__ in the __sento()__.

## Contents of code

#### 1. database server: dbServer.cpp

In this assignment, database server takes the responsibility to maintain a database which stores all the links' information and respond to the main server's query. We use a vector<int*> to store the database. The building procedure and search process is shown below:
```cpp
/* dbServer.cpp */
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
}       //read the file and build up the database

int ans [3] = {-1, -1, -1};         // all -1 array
for(int i=0; i<database.size(); i++)
{
    if (database[i][0] == num[0])       // Search in the database
    {
        for (int j=0; j<3; j++)
            ans[j] = database[i][j+1];
        break;
    }
}       // Search procedure, if not found, return an all -1 array
```

#### 2. calculation server: calcServer.cpp

Calculation server takes the data transmitted from the main Server and conducts the computation of transmission delay and propagation delay:
```cpp
/* calcServer.cpp */
int num[4];         // input data from main Server
float ans[3];       // reply results
ans[0] = 1000.0 * (8.0 * num[0]) / num[1];      // transportation delay : Tt = size/C
ans[1] = 1000.0 * num[2] / float(num[3]);       // propagation delay : Tp L/Vp
ans[2] = ans[0] + ans[1];       // total delay: Tt + Tp
```


#### 3. main server: mainServer.cpp

Although it looks like a server, actually it plays a role of client in the program: main server takes two input integers from the keyboard, and send a message to the database server asking for more information, gets reply and send a request to the calculation server for computation. Therefore, it does not need to bind to its port number, just build connections to the two servers respectively and exchange messages will meet the requirements:

```cpp
/* mainServer.cpp */
getaddrinfo(HOSTIP, DBPORT, &hints, &servinfo);     // get database server's address
db_socket = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol)      // create a socket connected to the database server
sendto(db_socket, num, sizeof(num), 0, servinfo->ai_addr, servinfo->ai_addrlen);
recvfrom(db_socket, ans, sizeof(ans), 0, (struct sockaddr*)&their_addr, &addr_len);        // communicate with database server

getaddrinfo(HOSTIP, CALCPORT, &hints, &servinfo);     // get calc server's address
calc_socket = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol)      // create a socket connected to the calculation server
sendto(calc_socket, num, sizeof(num), 0, servinfo->ai_addr, servinfo->ai_addrlen);
recvfrom(calc_socket, ans, sizeof(ans), 0, (struct sockaddr*)&their_addr, &addr_len);       // communicate with calc server
```

#### 4. makefile

makefile is used to compile and run the above cpp programs:
```bash
# /* Makefile */
all: mainServer.cpp dbServer.cpp calcServer.cpp
	g++ -o mainServer.o mainServer.cpp
	g++ -o dbServer.o dbServer.cpp
	g++ -o calcServer.o calcServer.cpp	

mainServer: mainServer.o
	./mainServer.o

dbServer: dbServer.o
	./dbServer.o

calcServer: calcServer.o
	./calcServer.o

clean:
	rm mainServer.o dbServer.o calcServer.o
```

Please use "make all" or "make" first to compile the files, and run the 2 programs respectively. If you need to re-compile, use "make clean" to delete all executable files.

## Message Type:

We use int[] and float[] as the contents of the message, because UDP messages can send integers and floats directly.

## Idiosyncrasy

UDP connections are very unstable, sometimes the program may raise an error as:
```shell
sendto: Can't assign requested address
```

Please note that this is quite normal when I test the program, please try more times and you will get a correct output.

## Reused Code

This assignment takes ___Beej's Guide-to Network Programming: Using Internet Sockets___ as the reference, but all the codes are written by myself.
