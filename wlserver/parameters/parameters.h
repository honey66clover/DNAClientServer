#ifndef _PARAMETERS_H
#define _PARAMETERS_H

#define REQUEST_PARAMETERS 0
#define REQUEST_UPDATE_DATA 1
#define FORCE_UPDATE 2

#define MAX_BIN 20000
//data in server, sum of clients
struct serverdata_type {
	int make_updates;
	int key;
	int measure;
	int Nbins;
	float start, end;
	unsigned int *Ustar;
	float *pstar;
	float Np;
	float dUstar;
	float dpstar;
	float dUstartol;
	int request_exit;
};
//data in client
struct parameter_type {
	int measure;
	int Nbins;
	float start, end;
	float dUstar;
	float dUstartol;
};

struct data_type {
	int key;
	int size;//# of bins
	float dUstar;
	unsigned int Ustar[MAX_BIN];
};

void setdUstar(double Us);

void processrequest(int socket, serverdata_type *r);
void initparameters(serverdata_type *r);

#endif

