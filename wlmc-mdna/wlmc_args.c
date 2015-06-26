int port;
double beta;
int node;
int Nc;
char host[128];
char inputfile[128];
char savefile[128];
long int Neq;
int usevis;

unsigned short key[3];

#include "jsocket/jsocket.h"

void *runvisualization(void *x) {
	if (x == NULL) return NULL;

	while (1) {

		sleep(2);

	}

	return NULL;
	
}

void printhelp() {
	printf("Usage: ./wlmc [extra parameters]\n\n");
	printf("-h hostname[127.0.0.1]: specify hostname of server\n");
	printf("-p port[7000]: specify port of server at hostname\n");
	printf("-n node[0]: specify port containing statistics server\n");
	printf("-i input-file[seq]: specify input sequence file\n");
	printf("-b beta [1.0]: specify beta");
	printf("-e number[10000]: specify number of equilibration moves\n");
	printf("-c number[10]: number of moves between server communications\n");
	printf("-k x y z[random]: specify random number key (three 16-bit integers)\n");
	printf("-w filename[none]: save configurations to filename\n");
	printf("-v: turn on connection to local visualization server (not available)\n\n");
}

void parsecmdline(int argc, char **argv) {
	port = 7000;
	strcpy(host, "127.0.0.1");
	strcpy(inputfile, "seq");
	strcpy(savefile, "");
	usevis = 0;
	Neq = 10000;
	beta = 1.0;
	node = 0;
	Nc = 10;

	key[0] = (unsigned short)getpid();
	key[1] = (unsigned short)time(NULL);
	key[2] = (unsigned short)getuid();

	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-h") == 0) {
			strcpy(host, argv[i+1]);
			i++;
		} else if (strcmp(argv[i], "-p") == 0) {
			sscanf(argv[i+1], "%d", &port);
			i++;
		} else if (strcmp(argv[i], "-i") == 0) {
			strcpy(inputfile, argv[i+1]);
			i++;
		} else if (strcmp(argv[i], "-k") == 0) {
			sscanf(argv[i+1], "%hu", &key[0]);
			sscanf(argv[i+2], "%hu", &key[1]);
			sscanf(argv[i+3], "%hu", &key[2]);
			i += 3;
		} else if (strcmp(argv[i], "-e") == 0) {
			sscanf(argv[i+1], "%ld", &Neq);
			i++;
		} else if (strcmp(argv[i], "-v") == 0) {
			usevis = 1;
		} else if (strcmp(argv[i], "-w") == 0) {
			strcpy(savefile, argv[i+1]);
			i++;
		} else if (strcmp(argv[i], "-b") == 0) {
			sscanf(argv[i+1], "%lf", &beta);
			i++;
		} else if (strcmp(argv[i], "-n") == 0) {
			sscanf(argv[i+1], "%d", &node);
			i++;
		} else if (strcmp(argv[i], "-c") == 0) {
			sscanf(argv[i+1], "%d", &Nc);
			i++;
		} else {
			printf("bad command line parameter %s\n", argv[i]);
			printhelp();
			exit(0);
		}
	}

}


