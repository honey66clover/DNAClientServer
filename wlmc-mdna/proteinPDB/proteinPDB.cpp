#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "proteinPDB.h"
#include "../bdna/bdna.h"


int pDNAoverlap(proteinPDB p, matrix Wp, int pos, int plen, FLOAT *x, FLOAT *y, FLOAT *z, int x1, int x2) {

  FLOAT *px = new FLOAT[p.natoms];
  FLOAT *py = new FLOAT[p.natoms];
  FLOAT *pz = new FLOAT[p.natoms];


 for (int i = 0; i < p.natoms; i++) {
	px[i] = Wp(1,4)+p.x[i]*Wp(1,1)+p.y[i]*Wp(1,2)+p.z[i]*Wp(1,3);
	py[i] = Wp(2,4)+p.x[i]*Wp(2,1)+p.y[i]*Wp(2,2)+p.z[i]*Wp(2,3);
	pz[i] = Wp(3,4)+p.x[i]*Wp(3,1)+p.y[i]*Wp(3,2)+p.z[i]*Wp(3,3);
  }

  for (int i = 0; i < p.natoms; i++) for (int j = x1; j < x2; j++) {
	if ((abs(pos+plen/2-j) > (plen/2+10)) && ((px[i]-x[j-x1])*(px[i]-x[j-x1])+(py[i]-y[j-x1])*(py[i]-y[j-x1])+(pz[i]-z[j-x1])*(pz[i]-z[j-x1]) < 16.0)) {
	  delete [] px;
	  delete [] py;
	  delete [] pz;
	  return 1;
	}
  }
  delete [] px;
  delete [] py;
  delete [] pz;

  return 0;
}



int pDNAoverlap(proteinPDB p, matrix Wp, int pos, int plen, bdna *b, FLOAT *x, FLOAT *y, FLOAT *z) {

  FLOAT *px = new FLOAT[p.natoms];
  FLOAT *py = new FLOAT[p.natoms];
  FLOAT *pz = new FLOAT[p.natoms];


 for (int i = 0; i < p.natoms; i++) {
	px[i] = Wp(1,4)+p.x[i]*Wp(1,1)+p.y[i]*Wp(1,2)+p.z[i]*Wp(1,3);
	py[i] = Wp(2,4)+p.x[i]*Wp(2,1)+p.y[i]*Wp(2,2)+p.z[i]*Wp(2,3);
	pz[i] = Wp(3,4)+p.x[i]*Wp(3,1)+p.y[i]*Wp(3,2)+p.z[i]*Wp(3,3);
  }

  for (int i = 0; i < p.natoms; i++) for (int j = 10; j < b->nsteps-10; j++) {
	if ((abs(pos+plen/2-j) > (plen/2+10)) && ((px[i]-x[j-10])*(px[i]-x[j-10])+(py[i]-y[j-10])*(py[i]-y[j-10])+(pz[i]-z[j-10])*(pz[i]-z[j-10]) < 16.0)) {
	  delete [] px;
	  delete [] py;
	  delete [] pz;
	  return 1;
	}
  }
  delete [] px;
  delete [] py;
  delete [] pz;

  return 0;
}

int proteinoverlap(proteinPDB p, proteinPDB q, matrix Wp, matrix Wq) {
  FLOAT *px = new FLOAT[p.natoms];
  FLOAT *py = new FLOAT[p.natoms];
  FLOAT *pz = new FLOAT[p.natoms];
  FLOAT *qx = new FLOAT[q.natoms];
  FLOAT *qy = new FLOAT[q.natoms];
  FLOAT *qz = new FLOAT[q.natoms];

 for (int i = 0; i < p.natoms; i++) {
	px[i] = Wp(1,4)+p.x[i]*Wp(1,1)+p.y[i]*Wp(1,2)+p.z[i]*Wp(1,3);
	py[i] = Wp(2,4)+p.x[i]*Wp(2,1)+p.y[i]*Wp(2,2)+p.z[i]*Wp(2,3);
	pz[i] = Wp(3,4)+p.x[i]*Wp(3,1)+p.y[i]*Wp(3,2)+p.z[i]*Wp(3,3);
  }

 for (int i = 0; i < q.natoms; i++) {
	qx[i] = Wq(1,4)+q.x[i]*Wq(1,1)+q.y[i]*Wq(1,2)+q.z[i]*Wq(1,3);
	qy[i] = Wq(2,4)+q.x[i]*Wq(2,1)+q.y[i]*Wq(2,2)+q.z[i]*Wq(2,3);
	qz[i] = Wq(3,4)+q.x[i]*Wq(3,1)+q.y[i]*Wq(3,2)+q.z[i]*Wq(3,3);
  }

  for (int i = 0; i < p.natoms; i++) {
	for (int j = 0; j < q.natoms; j++) {
	  if ((px[i]-qx[j])*(px[i]-qx[j])+(py[i]-qy[j])*(py[i]-qy[j])+(pz[i]-qz[j])*(pz[i]-qz[j]) < 4.0) {
		delete [] px; delete [] py; delete [] pz; delete [] qx; delete [] qy; delete [] qz;
		return 1;
	  }
    }
  }

  delete [] px; delete [] py; delete [] pz; delete [] qx; delete [] qy; delete [] qz;

  return 0;
}

proteinPDB *translateprotein(proteinPDB *p, matrix W) {
  proteinPDB *q = new proteinPDB(*p);
 //for all atoms 
  for (int i = 0; i < p->natoms; i++) {
	q->x[i] = W(1,4)+p->x[i]*W(1,1)+p->y[i]*W(1,2)+p->z[i]*W(1,3);
	q->y[i] = W(2,4)+p->x[i]*W(2,1)+p->y[i]*W(2,2)+p->z[i]*W(2,3);
	q->z[i] = W(3,4)+p->x[i]*W(3,1)+p->y[i]*W(3,2)+p->z[i]*W(3,3);
  }
 //for charged atoms
  for (int i = 0; i < p->neatoms; i++) {
	q->ex[i] = W(1,4)+p->ex[i]*W(1,1)+p->ey[i]*W(1,2)+p->ez[i]*W(1,3);
	q->ey[i] = W(2,4)+p->ex[i]*W(2,1)+p->ey[i]*W(2,2)+p->ez[i]*W(2,3);
	q->ez[i] = W(3,4)+p->ex[i]*W(3,1)+p->ey[i]*W(3,2)+p->ez[i]*W(3,3);
  }

  return q;
}

proteinPDB::~proteinPDB() {
  if (natoms != 0) {
	delete [] x;
	delete [] y;
	delete [] z;
  }
}

proteinPDB::proteinPDB() {
  natoms = 0;
}

proteinPDB::proteinPDB(char *fname) {
    natoms = 0;
    float *xx = new float[50000]; float *xy = new float[50000]; float *xz = new float[50000];
    char atom[8];
    char anum[7];
    char at[7];
    char res[6];
    char chain[5]; 
    char resn[6];
    char none[6];
    FILE *f = fopen(fname, "r");
    char s[257];
    char *p;
    do {
      p = fgets(s, 256, f);
      strcpy(atom, "      ");
      strcpy(chain, "  ");
      strcpy(res, "    ");
      strcpy(resn, "    ");
      strcpy(at, "     ");
      strcpy(anum, "     ");
      strcpy(none, "    ");
      sscanf(s, "%6c%5c%5c%4c%2c%4c%4c%8f%8f%8f", atom, anum, at, res, chain, resn, none, &xx[natoms], &xy[natoms], &xz[natoms]);
      if (strcmp(atom, "ATOM  ") == 0) {
		
	    natoms++;
     // printf("%f %f %f", x, y, z);
//      fprintf(f2, "%6s%5s%5s%4s%2s%4s%4s%8.3f%8.3f%8.3f  1.00 15.00\n", atom, anum, at, res, chain, resn, none, xn, yn, zn);
      }
      
    } while ((strcmp(atom, "ATOM  ") == 0) && (p != NULL));
    fclose(f);

	x = new FLOAT[natoms]; y = new FLOAT[natoms]; z = new FLOAT[natoms];
	for (int i = 0; i < natoms; i++) {
	  x[i] = (FLOAT)xx[i];
	  y[i] = (FLOAT)xy[i];
	  z[i] = (FLOAT)xz[i];
	}

	delete [] xx;
	delete [] xy;
	delete [] xz;

}

int proteinPDB::readPDB(char *fname) {
    natoms = 0;
	neatoms=0;
	//temporary store positions of all atoms 
    float *xx = new float[50000]; float *xy = new float[50000]; float *xz = new float[50000];
	//temporary store positions of charged atoms
	float *xex = new float[50000]; float *xey = new float[50000]; float *xez = new float[50000];
	//temporary store charges
	float *xe = new float[50000]; 
    char atom[8];
    char anum[7];
    char at[7];
    char res[6];
    char chain[5]; 
    char resn[6];
    char none[6];
    FILE *f = fopen(fname, "r");
	if (f == NULL) return 0;
    char s[257];
    char *p;
    do {
      p = fgets(s, 256, f);
      strcpy(atom, "      ");
      strcpy(chain, "  ");
      strcpy(res, "    ");
      strcpy(resn, "    ");
      strcpy(at, "     ");
      strcpy(anum, "     ");
      strcpy(none, "    ");
      sscanf(s, "%6c%5c%5c%4c%2c%4c%4c%8f%8f%8f", atom, anum, at, res, chain, resn, none, &xx[natoms], &xy[natoms], &xz[natoms]);
      if (strcmp(atom, "ATOM  ") == 0) {
		
	    natoms++;
     // printf("%f %f %f", x, y, z);
//      fprintf(f2, "%6s%5s%5s%4s%2s%4s%4s%8.3f%8.3f%8.3f  1.00 15.00\n", atom, anum, at, res, chain, resn, none, xn, yn, zn);
      }
	  //positive atoms
	  if ((strcmp(at, "  NZ ") == 0 && strcmp(res, " LYS") == 0) ||(strcmp(at, "  CZ ") == 0 && strcmp(res, " ARG") == 0)) {
		neatoms++;
		xex[neatoms]=xx[natoms]; xey[neatoms]=xy[natoms]; xez[neatoms]=xz[natoms]; xe[neatoms]=1.0; 
		}
	  if ((strcmp(at, "  CG ") == 0 && strcmp(res, " ASP") == 0) ||(strcmp(at, "  CD ") == 0 && strcmp(res, " GLU") == 0)) {
		neatoms++;
		xex[neatoms]=xx[natoms]; xey[neatoms]=xy[natoms]; xez[neatoms]=xz[natoms]; xe[neatoms]=-1.0; 
		}
	
    } while ((strcmp(atom, "ATOM  ") == 0) && (p != NULL));
    fclose(f);

	x = new FLOAT[natoms]; y = new FLOAT[natoms]; z = new FLOAT[natoms];
	ex = new FLOAT[neatoms]; ey = new FLOAT[neatoms]; ez = new FLOAT[neatoms]; e = new FLOAT[neatoms];
	for (int i = 0; i < natoms; i++) {
	  x[i] = (FLOAT)xx[i];
	  y[i] = (FLOAT)xy[i];
	  z[i] = (FLOAT)xz[i];
	}
	for (int i = 0; i < neatoms; i++) {
	  ex[i] = (FLOAT)xex[i];
	  ey[i] = (FLOAT)xey[i];
	  ez[i] = (FLOAT)xez[i];
	  e[i] = (FLOAT)xe[i]; 
	}	

	delete [] xx;
	delete [] xy;
	delete [] xz;

	delete [] xex;
	delete [] xey;
	delete [] xez;
	delete [] xe;
	
	return 1;

}

proteinPDB::proteinPDB(const proteinPDB &p) {
	natoms = p.natoms;
	x = new FLOAT[natoms]; y = new FLOAT[natoms]; z = new FLOAT[natoms];
	for (int i = 0; i < p.natoms; i++) {
	  x[i] = p.x[i];
	  y[i] = p.y[i];
	  z[i] = p.z[i];
    }
}

