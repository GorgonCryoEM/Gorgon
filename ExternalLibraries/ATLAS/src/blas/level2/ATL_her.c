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
 * Contributor(s) : R. Clint Whaley
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
#include "atlas_level1.h"
#include "atlas_kernel2.h"
#include "atlas_lvl2.h"
#include "atlas_mv.h"
#include "atlas_r1.h"

#include "atlas_reflvl2.h"          /* temporary for building purposes */
#include "atlas_reflevel2.h"        /* used for gbmv, gpmv and gpr.    */

void Mjoin( PATL, her )
(
   const enum ATLAS_UPLO      UPLO,
   const int                  N,
   const TYPE                 ALPHA,
   const TYPE                 * X,
   const int                  INCX,
   TYPE                       * A,
   const int                  LDA
)
{
/*
 * Purpose
 * =======
 *
 * Mjoin( PATL, her ) performs the Hermitian rank 1 operation
 *
 *    A := alpha* x * conjg( x' ) + A,
 *
 * where  alpha is a real scalar, x is an n-element vector and A is an n
 * by n Hermitian matrix.
 *
 * This is a blocked version of the algorithm.  For a more detailed des-
 * cription of  the arguments of this function, see the reference imple-
 * mentation in the ATLAS/src/blas/reference directory.
 *
 * ---------------------------------------------------------------------
 */
/*
 * .. Local Variables ..
 */
   TYPE                       Calph[2];
#ifdef TREAL
#define    lda2               LDA
#define    one                ATL_rone
#else
   const TYPE                 one[2] = { ATL_rone, ATL_rzero };
   const int                  lda2   = ( LDA SHIFT );
#endif
   void                       * vx = NULL;
   TYPE                       * x, * y, * A0, * y0;
   int                        incA0, incA, incX, incY, incy, mb, mb1, n, nb;
#define   ger1    Mjoin( PATL, ger1c_a1_x1_yX )
/* ..
 * .. Executable Statements ..
 *
 */
   if( ( N == 0 ) || ( ALPHA == ATL_rzero ) ) return;

   if( ( INCX == 1 ) && ( ALPHA == ATL_rone ) )
   {
      x = y = (TYPE *)(X); incy = INCX;
   }
   else
   {
      vx = (void *)malloc( ATL_Cachelen + ATL_MulBySize( N ) );
      ATL_assert( vx ); x = ATL_AlignPtr( vx );
      *Calph = ALPHA; Calph[1] = ATL_rzero;
      Mjoin( PATL, cpsc )( N, Calph, X, INCX, x, 1 );
      y = (TYPE *)(X); incy = INCX;
   }

   ATL_GetPartR1( A, LDA, mb, nb );

   mb1  = N - ( ( N - 1 ) / mb ) * mb;

   if( UPLO == AtlasLower )
   {
      incA = ( incA0 = ( incX  = (mb SHIFT) ) ) + mb * lda2;

      Mjoin( PATL, herL )( mb1, x, y, incy, A, LDA );
      A0 = (TYPE *)( A += (mb1 SHIFT) ); A += mb1 * lda2; x += (mb1 SHIFT);

      for( n = mb1; n < N; n += mb, A0 += incA0, A += incA, x += incX )
      {
         ger1( mb, n, one, x, 1, y, incy, A0, LDA );
         Mjoin( PATL, herL )( mb, x, y + n*(incy SHIFT), incy, A, LDA );
      }
   }
   else
   {
      incA = mb * lda2 + ( incX = (mb SHIFT) ); incY = incX * incy;

      for( n  = N - mb, A0 = (TYPE *)(A) + mb * lda2, y0 = y + incY; n > 0;
           n -= mb, A0 += incA, A += incA, x += incX, y += incY, y0 += incY )
      {
         Mjoin( PATL, herU )( mb, x, y, incy, A, LDA );
         ger1( mb, n, one, x, 1, y0, incy, A0, LDA );
      }
      Mjoin( PATL, herU )( mb1, x, y, incy, A, LDA );
   }

   if( vx ) free( vx );
/*
 * End of Mjoin( PATL, her )
 */
}
