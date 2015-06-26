#include <stdio.h>
#include <stdlib.h>

#include "parameters.h"

// requestreturnoparameters :
// asks the server to supply the needed Wang-Landau parameters for simulation
// retrieves the current values
parameter_type *requestreturnparameters(socket_type *s) {
		
	int request = REQUEST_PARAMETERS;
	int mysock = connectsocket(s);
	if (mysock==-10){
		printf("request return parameters failed.\n");
		exit(0);
	}
	parameter_type *p = new parameter_type;

// tell the server we're asking for the Wang-Landau parameters	
    writesocket(mysock, &request, sizeof(int));
// read the Wang-Landau parameters
	readsocket(mysock, p, sizeof(parameter_type));
	
	closesocket(mysock);
	
	return p;
	
}



// requestdataparameters : 
// Sends only the updated U* values and also the current value of dUstar, 
// retrieves the client-summed U* and any updated dUstar
data_type *requestupdateparameters(socket_type *s, data_type *d) {
	
	int request = REQUEST_UPDATE_DATA;	
	int mysock = connectsocket(s);
	
	if (mysock==-10){
		printf("request update parameters failed.\n");
		exit(0);
	}

	int size = (d->size + 3) * sizeof(int) + sizeof(float);
	data_type *new_data = new data_type;

	int i = 0;

// tell the server we're asking to update
	writesocket(mysock, &request, sizeof(int));
// send the data to the server
	if (writesocket(mysock, d, size) != size) printf("something fucked up\n");

// retrieve the summed data and new dUstar
	if (readsocket(mysock, new_data, size) != size) printf("something fucked up\n");
		
	closesocket(mysock);
	
	return new_data;

}
