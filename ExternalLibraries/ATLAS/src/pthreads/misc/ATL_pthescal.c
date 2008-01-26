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
#include "atlas_ptmisc.h"

PT_FUN_ARG_T Mjoin( PATL, pthescal0 )( PT_FUN_ARG_T ARGS )
{
/*
 * .. Local Variables ..
 */
   PT_TREE_T                  root = (PT_TREE_T)(ARGS);
   PT_TZSCAL_ARGS_T           * arg;
   int                        k, lda, m, n;
/* ..
 * .. Executable Statements ..
 *
 */
   ATL_wait_tree( root );
   arg = (PT_TZSCAL_ARGS_T *)(root->arg);

#define  _PT_ALPHA     (SCALAR)(arg->al)
#define  _PT_ALPHA_R   *((TYPE *)(arg->al))
#define  _PT_A         ((TYPE *)(arg->a))

   k = arg->k; lda = arg->la; m = arg->m; n = arg->n;

   if( arg->up == AtlasLower )
   {
      Mjoin( PATL, gescal )( m + k, n, _PT_ALPHA, _PT_A, lda );
      Mjoin( PATL, hescal )( AtlasLower, m + k, k, _PT_ALPHA_R,
                             _PT_A + ((n * lda) SHIFT), lda );
   }
   else
   {
      Mjoin( PATL, gescal )( m, n + k, _PT_ALPHA, _PT_A, lda );
      Mjoin( PATL, hescal )( AtlasUpper, k, n + k, _PT_ALPHA_R,
                             _PT_A + (m SHIFT), lda );
   }
   ATL_signal_tree( root );

   return( NULL );
/*
 * End of Mjoin( PATL, pthescal0 )
 */
}

void Mjoin( PATL, pthescal )
(
   const enum ATLAS_UPLO      UPLO,
   const int                  M,
   const int                  N,
   const TYPE                 ALPHA,
   TYPE                       * A,
   const int                  LDA
)
{
/*
 * Purpose
 * =======
 *
 * Mjoin( PATL, pthescal )  scales a trapezoidal Hermitian m-by-n matrix
 * A by the real scalar alpha.  The imaginary parts of the diagonal ele-
 * ments of A need not be set on input, they are assumed to be zero, and
 * on exit they are set to zero.
 *
 * This is a multi-threaded version of the algorithm.
 *
 * ---------------------------------------------------------------------
 */
/*
 * .. Local Variables ..
 */
   pthread_attr_t             attr;
   PT_TREE_T                  root = NULL;
   TYPE                       alpha[2];
/* ..
 * .. Executable Statements ..
 *
 */
   if( ( M <= 0 ) || ( N <= 0 ) ) return;

   alpha[0] = ALPHA; alpha[1] = ATL_rzero;

   ATL_thread_init( &attr );
   root = Mjoin( PATL, pthescal_nt )( ATL_NTHREADS, &attr, UPLO, M, N,
                                      (void *)(alpha), (void *)(A), LDA );
   ATL_join_tree  ( root );
   ATL_free_tree  ( root );
   ATL_thread_exit( &attr );
/*
 * End of Mjoin( PATL, pthescal )
 */
}

PT_TREE_T Mjoin( PATL, pthescal_nt )
(
   const unsigned int         THREADS,
   pthread_attr_t             * ATTR,
   const enum ATLAS_UPLO      UPLO,
   const int                  M,
   const int                  N,
   const void                 * ALPHA,
   void                       * A,
   const int                  LDA
)
{
/*
 * Purpose
 * =======
 *
 * Mjoin( PATL, pthescal_nt )  scales a trapezoidal Hermitian m-by-n matrix
 * A by the real scalar alpha.  The imaginary parts of the diagonal ele-
 * ments of A need not be set on input, they are assumed to be zero, and
 * on exit they are set to zero.
 *
 * This is a multi-threaded version of the algorithm.
 *
 * ---------------------------------------------------------------------
 */
/*
 * .. Local Variables ..
 */
   PT_TREE_T                  root = NULL;
   PT_MISC_TYPE_T             type;
   double                     tblks, tmnblks;
   unsigned int               nthreads;
   int                        mn, nb, nbm1;
/* ..
 * .. Executable Statements ..
 *
 */
/*
 * Make sure we don't thread this for the time being
 */
   if( THREADS >= 1 )
   {
      Mjoin( PATL, hescal )( UPLO, M, N, ((TYPE *)(ALPHA))[0],
                             (TYPE *)(A), LDA );
      return( root );
   }

   nbm1    = ( nb = Mjoin( PATL, GetNB )() ) - 1;
   mn      = Mmin( M, N );
   tmnblks = (double)( (mn+nbm1) / nb );
   tblks   = tmnblks * tmnblks;

   if( UPLO == AtlasLower )
   { tblks += (double)( (N+nbm1) / nb ) * (double)( (M-mn+nbm1) / nb ); }
   else
   { tblks += (double)( (M+nbm1) / nb ) * (double)( (N-mn+nbm1) / nb ); }

   if( ( tblks <= (double)(ATL_XOVER_MI_DEFAULT) ) || ( THREADS <= 1 ) )
   {
      Mjoin( PATL, hescal )( UPLO, M, N, ((TYPE *)(ALPHA))[0],
                             (TYPE *)(A), LDA );
      return( root );
   }

   if( tblks >= (double)(THREADS) ) { nthreads = THREADS; }
   else    { nthreads = (unsigned int)floor( tblks + 0.5 ); }

   type.size = sizeof( TYPE[2] ); type.fun = Mjoin( PATL, pthescal0 );

   if( UPLO == AtlasLower )
   {
      root = ATL_Stzscal( &type, 0, nthreads, ATTR, nb, AtlasLower,
                          M - mn, 0, mn, ALPHA, A, LDA );
   }
   else
   {
      root = ATL_Stzscal( &type, 0, nthreads, ATTR, nb, AtlasUpper,
                          0, N - mn, mn, ALPHA, A, LDA );
   }
   ATL_thread_tree( root, ATTR );

   return( root );
/*
 * End of Mjoin( PATL, pthescal_nt )
 */
}
