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

#include "wlmc_args.c" //help menu, command line introduction

void randomize(unsigned short *key) {
	printf("Key = %d %d %d\n", key[0], key[1], key[2]);
	seed48(key);
}

int main(int argc, char **argv) {

	parsecmdline(argc, argv);
	
	randomize(key);//print random keys
	
	printf("\ninitializing system: placing the polymer and proteins\n");
	dna mydna(inputfile);//read seq
	pdna *b = new pdna();
	b->initialize_bdna(mydna);//set v to intrinsic values
	b->initialize_params(beta);//set value for matrices lambda, factor, z
	b->init_proteins();//put all structures in list and store pdbfiles in proteinPDB
	
	printf("contacting server\n");
	socket_type *connection = initsocket(host, port);
	printf("requesting parameters\n");
	parameter_type *params = requestreturnparameters(connection);//server.dat
	

	printf("Finding starting configuration between %f and %f\n", params->start, params->end);
    //between 0.000000 and 100.000000, 3rd and 4th number in server.dat 
	double E1, E2;

	b->generate_config();//set random values to v <- F 
	b->convert_steps();//convert v <- F, I 
	printf("%lf %lf %lf\n", b->calculate_twist_open(), b->calculate_writhe_open(), mold(b, params));
    //7.476979 0.010567 0.000000
	
	//move the configuration until falls within bin range
    while (((mold(b, params) < params->start) || (mold(b, params) > params->end)) && (params->measure != 3)) {
		b->move(1.0);
		if (b->dE(E1, E2) > 0.0) b->accept();
		else b->revert();
	};
		
	printf("End-to-end = %d percent\n", b->oldEE());
	//End-to-end = 0 percent, 100r/L	
	
	printf("\nrunning system simulation\n");
	runsimulation(b, params, connection, beta, host, node, Neq, savefile, Nc);
	delete params;
	delete connection;
	delete b;
	
	return 0;
}


