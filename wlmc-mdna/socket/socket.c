// Standard C and UNIX functions
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>

// TCP include files
#include <sys/socket.h>
#include <netdb.h>

#include "socket.h"

socket_type *initsocket(char *hostname, int port) {
	
	socket_type *mysocket = new socket_type;
	
	bzero((char *)&mysocket->caddr, sizeof(mysocket->caddr));
	mysocket->caddr.sin_family = AF_INET;
	mysocket->caddr.sin_addr.s_addr = inet_addr(inet_ntoa(*(struct in_addr *)*gethostbyname(hostname)->h_addr_list));
	mysocket->caddr.sin_port = htons(port);
	mysocket->len = sizeof(mysocket->caddr);

	return mysocket;
	
}

int connectsocket(socket_type *mysocket) {

	int result;
	
	mysocket->socketID = socket(AF_INET, SOCK_STREAM, 0);
	// corrected code
	if ((result = connect(mysocket->socketID, (struct sockaddr *)&mysocket->caddr, mysocket->len)) != 0) {
		printf("connection failed, sleeping for 5 seconds and retrying\n");
		mysocket->socketID = -10;
	}
	/*
	while ((result = connect(mysocket->socketID, (struct sockaddr *)&mysocket->caddr, mysocket->len)) != 0) {
		printf("connection failed, sleeping for 5 seconds and retrying\n");
		sleep(5);
		mysocket->socketID = socket(AF_INET, SOCK_STREAM, 0);
	}
	return mysocket->socketID;
	*/
	return mysocket->socketID;
	//
}

void closesocket(int socketID) {
	close(socketID);
}

int writesocket(int socketID, void *data, size_t l) {
	return write(socketID, data, l);
}

int readsocket(int socketID, void *data, size_t l) {
	return read(socketID, data, l);
}

