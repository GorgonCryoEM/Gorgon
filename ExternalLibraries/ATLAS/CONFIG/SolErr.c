
#include <stdio.h>

void ATL_dlascal(const int M, const int N, const double *x, double *A,
                 const int lda)
{

fprintf(stderr, "line %d of %s \n", __LINE__, __FILE__);
fprintf(stderr, "line %d of %s, x=%f\n", __LINE__, __FILE__, *x);
   if (*x != 0.0);
fprintf(stderr, "line %d of %s \n", __LINE__, __FILE__);
}

void bob(int M, int N, const double *A, int LDA, const double *B, int LDB,
         const double bet0, double *C, int LDC )
{
   ATL_dlascal(M, N, &bet0, C, LDC);
}

main()
{
   double A[110], B[110], C[110];
   double bet=22.8;
   ATL_dlascal(1,1,&bet,C,11);
   bob(10, 10, A, 11, B, 11, 3.3, C, 11);
   exit(0);
}
