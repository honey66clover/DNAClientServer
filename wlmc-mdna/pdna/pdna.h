#ifndef _PDNA_H
#define _PDNA_H

#include "../bdna/bdna.h"
#include "../proteinPDB/proteinPDB.h"

FLOAT randg();

//collection of proteins on DNA
struct protein {
	int pos;//position on DNA
	int type;//index of type
	int str;//index of structure in all structure list
	int dir;//direction
};

class pdna : public bdna {

  matrix *z;
  matrix *lambda;

  proteinPDB *proteinPDBs;//for all different structures of all types of proteins
  bdna *proteins;//stores nsteps and v for all different structures of all types of proteins 

  matrix *factor;

  int ntypes; //# of types of proteins, types could be HU, Hbb...
  int nstructs; //# of all structures of all types, structures of HU are HU-1,HU-2,HU-3,HU-4
  int *nproteins;//# of structures of each types
  int *structtype;//index of structures within each type
  char pfilenames[100][20];//filenames of each structure of proteins in all structure list, such as HU-1,...HU-4;Hbb...
  char pnames[100][20]; //names of each type of proteins, such as HU, Hbb...
  int *psteps;//# of steps bound on DNA, 14 for HU
  int *pstartindex;//index of type, pstartindex[0]=0, pstartindex[1]=4, where the types are HU, Hbb...
  FLOAT *Pp;
FLOAT *Pp2;
  matrix *vchange;

  matrix *v2; //value set in bdna::dE(), used in various ending conditions 
  matrix *vchange2;
bdna *q;
  protein *pos; //pos[i]:ith protein on DNA chain
  protein *poschange;
  int nP, nPchange;

public:

  	pdna();
  	~pdna();


  	void printeigenvalues();//print lambda, F, z matrices on screen

  	void initialize_params(double b);

  	void generate_config();
  	void convert_steps();

  	void set_protein(int n, FLOAT x);
  	void init_proteins();

  	double move(double beta);

	void printnew3dna(char *filename);//write step parameters files, using v2

 	//put step parameters from protein into v[i] 
	//randomly chose a structure from a type of protein ant put it from step a on DNA b 	
	void place_protein(bdna *b, short int a, short int type, char reverse); 

  	void illustrate_config(char *f1, char *f2);//write step parameters and protein pdb files, using v2
                                              //calling printnew3dna
  	int overlap(); //DNA self-overlap
  	int overlap(protein *locs, int nP); //protein-protein, pDNA overlap

	void revert();
	void accept();
	int isvalid();
	double omega(int N, int N2, int m);

	inline int Pnum() { return nP; }
	inline int Pnumnew() { return nPchange; }
	void printP();
	
	int oldEEp();
	int newEEp();
        int oldEEbin();
        int newEEbin();
	double oldsigma();
	double newsigma();
	double oldEEfull();
	double newEEfull();

	
	int oldEEt();
	int newEEt();
	double chainS(int x1, int x2);
	double twist();
	int newEE();
	int oldEE();
	
	double dE(double &elOld, double &elNew);

  	FLOAT *Pproteins;
	
	FLOAT calculate_esenergy(protein *locs, int nP, matrix *V);
	FLOAT p_esenergy(proteinPDB &p, proteinPDB &q, matrix Wp, matrix Wq);
	FLOAT pDNA_esenergy(proteinPDB &p, matrix Wp, matrix *V);
	
	void print_energy();
	FLOAT calculate_energy(matrix &W);
};

#endif

