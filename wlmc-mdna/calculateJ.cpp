
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main(int argc, char **argv) {
	if (argc < 4) return -1;
	FILE *f = fopen(argv[1], "r");
	double Ustar[100];
	double p = 0.0;
	double pT = 0.0;
	for (int i = 0; i < 100; i++) {
		fscanf(f, "%lf", &Ustar[i]);
		pT += exp(Ustar[i]);
	}
	printf("Probability density = %le\n", p = exp(Ustar[0])/pT);
	printf("Jfactor = %le\n", p * 4.0 * M_PI / (6.022e23 * (1-cos(M_PI/10.0)) * 4.0 * M_PI * pow(3.4*atoi(argv[3])*0.025*1e-9, 3.0) * 2.0 * M_PI/(10.0*3.0)));
	return 0;
}
