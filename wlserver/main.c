#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include "serversocket/serversocket.h"
#include "parameters/parameters.h"
#include "input/input.h"

#include "server_args.c"

#define FOREVER_EVER_EVER 1

int main(int argc, char **argv) {

	int usemenu = parsecmdline(argc, argv);//./server port -i -n -l

	short unsigned int key[3];
	key[0] = getpid();
	key[1] = time(NULL);
	seed48(key);
			
    serverdata_type *s = readinput(inputfile, sdata);//read server.dat or saved-data to initiate dU* and p*

	pthread_t thread;
	if (usemenu) {int pthread_id = pthread_create(&thread, NULL, runmenu, (void *)s);
	//printf("pthread_id=%d\n",pthread_id);
	}
	int socket = bindsocket(port);
	//printf("socket=%d\n",socket);

	if (!usemenu) printf("No menu will be available\n");
	initparameters(s);
	//printf("key = %d\n", s->key);
// main loop - we read requests and complete transactions
	while (FOREVER_EVER_EVER) {
		//printf("entering loop\n");
		processrequest(socket, s);
		//printf("finishing loop\n");
	}

	return 0;

}

