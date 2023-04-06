libs: libmsocket.a #this is a target to make the library

all: client server

client: client.o libmsocket.a #link the client with the library
			gcc -o client client.o -L. -lmsocket -lpthread

server: server.o libmsocket.a #link the server with the library
			gcc -o server server.o -L. -lmsocket -lpthread

client.o: client.c
			gcc -c client.c
server.o: server.c
			gcc -c server.c

mysocket.o: mysocket.c mysocket.h
			gcc -c mysocket.c

libmsocket.a: mysocket.o #let's link library files into a static library
			ar rcs libmsocket.a mysocket.o

clean:
			rm -f *.o *.a client server