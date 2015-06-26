#ifndef _PARAMETERS_H
#define _PARAMETERS_H

#define REQUEST_PARAMETERS 0
#define REQUEST_UPDATE_DATA 1

#define MAX_BIN 65536

#include "../socket/socket.h"

struct parameter_type {
	int measure;
	int Nbins;
	float start, end;
	float dUstar;
	float dUstartol;
};

struct data_type {
	int key;
	int size;
	float dUstar;
	unsigned int Ustar[MAX_BIN];
};

parameter_type *requestreturnparameters(socket_type *s);

data_type *requestupdateparameters(socket_type *s, data_type *d);

#endif

