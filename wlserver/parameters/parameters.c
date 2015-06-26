#include <stdio.h>
#include <stdlib.h>

#include "parameters.h"
#include "../serversocket/serversocket.h"

data_type data_in;
data_type data_out;
parameter_type p;
double dUstar;

void setdUstar(double Us) {
	dUstar = Us;
}

void processrequest(int socket, serverdata_type *r) {

	int mysock = acceptsocket(socket);
	//printf("accept_id=%d\n",mysock);
	int request_type;
	static double Nptotal = 0.0;
	static int shit_out_of_luck = 0;
	
	//if (readsocket(mysock, &request_type, sizeof(int)) != sizeof(int)) return;
	int read_id = readsocket(mysock, &request_type, sizeof(int));
	//printf("read_id=%d, request_type=%d\n",read_id, request_type);
	if (read_id != sizeof(int)) return;
// send the client the parameters to start the simulation
	if (request_type == REQUEST_PARAMETERS) {

		p.measure = r->measure;
		p.Nbins = r->Nbins;
		p.start = r->start;
		p.end = r->end;
		p.dUstar = dUstar;
		p.dUstartol = r->dUstartol;

		writesocket(mysock, &p, sizeof(parameter_type));
		closesocket(mysock);
		return;

	}

	if (request_type == FORCE_UPDATE) {
		for (int i = 0; i < r->Nbins; i++) r->pstar[i] = 1000000;
		r->Np = r->Nbins*1000000;
		return;
        }

// get the new data, add the data if dUstar's match, and return the summed Ustars
	if (request_type == REQUEST_UPDATE_DATA) {

		int size = (r->Nbins+3) * sizeof(int) + sizeof(float);
		int sz;

		sz = readsocket(mysock, &data_in, size);
		float pv = 0.0;
		if ((r->Nbins != data_in.size)) {
			printf("warning, bad data or client and server bin sizes do not match! fixing...\n");
		} else
		for (int i = 0; i < data_in.size; i++) {
			
			if (r->Ustar[i] > (unsigned int)(4294967290)) {
				printf("Sorry buddy, I've run out of 32-bit precision\n");
				shit_out_of_luck = 1;
			}
            // if the dUstars match, we add in the new data to summed Ustars
			if ((data_in.dUstar == r->dUstar) && (!shit_out_of_luck)) {
				if (r->make_updates) r->Ustar[i] += data_in.Ustar[i];
				r->pstar[i] += (pv = data_in.Ustar[i]);
				r->Np += pv;
			}
			
			data_out.Ustar[i] = r->Ustar[i];

		}
		//otherwise we discard the data and return the server's summed Ustars
		if (data_in.dUstar != r->dUstar) {
			printf("Warning! %f %f\n", data_in.dUstar, r->dUstar);
		}
		
		if ((data_in.dUstar == r->dUstar) && (data_in.key == r->key)) {
// calculate p* error, see if we need to update dU*		
			float pstar_avg = r->Np / (float)r->Nbins;
			int updateUstar = 1;
			if (r->Np == 0) updateUstar = 0;
			for (int i = 0; (i < data_in.size) && (updateUstar); i++) {
				if ((pstar_avg > r->pstar[i] ? (pstar_avg - r->pstar[i]) : (r->pstar[i] - pstar_avg)) / pstar_avg > r->dpstar) updateUstar = 0;
			}
			if (r->make_updates == 0) updateUstar = 0;
// go to the next step of the simulation by changing dU*
			if (updateUstar) {
				Nptotal += r->Np;
				printf("Updating dU*, %lf total moves\n", Nptotal);
				dUstar /= 4.0;
				r->dUstar = dUstar;

				r->Np = 0.0;
				unsigned int minbin = (unsigned int)(3000000000);
				unsigned int min = 0;
				for (int i = 0; i < data_in.size; i++) {
					r->pstar[i] = 0.0;
					if (r->Ustar[i] < minbin) {
						minbin = r->Ustar[i];
						min = i;
					}
				}
				for (int i = 0; i < data_in.size; i++) {
					r->Ustar[i] -= minbin;
					r->Ustar[i] *= 4;
				}
// are we done?  if so, set system to exit and write out the result
				if (r->dUstar < r->dUstartol) {
					if (!r->request_exit) {
						FILE *f = fopen("./output/results.txt", "w");
						printf("\n");
						for (int i = 0; i < r->Nbins; i++) {
							printf("%d %lf\n", i+1, dUstar * r->Ustar[i]);
							fprintf(f, "%d %lf\n", i+1, dUstar * r->Ustar[i]);
						}
						fclose(f);
						r->request_exit = 1;
					}
					printf("converged, turning off updates\n");
					r->make_updates = 0;
					// here we should exit, but we need to tell all the remaining clients to stop
				}
			}
		}
		
		data_out.dUstar = r->dUstar;
		data_out.key = r->key;
		writesocket(mysock, &data_out, size);
		closesocket(mysock);

		return;
		
	}
	
	closesocket(mysock);
	
}

void initparameters(serverdata_type *r) {
	r->key = (int)(1000.0*drand48());
	r->make_updates = 1;
	dUstar = r->dUstar;
        if (r->dUstar < r->dUstartol) r->make_updates = 0;
}

