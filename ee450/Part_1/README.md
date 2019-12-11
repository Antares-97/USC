# EE450 - Term Project - part 1
#### Junzhe Liu / 2270250947
#### Oct 13th

In this assignment, we simulate the TCP connections between a server and the clients using socket programming.

## Contents of code

#### 1.server

In this part we simulated a server on IP address "127.0.0.1" and port number "21947". The server will first request a DNS query using function **getaddrinfo()** and then creates a socket and bind the created socket with the local host using functions **socket()** and **::bind()**. Whenever the server is on-line, it will constantly waiting for new connections by **listen()** to its socket. When a client is asking for connection, it will **accept()** and creates a new socket and **fork()** s a child process to handle the communication. After all communications are done, it will **close()** the socket and tear down the connection.

Especially, we wrote a program to kill all the zombie processes that occupies the port, this part is copied from  ___Beej's Guide-to Network Programming: Using Internet Sockets___ :
```cpp
include<signnal.h>
void sigchld_handler(int s)
{
    int saved_errno = errno;
    while (waitpid(-1, NULL, WNOHANG) > 0);
    errno = saved_errno;
}

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
```

Zombie processes are often produced from **fork()** when the main process generates another child process. If we do not kill them, then in the **accept()** part will raise an error: "Interrupted System Call".

#### 2. client

We implements the part of clients in client.cpp. Clients differs with servers in that they do not have to bind a socket with its port, they just have to first create a socket by **socket()** and call **connect()** function to try to make connection with the server. After the connection is built, the client can communicate with the server using **send()** and **recv()** functions to send and receive messages respectively via the created socket. What the client needs to know is the server's IP address and the assigned port number, and query the server's information using **getaddrinfo()**.

#### 3. makefile

makefile is used to compile and run the above cpp programs:
```bash
/* Makefile */
all: client.cpp server.cpp
	g++ -o client client.cpp
	g++ -o server server.cpp

clean:
	rm client server
```

Please use "make all" or "make" first to compile the files, and run the 2 programs respectively. If you need to re-compile, use "make clean" to delete all executable files.

## Idiosyncrasy

Please follow strictly the compile and run commands written in the project instruction manual. First compile the codes by "make all" or "make", then runs the server with "./server <NAME2>" and the client with "./client <NAME1>" respectively. Runnning server or client without assigning the names are prohibited, nor does the user inputs something apart from the name.

e.g. neither "./server" nor "./server LJZ USC" can run the server, and so does the client.

## Reused Code

This assignment takes ___Beej's Guide-to Network Programming: Using Internet Sockets___ as the reference, and some part of the codes are copied from this book. Copied parts are marked in the codes.
