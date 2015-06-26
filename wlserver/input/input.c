#include <stdio.h>
#include <stdlib.h>

#include "input.h"

serverdata_type *readinput(char *input, char *start) {
	FILE *f = fopen(input, "r");
	if (f == NULL) {
		printf("error opening server input file %s\n", input);
		exit(0);
	}
	
	serverdata_type *sd = new serverdata_type;
	
	fscanf(f, "%d %d %f %f %f %f %f", &sd->Nbins, &sd->measure, &sd->start, &sd->end, &sd->dUstar, &sd->dpstar, &sd->dUstartol);

	printf("%d %d %f %f %f %f %f\n", sd->Nbins, sd->measure, sd->start, sd->end, sd->dUstar, sd->dpstar, sd->dUstartol);
	
	sd->Ustar = new unsigned int[sd->Nbins+20];
	sd->pstar = new float[sd->Nbins+20];
	sd->Np = 0.0;
	
	for (int i = 0; i < sd->Nbins+10; i++) {
		sd->Ustar[i] = 0;
		sd->pstar[i] = 0.0;
	}
	
	sd->request_exit = 0;
	
	fclose(f);

	if (start != NULL) {
		double x[1024];
		int m;
		f = fopen(start, "r");
		for (int i = 0; i < sd->Nbins; i++) {
			fscanf(f, "%d %lf", &m, &x[i]);
			sd->Ustar[i] = (int)(x[i]/sd->dUstar);
		}
	}	

	return sd;
	
}

