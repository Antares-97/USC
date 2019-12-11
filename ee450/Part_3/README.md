# EE450 - Term Project - part 3
#### Junzhe Liu / 2270250947
#### Dec 10th

In this assignment, we use both TCP and UDP to simulate the communications between clients and servers.



## Contents of code

Judging from the project, we can know that the communications between main server and database server, as well as the connections between main server and calculation server are exactly the same as term project part 2, therefore I reused the code of TP2 in this part. As for the other part: the conversation between clients and the main server, it uses TCP and thus TP1 could be modified and reused. In conclusion, my project contains the following parts:


#### 1. client: client.cpp

We implements the part of clients in client.cpp. Clients differs with servers in that they do not have to bind a socket with its port, they just have to first create a socket by **socket()** and call **connect()** function to try to make connection with the server. After the connection is built, the client can communicate with the server using **send()** and **recv()** functions to send and receive messages respectively via the created socket. What the client needs to know is the server's IP address and the assigned port number, and query the server's information using **getaddrinfo()**.

#### 2.main server: mainServer.cpp

The server will first request a DNS query using function **getaddrinfo()** and then creates a socket and bind the created socket with the local host using functions **socket()** and **::bind()**. Whenever the server is online, it will constantly waiting for new connections by **listen()** to its socket. When a client is asking for connection, it will **accept()** and creates a new socket. After all communications are done, it will **close()** the socket and tear down the connection.

What differs from the TP1 here is that we did not use **fork()** in this program, because it seems difficult to close the child process and return to the parent process when we applies UDP connection inside it.


#### 3. database server: dbServer.cpp

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

#### 4. calculation server: calcServer.cpp

Calculation server takes the data transmitted from the main Server and conducts the computation of transmission delay and propagation delay:
```cpp
/* calcServer.cpp */
int num[4];         // input data from main Server
float ans[3];       // reply results
ans[0] = 1000.0 * (8.0 * num[0]) / num[1];      // transportation delay : Tt = size/C
ans[1] = 1000.0 * num[2] / float(num[3]);       // propagation delay : Tp L/Vp
ans[2] = ans[0] + ans[1];       // total delay: Tt + Tp
```

#### 5. makefile

makefile is used to compile and run the above cpp programs:
```bash
# /* Makefile */
all: mainServer.cpp dbServer.cpp calcServer.cpp client.cpp 
	g++ -o mainServer.o mainServer.cpp
	g++ -o dbServer.o dbServer.cpp
	g++ -o calcServer.o calcServer.cpp
	g++ -o client client.cpp	

mainServer: mainServer.o
	./mainServer.o

dbServer: dbServer.o
	./dbServer.o

calcServer: calcServer.o
	./calcServer.o

clean:
	rm mainServer.o dbServer.o calcServer.o client
```

Please use "make all" or "make" first to compile the files, and run the 2 programs respectively. If you need to re-compile, use "make clean" to delete all executable files.

## Message Type:

We use int[] and float[] as the contents of the message, because TCP and UDP messages can send integers and floats directly.

## Idiosyncrasy

No errors found up to now.

## Reused Code

This assignment reuses the TP1 and TP2 of myself.
