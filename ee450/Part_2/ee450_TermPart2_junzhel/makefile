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

