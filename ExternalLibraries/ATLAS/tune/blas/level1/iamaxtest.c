#include "atlas_misc.h"
#include <assert.h>

#define dumb_seed(iseed_) srand(iseed_)
#ifndef RAND_MAX  /* rather dangerous non-ansi workaround */
   #define RAND_MAX ((unsigned long)(1<<30))
#endif
#define dumb_rand() ( 0.5 - ((double)rand())/((double)RAND_MAX) )
#ifndef TEST_IAMAX
   #define TEST_IAMAX ATL_IAMAX
#endif
int TEST_IAMAX(const int N, const TYPE *X, const int incX);

#ifdef TREAL
static int GOOD_IAMAX(const int N, const TYPE *X, const int incX)
{
   register TYPE t0, xmax=0.0;
   int i, imax=0;

   assert(incX > 0);
   for (i=0; i < N; i++)
   {
      t0 = *X;
      X += incX;
      if (t0 < ATL_rzero) t0 = -t0;
      if (t0 > xmax) { xmax = t0; imax = i; }
   }
   return(imax);
}

static TYPE *GetVec(int N, int incX, int imax)
/*
 * Allocates, and generates vec with amax in position imax, if imax < 0,
 * have random max
 */
{
   TYPE *X, t0;
   int i, n;

   n = 1 + (N-1)*incX;
   X = malloc(ATL_MulBySize(n));
   assert(X);
   if (incX != 1)  /* pad with value that will trigger max */
      for (i=0; i < n; i++) X[i] = 524288.0;
   for (i=0; i < N; i++) X[i*incX] = dumb_rand();
   if (imax >= 0) /* find maxval, and swap into imax */
   {
      i = GOOD_IAMAX(N, X, incX);
      t0 = X[i*incX]; t0 += 0.1*t0;
      X[i*incX] = X[imax*incX];
      X[imax*incX] = t0;
   }
   return(X);
}

#else

static int GOOD_IAMAX(const int N, const TYPE *X, const int incX)
{
   register TYPE xr, xi, xmax=0.0;
   int i, imax=0;
   const int incx = incX<<1;

   assert(incX > 0);
   for (i=0; i < N; i++)
   {
      xr = *X;
      xi = X[1];
      X += incx;
      if (xr < ATL_rzero) xr = -xr;
      if (xi < ATL_rzero) xi = -xi;
      xr += xi;
      if (xr > xmax) { xmax = xr; imax = i; }
   }
   return(imax);
}

static TYPE *GetVec(int N, int incx, int imax)
/*
 * Allocates, and generates vec with amax in position imax, if imax < 0,
 * have random max
 */
{
   TYPE *X, t0, t1;
   int i, n;
   const int incX = incx*2;

   n = 1 + (N-1)*incx;
   X = malloc(ATL_MulBySize(n));
   assert(X);
   if (incx != 1)  /* pad with value that will trigger max */
      for (n *= 2, i=0; i < n; i++) X[i] = 524288.0;
   for (i=0; i < N; i++)
   {
      X[i*incX] = dumb_rand();
      X[i*incX+1] = dumb_rand();
   }
   if (imax >= 0) /* find maxval, and swap into imax */
   {
      imax *= 2;
      i = GOOD_IAMAX(N, X, incx) * 2;
      t0 = X[i*incx]; t0 += 0.1*t0;
      t1 = X[i*incx+1];
      X[i*incx] = X[imax*incx];
      X[i*incx+1] = X[imax*incx+1];
      X[imax*incx] = t0;
      X[imax*incx+1] = t1;
   }
   return(X);
}
#endif

void PrintError(int line, int icor, int itst, const TYPE *X, int incX)
{
   #ifdef TREAL
      fprintf(stderr,
              "   IAMAX ERROR %d: correct=%d (%f), computed=%d (%f)\n",
              line, icor, X[icor], itst, X[itst]);
   #else
      icor *= 2; itst *= 2;
      fprintf(stderr,
              "   IAMAX ERROR %d: correct=%d (%f,%f), computed=%d (%f,%f)\n",
              line, icor/2, X[icor*incX], X[icor*incX+1], itst/2,
              X[itst*incX], X[itst*incX+1]);
   #endif
}

int TestVec(int N, const TYPE *X, int incX)
{
   int icor, itst;
   icor = GOOD_IAMAX(N, X, incX);
   itst = TEST_IAMAX(N, X, incX);
   if (icor == itst) return(0);
   else PrintError(__LINE__, icor, itst, X, incX);
   return(1);
}

int TestTie(int N, int incX, int imax, int itie)
/*
 * Puts max in location imax, and a tie in location itie: imax < itie
 * should probably have a tie in cplx that is not exact same, but not for now
 */
{
   TYPE *X;
   int i;
   assert(imax < itie && itie < N && imax >= 0);
   X = GetVec(N, incX, imax);
   #ifdef TREAL
      X[itie*incX] = X[imax*incX];
   #else
      X[2*itie*incX] = X[2*imax*incX];
      X[2*itie*incX+1] = X[2*imax*incX+1];
   #endif
   i = TEST_IAMAX(N, X, incX);
   if (i != imax) PrintError(__LINE__, imax, i, X, incX);
   free(X);
   if (i == imax) return(0);
   else return(1);
}

int TestTies(int N, int incX)
{
   int ierr=0;
   if (N < 8) N = 8;

   ierr += TestTie(N, incX, N-2, N-1);
   ierr += TestTie(N, incX, 1, 2);
   ierr += TestTie(N, incX, N/2, N/2+3);
   ierr += TestTie(N, incX, 3, N/2);
   return(ierr);
}

int RunTest(int N, int incX, int imax, int flag)
{
   TYPE *X, t0;
   int i;

   X = GetVec(N, incX, imax);
   if (imax < 0) imax = GOOD_IAMAX(N, X, incX);
   if (flag == -1)  /* make max entry negative */
   { /* for cplx, leave imag alone */
      t0 = X[incX*imax SHIFT];
      if (t0 > ATL_rzero) X[incX*imax SHIFT] = -t0;
   }
   else if (flag == 1)   /* make max entry positive */
   { /* for cplx, leave imag alone */
      t0 = X[incX*imax SHIFT];
      if (t0 < ATL_rzero) X[incX*imax SHIFT] = -t0;
   }
   i = TEST_IAMAX(N, X, incX);
   if (i != imax) PrintError(__LINE__, imax, i, X, incX);
   free(X);

   if (i == imax) return(0);
   else return(1);
}

int RunTests(int N, int incX, int nrand)
{
   int i, ierr=0;
   int n = Mmax(N, 8);

/*
 * To catch common unrolling mistakes, put max in first and last 8 positions
 */
   fprintf(stdout, "\nTesting leading and trailing placement :\n");
   ierr += RunTest(n, incX, 0, -1);
   ierr += RunTest(n, incX, 1,  1);
   ierr += RunTest(n, incX, 2, -1);
   ierr += RunTest(n, incX, 3,  1);
   ierr += RunTest(n, incX, 4, -1);
   ierr += RunTest(n, incX, 5, -1);
   ierr += RunTest(n, incX, 6,  1);
   ierr += RunTest(n, incX, 7, -1);

   ierr += RunTest(n, incX, n-1,  1);
   ierr += RunTest(n, incX, n-2, -1);
   ierr += RunTest(n, incX, n-3,  1);
   ierr += RunTest(n, incX, n-4, -1);
   ierr += RunTest(n, incX, n-5,  1);
   ierr += RunTest(n, incX, n-6,  1);
   ierr += RunTest(n, incX, n-7, -1);
   ierr += RunTest(n, incX, n-8,  1);

   fprintf(stdout, "Testing ties:\n");
   ierr += TestTies(n, incX);
   fprintf(stdout, "Testing random:\n");
   for (i=0; i < nrand; i++) ierr += RunTest(N, incX, -1,  0);
   fprintf(stdout, "Done testing.\n");

   return(ierr);
}

int RunAllTests(int nN, int *Ns, int nX, int *incXs, int nrand)
{
   int in, ix, ierr=0;

   for (in=0; in < nN; in++)
   {
      for (ix=0; ix < nX; ix++)
      {
         ierr += RunTests(Ns[in], incXs[ix], nrand);
      }
   }
   return(ierr);
}

void PrintUsage(char *nam)
{
   fprintf(stderr,
      "USAGE: %s -N # n1 ... n# -n <n> -X # x1 ... x# -R <nrand>\n", nam);
   exit(-1);
}

void GetFlags(int nargs, char **args, int *nN, int **Ns, int *nX, int **incXs,
              int *nrand)
{
   int i, j, k;

   *nrand = 10;
   *nX = *nN = -1;

   for (i=1; i < nargs; i++)
   {
      if (args[i][0] != '-') PrintUsage(args[0]);
      if (i == nargs-1) PrintUsage(args[0]);
      switch(args[i][1])
      {
      case 'R':
         *nrand = atoi(args[++i]);
         break;
      case 'X':
         *nX = atoi(args[++i]);
         if (*nX > nargs-i) PrintUsage(args[0]);
         *incXs = malloc((*nX)*sizeof(int));
         assert(*incXs);
         for (j=0; j < *nX; j++)
         {
            k = atoi(args[++i]);
            (*incXs)[j] = Mabs(k);
         }
         break;
      case 'N':
         *nN = atoi(args[++i]);
         if (*nN > nargs-i) PrintUsage(args[0]);
         *Ns = malloc((*nN)*sizeof(int));
         assert(*Ns);
         for (j=0; j < *nN; j++) (*Ns)[j] = atoi(args[++i]);
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
      *nN = 1;
      *Ns = malloc(sizeof(int));
      assert(*Ns);
      **Ns = 777;
   }
   if (*nX < 0)
   {
      *nX = 1;
      *incXs = malloc(sizeof(int));
      assert(*incXs);
      **incXs = 1;
   }
}

main(int nargs, char **args)
{
   int nN, *Ns, nX, *incXs, nrand, ierr;
   GetFlags(nargs, args, &nN, &Ns, &nX, &incXs, &nrand);
   ierr = RunAllTests(nN, Ns, nX, incXs, nrand);
   free(incXs);
   free(Ns);
   if (ierr) fprintf(stderr, "%d TESTS FAILED!!!\n", ierr);
   else fprintf(stderr, "ALL SANITY TESTS PASSED.\n");
   exit(ierr);
}
