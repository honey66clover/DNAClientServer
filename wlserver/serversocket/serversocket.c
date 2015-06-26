#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#include "serversocket.h"

int bindsocket(int port) {
	
	socklen_t server_len;
	sockaddr_in saddr;
	
	int serversock = socket(AF_INET, SOCK_STREAM, 0);
	
	int yes = 1;
	setsockopt(serversock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
	
	bzero((char *) &saddr, sizeof(saddr));
	saddr.sin_family = AF_INET;
	saddr.sin_addr.s_addr = htonl(INADDR_ANY);
	saddr.sin_port = htons(port);
	server_len = sizeof(saddr);
	int bind_id = bind(serversock, (struct sockaddr *)&saddr, server_len);
	//printf("bind_id=%d\n",bind_id);
	int listen_id = listen(serversock, 16);
	//printf("listen_id=%d\n",listen_id);

	return serversock;

}


int acceptsocket(int s) {

//	struct sockaddr_in scaddr;
//	socklen_t client_len;
	
	int serversock = accept(s, NULL, NULL); // (struct sockaddr *)&scaddr, &client_len);

	if (serversock == -1) {
		printf("accept socket failed\n");
	}

	return serversock;
	
}


void closesocket(int s) {
	close(s);
}


int writesocket(int socketID, void *data, size_t l) {
	return send(socketID, data, l, 0);	
}


int readsocket(int socketID, void *data, size_t l) {
	return recv(socketID, data, l, 0);
}

