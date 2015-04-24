#include "atlas_misc.h"
#include <assert.h>
#include <math.h>
#include <float.h>

#define dumb_seed(iseed_) srand(iseed_)
#ifndef RAND_MAX  /* rather dangerous non-ansi workaround */
   #define RAND_MAX ((unsigned long)(1<<30))
#endif
#define dumb_rand() ( 0.5 - ((double)rand())/((double)RAND_MAX) )
#ifndef TEST_NRM2
   #define TEST_NRM2 ATL_UNRM2
#endif
TYPE TEST_NRM2(const int, const TYPE *, const int incX);

#ifdef TREAL
TYPE good_nrm2(const int N, const TYPE *X, const int incX)
{
   int i;
   TYPE t0, ax, ssq=ATL_rone, scal=ATL_rzero;

   for (i=N; i; i--, X += incX)
   {
      ax = *X;
      if (ax != ATL_rzero)
      {
         ax = Mabs(ax);
         if (scal < ax)
         {
            t0 = scal / ax;
            t0 *= t0;
            ssq = ATL_rone + ssq * t0;
            scal = ax;
         }
         else
         {
            t0 = ax / scal;
            ssq += t0*t0;
         }
      }
   }
   return(scal * sqrt(ssq));
}
int CheckPad(int npad, TYPE padval, int N, TYPE *Y, int incY)
{
   int i, iret=0;

   incY = Mabs(incY);
   for (i=0; i < npad; i++)
   {
      if (Y[i] != padval)
      {
         iret = i;
         fprintf(stderr, "OVERWRITE %f IN PREPAD %d before beginning of Y!!\n",
                 Y[i], npad-i);
      }
   }
   Y += npad;
   if (incY != 1)
   {
      for (i=0; i < N*incY; i++)
      {
         if (i%incY)
         {
            if (Y[i] != padval)
            {
               iret = i;
               fprintf(stderr, "INTERNAL OVERWRITE %f AT POSITION %d!!\n",
                       Y[i], i);
            }
         }
      }
   }
   Y += 1 + (N-1)*incY;
   for (i=0; i < npad; i++)
   {
      if (Y[i] != padval)
      {
         iret = i;
         fprintf(stderr, "OVERWRITE %f IN POSTPAD %d past end of Y!!\n",
                 Y[i], i+1);
      }
   }
   return(iret);
}
#else
TYPE good_nrm2(const int N, const TYPE *X, const int incX)
{
   int i, j;
   const int incx=incX+incX;
   TYPE ax, t0, ssq=ATL_rone, scal=ATL_rzero;

   for (i=0; i < N; i++, X += incx)
   {
      for (j=0; j < 2; j++)
      {
         ax = X[j];
         if (ax != ATL_rzero)
         {
            ax = Mabs(ax);
            if (scal < ax)
            {
               t0 = scal / ax;
               t0 *= t0;
               scal = ax;
               ssq = ATL_rone + ssq * t0;
            }
            else
            {
               t0 = ax / scal;
               ssq += t0*t0;
            }
         }
      }
   }
   return(scal * sqrt(ssq));
}
int CheckPad(int npad, TYPE padval, int N, TYPE *Y, int incY)
{
   int i, n, iret=0;

   incY = Mabs(incY);
   npad *= 2;
   for (i=0; i < npad; i++)
   {
      if (Y[i] != padval)
      {
         iret = i;
         fprintf(stderr, "OVERWRITE %f IN PREPAD %d before beginning of Y!!\n",
                 Y[i], npad-i);
      }
   }
   Y += npad;
   if (incY != 1)
   {
      for (i=0; i < N*incY; i++)
      {
         if (i%incY)
         {
            if (Y[2*i] != padval)
            {
               iret = i;
               fprintf(stderr, "INTERNAL REAL OVERWRITE %f AT POSITION %d!!\n",
                       Y[2*i], i);
            }
            if (Y[2*i+1] != padval)
            {
               iret = i+1;
               fprintf(stderr, "INTERNAL IMAG OVERWRITE %f AT POSITION %d!!\n",
                       Y[2*i+1], i);
            }
         }
      }
   }
   Y += 2 + 2*(N-1)*incY;
   for (i=0; i < npad; i++)
   {
      if (Y[i] != padval)
      {
         iret = i;
         fprintf(stderr, "OVERWRITE %f IN POSTPAD %d past end of Y!!\n",
                 Y[i], i+1);
      }
   }
   return(iret);
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

void vecsetX(int N, TYPE alpha, TYPE *X, int incX)
{
   int i;
#ifdef TREAL
   for (i=N; i; i--, X += incX) *X = alpha;
#else
   incX *= 2;
   for (i=N; i; i--, X += incX) *X = X[1] = alpha;
#endif
}
#if defined(SREAL) || defined(SCPLX)
   #define ATL_MAXNUM FLT_MAX
   #define ATL_MINNUM FLT_MIN
#else
   #define ATL_MAXNUM DBL_MAX
   #define ATL_MINNUM DBL_MIN
#endif
int DoStressTest(int N, int incX, int iflag)
/*
 * iflag = 0: normal test
 *       = 1: overflow test
 *       = 2: underflow test
 */
{
   int DoTest(int, int);
   int iret=0;
   const TYPE padval=(-2271.0);
   TYPE *X, *x, eps, diff, maxdiff;
   volatile TYPE tt[2];
   TYPE ansG, ansT, t0, t1;
   TYPE Mjoin(PATL, epsilon)(void);

   if (iflag == 0) return(DoTest(N, incX));
   eps = Mjoin(PATL,epsilon)();

   x = X = getvec(0, padval, N, incX);
   if (incX < 1) x -= ((N-1)SHIFT) * incX;

   if (iflag == 1)
   {
      t1 = ((N+1) SHIFT);
      t0 = ATL_MAXNUM / t1;
      t1 = (N SHIFT);
      ansG = sqrt(t1)*t0;
   }
   else
   {
      t0 = ATL_MINNUM;
      t1 = (N SHIFT);
      t1 = sqrt(t1);
      ansG = t0 * t1;
   }
   vecsetX(N, t0, x, incX);
   ansT = TEST_NRM2(N, x, incX);
   tt[0] = ansG;
   tt[1] = ansT;
   ansG = tt[0];  /* force to precision */
   ansT = tt[1];
   if (ansG >= ansT) diff = ansG - ansT;
   else diff = ansT - ansG;
   if (diff / ((2 SHIFT)*N*eps) > 10.0*t0) /* lame-ass norm */
   {
      if (iflag == 1)
      {
         fprintf(stderr,
         "OVERFLOW GUARD ERROR: N=%d, correct=%le, computed=%le, diff=%le!!\n",
                 N, ansG, ansT, diff);
      }
      else
      {
         fprintf(stderr,
         "UNDERFLOW GUARD ERROR: N=%d, correct=%e, computed=%e, diff=%e!!\n",
                 N, ansG, ansT, diff);
      }
      iret = 1;
   }
   free(X);
   return(iret);
}
int DoTest(int N, int incX)
{
   int iret=0;
   const TYPE padval=(-2271.0);
   TYPE *X, *x, eps, diff, maxdiff;
   TYPE ansG, ansT;
   TYPE Mjoin(PATL, epsilon)(void);
   eps = Mjoin(PATL,epsilon)();
   maxdiff = 5.0*(2 SHIFT)*N*eps;

   x = X = getvec(0, padval, N, incX);
   if (incX < 0) x -= ((N-1)SHIFT) * incX;

   ansG = good_nrm2(N, x, incX);
   ansT = TEST_NRM2(N, x, incX);
   diff = ansG - ansT;
   diff = Mabs(diff);
   if (diff > maxdiff)
   {
      fprintf(stderr,
              "   nrm2 ERROR: N=%d, correct=%e, computed=%e, diff=%e!!\n",
              N, ansG, ansT, diff);
      iret = 1;
   }

   free(X);
   return(iret);
}
int DoAllTests(int nN, int *Ns, int nX, int *Xs, int nY, int *Ys)
{
   int in, ix, iy, ia, ib, iret=0, i=0, j, k;
   char *passfail;

   char *t1="  ITST  STRESS         N  incX    TEST";
   char *t2="======  ======  ========  ====  ======";
   char *stresses[3] = {"NORMAL", "OVER  ", "UNDER "};
   fprintf(stdout, "%s\n", t1);
   fprintf(stdout, "%s\n", t2);
   for (in=0; in < nN; in++)
   {
      for (ix=0; ix < nX; ix++)
      {
         for (k=0; k < 3; k++)
         {
            j = DoStressTest(Ns[in], Xs[ix], k);
            iret += j;
            if (j == 0) passfail = "PASSED";
            else passfail = "FAILED";
            fprintf(stdout, "%6d  %6s  %9d %5d %s\n",
                    i, stresses[k], Ns[in], Xs[ix], passfail);
            i++;
         }
      }
   }
   if (iret == 0) fprintf(stdout, "ALL NRM2 SANITY TESTS PASSED.\n\n");
   else fprintf(stdout, "%d of %d NRM2 TESTS FAILED!!\n\n", iret, i);
   return(iret);
}

void PrintUsage(char *nam)
{
   fprintf(stderr,
      "USAGE: %s -N # n1 ... n# -n <n> -X # x1 ... x# \n", nam);
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
