#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <unistd.h>
#include <time.h>
#include <pthread.h>


#include "socket/socket.h"
#include "parameters/parameters.h"
#include "pdna/pdna.h"
#include "mc/mc.h"

#include "wlmc_args.c"

void randomize(unsigned short *key) {
	printf("Key = %d %d %d\n", key[0], key[1], key[2]);
	seed48(key);
}

int main(int argc, char **argv) {

	parsecmdline(argc, argv);
	
	randomize(key);
	
	printf("\ninitializing system: placing the polymer and proteins\n");
	dna mydna(inputfile);
	pdna *b = new pdna();
	b->initialize_bdna(mydna);
	b->initialize_params();
	b->init_proteins();
	
/*	int Nee = 0;
	printf("\nestimating R_ee > 0.5 L fraction\n");
	for (int i = 0; i < 100000; i++) {
		b->generate_config();
		b->convert_steps();
		if (b->oldEE() > 3.4*b->nsteps/2.0) Nee++;
	}
	printf("%lf of configurations have R_ee > 50 percent of L\n", (double)Nee/100000.0); */

	do {
		b->generate_config();
		b->convert_steps();
	} while (b->oldEEt() >= 2000);

	
	printf("contacting server\n");
	socket_type *connection = initsocket(host, port);
	printf("requesting parameters\n");
	parameter_type *params = requestreturnparameters(connection);
	
	if (usevis) {
		printf("spawning visualization client to connect to local port 7001\n");
		pthread_t thread;
    	pthread_create(&thread, NULL, runvisualization, (void *)NULL);
	}
	
	printf("\nrunning system simulation\n");
	runsimulation(b, params, connection, savefile);
	delete params;
	delete connection;
	delete b;
	
	return 0;
}


