//calculate ES energy between two proteins
FLOAT pdna::p_esenergy(proteinPDB p, proteinPDB q, matrix Wp, matrix Wq) {
  FLOAT E = 0.0;
  FLOAT kap = 0.329*sqrt(ion);
  FLOAT r,x,y,z;
  FLOAT q1, q2;
  /*
  const FLOAT eps = 77.7;
  const FLOAT e = 0.24*1.6021773e-19;
  const FLOAT eps0 = 8.854188e-32;
  const FLOAT kT = 1.380658e-13*300.0;
  const FLOAT coeff = e*e/(4*3.14159*eps0*eps*kT);
  */
  //translate the positions of the two proteins
  proteinPDB *tp = *translateprotein(*p, Wp);
  proteinPDB *tq = *translateprotein(*q, Wq);

  for (int i = 0; i < tp.neatoms; i++) {
	q1 = tp.e[i];
	for (int j = 0; j < tq.neatoms; j++) {
	  x = tp.ex[i]-tq.ex[j];
	  y = tp.ey[i]-tq.ey[j];
	  z = tp.ez[i]-tq.ez[j];
	  r = sqrt(x*x+y*y+z*z);
	  q2 = tq.e[j];
	  E += q1*q2*exp(-kap*r)/r;
    }
  }
  E *= coeff;

  return E;
}
//calculate ES energy between two proteins
FLOAT pdna::p_esenergy(proteinPDB p, proteinPDB q, matrix Wp, matrix Wq) {
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