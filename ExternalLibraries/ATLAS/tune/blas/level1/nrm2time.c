#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define dumb_seed(iseed_) srand(iseed_)
#ifndef RAND_MAX  /* rather dangerous non-ansi workaround */
   #define RAND_MAX ((unsigned long)(1<<30))
#endif
#define dumb_rand() ( 0.5 - ((double)rand())/((double)RAND_MAX) )

#if defined(WALL) || defined(PentiumCPS)
   #define time00 ATL_walltime
#else
   #define time00 ATL_cputime
#endif
double time00();

#define Mjoin(pre, nam) my_join(pre, nam)
#define my_join(pre, nam) pre ## nam
#define Mstr2(m) # m
#define Mstr(m) Mstr2(m)
#define Mmin(x, y) ( (x) > (y) ? (y) : (x) )
#define Mmax(x, y) ( (x) < (y) ? (y) : (x) )
#define Mabs(x) ( (x) >= 0.0 ? (x) : -(x) )

#ifdef sREAL
   #define SREAL
#elif defined(dREAL)
   #define DREAL
#elif defined(sCPLX)
   #define SCPLX
#elif defined(dCPLX)
   #define DCPLX
#endif

#if defined (SREAL)
   #define TYPE float
   #define SCALAR float
   #define PRE s
#elif defined (DREAL)
   #define TYPE double
   #define SCALAR double
   #define PRE d
#elif defined (SCPLX)
   #define TYPE float
   #define SCALAR float*
   #define PRE c
#elif defined (DCPLX)
   #define TYPE double
   #define SCALAR double*
   #define PRE z
#endif

#if defined(SREAL) || defined(DREAL)
   #define TREAL
   #define SHIFT
   #define ATL_sizeof sizeof(TYPE)
#elif defined(SCPLX) || defined(DCPLX)
   #define TCPLX
   #define ATL_sizeof (2*sizeof(TYPE))
   #define SHIFT <<1
#endif

#include Mstr(Mjoin(Mjoin(atlas_,PRE),sysinfo.h))
#ifndef TEST_NRM2
   #define TEST_NRM2 ATL_NRM2
#endif

double DoTiming(int N, int nkflop, int cachesize, int incX)
{
   TYPE TEST_NRM2(const int N, const TYPE *X, const int incX);
   const int incx = Mabs(incX)SHIFT;
   int i, j, ii, n, NN, nrep, nvec, ix;
   TYPE *X, *stX, *x;
   TYPE tmp;
   double t0, t1;

   assert(incX > 0);
   nrep = (nkflop * 1000) / (N*2);
   if (nrep < 1) nrep = 1;
   i = cachesize / (ATL_sizeof);
   if (N < i) nvec = (i + 2*N) / N;
   else nvec = 2;
   NN = nvec * N;

   if (incx > 4) ii = (incx + 3)/4;
   else ii = incx;
   NN *= ii;
   ii = N*incx;
   if (NN / ii) NN += ii - (NN % ii);

   #ifdef TREAL
      x = X = malloc(ATL_sizeof*NN);
   #else
      x = X = malloc((ATL_sizeof>>1)*NN);
   #endif
   assert(X);
   stX = X + NN;
   dumb_seed(NN);
   for (n=NN, i=0; i < n; i++) X[i] = dumb_rand();
   ix = (N SHIFT) * incX;

   t0 = time00();
   for (tmp=0,i=nrep; i; i--)
   {
      tmp += TEST_NRM2(N, x, incX);
      x += ix;
      if (x == stX) x = X;
   }
   t1 = time00() - t0;
   free(X);
   return(t1/nrep);
}

void GetTimes(int nrep, double *tims, int N, int nkflop, int cachesize,
              int incX)
/*
 * Gets nrep times
 */
{
   int i;
   for (i=0; i < nrep; i++)
   {
      tims[i] = DoTiming(N, nkflop, cachesize, incX);
      fprintf(stdout, "      N=%d, tim=%e\n", N, tims[i]);
   }
}

void PutClosestFirst(int nrep, double *tims, double t0)
/*
 * examines all nrep entries, and puts one closest to t0 first
 */
{
   int i, im=0;
   double diff, dm=100000000.0;
   for (i=0; i < nrep; i++)
   {
      diff = tims[i] - t0;
      diff = Mabs(diff);
      if (diff < dm)
      {
         dm = diff;
         im = i;
      }
   }
   if (im)
   {
      diff = tims[0];
      tims[0] = tims[im];
      tims[im] = diff;
   }
}

void CrunchTims(int nrep, double *tims)
/*
 * orders times from closest to furthest
 */
{
   int i, j, im=0, in=0;
   double df, dfm=1000000.0;

   if (nrep < 3) return;
/*
 * Find two closest timings, take them as basis
 */
   for (i=0; i < nrep; i++)
   {
       for (j=0; j < nrep; j++)
       {
          if (i != j)
          {
             df = tims[i] - tims[j];
             df = Mabs(df);
             if (df < dfm)
             {
                im = i; in = j;
                dfm = df;
             }
          }
       }
   }
   df = tims[0];
   tims[0] = tims[im];
   tims[im] = df;
   df = tims[1];
   tims[1] = tims[in];
   tims[in] = df;
   df = (tims[0] + tims[1]) / 2.0;
   for (i=2; i < nrep; i++)
   {
      PutClosestFirst(nrep-i, tims+i, df);
      df = (i*df + tims[i]) / (i+1.0);
   }
}

int ApplyIntolerance(int nrep, double *tims, double TOL)
/*
 * Takes list ordered by closeness, and throws out outliers
 */
{
   int i, j=nrep;
   double t0, t1;

   t0 = tims[0];
   assert(t0 > 0.0);
   for (i=1; i < nrep; i++)
   {
      t1 = tims[i] - t0;
      t1 = Mabs(t1);
      if (t1 > t0*TOL) return(i);
   }
   return(nrep);
}
double GetAvgMf(int N, int nkflop, int cachesize, char *fout, int incX,
                 int nrep, double TOL)
/*
 * Gets nrep timings within TOL of each other.
 */
{
   double t0;
   #ifdef TREAL
      double mf = (2.0*N)/1000000.0;
   #else
      double mf = (4.0*N)/1000000.0;
   #endif
   double *tims;
   int n, i, j, k;
   FILE *fpout=NULL;

   tims = malloc(sizeof(double)*nrep);
   assert(tims);

   k = i = n = 0;
   do
   {
      if (i++ > 4)
      {
         if (k == 0) /* try again from scratch */
         {
            n = 0;
            k = 1;
         }
         else
         {
            fprintf(stderr,
                    "\n\nCANNOT GET TIMINGS IN TOLERANCE, EXITING!!\n\n");
            exit(4*k+i);
         }
      }
      GetTimes(nrep-n, tims+n, N, nkflop, cachesize, incX);
      CrunchTims(nrep, tims);  /* group times that are close */
      n = ApplyIntolerance(nrep, tims, TOL);
   }
   while (n != nrep);
   for (t0=0.0, i=0; i < nrep; i++) t0 += tims[i];
   t0 /= nrep;
   mf /= t0;
   fprintf(stdout, "   N=%d, time=%e, mflop=%f\n", N, t0, mf);
   if (fout)
   {
      fpout = fopen(fout, "w");
      assert(fpout);
      fprintf(fpout, "%lf\n", mf);
      fclose(fpout);
   }
   return(mf);
}

void DoTimings(int nN, int *Ns, int nkflop, int cachesize, char *fout, int incX)
{

   int i;
   double t0;
   for (i=0; i < nN; i++)
   {
      t0 = GetAvgMf(Ns[i], nkflop, cachesize, fout, incX, 3, 0.1);
      fprintf(stdout, "N=%d, incX=%d, mflop = %f\n", Ns[i], incX, t0);
   }
}

void PrintUsage(char *nam)
{
   fprintf(stderr, "USAGE: %s -n <N> -N <#> N1 ... N# <-F kflops> -C <cacheflush> -f <fout> -X <incX>\n", nam);
   exit(-1);
}

void GetFlags(int nargs, char **args, int *nN, int **Ns, int *nkflops,
              int *cachesize, char **fnam, int *incX)
{
   int i, j;

   *nN = -1;
   *nkflops = ATL_nkflop / 10;
   *cachesize = 4*L2SIZE;
   *fnam = NULL;
   *incX = 1;

   for (i=1; i < nargs; i++)
   {
      if (args[i][0] != '-') PrintUsage(args[0]);
      if (i == nargs-1) PrintUsage(args[0]);
      switch(args[i][1])
      {
      case 'X':
         *incX = atoi(args[++i]);
         break;
      case 'C':
         *cachesize = atoi(args[++i]);
         break;
      case 'f':
         *fnam = args[++i];
         break;
      case 'F':
         *nkflops = atoi(args[++i]);
         break;
      case 'N':
         *nN = atoi(args[++i]);
         if (*nN > nargs-i) PrintUsage(args[0]);
         *Ns = malloc((*nN)*sizeof(int));
         assert(*Ns);
         for (j=0; j < *nN; j++) (*Ns)[j] = atoi(args[++i]);
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
}

main(int nargs, char **args)
{
   int nN, nkflops, cachesize, incX;
   int *Ns;
   char *fout;
   GetFlags(nargs, args, &nN, &Ns, &nkflops, &cachesize, &fout, &incX);
   DoTimings(nN, Ns, nkflops, cachesize, fout, incX);
   free(Ns);
   exit(0);
}
