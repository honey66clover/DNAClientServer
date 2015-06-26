
all: server

server: main.o serversocket.o parameters.o input.o
	g++ -Wall -lpthread -o server main.o serversocket.o parameters.o input.o
	
	
main.o: main.c server_args.c
	g++ -Wall -c main.c
	
serversocket.o: serversocket/serversocket.c serversocket/serversocket.h
	g++ -Wall -c serversocket/serversocket.c
	
parameters.o: parameters/parameters.c parameters/parameters.h
	g++ -Wall -c parameters/parameters.c
	
input.o: input/input.c input/input.h
	g++ -Wall -c input/input.c
	