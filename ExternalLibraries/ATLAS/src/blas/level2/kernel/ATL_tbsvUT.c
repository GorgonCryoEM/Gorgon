/* ---------------------------------------------------------------------
 *
 * -- Automatically Tuned Linear Algebra Software (ATLAS)
 *    (C) Copyright 2000 All Rights Reserved
 *
 * -- ATLAS routine -- Version 3.2 -- December 25, 2000
 *
 * -- Suggestions,  comments,  bugs reports should be sent to the follo-
 *    wing e-mail address: atlas@cs.utk.edu
 *
 * Author         : Antoine P. Petitet
 * University of Tennessee - Innovative Computing Laboratory
 * Knoxville TN, 37996-1301, USA.
 *
 * ---------------------------------------------------------------------
 *
 * -- Copyright notice and Licensing terms:
 *
 *  Redistribution  and  use in  source and binary forms, with or without
 *  modification, are  permitted provided  that the following  conditions
 *  are met:
 *
 * 1. Redistributions  of  source  code  must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce  the above copyright
 *    notice,  this list of conditions, and the  following disclaimer in
 *    the documentation and/or other materials provided with the distri-
 *    bution.
 * 3. The name of the University,  the ATLAS group,  or the names of its
 *    contributors  may not be used to endorse or promote products deri-
 *    ved from this software without specific written permission.
 *
 * -- Disclaimer:
 *
 * THIS  SOFTWARE  IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES,  INCLUDING,  BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE UNIVERSITY
 * OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,  INDIRECT, INCIDENTAL, SPE-
 * CIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO,  PROCUREMENT  OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEO-
 * RY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT  (IN-
 * CLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * ---------------------------------------------------------------------
 */
/*
 * Include files
 */
#include "atlas_misc.h"
#include "atlas_mv.h"
#include "atlas_r1.h"
#include "atlas_level1.h"
#include "atlas_kernel2.h"
#include "atlas_reflvl2.h"
#include "atlas_lvl2.h"

void Mjoin( PATL, tbsvUT )
(
   const enum ATLAS_DIAG      DIAG,
   const int                  N,         /* N > 0 assumed */
   const int                  K,
   const TYPE                 * A,
   const int                  LDA,
   TYPE                       * X
)
{
/*
 * Purpose
 * =======
 *
 * Mjoin( PATL, tbsvUT ) solves the following triangular system of equations
 *
 *    A'* x = b,
 *
 * where b and x are n-element vectors and  A  is an n by n unit or non-
 * unit, upper triangular band matrix.
 *
 * No test for  singularity  or  near-singularity  is included  in  this
 * routine. Such tests must be performed before calling this routine.
 *
 * This is a blocked version of the algorithm.  For a more detailed des-
 * cription of  the arguments of this function, see the reference imple-
 * mentation in the  ATLAS/src/blas/reference directory.
 *
 * ---------------------------------------------------------------------
 */
/*
 * .. Local Variables ..
 */
   void                       (*tbsv0)( const int, const int, const TYPE *,
                              const int, TYPE * );
#ifdef TREAL
#define    lda2               LDA
#define    one                ATL_rone
#define    none               ATL_rnone
#else
   TYPE                       none[2] = { ATL_rnone, ATL_rzero },
                              one [2] = { ATL_rone,  ATL_rzero };
   const int                  lda2   = ( LDA SHIFT );
#endif
   int                        ia, ian, j, kl, ku, ma, mb, mb1, n, na, nb;
/* ..
 * .. Executable Statements ..
 *
 */
   ATL_GetPartMVT( A, LDA, &mb, &nb );

   if( DIAG == AtlasNonUnit ) tbsv0 = Mjoin( PATL, tbsvUTN );
   else                       tbsv0 = Mjoin( PATL, tbsvUTU );

   mb1  = N - ( ( N - 1 ) / mb ) * mb;

   for( n = N - mb, j = 0; n > 0; n -= mb, j += mb )
   {
      ian = j + mb; ia = mb - K; ia = j + Mmax( ia, 0 ); ma = ian - ia;
      na = N - ian; na = Mmin( na, K ); kl = ma - 1; kl = Mmax( kl, 0 );
      ku = K - 1 - kl; ku = Mmax( ku, 0 );
      tbsv0( mb, K, A+j*lda2, LDA, X+(j SHIFT) );
      Mjoin( PATL, gbmv )( AtlasTrans,     na, ma, kl, ku, none, A+ian*lda2,
                           LDA, X+(ia SHIFT), 1, one, X+(ian SHIFT), 1 );
   }
   tbsv0( mb1, K, A+j*lda2, LDA, X+(j SHIFT) );
/*
 * End of Mjoin( PATL, tbsvUT )
 */
}
