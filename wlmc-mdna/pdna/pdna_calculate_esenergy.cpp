//total ES energy, adpated from pdna::overlap()
FLOAT pdna::calculate_esenergy(long int *locs, int nP, bdna *b) {

  FLOAT E = 0.0;
  FLOAT kap = 0.329*sqrt(ion);
 
  matrix W;    
  E += calculate_DNA_esenergy(W);
  if (nP == 0) return E;
	
  int *pos = new int[nP];//positions of each protein on DNA chain
  int *rev = new int[nP];//reversed or not
  int *num = new int[nP];//index of the structure in all structure list
  matrix *Ws = new matrix[nP];//matrix for protein on its position on DNA chain
  for (int i = 0; i < nP; i++) {//i->ith protein on DNA chain
  //based on these three relations we can determine locs[i] for each protein
  //given its position, orientation,index and nstructs: locs=2*nstructs*pos+nstructs*rev+num
	pos[i] = locs[i]/(2*nstructs);
	rev[i] = locs[i]%(2*nstructs)/nstructs;
	num[i] = locs[i]%(2*nstructs)%nstructs;
	
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
    for (int j = i+1; j < nP; j++) E += p_esenergy(proteinPDBs[num[i]], proteinPDBs[num[j]], Ws[i], Ws[j]);
	E+=pDNA_esenergy(proteinPDBs[num[i]], Ws[i],this);
  }
  
  delete [] pos;
  delete [] rev;
  delete [] num;
  delete [] Ws;
  return E;
}
