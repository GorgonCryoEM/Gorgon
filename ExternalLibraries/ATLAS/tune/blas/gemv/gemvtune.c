/*
 *             Automatically Tuned Linear Algebra Software v3.6.0
 *                    (C) Copyright 1999 R. Clint Whaley
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

#include "atlas_misc.h"
#include "atlas_lvl2.h"
#include "atlas_fopen.h"

#define ATL_MVNoBlock(iflag_) ( ((iflag_) | 32) == (iflag_) )

double time00();
double gemvcase(const int MFLOP, const enum ATLAS_TRANS TA, const int l2size,
                const int M, const int N, const SCALAR alpha, const int lda,
                const SCALAR beta)
{
   unsigned long reps;
   int i, lx, ly, la;
   #ifdef TREAL
      const double flops = 2.0 * M * N;
   #else
      const double flops = 8.0 * M * N;
   #endif
   double ttest, mftest, t0;
   const int aincY=1, aincX=1, incY=1, incX=1;
   const int inca = (TA == AtlasNoTrans) ? lda * (N SHIFT) : lda * (M SHIFT);
   const int incx = N*incX SHIFT, incy = M*incY SHIFT;
   TYPE *a, *A, *stA, *A0, *x, *X, *X0, *stX, *y, *Y, *Y0, *stY;
   #ifdef TREAL
      const TYPE nbeta = -beta;
      TYPE bet = beta;
   #else
      const TYPE *bet = beta;
      TYPE nbeta[2];
      nbeta[0] = -beta[0]; nbeta[1] = -beta[1];
   #endif

   i = (ATL_DivBySize(l2size)+N-1)/N;
   if (i < 1) i = 1;
   lx = i * N * aincX;
   X0 = X = x = malloc(ATL_MulBySize(lx));
   if (x == NULL) return(-1);

   i = (ATL_DivBySize(l2size)+M-1)/M;
   if (i < 1) i = 1;
   ly = i * M * aincY;
   Y0 = Y = y = malloc(ATL_MulBySize(ly));
   if (y == NULL)
   {
      free(x);
      return(-1);
   }
   i = (ATL_DivBySize(l2size)+M*N)/(M*N);
   la = i * inca;
   A0 = A = a = malloc(ATL_MulBySize(la));
   if (a == NULL)
   {
      free(x);
      free(y);
      return(-1);
   }
   if (incX < 1)
   {
      stX = x;
      x = X = x + (lx SHIFT);
   }
   else stX = x + (lx SHIFT);
   if (incY < 1)
   {
      stY = y;
      y = Y = y + (ly SHIFT);
   }
   else stY = y + (ly SHIFT);
   stA = a + (la SHIFT);

   reps = (MFLOP * 1000000.0) / flops;
   if (reps < 1) reps = 1;
   Mjoin(PATL,gegen)(ly, 1, Y0, ly, M*incY);
   Mjoin(PATL,gegen)(lx, 1, X0, lx, N*incY+127*50+77);
   Mjoin(PATL,gegen)(la, 1, A0, la, N*M+513*7+90);

   t0 = time00();
   for (i=reps; i; i--)
   {
      #ifdef SYMM_
         Mjoin(PATL,symv)(AtlasLower, N, alpha, a, lda, x, incX, beta, y, incY);
      #else
         Mjoin(PATL,gemv)(TA, M, N, alpha, a, lda, x, incX, beta, y, incY);
      #endif
      x += incx;
      y += incy;
      a += inca;
      if (x == stX) x = X;
      if (y == stY)
      {
         y = Y;
         if (bet == beta) bet = nbeta;
         else bet = beta;
      }
      if (a == stA) a = A;
   }
   ttest = time00() - t0;

   if (ttest > 0.0) mftest = (reps * flops) / (1000000.0 * ttest);
   else mftest = 0.0;

   free(A0);
   free(X0);
   free(Y0);
   return(mftest);
}
void PrintUsage(char *nam)
{
   fprintf(stderr,
      "USAGE: %s -C <case #> -l <l1mul> -F <mflop> -A <t/n> -m <M> -n <N> -L <lda> -b <beta> -o <output filename> -2 <flush size (kbytes)>\n\n",
           nam);
   exit(-1);
}

void GetFlags(int nargs, char **args, char *pre, int *l2size, int *MFLOP,
              char *cta, int *M, int *N, TYPE *alpha, int *lda,
              TYPE *beta, char *outnam)
{
   char ctmp;
   int i, cas=0, iflag=0;
   int l1mul=75;

   *cta = 'N';
   outnam[0] = '\0';
   *l2size = L2SIZE;
   #if defined(DREAL)
      *pre = 'd';
   #elif defined(SREAL)
      *pre = 's';
   #elif defined(SCPLX)
      *pre = 'c';
   #elif defined(DCPLX)
      *pre = 'z';
   #endif
   #ifdef ATL_nkflop
      *MFLOP = (ATL_nkflop) / 1000.0;
      if (*MFLOP < 1) *MFLOP = 1;
   #else
      *MFLOP = 100;
   #endif
   *lda = *M = *N = 1000;
   *beta = *alpha = ATL_rone;
   #ifdef TCPLX
      beta[1] = alpha[1] = ATL_rzero;
   #endif
   for (i=1; i < nargs; i++)
   {
      if (args[i][0] != '-') PrintUsage(args[0]);
      switch(args[i][1])
      {
      case 'C': /* case */
         cas = atoi(args[++i]);
         break;
      case 'F': /* mflops */
         *MFLOP = atoi(args[++i]);
         break;
      case '2':
         *l2size = atoi(args[++i])*1024;
         break;
      case 'f': /* iflag */
         iflag = atoi(args[++i]);
         break;
      case 'L':
         *lda = atoi(args[++i]);
	 break;
      case 'l': /* mflops */
         l1mul = atoi(args[++i]);
         break;
      case 'A': /* trans */
         i++;
         ctmp = toupper(args[i][0]);
         if (ctmp == 'n') ctmp = 'N';
         else if (ctmp == 't') ctmp = 'T';
         else if (ctmp != 'N' && ctmp != 'T') PrintUsage(args[0]);
         *cta = ctmp;
         break;
      case 'm':
         *M = atoi(args[++i]);
         break;
      case 'n':
         *N = atoi(args[++i]);
         break;
      case 'b':
         *beta = atof(args[++i]);
         #ifdef TCPLX
            beta[1] = atof(args[++i]);
         #endif
         break;
      case 'o':
         strcpy(outnam, args[++i]);
         break;
      default:
         PrintUsage(args[0]);
      }
   }
   #ifdef SYMM_
      if (*M > *N) *N = *M;
      else if (*N > *M) *M = *N;
   #endif
   if (*cta == 'N') { ATL_assert(*lda >= Mmax(1,*M)); }
   else { ATL_assert(*lda >= Mmax(1,*N)); }
   if (outnam[0] == '\0')
   {
      #ifdef SYMM_
         if (ATL_MVNoBlock(iflag))
            sprintf(outnam, "res/%csymv%c_%d_0", *pre,*cta, cas);
         else sprintf(outnam, "res/%csymv%c_%d_%d", *pre,*cta, cas, l1mul);
      #else
         if (ATL_MVNoBlock(iflag))
            sprintf(outnam, "res/%cgemv%c_%d_0", *pre,*cta, cas);
         else sprintf(outnam, "res/%cgemv%c_%d_%d", *pre,*cta, cas, l1mul);
      #endif
   }
}

main(int nargs, char **args)
{
   char pre, fnam[128], cta;
   enum ATLAS_TRANS TA;
   int MFLOP, M, N, lda, i, l2size;
   double mf, mfs[3];
   #ifdef TREAL
      TYPE alpha, beta;
   #else
      TYPE alpha[2], beta[2];
   #endif
   FILE *fp;

   GetFlags(nargs, args, &pre, &l2size, &MFLOP, &cta, &M, &N, SADD alpha, &lda,
            SADD beta, fnam);

   if (cta == 'N' || cta == 'n') TA = AtlasNoTrans;
   else TA = AtlasTrans;
   if (!FileExists(fnam))
   {
      fp = fopen(fnam, "w");
      ATL_assert(fp);
      for (i=0; i < 3; i++)
      {
         mf = gemvcase(MFLOP, TA, l2size, M, N, alpha, lda, beta);
         fprintf(stdout, "      %s : %f MFLOPS\n", fnam, mf);
         fprintf(fp, "%lf\n", mf);
         mfs[i] = mf;
      }
   }
   else
   {
      fp = fopen(fnam, "r");
      for (i=0; i < 3; i++) ATL_assert(fscanf(fp, " %lf", &mfs[i]) == 1);
   }
   fclose(fp);
   mf = (mfs[0] + mfs[1] + mfs[2]) / 3.0;
   fprintf(stdout, "   %s : %.2f MFLOPS\n", fnam, mf);
   exit(0);
}
