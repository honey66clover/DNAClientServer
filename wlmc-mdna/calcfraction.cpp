#include <stdio.h>
#include <stdlib.h>

main() {
  FILE *f = fopen("fraction", "r");
  long int accum = 0;
  long int accum2 = 0;
  for (int i = 0; i < 100; i++)
  {
    int x;
    double y;
    fscanf(f, "%d %lf", &x, &y);
    if (i < 50) accum += (long int)y;
    accum2 += (long int)y;
  }
  printf("%lf\n", 1.0-(double)accum/(double)accum2);
  fclose(f);
}
