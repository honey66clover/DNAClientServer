#ifndef _SOCKET_H
#define _SOCKET_H

#include <arpa/inet.h>

struct socket_type {
	struct sockaddr_in caddr;
	socklen_t len;
	int socketID;
};

socket_type *initsocket(char *hostname, int port);

int connectsocket(socket_type *mysocket);
void closesocket(int socketID);

int writesocket(int socketID, void *data, size_t l);
int readsocket(int socketID, void *data, size_t l);

#endif

