
int port;
char inputfile[256];

void printmenu() {
	printf("SYSTEM MENU\n");
	printf("u: print U*\n");
	printf("uu: print U* as absolute updates\n");
	printf("sq: print rectangular U* 2D data\n");
	printf("p: print p*\n");
	printf("d: print dU*, dp*, and dU* tolerance\n");
	printf("w: print Wang-Landau measure values\n\n");
	printf("s: save Wang-Landau energy values\n");
	printf("l: load Wang-Landau energy values\n");
	printf("r: reset p* values\n");
	printf("f: force update to next dU*\n");
	printf("e: edit bin\n\n");
	printf("t: trim end bin\n");
	printf("a: add end bin\n\n");
	printf("sl: set up slope potential\n");
	printf("cl: clear everything\n");
	printf("te: turn edits on and off (switch from Wang and Landau to Metropolis)\n");
	printf("q: quit\n\n");
	printf("your command? ");
}

void printhelp() {
	printf("Usage: ./server port [extra parameters]\n\n");
	printf("-i input-file[server.dat]: specify input file\n");
	printf("-n : disable the interactive menu\n");
	printf("-l data-file : read the data-file with columns of bin and U*\n\n");
}

char *sdata;

int parsecmdline(int argc, char **argv) {

	sdata = NULL;

	int usemenu = 1;
	strcpy(inputfile, "server.dat");

	if (argc == 1) {
		printhelp();
		exit(0);
	}

	sscanf(argv[1], "%d", &port);
	
	if ((port < 10) || (port > 65535)) {
		printf("Invalid port number %d\n", port);
		exit(0);
	}

	for (int i = 2; i < argc; i++) {
		if (strcmp(argv[i], "-i") == 0) { //-i input-file[server.dat]: specify input file
			strcpy(inputfile, argv[i+1]);
			i++;
		} else if (strcmp(argv[i], "-n") == 0) { //-n : disable the interactive menu
			usemenu = 0;
		} else if (strcmp(argv[i], "-l") == 0) { //-l data-file : read the data-file with columns of bin and U*
			sdata = new char[256];
			strcpy(sdata, argv[i+1]);
			i++;
		} else {
			printf("bad command line parameter %s\n", argv[i]);
			printhelp();
			exit(0);
		}
	}

	return usemenu;
}


void *runmenu(void *x) {
	serverdata_type *s = (serverdata_type *)x;
	char str[16];
	for (;;) {
		printmenu();
		scanf("%s", str);
		if (strcmp(str, "u") == 0) {
			for (int i = 0; i < s->Nbins; i++) printf("%d %lf\n", i+1, s->dUstar * s->Ustar[i]);
			printf("\n");
		} else if (strcmp(str, "uu") == 0) {
			for (int i = 0; i < s->Nbins; i++) printf("%d %u\n", i+1, s->Ustar[i]);
			printf("\n");
		} else if (strcmp(str, "add") == 0) {
			printf("start bin, end bin, number of updates: \n");
			int x, y, z;
			scanf("%d %d %d", &x, &y, &z);
			for (int i = x-1; i < y; i++) s->Ustar[i] += z;
		} else if (strcmp(str, "sl") == 0) {
			int x;
			printf("Number of updates? ");
			scanf("%d", &x);
			for (int i = 0; i < s->Nbins; i++) s->Ustar[i] = i*x;
		} else if (strcmp(str, "sll") == 0) {
			int x, bin, val;
			printf("Starting bin, bin value, number of updates? ");
			scanf("%d %d %d", &bin, &val, &x);
			for (int i = bin-1; i < s->Nbins; i++) s->Ustar[i] = val-(i-bin+1)*x;
		} else if (strcmp(str, "cl") == 0) {
			for (int i = 0; i < s->Nbins; i++) {
				s->Ustar[i] = 0;
				s->pstar[i] = 0.0;
			}
		} else if (strcmp(str, "te") == 0) {
			if (s->make_updates == 0) s->make_updates = 1;
			else s->make_updates = 0;
		} else if (strcmp(str, "sq") == 0) {
			int sz;
			printf("Length: ");
			scanf("%d", &sz);
			if (sz != 0) for (int i = 0; i < s->Nbins; i++) {
				if (i % sz == 0) printf("\n");
				printf("%lf ", s->dUstar * s->Ustar[i]);
			}
			printf("\n\n");
		} else if (strcmp(str, "p") == 0) {
			for (int i = 0; i < s->Nbins; i++) printf("%d %f\n", i+1, s->pstar[i]);
			printf("\n");
		} else if (strcmp(str, "d") == 0) {
			printf("dU* = %e\ndp* = %f\ndU* tolerance = %e\n\n", s->dUstar, s->dpstar, s->dUstartol);
			printf("range = %f to %f\n", s->start, s->end);
			printf("updates = %d\n\n", s->make_updates);
		} else if (strcmp(str, "q") == 0) {
			s->request_exit = 1;
			return NULL;
		} else if (strcmp(str, "w") == 0) {
			printf("measure = %d\nnumber of bins = %d\nstart of range = %f\nend of range = %f\n\n", s->measure, s->Nbins, s->start, s->end);
		} else if (strcmp(str, "s") == 0) {
			FILE *f = fopen("saved-data", "w");
			fwrite(&s->dUstar, sizeof(float), 1, f);
			fwrite(s->Ustar, sizeof(unsigned int), s->Nbins, f);
			fclose(f);
			printf("Saved file saved-data, with dUstar and Ustar\n\n");
		} else if (strcmp(str, "l") == 0) {
			FILE *f = fopen("saved-data", "r");
			if (f != NULL) {
				fread(&s->dUstar, sizeof(float), 1, f);
				fread(s->Ustar, sizeof(unsigned int), s->Nbins, f);
				setdUstar((double)s->dUstar);
				fclose(f);
				printf("Loaded file saved-data, with dUstar and Ustar\n\n");
			} else printf("No saved-data file\n\n");
		} else if (strcmp(str, "stop") == 0) {
			s->make_updates = 0;
		} else if (strcmp(str, "start") == 0) {
			s->make_updates = 1;
		} else if (strcmp(str, "sb") == 0) {
			printf("Input start, end, and value in updates: ");
			int x, y, z;
			scanf("%d %d %d", &x, &y, &z);
			for (int i = x-1; i < y; i++) {
				s->Ustar[i] = z;
			}
		} else if (strcmp(str, "r") == 0) {
			for (int i = 0; i < s->Nbins; i++) s->pstar[i] = 0.0;
			printf("Resetted p* histogram\n\n");
		} else if (strcmp(str, "e") == 0) {
			int mybin;
			printf("Input bin number to edit (numbering starting with 1): ");
			scanf("%d", &mybin);
			printf("Input U* (as integer multiple of %f) and p*: ", s->dUstar);
			scanf("%u %f", &s->Ustar[mybin-1], &s->pstar[mybin-1]);
			printf("Done\n\n");
		} else if (strcmp(str, "f") == 0) {
			for (int i = 0; i < s->Nbins; i++) {
				s->pstar[i] = 100000.0;
			}

			s->Np = 100000.0*s->Nbins;
			printf("Done!\n\n");
		} else if (strcmp(str, "ff") == 0) {
			for (int i = 0; i < s->Nbins; i++) {
				s->Ustar[i] /= 4;
				s->pstar[i] = 0.0;
			}
			setdUstar(4*s->dUstar);
			s->dUstar *= 4.0;
		} else if (strcmp(str, "t") == 0) {
			s->Nbins--;
			s->end -= (s->end-s->start)/(float)(s->Nbins+1);
			s->key = (int)(drand48()*1000.0);
			printf("key = %d\n", s->key);
		} else if (strcmp(str, "a") == 0) {
			s->Nbins++;
			s->end++;
			s->key = (int)(drand48()*1000.0);
			printf("key = %d\n", s->key);
		} else if (strcmp(str, "ld") == 0) {
			printf("Number of elements and dUstar: ");
			int Ne;
			double dUstar2;
			scanf("%d %lf", &Ne, &dUstar2);
			setdUstar(dUstar2);
			s->dUstar = dUstar2;
			int bin;
			double value;
			FILE *f = fopen("data-set", "r");
			for (int i = 0; i < Ne; i++) {
				fscanf(f, "%d %lf", &bin, &value);
				s->Ustar[bin-1] = (int)(value/dUstar2);
			}
			fclose(f);
		}
	}
}

