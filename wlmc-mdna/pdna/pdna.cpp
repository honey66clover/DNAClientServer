
#include "../proteinPDB/proteinPDB.h"
#include "../translate_tools.h"
#include "pdna.h"
#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#define PIV 5.0

  const FLOAT eps = 77.7;
  const FLOAT e = 0.24*1.6021773e-19;
  const FLOAT eps0 = 8.854188e-32;
  const FLOAT kT = 1.380658e-13*300.0;
  const FLOAT coeff = e*e/(4*3.14159*eps0*eps*kT);
  
FLOAT randg() {

  static int iset = 0;
  static FLOAT gset;

  FLOAT v1, v2, fac, rsq;

  if (iset == 0) {
    do {
      v1 = 2.0*drand48() - 1.0;
      v2 = 2.0*drand48() - 1.0;
      rsq = v1*v1+v2*v2;
    } while ((rsq >= 1.0) || (rsq == 0));
    fac = sqrt(-2.0*log(rsq)/rsq);
    gset = v1*fac;
    iset = 1;
    return v2*fac;
  } else {
    iset = 0;
    return gset;
  }

}


pdna::pdna() {
}


pdna::~pdna() {
  	delete [] z;
  	delete [] lambda;
  	delete [] factor;
	delete [] vchange;
	delete [] v2;
	delete [] vchange2;
	delete [] pos;
	delete [] poschange;
}

//put all structures in list and store pdbfiles in proteinPDB
void pdna::init_proteins() {
  FILE *fp = fopen("/home/juanwei/mycode/dnatools/wlmc-mdna/proteins.dat", "r");
  char buf[80];

  int nt;
  int ns;

  fgets(buf, 80, fp);
  sscanf(buf, "%d %d", &nt, &ns);
  ntypes = nt;//# of types of proteins
  nstructs = ns;//# of all structures of all types of proteins

  if (nt == 0) return;

  nproteins = new int[nt];//# of structures of each type of protein
  psteps = new int[nt];
//  pnames = new char[nt][20];
//  for (int m = 0; m < nt; m++) pnames[m] = new char[20];
  pstartindex = new int[nt];
  Pp = new FLOAT[nt];
Pp2 = new FLOAT[nt];
  proteins = new bdna[ns];
//  pfilenames = new char[ns][20];
  structtype = new int[ns];

  proteinPDBs = new proteinPDB[ns];

  int pindex = 0;//pindex: index for structures within all types of proteins

  for (int i = 0; i < nt; i++) {//i->ith type of protein
    printf("Structure  %d\n", i+1);
    pstartindex[i] = pindex;//index for types 
    fgets(buf, 80, fp);
	int x1, y1;
	sscanf(buf, "%s %d %d %lf %lf", pnames[i], &x1, &y1, &Pp[i], &Pp2[i]);
	nproteins[i] = x1; //4 for HU
	psteps[i] = y1; //14 for HU
    printf("%s\n", pnames[i]);
    if (nproteins[i] != 0) 
	for (int j = 0; j < nproteins[i]; j++) {//j->jth structures within one type of protein
      if (pindex == ns) {
		printf("error, too many structures (i.e. change the first line of the protein data file)!\n");
	  }
      fgets(buf, 80, fp);
	  sscanf(buf, "%s", pfilenames[pindex]);//pfilenames[0]=HU-1,...pfilenames[3]=HU-4;pfilenames[4]=Hbb 
      char temp[100];
      char temp2[100];
      strcpy(temp, pfilenames[pindex]);
	  strcat(temp, ".dat");//HU-1.dat
	  printf("Loading %s\n", temp);
      strcpy(temp2, pfilenames[pindex]);
	  strcat(temp2, ".pdb");//HU-1.pdb
	  proteinPDBs[pindex].readPDB(temp2);//proteinPDBs are stored according to pindex
	  proteins[pindex].nsteps = psteps[i];//proteins[0].nsteps=14, psteps of ith type of protein 
	  proteins[pindex].v = new matrix[psteps[i]];//proteins[0].v reads HU-1.dat
	  for (int k = 0; k < psteps[i]; k++) proteins[pindex].v[k].setsize(6,1);
	  proteins[pindex].readconfig(temp);
      structtype[pindex] = j;//HU:structtype[0]=0,...,structtype[3]=3; Hbb:structtype[4]=0;...
	  printf("Loaded %s\n", pfilenames[pindex]);//Loaded HU-1
      pindex++;
    }
  }
  fclose(fp);
}

void pdna::set_protein(int n, FLOAT x) { Pp[n] = x; }

//randomly chose a structure from a type of protein ant put it from step a on DNA b
void pdna::place_protein(bdna *b, short int a, short int type, char reverse) {
  int c = (int)(nproteins[type]*drand48())+pstartindex[type];//c->index of the structure in all structures
  if (reverse) {
    for (int i = 0; i < psteps[type]; i++) {
      b->v[i+a] = proteins[c].v[psteps[type]-i-1];
      b->v[i+a].setv(1,1, -b->v[i+a](1,1));
      b->v[i+a].setv(4,1, -b->v[i+a](4,1));
    }
  } else
  for (int i = 0; i < psteps[type]; i++) 
    b->v[i+a] = proteins[c].v[i];
}


//print lambda, F, z matrices on screen
void pdna::printeigenvalues() {
  for (int i = 0 ; i < number_of_bases*number_of_bases; i++) {
    printf("%c%c\n", btranslate(i / number_of_bases), btranslate(i % number_of_bases));
    writematrix(stdout, lambda[i]);
    writematrix(stdout, F[i]);
    writematrix(stdout, z[i]);
  }
}




//set random values to v <- factor <- lambda <- d <- F
void pdna::generate_config() {
  for (int i = 0; i < nsteps; i++) {
    for (int j = 1; j <= 6; j++) {
      v[i].setv(j, 1, factor[bpstep[i]](j,1)*randg());
    }
	v[i].setv(4, 1, 0.0);
	v[i].setv(5, 1, 0.0);
	v[i].setv(6, 1, 0.0);
  }
}

//convert v according to z, I matrices
void pdna::convert_steps() {
    matrix tpt;
    for (int j = 0; j < nsteps; j++) {
       tpt = (z[bpstep[j]]*v[j])+I[bpstep[j]];
       v[j] = tpt;
    }
}


//set value for matrices lambda, factor, z
void pdna::initialize_params(double b) {
	beta = 1.0;
	lambda = new matrix[number_of_bases*number_of_bases];

	FLOAT *d = new FLOAT[6];

	z = new matrix[number_of_bases*number_of_bases];

	factor = new matrix[number_of_bases*number_of_bases];

	int i;
	int j;

	for (i = 0; i < number_of_bases*number_of_bases; i++) {

		z[i].setsize(6,6); 

		z[i] = jeigen(F[i], d);//diagonalized F matrix?


		lambda[i].setsize(6, 1);
    	for (j = 1; j <= 6; j++) lambda[i].setv(j, 1, d[j-1]);

    	factor[i].setsize(6,1);
    	for (j = 1; j <= 6; j++) { 
      		if (lambda[i](j,1) > 0.0)
        		factor[i].setv(j, 1, sqrt(1/(b*lambda[i](j,1))));
      		else
        		factor[i].setv(j, 1, sqrt(-1/(b*lambda[i](j,1))));
    	}

  	}
  	vchange = new matrix[nsteps];
	v2 = new matrix[nsteps];
	vchange2 = new matrix[nsteps];
	pos = new protein[nsteps];
	poschange = new protein[nsteps];
	nP = 0;
	nPchange = 0;
	q = new bdna(nsteps);
	for (i = 0; i < nsteps; i++) {
		vchange[i].setsize(6,1);
		v2[i].setsize(6,1);
		vchange2[i].setsize(6,1);
	}
	delete [] d;

}

//DNA self-overlap
int pdna::overlap() {
  FLOAT *x, *y, *z;
  x = new FLOAT[nsteps-15];
  y = new FLOAT[nsteps-15];
  z = new FLOAT[nsteps-15];
  matrix temp = identity(4);
  for (int i = 0; i < nsteps; i++) {
	temp = temp * calculateW(v[i]);
	if ((i >= 10) && (i < nsteps-10)) {
      x[i-10] = temp(1,4);
	  y[i-10] = temp(2,4);
	  z[i-10] = temp(3,4);
	  for (int j = i-30; j >= 0; j--)
		if (sqrt((x[i-10]-x[j])*(x[i-10]-x[j])+(y[i-10]-y[j])*(y[i-10]-y[j])+(z[i-10]-z[j])*(z[i-10]-z[j])) < 20.0) {
		  delete [] x;
		  delete [] y;
		  delete [] z;
		  return 1;
		}
   }
  }

  delete [] x;
  delete [] y;
  delete [] z;
  return 0;
}

//write step parameters files, using v2
void pdna::printnew3dna(char *filename) {
  FILE *f = fopen(filename, "w");
  if (f == NULL) {
    printf("PDB output file %s could not be opened for writing\n", filename);
    return;
  }
  fprintf(f, "  %d base-pairs\n", nsteps+1);
  fprintf(f, "   0  step parameters\n");
  fprintf(f, "      shift   slide    rise    tilt    roll   twist\n");
  fprintf(f, "A-T    0.00    0.00    0.00    0.00    0.00    0.00\n");
  for (int i = 0; i < nsteps; i++) {
	matrix vi = v2[i];
    fprintf(f, "A-T   %8.3lf %8.3lf %8.3lf %8.3lf %8.3lf %8.3lf\n", vi(4,1), vi(5,1), vi(6,1), vi(1,1), vi(2,1), vi(3,1));
  }
  fclose(f);
}

//write step parameters and protein pdb files, using v2
void pdna::illustrate_config(char *dnafile, char *proteinfile) {
	printnew3dna(dnafile);
    int append = 0;
    for (int i = 0; i < nP; i++) {
	  matrix Wplace = identity(4);
	  matrix temp = identity(4);
	  char s2[40];
  	  sprintf(s2, "%s.pdb", pfilenames[pos[i].str]);
	  printf("readpdb=%s\n",s2);
	  int prot = pos[i].str;
      int place = pos[i].pos;
	  int rever = pos[i].dir;
	  printf("1good\n");
	  if (rever) place += proteins[prot].nsteps;
	  printf("2good\n");
		for (int xy = 0; xy < place; xy++) {
		printf("place=%d\n",place);
		printf("xy=%d\n",xy);
		printf("v2=%f %f %f\n",v2[xy](1,1),v2[xy](2,1),v2[xy](3,1));
		printf("v2=%f %f %f\n",v2[xy](4,1),v2[xy](5,1),v2[xy](6,1));
		printf("3good\n");
		calculateW(v2[xy]);
		printf("4good\n");
		Wplace = Wplace * calculateW(v2[xy]);
		printf("44good\n");
		}
		printf("5good\n");
			if (rever) {
			  Wplace.setv(1,2, -Wplace(1,2));
		      Wplace.setv(2,2, -Wplace(2,2));
              Wplace.setv(3,2, -Wplace(3,2));
              Wplace.setv(1,3, -Wplace(1,3));
              Wplace.setv(2,3, -Wplace(2,3));
              Wplace.setv(3,3, -Wplace(3,3));
		    }
			printf("6good\n");
		printf("7good\n");
		printf("writepdb=%s\n",proteinfile);
		rewrite_pdb(s2, proteinfile, identity(4), Wplace, append);
		append = 1;
    }
}

//protein-DNA overlap
int pdna::overlap(protein *locs, int nP) {//long int *locs
//DNA self-overlap
  FLOAT *x, *y, *z;
  x = new FLOAT[nsteps-15];
  y = new FLOAT[nsteps-15];
  z = new FLOAT[nsteps-15];
  matrix temp = identity(4);
  for (int i = 0; i < nsteps; i++) {
	temp = temp * calculateW(v[i]);
	if ((i >= 10) && (i < nsteps-10)) {
      x[i-10] = temp(1,4);
	  y[i-10] = temp(2,4);
	  z[i-10] = temp(3,4);
	  for (int j = i-30; j >= 0; j--)
		if (sqrt((x[i-10]-x[j])*(x[i-10]-x[j])+(y[i-10]-y[j])*(y[i-10]-y[j])+(z[i-10]-z[j])*(z[i-10]-z[j])) < 20.0) {
		  delete [] x;
		  delete [] y;
		  delete [] z;
		  return 1;
		}
    }
  }
//dealing with proteins
  if (nP == 0) {
	delete [] x;
	delete [] y;
	delete [] z;
	return 0;
  }

  int *pos = new int[nP];//positions of each protein on DNA chain
  int *rev = new int[nP];//reversed or not
  int *num = new int[nP];//index of the structure in all structure list
  matrix *Ws = new matrix[nP];//matrix for protein on its position on DNA chain
  for (int i = 0; i < nP; i++) {//i->ith protein on DNA chain
  //based on these three relations we can determine locs[i] for each protein
  //given its position, orientation,index and nstructs: locs=2*nstructs*pos+nstructs*rev+num
	/*
	pos[i] = locs[i]/(2*nstructs);
	rev[i] = locs[i]%(2*nstructs)/nstructs;
	num[i] = locs[i]%(2*nstructs)%nstructs;
	*/
	pos[i] = locs[i].pos;
	rev[i] = locs[i].dir;
	num[i] = locs[i].str;
		
	Ws[i] = identity(4);
    for (int q = 0; q < (rev[i] ? pos[i]+proteins[num[i]].nsteps : pos[i]); q++) Ws[i] = Ws[i]*calculateW(v[q]);
    if (rev[i]) {
	  Ws[i].setv(1,2, -Ws[i](1,2));
	  Ws[i].setv(2,2, -Ws[i](2,2));
	  Ws[i].setv(3,2, -Ws[i](3,2));
	  Ws[i].setv(1,3, -Ws[i](1,3));
	  Ws[i].setv(2,3, -Ws[i](2,3));
	  Ws[i].setv(3,3, -Ws[i](3,3));
    }
  }

  for (int i = 0; i < nP; i++) {
  //protein-protein overlap
    for (int j = i+1; j < nP; j++) {
    if (proteinoverlap(proteinPDBs[num[i]], proteinPDBs[num[j]], Ws[i], Ws[j])) { 
//	  printf("PROTEIN overlap!!\n");
//	  illustrate_config(locs, nP);
	  delete [] pos;
	  delete [] rev;
	  delete [] num;
	  delete [] Ws;
	  delete [] x;
	  delete [] y;
	  delete [] z;

	  return 1; 
	}
    }
	//pDNA overlap
    if (pDNAoverlap(proteinPDBs[num[i]], Ws[i], pos[i], proteins[num[i]].nsteps, this, x, y, z)) {
	  delete [] pos;
	  delete [] rev;
	  delete [] num;
	  delete [] Ws;
	  delete [] x;
	  delete [] y;
	  delete [] z;

	  return 1;
	}
  }


  delete [] x;
  delete [] y;
  delete [] z;

  delete [] pos;
  delete [] rev;
  delete [] num;
  delete [] Ws;
  return 0;
}

//old configuration <- new configuration
void pdna::accept() {
	for (int i = 0; i < nsteps; i++) v[i] = vchange[i];
	nP = nPchange;
	for (int i = 0; i < nP; i++) pos[i] = poschange[i];
}

//new configuration <- old configuration
void pdna::revert() {
	for (int i = 0; i < nsteps; i++) vchange[i] = v[i];
	nPchange = nP;
	for (int i = 0; i < nP; i++) poschange[i] = pos[i];
}

double pdna::omega(int N, int N2, int m) {
	double accum = 0.0;
	for (int i = 1; i <= nsteps-N*(m-1); i++) {
		accum += log((double)i);
	}
	for (int i = 1; i <= nsteps-N2*(m-1); i++) {
		accum -= log((double)i);
	}
	for (int i = 1; i <= nsteps-N*m; i++) {
		accum -= log((double)i);
	}
	for (int i = 1; i <= nsteps-N2*m; i++) {
		accum += log((double)i);
	}
	for (int i = 1; i <= N; i++) {
		accum -= log((double)i);
	}
	for (int i = 1; i <= N2; i++) {
		accum += log((double)i);
	}
	return accum;
}

//change vchange, nPchange, poschange
double pdna::move(double b) {

if ((ntypes == 0) || (drand48() < 0.5)) {
		double piv;
  		if (drand48() < 0.8) {
		  int N = (int)(nsteps*drand48())+1;
		  int pos = (nsteps-N)*drand48();
		for (int j = pos; j < pos+N; j++) {
   	    	piv = PIV*drand48()/sqrt((double)N);
			vchange[j].setv(1,1, vchange[j](1,1)+2.0*piv*(drand48() - 0.5));
			vchange[j].setv(2,1, vchange[j](2,1)+2.0*piv*(drand48() - 0.5));
			vchange[j].setv(3,1, vchange[j](3,1)+1.0*piv*(drand48() - 0.5));
			vchange[j].setv(4,1, vchange[j](4,1)+0.05*(drand48() - 0.5));
			vchange[j].setv(5,1, vchange[j](5,1)+0.05*(drand48() - 0.5));
			vchange[j].setv(6,1, vchange[j](6,1)+0.05*(drand48() - 0.5));
  		}
		} else {
			piv = PIV*drand48();
			int which = (int)(nsteps*drand48());
			vchange[which].setv(1,1, vchange[which](1,1)+2.0*piv*(drand48() - 0.5));
			vchange[which].setv(2,1, vchange[which](2,1)+2.0*piv*(drand48() - 0.5));
			vchange[which].setv(3,1, vchange[which](3,1)+1.0*piv*(drand48() - 0.5));
		
		}
		if (drand48() < 0.2) {
			matrix temp;
			int nshuf = nsteps*drand48();
			int shuf1, shuf2;
			for (int j = 0; j < nshuf; j++) {
				shuf1 = nsteps*drand48();
				shuf2 = nsteps*drand48();
				temp = vchange[shuf1];
				int p = nsteps*drand48();
				vchange[shuf1] = vchange[shuf2];
				vchange[shuf2] = temp;
			}
		}

} else {
	if ((drand48() < 0.5) || (nPchange == 0)) {
			int index = (int)(drand48() * nsteps);
			for (int i = 0; i < nPchange; i++) {
				if (poschange[i].pos == index) {
					if (i != nPchange-1) poschange[i] = poschange[nPchange-1];
					nPchange--;
					return 0.0;
				}
			}
			poschange[nPchange].type = (int)(drand48() * ntypes);
			poschange[nPchange].pos = (int)(drand48() * nsteps);
			poschange[nPchange].str = (int)(nproteins[poschange[nPchange].type]*drand48())+pstartindex[poschange[nPchange].type];
			poschange[nPchange].dir = (int)(2.0 * drand48());
			nPchange++;
	} else {
		int index = drand48() * nPchange;
		int step = 50.0*drand48();
		if (drand48() < 0.5) {
			if ((poschange[index].pos + step) < nsteps) poschange[index].pos += step;
		} else if ((poschange[index].pos - step) >= 0) poschange[index].pos -= step;
		if (drand48() < 0.5) {
			if (poschange[index].dir == 0) poschange[index].dir = 1;
			else poschange[index].dir = 0;
		}
	}
}
return 0.0;
}

//r:end to end distance; L:DNA chain length
//2400r/L+nPchange?
int pdna::newEEp() {
	double L = nsteps*3.4;
	matrix W = identity(4);
	for (int i = 0; i < nsteps; i++) W = W * calculateW(vchange2[i]);
	return 60*(int)(40.0 * (sqrt(W(1,4)*W(1,4)+W(2,4)*W(2,4)+W(3,4)*W(3,4)) / L)) + nPchange;	
}
//2400r/L+nP
int pdna::oldEEp() {
	double L = nsteps*3.4;
	matrix W = identity(4);
	for (int i = 0; i < nsteps; i++) W = W * calculateW(v2[i]);
	return 60*(int)(40.0 * (sqrt(W(1,4)*W(1,4)+W(2,4)*W(2,4)+W(3,4)*W(3,4)) / L)) + nP;
}
//100r/L
int pdna::newEE() {
	double L = nsteps*3.4;
	matrix W = identity(4);
	for (int i = 0; i < nsteps; i++) W = W * calculateW(vchange2[i]);
	return (int)(100.0 * (sqrt(W(1,4)*W(1,4)+W(2,4)*W(2,4)+W(3,4)*W(3,4)) / L));	
}
//100r/L
int pdna::oldEE() {
	double L = nsteps*3.4;
	matrix W = identity(4);
	for (int i = 0; i < nsteps; i++) W = W * calculateW(v2[i]);
	return (int)(100.0 * (sqrt(W(1,4)*W(1,4)+W(2,4)*W(2,4)+W(3,4)*W(3,4)) / L));
}
//risize 100r/L
int pdna::oldEEbin() {
        double L = nsteps*3.4;
        matrix W = identity(4);
        for (int i = 0; i < nsteps; i++) W = W * calculateW(v2[i]);
        double v = 100.0 * sqrt(W(1,4)*W(1,4)+W(2,4)*W(2,4)+W(3,4)*W(3,4)) / L;       
        if (v < 60.0) return (int)v;
        if (v < 80.0) return 60+(int)(4.0*(v-60.0));
        if (v < 90.0) return 140+(int)(16.0*(v-80.0));
        return 300+(int)(32.0*(v-90.0));
}
//risize 100r/L
int pdna::newEEbin() {
        double L = nsteps*3.4;
        matrix W = identity(4);
        for (int i = 0; i < nsteps; i++) W = W * calculateW(vchange2[i]);
        double v = 100.0 * sqrt(W(1,4)*W(1,4)+W(2,4)*W(2,4)+W(3,4)*W(3,4)) / L;     
        if (v < 60.0) return (int)v;
        if (v < 80.0) return 60+(int)(4.0*(v-60.0));
        if (v < 90.0) return 140+(int)(16.0*(v-80.0));
        return 300+(int)(32.0*(v-90.0));
}
//200r/L
double pdna::newEEfull() {
	double L = nsteps*3.4;
	matrix W = identity(4);
	for (int i = 0; i < nsteps; i++) W = W * calculateW(vchange2[i]);
	return 200.0*sqrt(W(1,4)*W(1,4)+W(2,4)*W(2,4)+W(3,4)*W(3,4))/L;
//	return (200.0*(exp(2.0*sqrt(W(1,4)*W(1,4)+W(2,4)*W(2,4)+W(3,4)*W(3,4))/L)-1.0)/(exp(2.0)-1.0));
}
//200r/L
double pdna::oldEEfull() {
	double L = nsteps*3.4;
	matrix W = identity(4);
	for (int i = 0; i < nsteps; i++) W = W * calculateW(v2[i]);
	return 200.0*sqrt(W(1,4)*W(1,4)+W(2,4)*W(2,4)+W(3,4)*W(3,4))/L;
//	return (200.0*(exp(2.0*sqrt(W(1,4)*W(1,4)+W(2,4)*W(2,4)+W(3,4)*W(3,4))/L)-1.0)/(exp(2.0)-1.0));
}
//r>0.5L:1050+100r/L;	r<0.5L:constrained on all 3 end conditions
int pdna::newEEt() {
	double L = nsteps*3.4;
	matrix W = identity(4);
	for (int i = 0; i < nsteps; i++) W = W * calculateW(vchange2[i]);
	double twist = fabs(calculatetp(W)(3,1) * M_PI / 180.0);
	double r = sqrt(W(1,4)*W(1,4)+W(2,4)*W(2,4)+W(3,4)*W(3,4))/L;
	if (r > 0.5) return 2000 + (int)(100.0*(r-0.5));
	return (int)(10.0*(acos(W(3,3))/M_PI))+10*(int)(10.0 * twist/M_PI) + 100*(int)(40.0*r);	
}
//r>0.5L:1050+100r/L;	r<0.5L:constrained on all 3 end conditions
int pdna::oldEEt() {
	double L = nsteps*3.4;
	matrix W = identity(4);
	for (int i = 0; i < nsteps; i++) W = W * calculateW(v2[i]);
	double r = sqrt(W(1,4)*W(1,4)+W(2,4)*W(2,4)+W(3,4)*W(3,4))/L;
	double twist = fabs(calculatetp(W)(3,1) * M_PI / 180.0);
	if (r > 0.5) return 2000 + (int)(100.0*(r-0.5));   // smaller bins for end-to-end distance
	return (int)(10.0*(acos(W(3,3))/M_PI))+10*(int)(10.0 * twist/M_PI) + 100*(int)(40.0*r);
}
//Lk
double pdna::oldsigma() {
	double Lk_0 = nsteps/10.5;

	for (int i = 0; i < nsteps; i++) q->v[i] = v2[i];
	return (q->calculate_link_open() - Lk_0)/Lk_0;
}
//Lk
double pdna::newsigma() {
	double Lk_0 = nsteps/10.5;
	for (int i = 0; i < nsteps; i++) q->v[i] = vchange2[i];
	return (q->calculate_link_open() - Lk_0)/Lk_0;
}


double pdna::twist() {
	matrix W = identity(4);
	for (int i = 0; i < nsteps; i++) W = W * calculateW(v[i]);
	return fabs(calculatetp(W)(3,1));
}
//entropy difference between old and new configuration? 
double pdna::chainS(int x1, int x2) {
  FLOAT E = 0.0;
  FLOAT E2 = 0.0;
  matrix tp;
  matrix temp;

  for (int i = x1; i < x2; i++) {

    tp = vchange[i] - I[bpstep[i]];

    temp = F[bpstep[i]]*tp;

    for (int j = 1; j <= 2; j++) E += temp(j,1)*tp(j,1);

	tp = v[i] - I[bpstep[i]];
    temp = F[bpstep[i]]*tp;

    for (int j = 1; j <= 2; j++) E2 += temp(j,1)*tp(j,1);
	

  }

  E *= 0.5;
  E2 *= 0.5;

  return 0.5*(2.0*(x2-x1)-1.0)*(log(E2)-log(E));

}

//elastic energy difference between new and old configuration, change elOld and elNew
//elOld=old elastic energy of twist part? elNew=new elastic energy
double pdna::dE(double &elOld, double &elNew) {
  FLOAT kap = 0.329*sqrt(ion);  
  FLOAT E = 0.0, Eold = 0.0;

  elNew = 0;
  elOld = 0;
  matrix tp;
  matrix temp;
  matrix W;
//elastic energy of new configuration
  for (int i = 0; i < nsteps; i++) {
    tp = vchange[i] - I[bpstep[i]];
    temp = F[bpstep[i]]*tp;
    for (int j = 1; j <= 6; j++) {
	  E += temp(j,1)*tp(j,1);
	if (j == 3) elNew += temp(j,1)*tp(j,1);//twist
    }
  }
  E *= 0.5;
  elNew = E;

//elastic energy of old configuration 
  for (int i = 0; i < nsteps; i++) {
    tp = v[i] - I[bpstep[i]];
    temp = F[bpstep[i]]*tp;
    for (int j = 1; j <= 6; j++) {
	  Eold += temp(j,1)*tp(j,1);
	  if (j == 3) elOld += temp(j,1)*tp(j,1);
    }

	v2[i] = v[i];
	vchange2[i] = vchange[i];

  }
  Eold *= 0.5;

//naked DNA
  if (ntypes == 0){
  //ES energy of new configuration
	E+=DNA_esenergy(W, vchange);  
  //ES energy of old configuration
	Eold+=DNA_esenergy(W, v);    
	return E - Eold;  
  }

//with proteins
  E+=calculate_esenergy(poschange, nPchange, vchange);
  Eold+=calculate_esenergy(pos, nP, v);
  
//Pp2 McGhee von Hippel cooperativity constant omega energy, adding to E
  for (int i = 0; i < nP; i++) {
	E -= Pp[pos[i].type];
	for (int j = i+1; j < nP; j++) {
		if (pos[i].pos < pos[j].pos) {
			if (pos[i].pos + psteps[pos[i].type] == pos[j].pos) E -= Pp2[pos[i].type];
		} else {
			if (pos[j].pos + psteps[pos[j].type] == pos[i].pos) E -= Pp2[pos[j].type];
		}
	}
	for (int j = 0; j < psteps[pos[i].type]; j++) {
		if (pos[i].dir == 0) v2[pos[i].pos + j] = proteins[pos[i].str].v[j];
		if (pos[i].dir == 1) {
			matrix temp = proteins[pos[i].str].v[psteps[pos[i].type] - 1 - j];
			v2[pos[i].pos + j] = temp;
			v2[pos[i].pos + j].setv(1,1, -temp(1,1));
			v2[pos[i].pos + j].setv(4,1, -temp(4,1));			
		}
	}
  }
  for (int i = 0; i < nPchange; i++) {
	E += Pp[poschange[i].type];
	if (poschange[i].pos + psteps[poschange[i].type] > nsteps) return 1e20;
	for (int j = i+1; j < nPchange; j++) {
		if (poschange[i].pos < poschange[j].pos) {
			if (poschange[i].pos + psteps[poschange[i].type] > poschange[j].pos) return 1e20;
			if (poschange[i].pos + psteps[poschange[i].type] == poschange[j].pos) E += Pp2[poschange[i].type];
		}
		if (poschange[i].pos >= poschange[j].pos) {
			if (poschange[j].pos + psteps[poschange[j].type] > poschange[i].pos) return 1e20;
			if (poschange[j].pos + psteps[poschange[j].type] == poschange[i].pos) E += Pp2[poschange[j].type];
		}
	}
	for (int j = 0; j < psteps[poschange[i].type]; j++) {
		if (poschange[i].dir == 0) vchange2[poschange[i].pos + j] = proteins[poschange[i].str].v[j];
		if (poschange[i].dir == 1) {
			matrix temp = proteins[poschange[i].str].v[psteps[poschange[i].type] - 1 - j];
			vchange2[poschange[i].pos + j] = proteins[poschange[i].str].v[psteps[poschange[i].type] - 1 - j];
			vchange2[poschange[i].pos + j].setv(1,1, -temp(1,1));
			vchange2[poschange[i].pos + j].setv(4,1, -temp(4,1));			
		}
	}
  }

  matrix t1 = identity(4);
  matrix t2 = identity(4);  
for (int i = 0; i < nsteps; i++) {
	t1 = t1*calculateW(v2[i]);
	t2 = t2*calculateW(vchange2[i]);
}
double d1 = sqrt(t1(1,4)*t1(1,4)+t1(2,4)*t1(2,4)+t1(3,4)*t1(3,4));//end to end distance of old configuration
double d2 = sqrt(t2(1,4)*t2(1,4)+t2(2,4)*t2(2,4)+t2(3,4)*t2(3,4));//end to end distance of new configuration

return E - Eold; // - d2/41.4 + d1/41.4;
}

//protein position valid or not
int pdna::isvalid() {
	for (int i = 0; i <= nPchange; i++) {
	if (poschange[i].pos + psteps[poschange[i].type] > nsteps) return 0;//protein exceed DNA chain
	for (int j = i+1; j <= nPchange; j++) {//protein position overlap
		if (poschange[i].pos < poschange[j].pos) {
			if (poschange[i].pos + psteps[poschange[i].type] > poschange[j].pos) return 0;
		}
		if (poschange[i].pos >= poschange[j].pos) {
			if (poschange[j].pos + psteps[poschange[j].type] > poschange[i].pos) return 0;
		}
	}
	}
	return 1;
}
//print struct protein information
void pdna::printP() {
	for (int i = 0; i < nP; i++) {
		printf("%d (%d %d %d) ", pos[i].pos, pos[i].type, pos[i].str, pos[i].dir);
	}
	printf("\n");
}

//calculate ES energy between two proteins
FLOAT pdna::p_esenergy(proteinPDB &p, proteinPDB &q, matrix Wp, matrix Wq) {
  FLOAT E = 0.0;
  FLOAT kap = 0.329*sqrt(ion);
  FLOAT r,x,y,z;
  FLOAT q1, q2;

  //translate the positions of the two proteins
  FLOAT *pex = new FLOAT[p.neatoms];
  FLOAT *pey = new FLOAT[p.neatoms];
  FLOAT *pez = new FLOAT[p.neatoms];
  FLOAT *qex = new FLOAT[q.neatoms];
  FLOAT *qey = new FLOAT[q.neatoms];
  FLOAT *qez = new FLOAT[q.neatoms];
 
 for (int i = 0; i < p.neatoms; i++) {
	pex[i] = Wp(1,4)+p.ex[i]*Wp(1,1)+p.ey[i]*Wp(1,2)+p.ez[i]*Wp(1,3);
	pey[i] = Wp(2,4)+p.ex[i]*Wp(2,1)+p.ey[i]*Wp(2,2)+p.ez[i]*Wp(2,3);
	pez[i] = Wp(3,4)+p.ex[i]*Wp(3,1)+p.ey[i]*Wp(3,2)+p.ez[i]*Wp(3,3);
  }

 for (int i = 0; i < q.neatoms; i++) {
	qex[i] = Wq(1,4)+q.ex[i]*Wq(1,1)+q.ey[i]*Wq(1,2)+q.ez[i]*Wq(1,3);
	qey[i] = Wq(2,4)+q.ex[i]*Wq(2,1)+q.ey[i]*Wq(2,2)+q.ez[i]*Wq(2,3);
	qez[i] = Wq(3,4)+q.ex[i]*Wq(3,1)+q.ey[i]*Wq(3,2)+q.ez[i]*Wq(3,3);
  }
 
  for (int i = 0; i < p.neatoms; i++) {
	q1 = p.e[i];
	for (int j = 0; j < q.neatoms; j++) {
	  x = pex[i]-qex[j];
	  y = pey[i]-qey[j];
	  z = pez[i]-qez[j];
	  r = sqrt(x*x+y*y+z*z);
	  q2 = q.e[j];
	  E += q1*q2*exp(-kap*r)/r;
    }
  }
  E *= coeff;

  return E;
  delete [] pex; delete [] pey; delete [] pez; delete [] qex; delete [] qey; delete [] qez;  

}

//calculate ES energy between pDNA
FLOAT pdna::pDNA_esenergy(proteinPDB &p, matrix Wp,matrix *V) {// bdna *b, 
  FLOAT E = 0.0;
  FLOAT kap = 0.329*sqrt(ion);
  FLOAT r,x,y,z;
  FLOAT q1, q2;
  matrix W1 = identity(4);
  //translate the positions of the protein
  FLOAT *pex = new FLOAT[p.neatoms];
  FLOAT *pey = new FLOAT[p.neatoms];
  FLOAT *pez = new FLOAT[p.neatoms];
 for (int i = 0; i < p.neatoms; i++) {
	pex[i] = Wp(1,4)+p.ex[i]*Wp(1,1)+p.ey[i]*Wp(1,2)+p.ez[i]*Wp(1,3);
	pey[i] = Wp(2,4)+p.ex[i]*Wp(2,1)+p.ey[i]*Wp(2,2)+p.ez[i]*Wp(2,3);
	pez[i] = Wp(3,4)+p.ex[i]*Wp(3,1)+p.ey[i]*Wp(3,2)+p.ez[i]*Wp(3,3);
  }
  for (int i = 0; i < nsteps-1; i++) {//i < b->nsteps-1
 
    FLOAT X1 = W1(1,4);
    FLOAT Y1 = W1(2,4);
    FLOAT Z1 = W1(3,4);

    q1 = -0.25; //DNA screened charge

    W1 = W1 * calculateW(V[i]);
	for (int j = 0; j < p.neatoms; j++) {
      x = X1-pex[j];
      y = Y1-pey[j];
      z = Z1-pez[j]; 
	  q2 = p.e[j];
      r = sqrt(x*x+y*y+z*z);
      E += q1*q2*exp(-kap*r)/r;
    }
  }
  E *= coeff;

  return E;
  delete [] pex; delete [] pey; delete [] pez;

}

//total ES energy, adpated from pdna::overlap()
FLOAT pdna::calculate_esenergy(protein *locs, int nP, matrix *V) {// bdna *b,

  FLOAT E = 0.0;
  FLOAT kap = 0.329*sqrt(ion);
 
  matrix W;    
  E += DNA_esenergy(W,V);//b->DNA_esenergy(W,V);

  if (nP == 0) return E;
	
  int *pos = new int[nP];//positions of each protein on DNA chain
  int *rev = new int[nP];//reversed or not
  int *num = new int[nP];//index of the structure in all structure list
  matrix *Ws = new matrix[nP];//matrix for protein on its position on DNA chain
  for (int i = 0; i < nP; i++) {//i->ith protein on DNA chain
  //based on these three relations we can determine locs[i] for each protein
  //given its position, orientation,index and nstructs: locs=2*nstructs*pos+nstructs*rev+num
	pos[i] = locs[i].pos;
	rev[i] = locs[i].dir;
	num[i] = locs[i].str;
	Ws[i] = identity(4); 
    for (int q = 0; q < pos[i]; q++) Ws[i] = Ws[i]*calculateW(V[q]);
    if (rev[i]) {
	  Ws[i].setv(1,2, -Ws[i](1,2));
	  Ws[i].setv(2,2, -Ws[i](2,2));
	  Ws[i].setv(3,2, -Ws[i](3,2));
	  Ws[i].setv(1,3, -Ws[i](1,3));
	  Ws[i].setv(2,3, -Ws[i](2,3));
	  Ws[i].setv(3,3, -Ws[i](3,3));
    }
  }

  for (int i = 0; i < nP; i++) {
    for (int j = i+1; j < nP; j++) E += p_esenergy(proteinPDBs[num[i]], proteinPDBs[num[j]], Ws[i], Ws[j]);
	E+=pDNA_esenergy(proteinPDBs[num[i]],Ws[i],V);
  }

  delete [] pos;
  delete [] rev;
  delete [] num;
  delete [] Ws;
  return E;
}

void pdna::print_energy() {
  printf("Total Energy = %lf kT\n", calculate_elenergy()+calculate_penergy());
  printf("Elastic Energy = %lf kT,  Contact (r-12) Energy = %lf kT\n",
	 calculate_elenergy(), calculate_penergy());
#ifdef ELECTROSTATICS
  matrix W;
  printf("Electrostatic Energy = %lf kT\n", calculate_esenergy(W));
#endif
}

FLOAT pdna::calculate_energy(matrix &W) {
  FLOAT eng = calculate_elenergy();
#ifdef ELECTROSTATICS
  eng += calculate_esenergy(W);
#endif
#ifdef SELF_CONTACT
  eng += calculate_penergy();
#endif
#ifndef ELECTROSTATICS
  W = identity(4);
  for (int i = 0 ; i < nsteps; i++)
    W = W * calculateW(v[i]);
#endif
  return eng;
}