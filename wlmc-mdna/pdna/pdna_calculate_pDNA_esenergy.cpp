//calculate ES energy between pDNA
FLOAT pdna::pDNA_esenergy(proteinPDB p, matrix Wp, bdna *b) {
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
  
  for (int i = 0; i < b->nsteps-1; i++) {
 
    FLOAT X1 = W1(1,4);
    FLOAT Y1 = W1(2,4);
    FLOAT Z1 = W1(3,4);

    q1 = -0.25; //DNA screened charge

    W1 = W1 * calculateW(v[i]);

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