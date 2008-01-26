/*
 *             Automatically Tuned Linear Algebra Software v3.6.0
 *                    (C) Copyright 1997 R. Clint Whaley
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *   1. Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions, and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *   3. The name of the ATLAS group or the names of its contributers may
 *      not be used to endorse or promote products derived from this
 *      software without specific written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE ATLAS GROUP OR ITS CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "atlas_misc.h"
#include "atlas_fopen.h"

#define Mmin(x, y) ( (x) > (y) ? (y) : (x) )

#define TOLERANCE 1.2
#define REPS 4096
#define L1FNAME "L1CacheSize"
#define NTIM 3
#define MAXLAT 6

char LANG;

void PrintUsage(char *xnam)
{
   fprintf(stderr, "\n\nUsage: %s [-r #][-h][-f][-l #][-p s/d/c/z][-m #]\n",
           xnam);
   fprintf(stderr, "-h         : Print this help screen\n");
   fprintf(stderr, "-f         : Force complete search over given parameters\n");
   fprintf(stderr, "-p s/d/c/z : set the precision to search for\n");
   fprintf(stderr, "-r #       : Set max number of registers to use to # (default 32)\n");
   fprintf(stderr, "-m #       : Set max L1 cache size (kilobytes) to #\n");
   fprintf(stderr, "-L <c/f>   : Select what language to use (C or Fortran77)\n");
   fprintf(stderr, "-K #       : Set K-loop unrolling to # (-1 = K).\n");
   fprintf(stderr, "-l #       : Use latency factor #.  If set to 0,\n");
   fprintf(stderr,
"             do not do latency checking.  By default, latency checking is\n");
   fprintf(stderr,
"             done only if initial timings show it is a win.\n");
   exit(-1);
}

void GetSettings(int nargs, char *args[], char *pre, char *lang, int *ku,
                 int *LAT, int *FRC, int *nreg, int *MaxL1Size, int *ROUT)
{
   int i;

   *FRC = 0;
   *LAT = -1;
   *nreg = -1;
   *MaxL1Size = 64;
   *pre = 'd';
   *lang = 'C';
   *ku = 0;
   *ROUT = 0;
   for (i=1; i < nargs; i++)
   {
      if (*args[i] != '-') PrintUsage(args[0]);
      switch(args[i][1])
      {
      case 'K':
         *ku = atoi(args[++i]);
         break;
      case 'L':
         i++;
         if ( (*args[i] == 'F') || (*args[i] == 'f') ) *lang = 'F';
         break;
      case 'm' :
         *MaxL1Size = atoi(args[++i]);
         break;
      case 'r' :
         *nreg = atoi(args[++i]);
         break;
      case 'f' :
         *FRC = atoi(args[++i]);
         break;
      case 'l' :
         *LAT = atoi(args[++i]);
         break;
      case 'p' :
         *pre = *args[++i];
         break;
      default:
      case 'R':
         *ROUT = atoi(args[++i]);
         break;
      case 'h' :
         PrintUsage(args[0]);
      }
   }
}

int L1Elts(char pre, int MaxL1Size)
{
   FILE *L1f;
   int L1Size, tsize;
   char ln[128];

   if (!FileExists("res/L1CacheSize"))
   {
      sprintf(ln, "make RunL1 MaxL1=%d\n",MaxL1Size);
      if (system(ln) != 0)
      {
         remove("res/L1CacheSize");
         fprintf(stderr, "Error in command: %s", ln);
         exit(-1);
      }
   }
   L1f = fopen("res/L1CacheSize", "r");
   assert(L1f != NULL);
   fscanf(L1f, "%d", &L1Size);
   fclose(L1f);
   switch (pre)
   {
      case 's':
         tsize = sizeof(float);
         break;
      case 'd':
         tsize = sizeof(double);
         break;
      case 'q':
         tsize = sizeof(long double);
         break;
      case 'c':
         tsize = sizeof(float);
         break;
      case 'z':
         tsize = sizeof(double);
         break;
   }
   return( (L1Size*1024) / tsize);
}

int GetCacheSize(int MaxL1Size)
/*
 * Returns L1 size in kilobytes
 */
{
   FILE *L1f;
   int L1Size;
   char ln[32];

   if (!FileExists("res/L1CacheSize"))
   {
      sprintf(ln, "make RunL1 MaxL1=%d\n",MaxL1Size);
      if (system(ln) != 0)
      {
         remove("res/L1CacheSize");
         fprintf(stderr, "Error in command: %s", ln);
         exit(-1);
      }
   }
   L1f = fopen("res/L1CacheSize", "r");
   assert(L1f != NULL);
   fscanf(L1f, "%d", &L1Size);
   fclose(L1f);
   fprintf(stderr, "\n      Read in L1 Cache size as = %dKB.\n",L1Size);
   return(L1Size);
}

int GetTypeSize(char pre)
{
   int tsize;
   if (pre == 'c' || pre == 's') tsize = ATL_ssize;
   else tsize = ATL_dsize;
   return(tsize);
}
void findNBs(char prec, char *NBnam, int MaxL1Size)
{
   FILE *NBf;
   char ln[80];
   int i, L1Size, tmp, tsize, tL1Size, CL, nNB;
   int NB[100];

   fprintf(stderr, "NB setting not supplied; calculating:\n");

   L1Size = GetCacheSize(MaxL1Size);
   tsize = GetTypeSize(prec);

   tL1Size = L1Size * (1024 / tsize);
   tmp = CL = ATL_Cachelen / tsize;
   if (!tmp) tmp=1;
   nNB = 0;
   fprintf(stderr, "tmp=%d, tL1size=%d\n",tmp, tL1Size);
   while (tmp*tmp <= tL1Size)
   {
      if (tmp >= 16)        /* no block sizes smaller than 16 */
         NB[nNB++] = tmp;
      if (tmp >= 80) break;  /* no block sizes bigger than 80 */
      tmp += CL;
   }
   if (!nNB)  /* this should never happen */
   {
      nNB = 3;
      NB[0] = 8;
      NB[1] = 4;
      NB[2] = 16;
   }
   else if (nNB > 2)  /* put second biggest blocking factor first in list */
   {
      tmp = NB[nNB-2];
      NB[nNB-2] = NB[0];
      NB[0] = tmp;
   }

   NBf = fopen(NBnam, "w");
   fprintf(NBf, "%d\n", nNB);
   for (i=0; i != nNB; i++) fprintf(NBf, "%d\n", NB[i]);
   fclose(NBf);
}

int GetSafeNB(char pre, int MaxL1)
{
   int i, L1, tsize, inc;

   tsize = GetTypeSize(pre);
   inc = ATL_MinMMAlign / tsize;
   if (inc < 4) inc = 4;
   L1 = (GetCacheSize(MaxL1) * 1024) / tsize;
   for (i=inc; i*i < L1; i += inc);
   if (i*i > L1) i -= inc;
   if (pre == 'd' || pre == 's')
   {
      if (i*i == L1) i -= inc;
   }
   else
   {
      if (i*i == L1) i -= 2*inc;
      else i -= inc;
   }
   if (i < 16) i = 16;
   if (i > 80) i = 80;
   return(i);
}

double GetAvg(int n, double tolerance, double *mflop)
{
   int i, j;
   double t0, tavg;
/*
 * Sort results, largest first
 */
   for (i=0; i != n; i++)
   {
      for (j=i+1; j < n; j++)
      {
         if (mflop[i] < mflop[j])
         {
            t0 = mflop[i];
            mflop[i] = mflop[j];
            mflop[j] = t0;
         }
      }
   }

/*
 * Throw out result if it is outside tolerance; rerun if two mflop not within
 * tolerance;  this code assumes n == 3
 */
   if (tolerance*mflop[1] < mflop[0])  /* too big a range in results */
   {
      if (tolerance*mflop[2] < mflop[1]) return(-1.0);
      tavg = (mflop[1] + mflop[2]) / 2.0;
   }
   else if (tolerance*mflop[2] < mflop[0]) tavg = (mflop[0] + mflop[1]) / 2.0;
   else tavg = (mflop[0] + mflop[1] + mflop[2]) / 3.0;

   return(tavg);
}

double mms_case(char pre, int MULADD, int NB, int mu, int nu, int ku, int lat)
{
   char fnam[128], ln[256];
   int i;
   double mflop[NTIM], t0;
   FILE *fp;

   if (ku > NB) ku = NB;
   else if (ku == -1) ku = NB;
   sprintf(fnam, "res/%c%smm%c%c%d_%dx%dx%d_%dx%dx%d_%dx%dx%d%s%s_%dx%d_%d",
           pre, "JIK", 'T', 'N', NB, NB, NB, NB, NB, NB, 0, mu, nu, ku,
           "_a1", "_b1", MULADD, lat, 1);
#if 0
   sprintf(fnam, "res/%c%cNB%d_%dx%dx%d_%d-%d.mflop", LANG, pre, NB, mu, nu,
           ku, MULADD, lat);
#endif
   if (!FileExists(fnam))
   {
      if (pre == 'c' || pre == 'z')
         sprintf(ln,
" make mmcase pre=%c loopO=%s ta=%c tb=%c mb=%d nb=%d kb=%d lda=%d ldb=%d ldc=%d mu=%d nu=%d ku=%d alpha=%d beta=%d muladd=%d lat=%d csA=1 csB=1 csC=2 cleanup=%d\n",
                   pre, "JIK", 'T', 'N', NB, NB, NB, NB, NB, 0, mu, nu, ku,
                   1, 1, MULADD, lat, 1);
      else sprintf(ln,
" make mmcase pre=%c loopO=%s ta=%c tb=%c mb=%d nb=%d kb=%d lda=%d ldb=%d ldc=%d mu=%d nu=%d ku=%d alpha=%d beta=%d muladd=%d lat=%d cleanup=%d\n",
                   pre, "JIK", 'T', 'N', NB, NB, NB, NB, NB, 0, mu, nu, ku,
                   1, 1, MULADD, lat, 1);
      fprintf(stderr, "%s:\n",ln);
      if (system(ln) != 0)
      {
         fprintf(stderr, "ERROR IN COMMAND: %s", ln);
         fprintf(stderr, "   PROPOSED FILENAME: %s\n", fnam);
         sprintf(ln, "rm -f %s\n", fnam);
         system(ln);
         exit(-1);
      }
   }
   assert( (fp = fopen(fnam, "r")) != NULL );
   for (i=0; i != NTIM; i++)
   {
      assert( fscanf(fp, "%lf", &mflop[i]) == 1 );
   }
   fclose(fp);

   t0 = GetAvg(NTIM, TOLERANCE, mflop);
   if (t0 == -1.0)
   {
      fprintf(stderr, "NB=%d, MU=%d, NU=%d, KU=%d: rerun with higher reps; variation exceeds tolerence\n", NB, mu, nu, ku);
      sprintf(ln, "rm -f res/%s\n", fnam);
      system(ln);
      exit(-1);
   }
   fprintf(stdout,
      "\npre=%c, muladd=%d, lat=%d, nb=%d, mu=%d, nu=%d, ku=%d, mflop=%.2f\n",
           pre, MULADD, lat, NB, mu, nu, ku, t0);
   return(t0);
}

double mmcase0(char *nam, char pre, char *loopO, char ta, char tb,
              int M, int N, int K, int mb, int nb, int kb,
              int lda, int ldb, int ldc, int mu, int nu, int ku,
              int muladd, int lat, int beta, int csA, int csB, int csC,
              int FFetch, int ifetch, int nfetch, char *mmnam)
{
   char fnam[128], ln[512], bnam[16], casnam[128], mmcase[128];
   int i, N0, lda2=lda, ldb2=ldb, ldc2=ldc;
   double mflop[NTIM], t0;
   FILE *fp;

   if (lda < 0) { lda2 = -lda; lda = 0; }
   if (ldb < 0) { ldb2 = -ldb; ldb = 0; }
   if (ldc < 0) { ldc2 = -ldc; ldc = 0; }
   if (mmnam) sprintf(mmcase, "mmucase mmrout=%s", mmnam);
   else sprintf(mmcase, "mmcase");
   if (ifetch == -1 || nfetch == -1) { ifetch = mu+nu; nfetch = 1; }
   if (beta == 1) sprintf(bnam, "_b1");
   else if (beta == -1) sprintf(bnam, "_bn1");
   else if (beta == 0) sprintf(bnam, "_b0");
   else sprintf(bnam, "_bX");
   N0 = Mmax(M,N);
   if (N0 < K) N0 = K;
   if (ku > K) ku = K;
   else if (ku == -1) ku = K;
   if (nam)
   {
      strcpy(fnam, nam);
      sprintf(casnam, "casnam=%s", nam);
   }
   else
   {
      sprintf(fnam, "res/%c%smm%c%c%d_%dx%dx%d_%dx%dx%d_%dx%dx%d%s%s_%dx%d_%d",
              pre, loopO, ta, tb, N0, mb, nb, kb, lda, ldb, ldc, mu, nu, ku,
              "_a1", bnam, muladd, lat, 1);
      casnam[0] = '\0';
   }
   if (!FileExists(fnam))
   {
      if (pre == 'c' || pre == 'z')
         sprintf(ln,
" make %s pre=%c loopO=%s ta=%c tb=%c M=%d N=%d K=%d mb=%d nb=%d kb=%d lda=%d ldb=%d ldc=%d lda2=%d ldb2=%d ldc2=%d mu=%d nu=%d ku=%d alpha=%d beta=%d muladd=%d lat=%d cleanup=%d csA=%d csB=%d csC=%d ff=%d if=%d nf=%d %s\n",
                 mmcase,pre, loopO, ta, tb, M, N, K, mb, nb, kb, lda, ldb, ldc,
                 lda2, ldb2, ldc2, mu, nu, ku, 1, beta, muladd, lat, 1,
                 csA, csB, csC, FFetch, ifetch, nfetch, casnam);
      else sprintf(ln,
" make %s pre=%c loopO=%s ta=%c tb=%c M=%d N=%d K=%d mb=%d nb=%d kb=%d lda=%d ldb=%d ldc=%d lda2=%d ldb2=%d ldc2=%d mu=%d nu=%d ku=%d alpha=%d beta=%d muladd=%d lat=%d cleanup=%d ff=%d if=%d nf=%d %s\n",
                   mmcase, pre, loopO, ta, tb, M, N, K, mb, nb, kb, lda, ldb,
                   ldc, lda2, ldb2, ldc2, mu, nu, ku, 1, beta, muladd, lat, 1,
                   FFetch, ifetch, nfetch, casnam);
      fprintf(stderr, "%s:\n",ln);
      if (system(ln) != 0)
      {
/*
 *       User cases, and large leading dimensions can fail to run
 */
         if (mmnam) return(-1.0);  /* user cases can fail to compile */
         if (lda2 != lda || ldb2 != ldb || ldc2 != ldc) return(-1);
         fprintf(stderr, "Error in command: %s", ln);
         sprintf(ln, "rm -f %s\n", fnam);
         system(ln);
         exit(-1);
      }
   }
   fp = fopen(fnam, "r");
   if (!fp) fprintf(stderr, "ERROR: can't find file=%s\n", fnam);
   assert(fp);
   for (i=0; i != NTIM; i++)
   {
      assert(fscanf(fp, "%lf", &mflop[i]) == 1);
   }
   fclose(fp);

   t0 = GetAvg(NTIM, TOLERANCE, mflop);
   if (t0 == -1.0)
   {
      fprintf(stderr,
      "case=%s: rerun with higher reps; variation exceeds tolerence\n", fnam);
      sprintf(ln, "rm -f %s\n", fnam);
      system(ln);
      exit(-1);
   }
   fprintf(stdout,
"\n   pre=%c, loopO=%s, ta=%c tb=%c, mb=%d, nb=%d, kb=%d, lda=%d, ldb=%d, ldc=%d\n",
           pre, loopO, ta, tb, mb, nb, kb, lda, ldb, ldc);
  fprintf(stdout, "   mu=%d, nu=%d, ku=%d, muladd=%d, lat=%d ====> mflop=%f\n",
          mu, nu, ku, muladd, lat, t0);
   return(t0);
}

double mmucase(int ifile, char pre, int nb, int muladd, int lat,
               int mu, int nu, int ku, char *fnam)
{
   char fout[64];
   int iff;

   sprintf(fout, "res/%cuser%d", pre, ifile);
   if (mu == 1 && nu == 1) iff = 1;
   else iff = mu + nu;
   return(mmcase0(fout, pre, "JIK", 'T', 'N', nb, nb, nb, nb, nb, nb,
                  nb, nb, 0, mu, nu, ku, muladd, lat, 1, 1, 1, 2, 0, iff, 1,
                  fnam));
}

enum CW {CleanM=0, CleanN=1, CleanK=2, CleanNot=3};
double mmclean(char pre, enum CW which, char *loopO, char ta, char tb,
               int M, int N, int K, int mb, int nb, int kb,
               int lda, int ldb, int ldc, int mu, int nu, int ku,
               int muladd, int lat, int beta, int csA, int csB, int csC,
               int FFetch, int ifetch, int nfetch)
{
   char nam[128];
   char cwh[3] = {'M', 'N', 'K'};
   sprintf(nam, "res/%cClean%c_%dx%dx%d", pre, cwh[which], M, N, K);
   return(mmcase0(nam, pre, loopO, ta, tb, M, N, K, mb, nb, kb, lda, ldb, ldc,
                  mu, nu, ku, muladd, lat, beta, csA, csB, csC,
                  FFetch, ifetch, nfetch, NULL));
}

double mmcase(char *nam, char pre, char *loopO, char ta, char tb,
              int M, int N, int K, int mb, int nb, int kb,
              int lda, int ldb, int ldc, int mu, int nu, int ku,
              int muladd, int lat, int beta, int csA, int csB, int csC,
              int FFetch, int ifetch, int nfetch)
{
   return(mmcase0(nam, pre, loopO, ta, tb, M, N, K, mb, nb, kb, lda, ldb, ldc,
                  mu, nu, ku, muladd, lat, beta, csA, csB, csC,
                  FFetch, ifetch, nfetch, NULL));
}

int GetGoodLat(int MULADD, int kb, int mu, int nu, int ku, int lat)
{
   int slat, blat, i, ii = mu*nu*ku;
   if (MULADD) return(lat);
   if ( (lat > 1) && (kb > ku) && ((ii/lat)*lat != ii) )  /* lat won't work */
   {
      for (i=lat; i; i--) if ( (ii/i) * i == ii ) break;
      slat = i;
      for (i=lat; i < MAXLAT; i++) if ( (ii/i) * i == ii ) break;
      blat = i;
      if ( (ii/blat)*blat != ii ) blat = slat;
      if (slat < 2) lat = blat;
      else if (lat-slat < blat-lat) lat = slat;
      else lat = blat;
   }
   return(lat);
}

void FindMUNU(int muladd, int lat, int nr, int *MU, int *NU)
/*
 * Find near-square muxnu using nr registers or less
 */
{
   int j, mu, nu;

   if (nr < 1)
   {
      *MU = lat;
      *NU = 1;
      return;
   }
   if (muladd) j = nr;
   else j = nr - lat;
   if (j < 3) mu = nu = 1;
   else
   {
      mu = j + 1;
      for (nu=1; nu*nu < mu; nu++);
      if (nu*nu > mu) nu -= 2;
      else nu--;
      if (nu < 1) mu = nu = 1;
      else
      {
         mu = (nr-nu) / (1+nu);
         if (mu < 1) mu = 1;
      }
      if (mu < nu)
      {
         j = mu;
         mu = nu;
         nu = j;
      }
   }
   *MU = mu;
   *NU = nu;
}

void PutInstLogLine(FILE *fp, int muladd, int lat, int nb,
                    int mu, int nu, int ku, int ForceFetch,
                    int ifetch, int nfetch, double mflop)
{
   fprintf(fp, "%6d  %3d %3d %3d %3d %3d  %5d  %5d  %5d  %7.2lf\n",
           muladd, lat, nb, mu, nu, ku, ForceFetch, ifetch, nfetch, mflop);
}
void PutInstLogFile(FILE *fp, int muladd, int lat, int nb,
                    int mu, int nu, int ku, int ForceFetch,
                    int ifetch, int nfetch, double mflop)
{
   fprintf(fp, "MULADD  LAT  NB  MU  NU  KU  FFTCH  IFTCH  NFTCH    MFLOP\n");
   PutInstLogLine(fp, muladd, lat, nb, mu, nu, ku, ForceFetch,
                  ifetch, nfetch, mflop);
}
void PutInstLogFile1(char *fnam, char pre, int muladd, int lat, int nb,
                    int mu, int nu, int ku, int ForceFetch,
                    int ifetch, int nfetch, double mflop)
{
   FILE *fp;

   fp = fopen(fnam, "w");
   assert(fp);
   PutInstLogFile(fp, muladd, lat, nb, mu, nu, ku, ForceFetch, ifetch, nfetch,
                  mflop);
   fclose(fp);
}

void GetInstLogLine(FILE *fp, int *muladd, int *lat, int *nb,
                    int *mu, int *nu, int *ku, int *ForceFetch,
                    int *ifetch, int *nfetch, double *mflop)
{
   assert(fscanf(fp, " %d %d %d %d %d %d %d %d %d %lf\n",
                 muladd, lat, nb, mu, nu, ku, ForceFetch,
                 ifetch, nfetch, mflop) == 10);
}

void GetInstLogFile(char *nam, char pre, int *muladd, int *lat, int *nb,
                    int *mu, int *nu, int *ku, int *ForceFetch,
                    int *ifetch, int *nfetch, double *mflop)
{
   char ln[128];
   FILE *fp;

   fp = fopen(nam, "r");
   if (fp == NULL) fprintf(stderr, "file %s not found!!\n\n", nam);
   assert(fp);
   fgets(ln, 128, fp);
   GetInstLogLine(fp, muladd, lat, nb, mu, nu, ku, ForceFetch,
                  ifetch, nfetch, mflop);
   fclose(fp);
}



void CreateFinalSumm(char pre, int muladd, int lat, int nb, int mu, int nu,
                     int ku, int Ff, int If, int Nf, double gmf)
{
   char ln[64], auth[65];
   FILE *fp, *fp0;
   int icase, unb;
   double umf;

   sprintf(ln, "res/%cMMRES", pre);
   fp = fopen(ln, "w");
   PutInstLogFile(fp, muladd, lat, nb, mu, nu, ku, Ff, If, Nf, gmf);
   sprintf(ln, "res/%cuMMRES", pre);
   fp0 = fopen(ln, "r");
   assert(fp0);
   assert(fgets(ln, 64, fp0));
   assert(fscanf(fp0, " %d %d %lf \"%[^\"]\" \"%[^\"]", &icase, &unb, &umf,
                 ln, auth) == 5);
   fclose(fp0);
   fprintf(fp, "\nICASE  NB    MFLOP  ROUT  AUTHOR\n");
   fprintf(fp, "%5d %3d %8.2f  \"%.63s\" \"%.63s\"\n", icase, unb, umf,
           ln, auth);
   fclose(fp);
}


void FindFetch(char ta, char tb, char pre, int mb, int nb, int kb,
               int mu, int nu, int ku, int muladd, int lat,
               int *FFetch0, int *ifetch0, int *nfetch0)
/*
 * See what fetch patterns are appropriate
 */
{
   char fnam[128];
   const int nelts = mu+nu;
   int csA=1, csB=1, csC=1, nleft, i, j;
   int ifetch = mu+nu, nfetch = 1;
   double mf, mf0;

   if (pre == 'c' || pre == 'z') csC = 2;

   mf0 = mmcase(NULL, pre, "JIK", ta,  tb, mb, nb, kb, mb, nb, kb,
                kb, kb, 0, mu, nu, ku, muladd, lat, 0, csA, csB, csC,
                0, ifetch, nfetch);

   for (i=2; i < nelts; i++)
   {
      nleft = nelts - i;
      for (j=1; j <= nleft; j++)
      {
         sprintf(fnam, "res/%cMMfetch%d_%d", pre, i, j);
         mf = mmcase(fnam, pre, "JIK", ta,  tb, mb, nb, kb, mb, nb, kb,
                     kb, kb, 0, mu, nu, ku, muladd, lat, 0, csA, csB, csC,
                     0, i, j);
         if (mf > mf0)
         {
            mf = mf0;
            ifetch = i;
            nfetch = j;
         }
      }
   }
/*
 * See if prefetching good idea for beta=0 case
 */
   sprintf(fnam, "res/%cMM_b0", pre);
   mf0 = mmcase(fnam, pre, "JIK", ta,  tb, mb, nb, kb, mb, nb, kb,
                kb, kb, 0, mu, nu, ku, muladd, lat, 0, csA, csB, csC,
                0, ifetch, nfetch);

   sprintf(fnam, "res/%cMM_b0_pref", pre);
   mf = mmcase(fnam, pre, "JIK", ta,  tb, mb, nb, kb, mb, nb, kb,
               kb, kb, 0, mu, nu, ku, muladd, lat, 0, csA, csB, csC,
               1, ifetch, nfetch);

   *FFetch0 = (mf > mf0);
   *ifetch0 = ifetch;
   *nfetch0 = nfetch;
   fprintf(stdout, "\n\nFORCEFETCH=%d, IFETCH = %d, NFETCH = %d\n\n",
           *FFetch0, *ifetch0, *nfetch0);
}


void searchmu_nu(char pre, int nb, int maxreg, int Fku, int muladd, int LAT,
                 int NO1D, double *mfB, int *nbB, int *muB, int *nuB, int *kuB,
                 int *latB)
{
   int i, j, lat, ku, nr2, nreg=Mmin(nb, maxreg);
   double mf;

   for (i=1; i <= nreg; i++)
   {
      nr2 = maxreg / i;
      if (nr2 > nb) nr2 = nb;
      for (j=1; j <= nreg; j++)
      {
         if ( (((i==1) && (j > 4)) || ((j==1) && (i > 4))) && NO1D) continue;
         if (Fku == -1 || (!Fku) ) ku = nb;
         else if (Fku) ku = Fku;
         if (ku != nb) lat = GetGoodLat(muladd, nb, i, j, ku, LAT);
         else lat = LAT;
         if (j*i+j+i+(!muladd)*lat > maxreg) continue;  /* not enough regs */
         mf = mms_case(pre, muladd, nb, i, j, ku, lat);
         if (mf > *mfB)
         {
            *mfB = mf;
            *nbB = nb;
            *muB = i;
            *nuB = j;
            *kuB = ku;
            *latB = LAT;
         }
         if (!Fku)
         {
            lat = GetGoodLat(muladd, nb, i, j, 1, LAT);
            mf = mms_case(pre, muladd, nb, i, j, 1, lat);
            if (mf > *mfB)
            {
               *mfB = mf;
               *nbB = nb;
               *muB = i;
               *nuB = j;
               *kuB = 1;
               *latB = lat;
            }
         }
      }
   }
}

void FindKU(char pre, int muladd, int LAT, int nb, int mu, int nu,
            double *mfB, int *kuB, int *latB)
/*
 * For best case, try various ku's
 */
{
   int k, lat;
   double mf;

   fprintf(stderr, "Confirming K-loop unrollings for chosen NB:\n");
   mf = mms_case(pre, muladd, nb, mu, nu, nb, LAT);
   if (mf > *mfB)
   {
      *kuB = nb;
      *mfB = mf;
      *latB = LAT;
   }
   for (k=1; k < nb; k += 4)
   {
      if (k == 5) k = 4;
      if (k > nb/2) k = nb;
      lat = GetGoodLat(muladd, nb, mu, nu, k, *latB);
      mf = mms_case(pre, muladd, nb, mu, nu, k, lat);
      if (mf > *mfB)
      {
         *latB = lat;
         *kuB = k;
         *mfB = mf;
      }
   }
}

void FindLAT(char pre, int maxlat, int nb, int muladd, int mu, int nu, int ku,
             double *mfB, int *latB)

{
   int i, lat;
   double mf;

   fprintf(stderr, "\nConfirming latency factors for chosen parameters:\n");
   for (i=1; i <= maxlat; i++)
   {
      lat = GetGoodLat(muladd, nb, mu, nu, ku, i);
      if (lat == i)
      {
         mf = mms_case(pre, muladd, nb, mu, nu, ku, lat);
         if (mf > *mfB)
         {
            *mfB = mf;
            *latB = i;
         }
      }
   }
   fprintf(stderr, "\n\n   Best latency factor=%d\n\n", *latB);
}

int CheckUser(char pre, double adv, double gmf, int gnb, double *umf)
/*
 * Checks if user case is better than generated, and if so, return umb
 */
{
   FILE *fp;
   char fnam[128];
   int i, unb;
   double umflop;
   sprintf(fnam, "res/%cuMMRES", pre);
   if (!FileExists(fnam))  /* need to run user search */
   {
      sprintf(fnam, "make RunUMMSearch pre=%c nb=%d\n", pre, gnb);
      assert(system(fnam) == 0);
      sprintf(fnam, "res/%cuMMRES", pre);
   }
   fp = fopen(fnam, "r");
   assert(fp);
   assert(fgets(fnam, 128, fp));
   assert(fgets(fnam, 128, fp));
   fclose(fp);
   sscanf(fnam, " %d %d %lf", &i, &unb, &umflop);
   if (i >= 0 && umflop < 0.0)  /* need to retime */
   {
      sprintf(fnam, "make RunUMMSearch pre=%c nb=0\n", pre);
      assert(system(fnam) == 0);
      sprintf(fnam, "res/%cuMMRES", pre);
      fp = fopen(fnam, "r");
      assert(fp);
      assert(fgets(fnam, 128, fp));
      assert(fgets(fnam, 128, fp));
      fclose(fp);
      sscanf(fnam, " %d %d %lf", &i, &unb, &umflop);
   }
   fprintf(stdout, "\nBEST USER CASE: NB=%d, MFLOP=%.2f\n", unb, umflop);
   if (umf) *umf = umflop;
   if (adv*gmf > umflop || i < 1) return(gnb);
   else return(unb);
}

int GetNO1D(char pre, int nreg, int nb, int MULADD, int LAT)
{
   int lat, NO1D=0;
   double mf0, mf1, mf;

   if (pre == 'z') pre = 'd';
   else if (pre == 'c') pre = 's';

   lat = GetGoodLat(MULADD, nb, 3, 3, 1, LAT);
   if (nreg >= 15+(!MULADD)*Mmax(LAT,lat))
   {
      mf0 = mms_case(pre, MULADD, nb, 3, 3, 1, lat);
      mf1 = mms_case(pre, MULADD, nb, 3, 3, nb, LAT);
      mf = Mmax(mf1, mf0);
      mf0 = mms_case(pre, MULADD, nb, 9, 1, 1, lat);
      if (mf0 > mf) NO1D = 0;
      else if (mms_case(pre, MULADD, nb, 9, 1, nb, LAT) > mf) NO1D = 0;
      else if (mms_case(pre, MULADD, nb, 1, 9, nb, LAT) > mf) NO1D = 0;
      else if (mms_case(pre, MULADD, nb, 1, 9, 1, lat) > mf) NO1D = 0;
      else NO1D = 1;
   }
   return(NO1D);
}

void gmmsearch(char pre, int MULADD, int Fku, int nNBs, int *NBs, int nreg,
               int LAT, int Fnb)
/*
 * Does real generated mmsearch
 */
{
   int latB, muB, nuB, kuB, nbB;
   int i, j, k, NB, TEST_MU, TEST_NU, ku, nb, lat=LAT, nNB=nNBs, NO1D=0;
   int FFetch, ifetch, nfetch, muladd;
   double mf, mfB;
   char ln[32];
   FILE *fp;

   sprintf(ln, "res/%cgMMRES", pre);
   if (FileExists(ln)) /* already have needed result */
   {
      GetInstLogFile(ln, pre, &muladd, &lat, &nb, &muB, &nuB, &kuB, &FFetch,
                     &ifetch, &nfetch, &mf);
      if (mf <= 0.0)
      {
         mf = mmcase(NULL, pre, "JIK", 'T', 'N', nb, nb, nb, nb, nb, nb,
                     nb, nb, 0, muB, nuB, kuB, muladd, lat, 1, 1, 1, 2,
                     FFetch, ifetch, nfetch);
         PutInstLogFile1(ln, pre, muladd, lat, nb, muB, nuB, kuB, FFetch,
                         ifetch, nfetch, mf);
      }
      return;
   }
/*
 * Try not to tempt fate by using all registers
 */
   if (nreg > 16) i = nreg-2;
   else i = nreg;
   FindMUNU(MULADD, lat, i, &TEST_MU, &TEST_NU);
/*
 * First, find a good NB
 */
   mfB = 0.0;
   fprintf(stderr, "Doing initial NB search:\n");
   i = 0;
   for (k=0; k != nNBs; k++)
   {
      NB = NBs[k];
      ku = NB;
      mf = mms_case(pre, MULADD, NB, TEST_MU, TEST_NU, ku, lat);
      if (mf > mfB)
      {
         mfB = mf;
         nbB = NB;
         muB = TEST_MU;
         nuB = TEST_NU;
         kuB = ku;
         latB = lat;
         i = k;
      }
      if (Fku == 0)  /* try no K-loop unrolling */
      {
         lat = GetGoodLat(MULADD, NB, TEST_MU, TEST_NU, 1, LAT);
         mf = mms_case(pre, MULADD, NB, TEST_MU, TEST_NU, 1, lat);
         if (mf > mfB)
         {
            mfB = mf;
            nbB = NB;
            muB = TEST_MU;
            nuB = TEST_NU;
            kuB = 1;
            latB = lat;
            i = k;
         }
      }
      if (i)
      {
         j = NBs[i];
         NBs[i] = NBs[0];
         NBs[0] = j;
      }
      fprintf(stderr, "NB=%d selected:\n", NBs[0]);
   }

   if (!Fnb) nNB = 1;
   if (MULADD)
      fprintf(stderr, "\nCombined multiply add, latency factor=%d, NB=%d ku=%d, chosen; initial MFLOP=%f.  Beginning unroll search:\n", latB, NBs[0], kuB, mfB);
   else
      fprintf(stderr, "\nSeparate multiply and add, latency factor=%d, NB=%d ku=%d, chosen; initial MFLOP=%f.  Beginning unroll search:\n", latB, NBs[0], kuB, mfB);

   NO1D = GetNO1D(pre, nreg, NBs[0], MULADD, LAT);
   if (NO1D) fprintf(stderr, "\n\nSkipping most 1D cases\n\n");
   else fprintf(stderr, "\n\nTiming 1D cases\n\n");
   for (k=0; k != nNB; k++)
   {
      NB = NBs[k];
      searchmu_nu(pre, NB, nreg, Fku, MULADD, LAT, NO1D,
                  &mfB, &nbB, &muB, &nuB, &kuB, &latB);
   }
   fprintf(stderr, "\n\nBest case so far: nb=%d, mu=%d, nu=%d, ku=%d, lat=%d; MFLOPS=%f.\n",
           nbB, muB, nuB, kuB, latB, mfB);
   fprintf(stderr, "Trying various other NB and KU settings:\n\n");
/*
 * If we haven't checked all permutations, try other blocking factors
 */
   nb = nbB;
   if (!Fnb)
   {
      if (nNBs > 1) fprintf(stderr, "Trying various blocking factors:\n");
      mf = mms_case(pre, MULADD, NBs[0], muB, nuB, kuB, latB);
      for (k=0; k < nNBs; k++)
      {
         NB = NBs[k];
         if (Fku == -1) ku = NB;
         else if (Fku) ku = Fku;
         else if (kuB == nbB) ku = NB;
         else ku = kuB;
         if (ku != NB) lat = GetGoodLat(MULADD, NB, muB, nuB, ku, latB);
         else lat = latB;
         mf = mms_case(pre, MULADD, NB, muB, nuB, ku, lat);
         if (mf > mfB)
         {
            kuB = ku;
            mfB = mf;
            nbB = NB;
            latB = lat;
         }
      }
   }
   if (nb != nbB) fprintf(stderr, "\nNew block factor of %d chosen!!\n\n", nbB);
   NB = nbB;

/*
 * Try all ku's, and then valid latencies
 */
   FindKU(pre, MULADD, LAT, nbB, muB, nuB, &mfB, &kuB, &latB);
   FindLAT(pre, MAXLAT, nbB, MULADD, muB, nuB, kuB, &mfB, &latB);

/*
 * Make sure MULADD is correct
 */
   lat = GetGoodLat(!MULADD, nbB, muB, nuB, kuB, latB);
   mf = mms_case(pre, !MULADD, nbB, muB, nuB, kuB, lat);
   if (mf > mfB)
   {
      fprintf(stderr, "\n\nMULADD MAY BE WRONG!!, old=%f, new=%f\n", mfB, mf);
   }
/*
 * Try various fetch patterns
 */
   FindFetch('T', 'N', pre, nbB, nbB, nbB, muB, nuB, kuB, MULADD, latB,
             &FFetch, &ifetch, &nfetch);
   fprintf(stdout,
   "BEST GENERATED CASE: nb=%d, ma=%d, lat=%d mu=%d, nu=%d, ku=%d -- %.2f\n",
           nbB, MULADD, latB, muB, nuB, kuB, mfB);
   sprintf(ln, "res/%cgMMRES", pre);
   PutInstLogFile1(ln, pre, MULADD, latB, nbB, muB, nuB, kuB,
                   FFetch, ifetch, nfetch, mfB);
}

void mmsearch(char pre, int MULADD, int Fku, int nNBs, int *NBs, int nreg,
              int LAT, int Fnb)
{
   int latB, muB, nuB, kuB, nbB;
   int muladd, nb, ifetch, nfetch, FFetch;
   int i, j, k, NB;
   int NO1D;
   int umb, unb, ukb, ma;
   double mfB, gmf;
   char fnam[128];
   FILE *fp;

   sprintf(fnam, "res/%cMMRES", pre);
   if (FileExists(fnam)) /* already have result */
   {
      GetInstLogFile(fnam, pre, &muladd, &latB, &nb, &muB, &nuB, &kuB, &FFetch,
                     &ifetch, &nfetch, &mfB);
      if (mfB <= 0.0)
      {
         mfB = mmcase(NULL, pre, "JIK", 'T', 'N', nb, nb, nb, nb, nb, nb,
                      nb, nb, 0, muB, nuB, kuB, muladd, latB, 1, 1, 1, 2,
                      FFetch, ifetch, nfetch);
         gmmsearch(pre, muladd, Fku, nNBs, NBs, nreg, latB, Fnb);
         nbB = nb = CheckUser(pre, 1.02, mfB, nb, NULL);
         CreateFinalSumm(pre, muladd, latB, nb, muB, nuB, kuB, FFetch, ifetch,
                         nfetch, mfB);
      }
      else nbB = CheckUser(pre, 1.02, mfB, nb, NULL);
      sprintf(fnam, "res/%cNB", pre);
      fp = fopen(fnam, "w");
      fprintf(fp, "%d\n%d\n", 1, nbB);
      fclose(fp);
      return;
   }
   gmmsearch(pre, MULADD, Fku, nNBs, NBs, nreg, LAT, Fnb);
   sprintf(fnam, "res/%cgMMRES", pre);
   GetInstLogFile(fnam, pre, &muladd, &latB, &nbB, &muB, &nuB, &kuB, &FFetch,
                  &ifetch, &nfetch, &mfB);
   gmf = mfB;
   nb = CheckUser(pre, 1.02, mfB, nbB, NULL);
   if (nb != nbB)
   {
      if (kuB == nbB) kuB = nb;
      nbB = nb;
      if (nb % muB || nb % nuB)
      {
         NO1D = GetNO1D(pre, nreg, nb, MULADD, LAT);
         searchmu_nu(pre, nb, nreg, Fku, MULADD, LAT, NO1D,
                     &mfB, &nb, &muB, &nuB, &kuB, &latB);
      }
      FindKU(pre, MULADD, LAT, nbB, muB, nuB, &mfB, &kuB, &latB);
      FindLAT(pre, MAXLAT, nbB, MULADD, muB, nuB, kuB, &mfB, &latB);
      FindFetch('T', 'N', pre, nbB, nbB, nbB, muB, nuB, kuB, MULADD, latB,
                &FFetch, &ifetch, &nfetch);
   }
/*
 * Save NB we've found
 */
   sprintf(fnam, "res/%cNB", pre);
   fp = fopen(fnam, "w");
   fprintf(fp, "%d\n%d\n", 1, nbB);
   fclose(fp);
/*
 * Save best case parameters we have found
 */
   CreateFinalSumm(pre, MULADD, latB, nbB, muB, nuB, kuB, FFetch, ifetch,
                   nfetch, mfB);
}

void FindNC_0(char ta, char tb, char pre, int N, int mb, int nb, int kb,
              int mu, int nu, int ku, int muladd, int lat,
              int FFetch, int ifetch, int nfetch)
{
   int kuB=ku, latB=lat, lat0=lat, kb0=kb;
   int i, j, k, csA=1, csB=1, csC=1, kmax;
   double mf0, mf;
   char fnam[128];
   FILE *fp;

   sprintf(fnam, "res/%cbest%c%c_%dx%dx%d", pre, ta, tb, mb, nb, kb);
   if (FileExists(fnam)) /* default already exists */
   {
      GetInstLogFile(fnam, pre, &muladd, &lat, &nb, &mu, &nu, &ku,
                     &FFetch, &ifetch, &nfetch, &mf);
      if (mf < 0.0) /* need to retime */
      {
         mf = mmcase(NULL, pre, "JIK", ta, tb, nb, nb, nb,
                     nb, nb, nb, 0, 0, 0, mu, nu, ku, muladd, lat, 1,
                     1, 1, csC, FFetch, ifetch, nfetch);
         PutInstLogFile1(fnam, pre, muladd, lat, nb, mu, nu, ku,
                         FFetch, ifetch, nfetch, mf);
      }
      return;
   }
   if (pre == 'c' || pre == 'z') csA = csB = csC = 2;
   assert(N > 0);
   if (kb == 0)
   {
      kb0 = 100000;
      if ((mb*nb)/lat != lat) lat0 = GetGoodLat(muladd, kb0, mu, nu, 1, lat);
   }
   k = 1024 / (mu*nu);
   for (kmax=4; kmax*kmax < k; kmax += 4);
   if (pre == 'd' || pre == 's') kmax *= 2;
   if (kmax >= N) kmax = N;
   else if (kmax > N/2) kmax = N/2;
   if (kb == 0) kuB = k = Mmin(ku,kmax);
   else k = ku;
/*
 * Find best non-cleanup case
 */
   mf0 = mmcase(NULL, pre, "JIK", ta, tb, N, N, N, mb, nb, kb, 0, 0, 0,
                mu, nu, k, muladd, lat0, 1, csA, csB, csC,
                FFetch, ifetch, nfetch);
   latB = lat0;
/*
 * If kb is not known, try all available K unrollings; for large mu*nu*N
 * combinations, don't try maximal unrollings in order to avoid having
 * the compiler run out of space trying to optimize
 */
   if (kb == 0)
   {
      for (k=1; k < kmax; k += 4)
      {
         if (k == 5) k = 4;
         if (k > N/2) k = kmax;
         j = k;
         if (kb == 0) j = 1;
         i = GetGoodLat(muladd, kb0, mu, nu, j, lat);
         mf = mmcase(NULL, pre, "JIK", ta, tb, N, N, N, mb, nb, kb, 0, 0, 0,
                     mu, nu, k, muladd, i, 1, csA, csB, csC,
                     FFetch, ifetch, nfetch);
         if (mf > mf0)
         {
            mf0 = mf;
            kuB = k;
            latB = i;
         }
      }
   }
/*
 * If K is known, try only the most common unrollings
 */
   else
   {
      i = GetGoodLat(muladd, kb0, mu, nu, 1, lat);
      mf = mmcase(NULL, pre, "JIK", ta, tb, N, N, N, mb, nb, kb, 0, 0, 0,
                  mu, nu, 1, muladd, i, 1, csA, csB, csC,
                  FFetch, ifetch, nfetch);
      if (mf > mf0)
      {
         mf0 = mf;
         kuB = 1;
         latB = i;
      }
      i = GetGoodLat(muladd, kb0, mu, nu, 4, lat);
      mf = mmcase(NULL, pre, "JIK", ta, tb, N, N, N, mb, nb, kb, 0, 0, 0,
                  mu, nu, 4, muladd, i, 1, csA, csB, csC,
                  FFetch, ifetch, nfetch);
      if (mf > mf0)
      {
         mf0 = mf;
         kuB = 4;
         latB = i;
      }
      mf = mmcase(NULL, pre, "JIK", ta, tb, N, N, N, mb, nb, kb, 0, 0, 0,
                  mu, nu, kb, muladd, lat, 1, csA, csB, csC,
                  FFetch, ifetch, nfetch);
      if (mf > mf0)
      {
         mf0 = mf;
         kuB = kb;
         latB = lat;
      }
   }
/*
 * Try various latencies
 */
   if (kb) i = kuB;
   else i = 1;
   for (k=2; k < 9; k++)
   {
      if (((mu*nu*i)/k)*k == mu*nu*i)
      {
         mf = mmcase(NULL, pre, "JIK", ta, tb, N, N, N, mb, nb, kb, 0, 0, 0,
                     mu, nu, kuB, muladd, k, 1, csA, csB, csC,
                     FFetch, ifetch, nfetch);
         if (mf > mf0)
         {
            mf0 = mf;
            latB = k;
         }
      }
   }
   fprintf(stdout, "BEST for %c%c_%dx%dx%d: mflop=%.2f\n",
           ta, tb, mb, nb, kb, mf0);
   fprintf(stdout,
           "pre=%c ta=%c tb=%c nb=%d mu=%d nu=%d ku=%d muladd=%d lat=%d\n",
           pre, ta, tb, nb, mu, nu, kuB, muladd, latB);
   sprintf(fnam, "res/%cbest%c%c_%dx%dx%d", pre, ta, tb, mb, nb, kb);
   fp = fopen(fnam, "w");
   assert(fp);
   PutInstLogFile(fp,muladd, latB, N, mu, nu, kuB, FFetch, ifetch, nfetch, mf0);
   fclose(fp);
}

void FindNC0(char ta, char tb, char pre, int nb, int mu, int nu, int ku,
             int muladd, int lat, int FFetch, int ifetch, int nfetch)
{
   FindNC_0(ta, tb, pre, nb, nb, nb, nb, mu, nu, ku, muladd, lat, FFetch,
            ifetch, nfetch);
   FindNC_0(ta, tb, pre, nb, 0, 0, nb, mu, nu, ku, muladd, lat, FFetch,
            ifetch, nfetch);
   FindNC_0(ta, tb, pre, nb, 0, 0, 0, mu, nu, ku, muladd, lat, FFetch,
            ifetch, nfetch);
}

double NCcase(char pre, int nb, int mu, int nu, int ku, int ma, int lat,
              int ffetch, int ifetch, int nfetch)
{
   double mf;
   int ld=Mmax(1000,nb), cs=1;
   char fnam[128];

   if (pre == 'c' || pre == 'z') cs = 2;
   do
   {
      sprintf(fnam, "res/%cNCNB%d_%d", pre, nb, ld);
      mf = mmcase(fnam, pre, "JIK", 'N', 'N', nb, nb, nb, nb, nb, nb,
                  -ld, nb, nb, mu, nu, ku, ma, lat, 1, cs, cs, cs,
                  ffetch, ifetch, nfetch);
      ld -= 10;
   }
   while (mf <= 0.0 && ld >= nb);
   assert(mf > 0.0);
   return(mf);
}

int FindNoCopyNB(char pre, int nb, int mu, int nu, int ku0, int muladd, int lat,
                 int FFetch, int ifetch, int nfetch)
/*
 * See if a smaller blocking factor is needed for no-copy
 */
{
   char fnam[128];
   int i, ku, nbB=nb, csA=2, csB=2, csC=2, kuIsNB=0;
   double mf, mfB, mf0;
   const double dmul = 1.02;
   FILE *fp;

   if (ku0 == nb) kuIsNB = 1;
   sprintf(fnam, "res/%cNCNB", pre);
   if (!FileExists(fnam))
   {
      mfB = NCcase(pre, nb, mu, nu, ku0, muladd, lat, FFetch, ifetch, nfetch);
      mfB *= dmul;
      mf0 = mfB;
      for (i=nb-4; i >= 16; i -= 4)
      {
         if (kuIsNB) ku = i;
         else ku = Mmin(i, ku0);
         mf = NCcase(pre, i, mu, nu, ku, muladd, lat, FFetch, ifetch, nfetch);
         if (1.2*mf < mfB) break; /* stop search after 20% slowdown */
         if (nb%i == 0) mf *= dmul; /* give modest bonus to mults of nb */
         if (mf > mfB)
         {
            mfB = mf;
            nbB = i;
         }
      }
      fp = fopen(fnam, "w");
      assert(fp);
      fprintf(fp, "%d\n", nbB);
   }
   else
   {
      fp = fopen(fnam, "r");
      mf0 = mfB = -1.0;
      fscanf(fp, "%d\n", &nbB);
   }
   fclose(fp);
   fprintf(stdout, "\n%cNB = %d (%.2f), No copy %cNB = %d (%.2f)\n\n",
           pre, nb, mf0, pre, nbB, mfB);
   return(nbB);
}

void FindNoCopy(char pre)
{
   char ln[128];
   int nb, mu, nu, ku, muladd, lat, FFetch, ifetch, nfetch, i;
   double mf;
   FILE *fp;

   sprintf(ln, "res/%cMMRES", pre);
   GetInstLogFile(ln, pre, &muladd, &lat, &nb, &mu, &nu, &ku,
                  &FFetch, &ifetch, &nfetch, &mf);
   nb = CheckUser(pre, 1.02, mf, nb, NULL);
   nb = FindNoCopyNB(pre, nb, mu, nu, ku, muladd, lat, FFetch, ifetch, nfetch);
   ku = Mmin(ku, nb);
   FindNC0('N', 'N', pre, nb, mu, nu, ku, muladd, lat, FFetch, ifetch, nfetch);
   FindNC0('N', 'T', pre, nb, mu, nu, ku, muladd, lat, FFetch, ifetch, nfetch);
   FindNC0('T', 'N', pre, nb, mu, nu, ku, muladd, lat, FFetch, ifetch, nfetch);
   FindNC0('T', 'T', pre, nb, mu, nu, ku, muladd, lat, FFetch, ifetch, nfetch);
}

void FindCleanupK(char pre, int nb, int mu, int nu, int ku0, int muladd,
                  int lat0, int FFetch, int ifetch, int nfetch)
{
   char fnam[256];
   int genlat, genku, speclat, ku, kumax;
   int kb, beta, csC;
   double mf, genmf, specmf;
   int i, TimeIt=0;
   FILE *fp;

   i = 1024 / (mu*nu);
   for (kumax=4; kumax*kumax < i; kumax += 4);
   if (pre == 'd' || pre == 's') kumax *= 2;
   if (kumax >= nb) kumax = nb;
   else if (kumax > nb/2) kumax = nb/2;
   if (ifetch == -1 || nfetch == -1) { ifetch = mu+nu; nfetch = 1; }
   if (pre == 's' || pre == 'd')
   {
      csC = 1;
      beta = 1;
   }
   else
   {
      csC = 2;
      beta = 8;
   }
   sprintf(fnam, "res/%cCleanK", pre);
   if (FileExists(fnam)) /* file already there */
   {
      fp = fopen(fnam, "r");
      assert(fgets(fnam, 256, fp));
      assert(fscanf(fp, " %d", &kb) == 1);
      fclose(fp);
      if (kb > 0 && kb != nb) TimeIt = 1;
      sprintf(fnam, "res/%cCleanK", pre);
   }
   else TimeIt = 1;
   if (TimeIt)
   {
      fp = fopen(fnam, "w");
      assert(fp);
      fprintf(fp, " KB  MULADD  LAT  NB  MU  NU  KU  FFTCH  IFTCH  NFTCH  GEN-MFLOP  SPC-MFLOP\n");

      for (kb = nb; kb; kb--)
      {
         ku = Mmin(ku0, kb);
         sprintf(fnam, "res/%cKB_%d", pre, kb);
         speclat = GetGoodLat(muladd, kb, mu, nu, ku, lat0);
         specmf = mmcase(fnam, pre, "JIK", 'T', 'N', nb, nb, kb, 0, 0,
                         kb, kb, kb, 0, mu, nu, ku, muladd, speclat, beta,
                         1, 1, csC, FFetch, ifetch, nfetch);

         sprintf(fnam, "res/%cKB_0_%d", pre, ku);
         genlat = GetGoodLat(muladd, 8000, mu, nu, 1, lat0);
         genku = Mmin(kumax, ku);
         genmf = mmcase(fnam,pre, "JIK", 'T', 'N', nb, nb, kb, 0, 0, 0, 0, 0, 0,
                        mu, nu, genku, muladd, genlat, beta, 1, 1, csC,
                        FFetch, ifetch, nfetch);
         if (ku != 1)  /* always try ku == 1 for general case */
         {
            sprintf(fnam, "res/%cKB_0_1", pre);
            mf = mmcase(fnam,pre, "JIK", 'T', 'N', nb, nb, kb, 0, 0, 0, 0, 0, 0,
                        mu, nu, 1, muladd, genlat, beta, 1, 1, csC,
                        FFetch, ifetch, nfetch);
            if (mf > genmf) { genku = 1; genmf = mf; }
         }
         if (1.01 * genmf > specmf) break;
         fprintf(fp,
            "%3d  %6d  %3d %3d %3d %3d %3d  %5d  %5d  %5d  %9.2lf  %9.2lf\n",
                 kb, muladd, speclat, nb, mu, nu, ku, FFetch, ifetch, nfetch,
                 specmf, genmf);
         fflush(fp);
      }
      fprintf(fp,
              "%3d  %6d  %3d %3d %3d %3d %3d  %5d  %5d  %5d  %9.2lf  %9.2lf\n",
              0, muladd, genlat, nb, mu, nu, genku, FFetch, ifetch, nfetch,
              specmf, genmf);
      fclose(fp);
   }
}

FindCleanupMN(char pre, char cwh, int nb, int mu, int nu, int ku,
              int muladd, int lat, int FFetch, int ifetch, int nfetch)
{
   char fnam[128];
   int nnb=nb, beta=1, csC=1, TimeIt=0;
   int Mb=nb, Nb=nb;
   int mu0, nu0, ku0, ma0, lat0, ff0, if0, nf0;
   double mf;
   FILE *fp;

   if (cwh == 'M') Mb = 0;
   else Nb = 0;
   if (ifetch == -1 || nfetch == -1) { ifetch = mu+nu; nfetch = 1; }
   if (pre == 'c' || pre == 'z')
   {
      beta = 8;
      csC = 2;
   }
   sprintf(fnam, "res/%cClean%c", pre, cwh);
   if (FileExists(fnam))
   {
      GetInstLogFile(fnam, pre, &ma0, &lat0, &nnb, &mu0, &nu0, &ku0, &ff0,
                     &if0, &nf0, &mf);
      if (nnb != nb || mf <= 0.0) TimeIt = 1;
   }
   else TimeIt = 1;
   if (TimeIt)
   {
      mf = mmcase(NULL, pre, "JIK", 'T', 'N', nb, nb, nb, Mb, Nb, nb, nb, nb, 0,
                  mu, nu, ku, muladd, lat, beta, 1, 1, csC,
                  FFetch, ifetch, nfetch);
      fp = fopen(fnam, "w");
      assert(fp);
      PutInstLogFile(fp, muladd, lat, nb, mu, nu, ku,
                     FFetch, ifetch, nfetch, mf);
      fclose(fp);
   }
}

typedef struct CleanCase CLEANCASE;
struct CleanCase
{
   double mflop;
   CLEANCASE *next;
   int imult, icase, fixed, nb, nb0, nb1, nb2;
};

void PrintCleanCases(CLEANCASE *cp)
{
   for (; cp; cp = cp->next)
   {
      fprintf(stdout,
              "imult=%d, icase=%d, fixed=%d, nb=%d, %d,%d,%d, mflop=%.2f\n",
              cp->imult, cp->icase, cp->fixed, cp->nb, cp->nb0, cp->nb1,
              cp->nb2, cp->mflop);
   }
   fprintf(stdout, "\n");
}
CLEANCASE *GetUserCleanup(char pre, int nb, enum CW which)
/*
 * Read in user clean file
 */
{
   FILE *fp;
   CLEANCASE *cp, *cp0;
   int i, n;
   char cwh[3] = {'M', 'N', 'K'};
   char ln[128];

   sprintf(ln, "res/%cuClean%c", pre, cwh[which]);
   if (!FileExists(ln))
   {
      sprintf(ln, "make RunUMMClean pre=%c nb=%d which=%c\n",
              pre, nb, tolower(cwh[which]));
      assert(system(ln) == 0);
      sprintf(ln, "res/%cuClean%c", pre, cwh[which]);
   }
   fp = fopen(ln, "r");
   assert(fp);
   assert(fgets(ln, 128, fp));
   assert(fgets(ln, 128, fp));
   sscanf(ln, " %d", &n);
   if (n < 1) return(NULL);
   cp0 = cp = malloc(sizeof(CLEANCASE));
   assert(cp0);
   for (i=0; i < n; i++)
   {
      assert(fgets(ln, 128, fp));
      sscanf(ln, " %d %d %d %d %d %d %d %lf", &cp->imult, &cp->icase,&cp->fixed,
             &cp->nb, &cp->nb0, &cp->nb1, &cp->nb2, &cp->mflop);
      if (i != n-1)
      {
         cp->next = malloc(sizeof(CLEANCASE));
         assert(cp->next);
         cp = cp->next;
      }
      else cp->next = NULL;
   }
   fclose(fp);
   return(cp0);
}

int *GetKBs(char pre, int nb)
/*
 * returns nb+1 length vector, KB[i] is KB & lda of KB Cleanup; 0 means var
 */
{
   FILE *fp;
   int k, *KB;
   char ln[128];

   sprintf(ln, "res/%cCleanK", pre);
   fp = fopen(ln, "r");
   assert(fp);
   assert(fgets(ln, 128, fp));  /* skip titles */
   KB = malloc((nb+1)*sizeof(int));
   assert(KB);
   for (k=nb; k; k--)
   {
      if (fgets(ln, 128, fp)) { assert(sscanf(ln, " %d", KB+k)==1); }
      else break;
   }
   for(; k; k--) KB[k] = 0;
   return(KB);
}

double RebuttUserKCase(char pre, int nb, int mu, int nu, int ku, int ma,
                       int lat, int FF, int iff, int nf, int *KBs, int *NBs)
{
   double mf, mf0=0.0;
   int K, csC, i, ld, iku, ilat;

   if (pre == 'c' || pre == 'z') csC = 2;
   else csC = 1;

   for(i=0; i < 3 && NBs[i]; i++)
   {
      K = NBs[i];
      ld = KBs[K];
      iku = Mmin(ku, K);
      ilat = GetGoodLat(ma, K, mu, nu, 1, lat);
      mf = mmclean(pre, CleanK, "JIK", 'T', 'N', nb, nb, K,
                   nb, nb, ld, ld, ld, 0,
                   mu, nu, iku, ma, ilat, 1, 1, 1, csC, FF, iff, nf);
      fprintf(stdout, "   CleanK: %dx%dx%d : %.2f\n", nb, nb, K, mf);
      assert(mf > 0.0);
      mf0 += mf;
   }
   return(mf0 / i);
}

CLEANCASE *RebuttUserKClean(char pre, int nb, int mu, int nu, int ku,
                            int muladd, int lat, int FF, int iff, int nf)
{
   double gmf, umf;
   int NB[3], *KBs;
   CLEANCASE *cp0, *cp;

   KBs = GetKBs(pre, nb);

   cp0 = GetUserCleanup(pre, nb, CleanK);
   for (cp=cp0; cp; cp = cp->next)
   {
      NB[0] = cp->nb0; NB[1] = cp->nb1; NB[2] = cp->nb2;
      gmf = RebuttUserKCase(pre, nb, mu, nu, ku, muladd, lat, FF, iff, nf,
                            KBs, NB);
      fprintf(stdout, "   pKBmm_%d: user=%.2f  generated=%.2f\n",
              cp->imult, cp->mflop, gmf);
      if (1.02*gmf > cp->mflop) cp->icase = -1;
   }
   free(KBs);
   return(cp0);
}

double RebuttUserCase(char pre, int nb, enum CW which, int mu, int nu, int ku,
                      int ma, int lat, int FF, int iff, int nf, int *NBs)
{
   double mf, mf0=0.0;
   int NB[3], M[3], NU[3], csC, i, j, NUmax, ilat;
   char cwh[3] = {'M', 'N', 'K'};

   if (pre == 'c' || pre == 'z') csC = 2;
   else csC = 1;
   NB[0] = NB[1] = NB[2] = M[0] = M[1] = M[2] = nb;
   NB[which] = 0;
   NU[0] = mu; NU[1] = nu; NU[2] = ku;

   NUmax = NU[which];
   for(i=0; i < 3 && NBs[i]; i++)
   {
      j = M[which] = NBs[i];
      NU[which] = Mmin(j, NUmax);
      ilat = GetGoodLat(ma, M[2], NU[0], NU[1], NU[2], lat);
      mf = mmclean(pre, which, "JIK", 'T', 'N', M[0], M[1], M[2],
                   NB[0], NB[1], NB[2], nb, nb, 0,
                   mu, nu, ku, ma, lat, 1, 1, 1, csC, FF, iff, nf);
      fprintf(stdout, "   Clean%c: %dx%dx%d : %.2f\n", cwh[which],
              M[0], M[1], M[2], mf);
      assert(mf > 0.0);
      mf0 += mf;
   }
   return(mf0 / i);
}

CLEANCASE *RebuttUserCases(char pre, int nb, enum CW which,
                           int mu, int nu, int ku, int muladd, int lat,
                           int FF, int iff, int nf)
{
   double gmf, umf;
   int NB[3];
   CLEANCASE *cp0, *cp;
   char cwh[3] = {'M', 'N', 'K'};

   if (which == CleanK)
      return(RebuttUserKClean(pre, nb, mu, nu, ku, muladd, lat, FF, iff, nf));
   cp0 = GetUserCleanup(pre, nb, which);
   for (cp=cp0; cp; cp = cp->next)
   {
      NB[0] = cp->nb0; NB[1] = cp->nb1; NB[2] = cp->nb2;
      gmf = RebuttUserCase(pre, nb, which, mu, nu, ku, muladd, lat,
                           FF, iff, nf, NB);
      fprintf(stdout, "   p%cBmm_%d: user=%.2f  generated=%.2f\n",
              cwh[which], cp->imult, cp->mflop, gmf);
      if (1.02*gmf > cp->mflop) cp->icase = -1;
   }
   return(cp0);
}

CLEANCASE *WeedOutLosers(CLEANCASE *cp0)
{
   CLEANCASE *cp, *cp1;

   while(cp0 && cp0->icase == -1)
   {
      cp = cp0->next;
      free(cp0);
      cp0 = cp;
   }
   if (cp0 && cp0->next)
   {
      for (cp=cp0; cp->next; cp = cp->next)
      {
         cp1 = cp->next;
         if (cp1->icase == -1)
         {
            cp->next = cp1->next;
            free(cp1);
            if (cp->next == NULL) break;
         }
      }
   }
   return(cp0);
}

void KillAllCleans(CLEANCASE *cp)
{
   CLEANCASE *cp1;
   while (cp)
   {
      cp1 = cp->next;
      free(cp);
      cp = cp1;
   }
}

int NumUserCleans(CLEANCASE *cp)
{
   int i;
   for (i=0; cp; cp = cp->next) if (cp->icase != -1) i++;
   return(i);
}

void FindUserCleanup(char pre, int nb, enum CW which, int mu, int nu, int ku,
                     int ma, int lat, int FF, int iff, int nf)
{
   CLEANCASE *cp, *cp0;
   FILE *fp;
   char ln[128];
   char cwh[3] = {'M', 'N', 'K'};

   sprintf(ln, "res/%cuClean%cF", pre, cwh[which]);
   if (FileExists(ln)) return;/* already done */
   cp = RebuttUserCases(pre, nb, which, mu, nu, ku, ma, lat, FF, iff, nf);
   cp = WeedOutLosers(cp);
   fp = fopen (ln, "w");
   assert(fp);
   fprintf(fp, "MULT  ICASE  FIXED  NB\n");
   fprintf(fp, "%d\n", NumUserCleans(cp));
   for(cp0=cp; cp; cp = cp->next)
      fprintf(fp, "%4d  %5d  %5d %3d\n",
              cp->imult, cp->icase, cp->fixed, cp->nb);
   fclose(fp);
   KillAllCleans(cp0);
}

void FindAllUserClean(char pre, int nb, int mu, int nu, int ku,
                      int ma, int lat, int FF, int iff, int nf)
{
   FindUserCleanup(pre, nb, CleanM, mu, nu, ku, ma, lat, FF, iff, nf);
   FindUserCleanup(pre, nb, CleanN, mu, nu, ku, ma, lat, FF, iff, nf);
   FindUserCleanup(pre, nb, CleanK, mu, nu, ku, ma, lat, FF, iff, nf);
}

void FindAllUserClean0(char pre)
{
   double mf;
   int nb, mu, nu, ku, muladd, lat, FF, iff, nf;
   char ln[64];

   sprintf(ln, "res/%cMMRES", pre);
   GetInstLogFile(ln, pre, &muladd, &lat, &nb, &mu, &nu, &ku, &FF,
                     &iff, &nf, &mf);
   nb = CheckUser(pre, 1.02, mf, nb, NULL);
   FindAllUserClean(pre, nb, mu, nu, ku, muladd, lat, FF, iff, nf);
}

void FindCleanup(char pre, int nb, int mu, int nu, int ku, int muladd, int lat,
                 int FFetch, int ifetch, int nfetch)
{
   FindCleanupMN(pre, 'M', nb, mu, nu, ku, muladd, lat, FFetch, ifetch, nfetch);
   FindCleanupMN(pre, 'N', nb, mu, nu, ku, muladd, lat, FFetch, ifetch, nfetch);
   FindCleanupK(pre, nb, mu, nu, ku, muladd, lat, FFetch, ifetch, nfetch);
   FindAllUserClean(pre, nb, mu, nu, ku, muladd, lat, FFetch, ifetch, nfetch);
}
void FindAllClean(char pre)
{
   double mf;
   int nb, mu, nu, ku, muladd, lat, FF, iff, nf;
   char ln[64];

   sprintf(ln, "res/%cMMRES", pre);
   fprintf(stderr, "\n\nSTARTING CLEANUP SEARCH\n\n");
   GetInstLogFile(ln, pre, &muladd, &lat, &nb, &mu, &nu, &ku, &FF,
                     &iff, &nf, &mf);
   nb = CheckUser(pre, 1.02, mf, nb, NULL);
   FindCleanup(pre, nb, mu, nu, ku, muladd, lat, FF, iff, nf);
   fprintf(stderr, "\n\nDONE CLEANUP SEARCH\n\n");
}

int GetNumRegs0(char pre, int muladd, int nb, int lat,
                int nr0, int nrN, int incN)
{
   int n, nr, i, imax, nu, mu;
   double *rates, mf, mmf=0.0;

   n = 0;
   i = nr0;
   while (i <= nrN)
   {
      if (incN == -2) i <<= 1;
      else i += incN;
      n++;
   }
   rates = malloc(n * sizeof(double));
   nr = nr0;
   for (i=0; i < n; i++)
   {
      FindMUNU(muladd, lat, nr, &mu, &nu);
      mf = rates[i] = mms_case(pre, muladd, nb, mu, nu, nb, lat);
      if (1.09*mf > mmf)
      {
         if (mf > mmf) mmf = mf;
         imax = i;
      }
      if (incN == -2) nr <<= 1;
      else nr += incN;
   }
   if (imax < n)
   {
      for (i=imax+1; i < n && 1.10*rates[i] < mmf; i++);
      if (i != n) i = -1;
      else if (incN == -2) i = (nr0 << imax);
      else i = nr0 + imax*incN;
   }
   else i = nrN;
   free(rates);
   return(i);
}

int RefineNumRegs(char pre, int muladd, int nb, int lat, int nr0, int nrN)
/*
 * recursively halves gap until true number is found
 */
{
   int i, nr;

   i = (nrN - nr0) / 2;
   if (i < 1) return(nr0);
   nr = GetNumRegs0(pre, muladd, nb, lat, nr0, nr0+i, i);
   if (nr != nr0) /* positive or no difference in two points, so go larger */
      nr0 += i;
   else          /* difference, point is between */
      nrN = nr0 + i;
   return(RefineNumRegs(pre, muladd, nb, lat, nr0, nrN));
}

int GetNumRegs00(char pre, int muladd, int nb, int lat, int maxnr)
{
   int nr, i;

   fprintf(stderr, "\n\nFINDING ROUGHLY HOW MANY REGISTERS TO USE:\n\n");

   nr = GetNumRegs0(pre, muladd, nb, lat, 4, maxnr, -2);
/*
 * Refine number of regs
 */
   if (nr != -1) i = RefineNumRegs(pre, muladd, nb, lat, nr, nr<<1);
   else i = nr;
   fprintf(stderr, "\n\nAPPROXIMATE NUMBER OF USABLE REGISTERS=%d\n\n", i);
   return(i);
}

int GetNumRegs(char pre, int MaxL1, int maxnreg)
{
   FILE *fp;
   int nreg, muladd, lat;
   char nam[32];
   void GetMulAdd(char pre, int *MULADD, int *lat);

   if (pre == 'z') pre = 'd';
   else if (pre == 'c') pre = 's';

   sprintf(nam, "res/%cnreg", pre);
   if (!FileExists(nam))
   {
      GetMulAdd(pre, &muladd, &lat);
      nreg = GetNumRegs00(pre, muladd, GetSafeNB(pre, MaxL1), lat, maxnreg);
      fp = fopen(nam, "w");
      fprintf(fp, "%d\n", nreg);
   }
   else
   {
      fp = fopen(nam, "r");
      fscanf(fp, " %d", &nreg);
   }
   fclose(fp);
   fprintf(stdout, "mmnreg = %d\n", nreg);
   return(nreg);
}

void RunTimes(char pre)
{
   const char TR[2] = {'N', 'T'};
   char fnam[128], fnam2[128], ln[128];
   const int COMPLEX = (pre == 'c' || pre == 'z');
   int csC = (COMPLEX ? 2 : 1);
   int NB, muladd, lat, nb, mu, nu, ku, ffetch, ifetch, nfetch, ia, ib;
   int uma, ulat, unb=0, umu, unu, uku, uff, uif, unf;
   int maxreg;
   double mf, umf;
   FILE *fp;

   fprintf(stderr, "\n\nStart RunTimes\n");
   sprintf(fnam, "res/%cgMMRES", pre);
   fp = fopen(fnam, "r");
   assert(fp);
   fgets(ln, 128, fp);
   GetInstLogLine(fp, &muladd, &lat, &nb, &mu, &nu, &ku, &ffetch,
                  &ifetch, &nfetch, &mf);
   fclose(fp);
   if (mf < 0.0) /* need to retime */
   {
      sprintf(ln, "make RunUMMSearch pre=%c nb=-1\n", pre);
      assert(system(ln) == 0);
      mf = mmcase(NULL, pre, "JIK", 'T', 'N', nb, nb, nb, nb, nb, nb,
                  nb, nb, 0, mu, nu, ku, muladd, lat, 1, 1, 1, csC,
                  ffetch, ifetch, nfetch);
      PutInstLogFile1(fnam, pre, muladd, lat, nb, mu, nu, ku,
                      ffetch, ifetch, nfetch, mf);
      sprintf(fnam, "res/%cMMRES", pre);
      if (!FileExists(fnam))
      {
         sprintf(ln, "make res/%cMMRES pre=%c\n", pre, pre);
         assert(system(ln) == 0);
      }
      else
      {
         GetInstLogFile(fnam, pre, &muladd, &lat, &nb, &mu, &nu, &ku, &ffetch,
                        &ifetch, &nfetch, &mf);
         mf = mmcase(NULL, pre, "JIK", 'T', 'N', nb, nb, nb, nb, nb, nb,
                     nb, nb, 0, mu, nu, ku, muladd, lat, 1, 1, 1, csC,
                     ffetch, ifetch, nfetch);
         CreateFinalSumm(pre, muladd, lat, nb, mu, nu, ku, ffetch, ifetch,
                         nfetch, mf);
      }
   }
   sprintf(fnam, "res/%cNCNB", pre);
   if (!FileExists(fnam)) return;
   fp = fopen(fnam, "r");
   assert(fp);
   assert(fscanf(fp, " %d", &NB) == 1);
   fclose(fp);

   for (ia=0; ia < 2; ia++)
   {
      for (ib=0; ib < 2; ib++)
      {
         sprintf(fnam, "res/%cbest%c%c_%dx%dx%d", pre, TR[ia], TR[ib],
                 NB, NB, NB);
         if (FileExists(fnam))
         {
            GetInstLogFile(fnam, pre, &muladd, &lat, &nb, &mu, &nu, &ku,
                           &ffetch, &ifetch, &nfetch, &mf);
            if (mf < 0.0) /* need to retime */
            {
               mf = mmcase(NULL, pre, "JIK", TR[ia], TR[ib], nb, nb, nb,
                           nb, nb, nb, 0, 0, 0, mu, nu, ku, muladd, lat, 1,
                           1, 1, csC, ffetch, ifetch, nfetch);
               PutInstLogFile1(fnam, pre, muladd, lat, nb, mu, nu, ku,
                              ffetch, ifetch, nfetch, mf);
            }
         }
         sprintf(fnam, "res/%cbest%c%c_%dx%dx%d", pre, TR[ia], TR[ib],
                 0, 0, NB);
         if (FileExists(fnam))
         {
            GetInstLogFile(fnam, pre, &muladd, &lat, &nb, &mu, &nu, &ku,
                           &ffetch, &ifetch, &nfetch, &mf);
            if (mf < 0.0) /* need to retime */
            {
               mf = mmcase(NULL, pre, "JIK", TR[ia], TR[ib], nb, nb, nb,
                           0, 0, nb, 0, 0, 0, mu, nu, ku, muladd, lat, 1,
                           1, 1, csC, ffetch, ifetch, nfetch);
               PutInstLogFile1(fnam, pre, muladd, lat, nb, mu, nu, ku,
                              ffetch, ifetch, nfetch, mf);
            }
         }
         sprintf(fnam, "res/%cbest%c%c_%dx%dx%d", pre, TR[ia], TR[ib],
                 0, 0, 0);
         if (FileExists(fnam))
         {
            GetInstLogFile(fnam, pre, &muladd, &lat, &nb, &mu, &nu, &ku,
                           &ffetch, &ifetch, &nfetch, &mf);
            if (mf < 0.0) /* need to retime */
            {
               mf = mmcase(NULL, pre, "JIK", TR[ia], TR[ib], nb, nb, nb,
                           0, 0, 0, 0, 0, 0, mu, nu, ku, muladd, lat, 1,
                           1, 1, csC, ffetch, ifetch, nfetch);
               PutInstLogFile1(fnam, pre, muladd, lat, nb, mu, nu, ku,
                              ffetch, ifetch, nfetch, mf);
            }
         }
      }
   }
   fprintf(stderr, "\nDone  RunTimes\n\n");
}

void cmmsearch(char pre, int MULADD, int Fku, int nNBs, int *NBs, int nreg,
               int LAT, int Fnb)
/*
 * With all other parameters set by real search, find good complex NB
 */
{
   char *typ, ln[64], upre;
   int i, k, mnb=0, muladd, lat, nb, mu, nu, ku, ffetch, ifetch, nfetch;
   int KUisNB, NO1D;
   double mf, mmf=0.0, umf;
   FILE *fp;

   if (pre == 'c') upre = 's';
   else upre = 'd';

   sprintf(ln, "res/%cMMRES", pre);
   if (FileExists(ln)) /* already have result */
   {
      GetInstLogFile(ln, pre, &muladd, &lat, &nb, &mu, &nu, &ku, &ffetch,
                     &ifetch, &nfetch, &mf);
      if (mf <= 0.0)
      {
         mf = mmcase(NULL, pre, "JIK", 'T', 'N', nb, nb, nb, nb, nb, nb,
                     nb, nb, 0, mu, nu, ku, muladd, lat, 1, 1, 1, 2,
                     ffetch, ifetch, nfetch);
         gmmsearch(upre, muladd, Fku, nNBs, NBs, nreg, lat, Fnb);
         nb = CheckUser(pre, 1.02, mf, nb, NULL);
         CreateFinalSumm(pre, muladd, lat, nb, mu, nu, ku, ffetch, ifetch,
                         nfetch, mf);
      }
      else nb = CheckUser(pre, 1.02, mf, nb, NULL);
      sprintf(ln, "res/%cNB", pre);
      fp = fopen(ln, "w");
      fprintf(fp, "%d\n%d\n", 1, nb);
      fclose(fp);
      return;
   }
   gmmsearch(upre, MULADD, Fku, nNBs, NBs, nreg, LAT, Fnb);
   sprintf(ln, "res/%cgMMRES", upre);
   GetInstLogFile(ln, upre, &muladd, &lat, &nb, &mu, &nu, &ku, &ffetch,
                  &ifetch, &nfetch, &mf);
   KUisNB = (nb <= ku);

   for (i=0; i < nNBs; i++)
   {
      if (KUisNB) k = NBs[i];
      else k = Mmin(ku, NBs[i]);
      mf = mms_case(pre, muladd, NBs[i], mu, nu, k, lat);
      if (mf > mmf)
      {
         mmf = mf;
         mnb = NBs[i];
      }
   }
   if (KUisNB) ku = mnb;
   else ku = Mmin(ku, mnb);
   sprintf(ln, "res/%cgMMRES", pre);
   PutInstLogFile1(ln, pre, muladd, lat, mnb, mu, nu, ku,
                   ffetch, ifetch, nfetch, mmf);

   nb = CheckUser(pre, 1.02, mmf, mnb, &umf);
   if (nb != mnb)
   {
      if (ku == mnb) ku = nb;
      mnb = nb;
      if (nb % mu || nb % nu)
      {
         NO1D = GetNO1D(upre, nreg, nb, MULADD, LAT);
         searchmu_nu(upre, nb, nreg, Fku, MULADD, LAT, NO1D,
                     &mmf, &nb, &mu, &nu, &ku, &lat);
      }
      FindKU(upre, muladd, LAT, nb, mu, nu, &mmf, &ku, &lat);
      FindLAT(upre, MAXLAT, nb, muladd, mu, nu, ku, &mmf, &lat);
      FindFetch('T', 'N', upre, nb, nb, nb, mu, nu, ku, muladd, lat,
                &ffetch, &ifetch, &nfetch);
   }
/*
 * Save NB we've found
 */
   sprintf(ln, "res/%cNB", pre);
   fp = fopen(ln, "w");
   fprintf(fp, "%d\n%d\n", 1, mnb);
   fclose(fp);
   CreateFinalSumm(pre, muladd, lat, mnb, mu, nu, ku,
                   ffetch, ifetch, nfetch, mf);
}
void GetMulAdd(char pre, int *MULADD, int *lat)
{
   char nam[64], ln[128];
   FILE *fp;

   sprintf(nam, "res/%cMULADD", pre);
   if (!FileExists(nam))
   {
      sprintf(ln, "make RunMulAdd pre=%c maxlat=%d mflop=%d\n", pre, 6, 200);
      assert(system(ln) == 0);
   }
   fp = fopen(nam, "r");
   assert(fp != NULL);
   fscanf(fp, "%d", MULADD);
   fscanf(fp, "%d", lat);
   fclose(fp);
}

int GetNumRegsMM(char pre, int MaxL1Size, int MAX_NREG)
{
   int nreg;
   nreg = GetNumRegs(pre, MaxL1Size, MAX_NREG);
   if (nreg == -1)
   {
      fprintf(stderr,
         "\nUNABLE TO FIND NUMBER OF REGISTERS, ASSUMMING 32.\n\n");
      nreg = 32;
   }
   if (nreg > 128)
   {
      fprintf(stderr, "FOUND NUMBER OF REGISTERS TO BE %d; THIS WOULD TAKE TOO LONG TO SEARCH, SO SETTING TO 128.\n", nreg);
      nreg = 128;
   }
   if (nreg < 8)
   {
      fprintf(stderr,
              "FOUND # OF REGISTERS TO BE %d; TRYING 8 FOR SAFETY.\n", nreg);
      nreg = 8;
   }
   else if (nreg < 16)
   {
      fprintf(stderr,
              "FOUND # OF REGISTERS TO BE %d; TRYING 16 FOR SAFETY.\n", nreg);
      nreg = 16;
   }
   return(nreg);
}

void GetMMRES(char pre, int nreg, int MaxL1Size, int ForceLat)
{
   char upre;
   char ln[128];
   int i, nNBs, muladd, lat;
   int *NBs;
   FILE *fp;

   if (pre == 'c') upre = 's';
   else if (pre == 'z') upre = 'd';
   else upre = pre;

   sprintf(ln, "res/%cNB", pre);
   if (!FileExists(ln)) findNBs(upre, ln, MaxL1Size);
   assert( (fp = fopen(ln, "r")) != NULL );
   fscanf(fp, "%d", &nNBs);
   fprintf(stdout, "\nNB's to try: ");
   NBs = malloc(nNBs*sizeof(int));
   for (i=0; i != nNBs; i++)
   {
      fscanf(fp, "%d", NBs+i);
      fprintf(stdout, "%d   ",NBs[i]);
   }
   fprintf(stdout, "\n\n");

   GetMulAdd(upre, &muladd, &lat);
   if (ForceLat != -1) lat = ForceLat;


   if (pre == 'c' || pre == 'z')
      cmmsearch(pre, muladd, 0, nNBs, NBs, nreg, lat, 0);
   else mmsearch(pre, muladd, 0, nNBs, NBs, nreg, lat, 0);
   free(NBs);
}
#ifndef ATL_MAXNREG
   #define ATL_MAXNREG 64
#endif
main(int nargs, char *args[])
{
   char prec, upre, lang;
   int MULADD, MaxL1Size, ForceLat, i, nreg, ROUT, FRC;
   int muladd, lat, nb, mu, nu, ku, ffetch, ifetch, nfetch;
   int unb, icase;
   double mf, umf;
   FILE *fp;
   char ln[128], auth[65];

   GetSettings(nargs, args, &prec, &lang, &ku, &ForceLat, &FRC, &nreg,
               &MaxL1Size, &ROUT);
   assert(FRC == 0 && ku == 0); /* obsolete flags */
   LANG = lang;

   if (prec == 'z') upre = 'd';
   else if (prec == 'c') upre = 's';
   else upre = prec;
   if (ROUT == -3)
   {
      FindAllClean(prec);
      exit(0);
   }
   else if (ROUT == -4)
   {
      FindAllUserClean0(prec);
      exit(0);
   }
   else if (ROUT == -5) /* produce ATL_mmnreg for sysinfo */
   {
      GetNumRegs(prec, MaxL1Size, ATL_MAXNREG);
      exit(0);
   }
   fprintf(stderr, "Precision='%c', FORCE=%d, LAT=%d, nreg=%d, MaxL1=%d\n",
           prec, FRC, ForceLat, nreg, MaxL1Size);

   if (nreg == -1) nreg = GetNumRegsMM(upre, MaxL1Size, ATL_MAXNREG);
   GetMMRES(prec, nreg, MaxL1Size, ForceLat);
   FindNoCopy(prec);
   FindAllClean(prec);

   sprintf(ln, "res/%cMMRES", prec);
   fp = fopen(ln, "r");
   assert(fp);
   assert( fgets(ln, 128, fp) != NULL );
   GetInstLogLine(fp, &muladd, &lat, &nb, &mu, &nu, &ku, &ffetch,
                     &ifetch, &nfetch, &mf);
   assert( fgets(ln, 128, fp) != NULL );
   assert(fscanf(fp, " %d %d %lf \"%[^\"]\" \"%[^\"]", &icase, &unb, &umf,
                 ln, auth) == 5);
   fclose(fp);
   if (mf >= umf)
   {
      fprintf(stdout, "\n\nFor this run, the best parameters found were MULADD=%d, lat=%d, NB=%d, MU=%d, NU=%d, KU=%d\n",
              muladd, lat, nb, mu, nu, ku);
   }
   else
   {
       mf = umf;
       fprintf(stdout,
               "\n\nFor this run, the best case found was NB=%d user case %d\n",
               unb, icase);
       fprintf(stdout, "written by %s.\n", auth);
   }
   fprintf(stdout, "This gave a performance = %f MFLOP.\n", mf);
   fprintf(stdout,
"The necessary files have been created.  If you are happy with\n");
   fprintf(stdout,
"the above mflops for your system, type 'make %cinstall'.\n", prec);
   fprintf(stdout,
"Otherwise, try the xmmsearch with different parameters, or hand\n");
   fprintf(stdout, "tweak the code.\n");
   exit(0);
}
