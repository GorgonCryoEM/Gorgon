#include "atlas_misc.h"
#include <assert.h>

#define dumb_seed(iseed_) srand(iseed_)
#ifndef RAND_MAX  /* rather dangerous non-ansi workaround */
   #define RAND_MAX ((unsigned long)(1<<30))
#endif
#define dumb_rand() ( 0.5 - ((double)rand())/((double)RAND_MAX) )
#ifndef TEST_DOT
   #define TEST_DOT ATL_UDOT
#endif
#ifdef TREAL
   TYPE TEST_DOT(const int, const TYPE*, const int, const TYPE*, const int);
#else
   TYPE TEST_DOT(const int, const TYPE*, const int, const TYPE*, const int, SCALAR);
#endif

#ifdef TREAL
TYPE good_dot(const int N, const TYPE *X, const int incX,
              TYPE *Y, const int incY)
{
   int i;
   TYPE dot=ATL_rzero;
   for (i=0; i < N; i++, Y += incY, X += incX) dot += *X * *Y;
   return(dot);
}
#else
void good_dot(const int N, const TYPE *X, const int incX,
              TYPE *Y, const int incY, SCALAR dot)
{
   int i;
   const int incx=incX+incX, incy=incY+incY;
   register TYPE rx, ry, ix, iy, rdot=ATL_rzero, idot=ATL_rzero;
   for (i=0; i < N; i++, Y += incy, X += incx)
   {
      #ifndef Conj_
         rx = *X; ix = X[1];
      #else
         rx = *X; ix = -X[1];
      #endif
      ry = *Y; iy = Y[1];
      rdot += rx * ry - ix * iy;
      idot += rx * iy + ix * ry;
   }
   dot[0] = rdot;
   dot[1] = idot;
}
#endif

static void vecset(int N, TYPE alpha, TYPE *X)
{
   int i;
   #ifdef TCPLX
      N *= 2;
   #endif
   for (i=0; i < N; i++) X[i] = alpha;
}

static TYPE *getvec(int npad, TYPE padval, int N, int incX)
{
   TYPE *X, *x;
   int i, n;

   if (N <= 0) return(NULL);
   incX = Mabs(incX);
   n = 2*npad + 1+(N-1)*incX;
   X = malloc( ATL_sizeof*n );
   assert(X);
   vecset(n, padval, X);
   #ifdef TCPLX
      npad *= 2;
      incX *= 2;
   #endif
   x = X + npad;
   for (i=0; i < N; i++, x += incX)
   {
      #ifdef TREAL
         *x = dumb_rand();
      #else
         *x   = dumb_rand();
         x[1] = dumb_rand();
      #endif
   }
   return(X);
}

static void copyvec(int N, const TYPE *X, int incX, TYPE *Y, int incY)
{
   int i;
   #ifdef TREAL
      for (i=0; i < N; i++, X += incX, Y += incY) *Y = *X;
   #else
      incX *= 2; incY *= 2;
      for (i=0; i < N; i++, X += incX, Y += incY) { *Y = *X; Y[1] = X[1]; }
   #endif
}

static TYPE *dupvec(int npad, int N, TYPE *X, int incX)
{
   int i, n;
   TYPE *y;

   incX = Mabs(incX);
   n = 1+(N-1)*incX + 2*npad;
   y = malloc(ATL_sizeof*n);
   assert(y);
   #ifdef TCPLX
      n *= 2;
   #endif
   for (i=0; i < n; i++) y[i] = X[i];
   return(y);
}

static TYPE *gen_dupvec(int N, TYPE padval, int npadX, TYPE *X, int incX,
                        int npadY, int incY)
{
   int i, n;
   TYPE *y, *yy, *xx=X+(npadX SHIFT);

   y = getvec(npadY, padval, N, incY);
   yy = y + (npadY SHIFT);
   if (incY < 1) yy -= ((N-1)SHIFT) * incY;
   if (incX < 1) xx -= ((N-1)SHIFT) * incX;
   copyvec(N, xx, incX, yy, incY);
   return(y);
}

int DoTest(int N, int incX, int incY)
{
   int iret=0;
   const int npad=Mmax(4*Mabs(incY), 16);
   const TYPE padval=(-2271.0);
   TYPE *Y, *X, *x, *y, eps, diff;
   #ifdef TREAL
      TYPE dotG, dotT;
   #else
      TYPE dotG[2], dotT[2];
   #endif
   TYPE Mjoin(PATL, epsilon)(void);
   eps = Mjoin(PATL,epsilon)();

   x = X = getvec(0, padval, N, incX);
   y = Y = getvec(0, padval, N, incY);
   if (incX < 1) x -= ((N-1)SHIFT) * incX;
   if (incY < 1) y -= ((N-1)SHIFT) * incY;

   #ifdef TREAL
      dotG = good_dot(N, x, incX, y, incY);
      dotT = TEST_DOT(N, x, incX, y, incY);
      diff = dotG - dotT;
      diff = Mabs(diff);
      if (diff > 2*N*eps) /* could be double this, but shouldn't in practice */
      {
         fprintf(stderr,
                 "   DOT ERROR: N=%d, correct=%e, computed=%e, diff=%e!!\n",
                 N, dotG, dotT, diff);
         iret = 1;
      }
   #else
      good_dot(N, x, incX, y, incY, dotG);
      TEST_DOT(N, x, incX, y, incY, dotT);
      diff = dotG[0] - dotT[0];
      diff = Mabs(diff);
      if (diff > 4*N*eps)
      {
         fprintf(stderr,
                 "   RDOT ERROR: N=%d, correct=%e, computed=%e, diff=%e!!\n",
                 N, dotG[0], dotT[0], diff);
         iret = 1;
      }
      diff = dotG[1] - dotT[1];
      diff = Mabs(diff);
      if (diff > 4*N*eps)
      {
         fprintf(stderr,
                 "   IDOT ERROR: N=%d, correct=%e, computed=%e, diff=%e!!\n",
                 N, dotG[1], dotT[1], diff);
         iret = 1;
      }
   #endif

   free(X);
   free(Y);
   return(iret);
}
int DoAllTests(int nN, int *Ns, int nX, int *Xs, int nY, int *Ys)
{
   int in, ix, iy, ia, ib, iret=0, i=0, j, k;
   char *passfail;

   char *t1="  ITST         N  incX  incY    TEST";
   char *t2="======  ========  ====  ====  ======";
   fprintf(stdout, "%s\n", t1);
   fprintf(stdout, "%s\n", t2);
   for (in=0; in < nN; in++)
   {
      for (ix=0; ix < nX; ix++)
      {
         for (iy=0; iy < nY; iy++)
         {
            j = DoTest(Ns[in], Xs[ix], Ys[iy]);
            iret += j;
            if (j == 0) passfail = "PASSED";
            else passfail = "FAILED";
            fprintf(stdout, "%6d %9d %5d %5d  %s\n",
                    i, Ns[in], Xs[ix], Ys[iy], passfail);
            i++;
         }
      }
   }
   if (iret == 0) fprintf(stdout, "ALL DOT SANITY TESTS PASSED.\n\n");
   else fprintf(stdout, "%d of %d DOT TESTS FAILED!!\n\n", iret, i);
   return(iret);
}

void PrintUsage(char *nam)
{
   fprintf(stderr,
           "USAGE: %s -N # n1 ... n# -n <n> -X # x1 ... x# -Y # y1 ... y#\n",
           nam);
   exit(-1);
}

void GetFlags(int nargs, char **args, int *nN, int **Ns, int *nX, int **incXs,
              int *nY, int **incYs)
{
   int i, j;

   *nY = *nX = *nN = -1;

   for (i=1; i < nargs; i++)
   {
      if (args[i][0] != '-') PrintUsage(args[0]);
      if (i == nargs-1) PrintUsage(args[0]);
      switch(args[i][1])
      {
      case 'Y':
         *nY = atoi(args[++i]);
         if (*nY > nargs-i) PrintUsage(args[0]);
         *incYs = malloc((*nY)*sizeof(int));
         assert(*incYs);
         for (j=0; j < *nY; j++) (*incYs)[j] = atoi(args[++i]);
         break;
      case 'X':
         *nX = atoi(args[++i]);
         if (*nX > nargs-i) PrintUsage(args[0]);
         *incXs = malloc((*nX)*sizeof(int));
         assert(*incXs);
         for (j=0; j < *nX; j++) (*incXs)[j] = atoi(args[++i]);
         break;
      case 'N':
         *nN = atoi(args[++i]);
         if (*nN > nargs-i) PrintUsage(args[0]);
         *Ns = malloc((*nN)*sizeof(int));
         assert(*Ns);
         for (j=0; j < *nN; j++) (*Ns)[j] = atoi(args[++i]);
         break;
      case 'y':
         *nY = 1;
         *incYs = malloc(sizeof(int));
         assert(*incYs);
         **incYs = atoi(args[++i]);
         break;
      case 'x':
         *nX = 1;
         *incXs = malloc(sizeof(int));
         assert(*incXs);
         **incXs = atoi(args[++i]);
         break;
      case 'n':
         *nN = 1;
         *Ns = malloc(sizeof(int));
         assert(*Ns);
         **Ns = atoi(args[++i]);
         break;
      default:
         PrintUsage(args[0]);
      }
   }
   if (*nN < 0)
   {
      *nN = 4;
      *Ns = malloc((*nN)*sizeof(int));
      assert(*Ns);
      **Ns = 777;
      (*Ns)[1] = 1;
      (*Ns)[2] = 3;
      (*Ns)[3] = 7;
   }
   if (*nX < 0)
   {
      *nX = 1;
      *incXs = malloc((*nX)*sizeof(int));
      assert(*incXs);
      **incXs = 1;
   }
   if (*nY < 0)
   {
      *nY = 1;
      *incYs = malloc((*nY)*sizeof(int));
      assert(*incYs);
      **incYs = 1;
   }
}

main(int nargs, char **args)
{
   int nN, nX, nY, nal, nbe;
   int *Ns, *incXs, *incYs;
   TYPE *alphas=NULL, *betas=NULL;
   int ierr;
   GetFlags(nargs, args, &nN, &Ns, &nX, &incXs, &nY, &incYs);
   ierr = DoAllTests(nN, Ns, nX, incXs, nY, incYs);
   free(incXs);
   free(incYs);
   exit(ierr);
}
