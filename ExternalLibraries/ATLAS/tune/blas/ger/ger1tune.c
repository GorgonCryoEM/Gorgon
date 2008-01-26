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

#define ATL_NoBlock(iflag_) ( ((iflag_) | 32) == (iflag_) )

double time00();
#ifdef TREAL
   #define test_ger(M, N, alpha, X, incX, Y, incY, A, lda) \
      Mjoin(PATL,ger)(M, N, alpha, X, incX, Y, incY, A, lda)
#else
   #define test_ger(M, N, alpha, X, incX, Y, incY, A, lda) \
      Mjoin(PATL,geru)(M, N, alpha, X, incX, Y, incY, A, lda)
#endif

double gercase(const int MFLOP, const int M, const int N, const SCALAR alpha,
               const int lda)
{
   unsigned long reps;
   const int aincX = 1, aincY = 1;
   const int incX = 1, incY = 1;
   int i, lx, ly, la;
   #ifdef TREAL
      const double flops = 2.0 * M * N;
   #else
      const double flops = 8.0 * M * N;
   #endif
   double ttest, mftest, t0;
   const int inca = lda*N SHIFT, incx = M*incX SHIFT, incy = N*incY SHIFT;
   TYPE *a, *A, *stA, *A0, *x, *X, *X0, *stX, *y, *Y, *Y0, *stY;
   #ifdef TREAL
      const TYPE nalpha = -alpha;
      TYPE alp = alpha;
   #else
      const TYPE *alp = alpha;
      TYPE nalpha[2];
   #endif
   #ifdef TCPLX
      nalpha[0] = -alpha[0]; nalpha[1] = alpha[1];
   #endif

   i = (ATL_DivBySize(L2SIZE)+M-1)/M;
   lx = i * M * aincX;
   X0 = X = x = malloc(ATL_MulBySize(lx));
   if (x == NULL) return(-1);

   i = (ATL_DivBySize(L2SIZE)+N-1)/N;
   ly = i * N * aincY;
   Y0 = Y = y = malloc(ATL_MulBySize(ly));
   if (y == NULL)
   {
      free(x);
      return(-1);
   }
   i = (ATL_DivBySize(L2SIZE)+M*N)/(M*N);
   la = i * lda*N;
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
      test_ger(M, N, alp, x, incX, y, incY, A0, lda);
      x += incx;
      y += incy;
      a += inca;
      if (x == stX) x = X;
      if (y == stY) y = Y;
      if (a == stA)
      {
         a = A;
         if (alp == alpha) alp = nalpha;
         else alp = alpha;
      }
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
   fprintf(stderr, "USAGE: %s -C <case #> -l <l1mul> -F <mflop> -m <M> -n <N> -f <iflag> -a <alpha> -o <outfile>\n\n", nam);
   exit(-1);
}

void GetFlags(int nargs, char **args, char *pre, int *MFLOP,
              int *M, int *N, TYPE *alpha, int *lda, char *outnam)
{
   int l1mul;
   int i, cas, iflag=0;

   l1mul = 75;
   outnam[0] = '\0';
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
      *MFLOP = (ATL_nkflop) / 2000.0;
      if (*MFLOP < 1) *MFLOP = 1;
   #else
      *MFLOP = 75;
   #endif
   *lda = *M = *N = 1000;
   *alpha = ATL_rone;
   #ifdef TCPLX
      alpha[1] = ATL_rzero;
   #endif
   cas = 1;
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
      case 'f': /* iflag */
         iflag = atoi(args[++i]);
         break;
      case 'm':
         *M = atoi(args[++i]);
         break;
      case 'n':
         *N = atoi(args[++i]);
         break;
      case 'l':
         l1mul = atoi(args[++i]);
         break;
      case 'a':
         *alpha = atof(args[++i]);
         #ifdef TCPLX
            alpha[1] = atof(args[++i]);
         #endif
         break;
      case 'o':
         strcpy(outnam, args[++i]);
         break;
      default:
         PrintUsage(args[0]);
      }
   }
   if (outnam[0] == '\0')
   {
      if (ATL_NoBlock(iflag))
         sprintf(outnam, "res/%cger1_%d_0", *pre, cas);
      else sprintf(outnam, "res/%cger1_%d_%d", *pre, cas, l1mul);
   }
}

main(int nargs, char **args)
{
   char pre, fnam[128];
   int MFLOP, M, N, lda, cas, i;
   double mf, mfs[3];
   #ifdef TREAL
      TYPE alpha;
   #else
      TYPE alpha[2];
   #endif
   FILE *fp;

   GetFlags(nargs, args, &pre, &MFLOP, &M, &N, SADD alpha, &lda, fnam);

   if (!FileExists(fnam))
   {
      fp = fopen(fnam, "w");
      ATL_assert(fp);
      for (i=0; i < 3; i++)
      {
         mf = gercase(MFLOP, M, N, alpha, lda);
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
