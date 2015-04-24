/*
 *             Automatically Tuned Linear Algebra Software v3.6.0
 *                    (C) Copyright 2000 R. Clint Whaley
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
#include "atlas_tst.h"
#include "atlas_lvl2.h"

static void dumb_ger(int Conj, int M, int N, SCALAR alpha, TYPE *X, int incX,
                     TYPE *Y, int incY, TYPE *A, int lda)
{
   #ifdef TCPLX
      TYPE tmp[2];
   #endif
   int j;
   ATL_assert(incX == 1);
   ATL_assert( SCALAR_IS_ONE(alpha) );
   for (j=0; j < N; j++)
   {
   #ifdef TREAL
      Mjoin(PATL,axpy)(M, Y[j*incY], X, 1, A+lda*j, 1);
   #else
      tmp[0] = Y[2*j*incY];
      tmp[1] = Y[2*j*incY+1];
      if (Conj) tmp[1] = -tmp[1];
      Mjoin(PATL,axpy)(M, tmp, X, 1, A+2*lda*j, 1);
   #endif
   }
}

static int CheckAns(int M, int N, TYPE *G, int ldg, TYPE *U, int ldu)
{
   TYPE diff, eps;
   int i, j, ierr=0;
   #ifdef TREAL
      const int M2 = M, emul=4;
   #else
      const int M2 = M<<1, emul=16;
      ldg <<= 1; ldu <<= 1;
   #endif

   eps = Mjoin(PATL,epsilon)();
   for (j=0; j < N; j++, G += ldg, U += ldu)
   {
      for (i=0; i < M2; i++)
      {
         diff = G[i] - U[i];
         if (diff < ATL_rzero) diff = -diff;
         if (diff > emul*eps)
         {
            fprintf(stderr, "A(%d,%d): Good=%f, Computed=%f\n",
                    i, j, G[i], U[i]);
            if (!ierr) ierr = i+j*M+1;
         }
      }
   }
   return(ierr);
}

static int RunTest(int CONJ, int M, int N, int incY, int lda)
{
   #ifdef TCPLX
      TYPE one[2] = {ATL_rone, ATL_rzero};
   #else
      TYPE one = ATL_rone;
   #endif
   TYPE *A, *A0, *X, *Y, *y;
   const int aincY = Mabs(incY);
   int ierr;

   fprintf(stdout, "   TEST CONJ=%d, M=%d, N=%d, lda=%d, incY=%d, STARTED\n",
           CONJ, M, N, lda, incY);
   A = malloc(ATL_MulBySize(lda)*N);
   A0 = malloc(ATL_MulBySize(M)*N);
   X = malloc(ATL_MulBySize(M));
   Y = malloc(ATL_MulBySize(N)*aincY);
   ATL_assert(A && A0 && X && Y);
   Mjoin(PATL,gegen)(1, N, Y, aincY, N*aincY);
   Mjoin(PATL,gegen)(M, 1, X, M, N*aincY+127*50+77);
   Mjoin(PATL,gegen)(M, N, A0, M, N*M+513*7+90);
   Mjoin(PATL,gegen)(M, N, A, lda, N*M+513*7+90);
   if (incY < 0) Y += (N-1) * (aincY SHIFT);

#ifdef TCPLX
   if (CONJ)
      Mjoin(PATL,ger1c_a1_x1_yX)(M, N, one, X, 1, Y, incY, A, lda);
   else Mjoin(PATL,ger1u_a1_x1_yX)(M, N, one, X, 1, Y, incY, A, lda);
#else
   Mjoin(PATL,ger1_a1_x1_yX)(M, N, one, X, 1, Y, incY, A, lda);
#endif
   dumb_ger(CONJ, M, N, one, X, 1, Y, incY, A0, M);

   if (incY < 0) Y -= (N-1) * (aincY SHIFT);
   free(Y);
   free(X);
   ierr = CheckAns(M, N, A0, M, A, lda);
   free(A);
   free(A0);

   if (ierr)
      fprintf(stdout, "   TEST CONJ=%d, M=%d, N=%d, lda=%d, incY=%d, FAILED\n",
              CONJ, M, N, lda, incY);
   else fprintf(stdout,
                "   TEST CONJ=%d, M=%d, N=%d, lda=%d, incY=%d, PASSED\n",
                CONJ, M, N, lda, incY);
   return(ierr);
}

static int RunTests(int M, int N, int incY, int lda)
{
   int j, k=0, ierr = 0;

   ATL_assert(incY != 0);

#ifdef TCPLX
   for (k=0; k < 2; k++)
#endif
   {
      j = RunTest(k, M, N, 1, lda);
      if (!ierr) ierr = j;
      j = RunTest(k, M, N, incY, lda);
      if (!ierr) ierr = j;
      j = RunTest(k, M, N, -incY, lda);
      if (!ierr) ierr = j;
   }
   return(ierr);
}

static void PrintUsage(char *nam)
{
   fprintf(stderr, "USAGE: %s -m <M> -n <N> -Y <incY> -l <lda>\n",
           nam);
   exit(-1);
}

void GetFlags(int nargs, char **args, int *M, int *N, int *incY, int *lda)
{
   int i;

   *M = 997;
   *N = 177;
   *incY = 3;
   *lda = -1;

   for (i=1; i < nargs; i++)
   {
      if (args[i][0] != '-') PrintUsage(args[0]);
      switch(args[i][1])
      {
      case 'm':
         *M = atoi(args[++i]);
         break;
      case 'n':
         *N = atoi(args[++i]);
         break;
      case 'l':
         *lda = atoi(args[++i]);
         break;
      case 'Y':
         *incY = atoi(args[++i]);
         break;
      default:
         PrintUsage(args[0]);
      }
   }
   if (*lda < *M) *lda = *M + 9;
}

main(int nargs, char **args)
{
   int M, N, incY, lda, ierr=0;

   GetFlags(nargs, args, &M, &N, &incY, &lda);
   ierr = RunTests(M, N, incY, lda);
   exit(ierr);
}
