#include <stdio.h>
#include <stdlib.h>

main() {
  printf("Nbins: \n");
  int N;
  scanf("%d", &N);
  FILE *f = fopen("data", "r");
  double d[100];
  int x;
  for (int i = 0; i < N; i++) {
    fscanf(f, "%d %lf", &x, &d[i]);
  }
  fclose(f);
  printf("%lf \n", -41.4195*(d[1]-d[0])/(34.0));
  for (int i = 1; i < N-1; i++) {
   printf("%lf \n", -41.4195*((d[i]-d[i-1]) + (d[i+1]-d[i]))/(2.0*34.0));
  }
  printf("%lf \n", -41.4195*(d[N-1]-d[N-2])/(34.0));
}
