#ifndef _SERVERSOCKET_H
#define _SERVERSOCKET_H

int bindsocket(int port);

int acceptsocket(int s);
void closesocket(int s);

int writesocket(int socketID, void *data, size_t l);
int readsocket(int socketID, void *data, size_t l);

#endif
