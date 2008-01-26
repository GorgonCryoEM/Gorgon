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
#include "atlas_misc.h"
#include "atlas_lvl3.h"
#include "atlas_cacheedge.h"

#ifdef NoTransA_
   #define ETA AtlasNoTrans
   #define TA_ N
   #define ta_ n
   #define incA (Kp*lda SHIFT)
#elif defined(TransA_)
   #define ETA AtlasTrans
   #define TA_ T
   #define ta_ t
   #define incA (Kp SHIFT)
#elif defined(ConjTransA_)
   #define ETA AtlasConjTrans
   #define TA_ C
   #define ta_ c
   #define incA (Kp SHIFT)
#endif

#ifdef NoTransB_
   #define ETB AtlasNoTrans
   #define TB_ N
   #define tb_ n
   #define incB (Kp SHIFT)
#elif defined (TransB_)
   #define ETB AtlasTrans
   #define TB_ T
   #define tb_ t
   #define incB (Kp*ldb SHIFT)
#elif defined (ConjTransB_)
   #define ETB AtlasConjTrans
   #define TB_ C
   #define tb_ c
   #define incB (Kp*ldb SHIFT)
#endif
#define tatb Mjoin(ta_,tb_)
#define TATB Mjoin(TA_,TB_)

#ifdef BIG_MM
   #define ATL_OOM ATL_bigmmOutOfMem =
#else
   #define ATL_OOM
#endif

#ifndef Cgemm__
   #define UNDEF_ATL_VOID
   #define ATL_VOID void
   #ifdef ALIASED_GEMM
      #define Cgemm__ Mjoin(Mjoin(PATL,aliased_gemm),TATB)
   #else
      #define Cgemm__ Mjoin(Mjoin(PATL,gemm),TATB)
   #endif
#endif

ATL_VOID Cgemm__(const int M, const int N, const int K, const SCALAR alpha,
                 const TYPE *A, const int lda, const TYPE *B, const int ldb,
                 const SCALAR beta, TYPE *C, const int ldc)
{
   int DOCOPY;
   #if defined(CacheEdge) || defined(FindingCE)
      #ifdef FindingCE
         extern int FoundCE;
         const int CE_maxK = ((ATL_DivBySize(FoundCE)-(NBNB SHIFT)) /
                              (NB*(NB+NB)))*NB;
      #else
         static const int CE_maxK = ((ATL_DivBySize(CacheEdge)-(NBNB SHIFT)) /
                                     (NB*(NB+NB)))*NB;
      #endif
      int Kp, k=K;
      #ifdef TCPLX
         const TYPE ONE[2] = {1.0, 0.0};
         const TYPE *bet=beta;
      #else
         const TYPE ONE=1.0;
         TYPE bet=beta;
      #endif
   #endif
   MMINTR mm1, mm2, mmNC;

   if (!M  ||  !N || !K) return;  /* quick return */
   #ifdef USERGEMM
      mm1 = mm2 = Mjoin(PATU,usergemm);
      if (N >= M) mmNC = Mjoin(PATL,NCmmJIK);
      else mmNC = Mjoin(PATL,NCmmIJK);
   #else
      if (N >= M)
      {
         mm1 = Mjoin(PATL,mmJIK);
         mm2 = Mjoin(PATL,mmIJK);
         mmNC = Mjoin(PATL,NCmmJIK);
      }
      else
      {
         mm1 = Mjoin(PATL,mmIJK);
         mm2 = Mjoin(PATL,mmJIK);
         mmNC = Mjoin(PATL,NCmmIJK);
      }
   #endif

/*
 * See what shape matrix has, in order to determine crossover point
 */
   #if defined(SMALL_MM)
      DOCOPY = 0;
   #elif defined(BIG_MM) || defined(FindingCE) || defined(ALIASED_GEMM)
      DOCOPY = 1;
   #else
      if (K <= ATL_3NB) DOCOPY = M*N >= Mjoin(TATB,_MNK_K)/K;
      else if (N <= ATL_3NB)
      {
         if (M <= ATL_3NB) DOCOPY = M*N >= Mjoin(TATB,_MNK_MN)/K;
         else DOCOPY = M*N >= Mjoin(TATB,_MNK_N)/K;
      }
      else if (M <= ATL_3NB) DOCOPY = M*N >= Mjoin(TATB,_MNK_M)/K;
      else DOCOPY = M*N >= Mjoin(TATB,_MNK_GE)/K;
   #endif
   if (!DOCOPY) mm1 = mm2 = mmNC;
/*
 * If CacheEdge is set, try to partition K so we have cache reuse
 * on panels of outer matrix
 */
   #if defined(CacheEdge) || defined(FindingCE)
      Kp = Mmin(CE_maxK, K);
      if (Kp < NB) Kp = K;
/*
 *    If partitioning K would cause us to do more than 1/2 as many extra writes
 *    as we save in reads, forget it
 */
      else if (ATL_MulByNB(K) >= (N<<1)*Kp) Kp = K;
      do
      {
            if ( mm1(ETA, ETB, M, N, Kp, alpha, A, lda, B, ldb, bet, C, ldc) )
               if ( ATL_OOM mm2(ETA, ETB, M, N, Kp, alpha, A, lda, B, ldb,
                                bet, C, ldc) )
                  ATL_assert(mmNC(ETA, ETB, M, N, Kp, alpha, A, lda, B, ldb,
                                  bet, C, ldc) == 0);
         bet = ONE;
         A += incA;
         B += incB;
         k -= Kp;
         if (Kp > k) Kp = k;
      }
      while(k);
   #else
      if ( mm1(ETA, ETB, M, N, K, alpha, A, lda, B, ldb, beta, C, ldc) )
         if ( mm2(ETA, ETB, M, N, K, alpha, A, lda, B, ldb, beta, C, ldc) )
            ATL_assert(mmNC(ETA, ETB, M, N, K, alpha, A, lda, B, ldb,
                            beta, C, ldc) == 0);
   #endif
}

#undef TATB
#undef tatb
#undef ETB
#undef TB_
#undef tb_
#undef incB
#undef ETA
#undef TA_
#undef ta_
#undef incA
#ifdef UNDEF_ATL_VOID
   #undef UNDEF_ATL_VOID
   #undef ATL_VOID
#endif
