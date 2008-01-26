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
#include "atlas_ptlvl3.h"
#include "atlas_ptlevel3.h"

PT_FUN_ARG_T Mjoin( PATL, pttrsm0 )( PT_FUN_ARG_T ARGS )
{
/*
 * .. Local Variables ..
 */
   PT_TREE_T                  root = (PT_TREE_T)(ARGS);
   PT_TRSM_ARGS_T             * arg;
/* ..
 * .. Executable Statements ..
 *
 */
   ATL_wait_tree( root );
   arg = (PT_TRSM_ARGS_T *)(root->arg);

#define  _PT_SIDE      (arg->si)
#define  _PT_UPLO      (arg->up)
#define  _PT_TRANS     (arg->tr)
#define  _PT_DIAG      (arg->di)

#define  _PT_M         (arg->m)
#define  _PT_N         (arg->n)

#ifdef TREAL
#define  _PT_ALPHA     (SCALAR)(*((TYPE *)(arg->al)))
#else
#define  _PT_ALPHA     (SCALAR)(arg->al)
#endif

#define  _PT_A         ((TYPE *)(arg->a))
#define  _PT_LDA       (arg->la)
#define  _PT_B         ((TYPE *)(arg->b))
#define  _PT_LDB       (arg->lb)

   Mjoin( PATL, trsm )( _PT_SIDE, _PT_UPLO, _PT_TRANS, _PT_DIAG, _PT_M,
                        _PT_N, _PT_ALPHA, _PT_A, _PT_LDA, _PT_B, _PT_LDB );
   ATL_signal_tree( root );

   return( NULL );
/*
 * End of Mjoin( PATL, pttrsm0 )
 */
}

void Mjoin( PATL, pttrsm )
(
   const enum ATLAS_SIDE      SIDE,
   const enum ATLAS_UPLO      UPLO,
   const enum ATLAS_TRANS     TRANS,
   const enum ATLAS_DIAG      DIAG,
   const int                  M,
   const int                  N,
   const SCALAR               ALPHA,
   const TYPE                 * A,
   const int                  LDA,
   TYPE                       * B,
   const int                  LDB
)
{
/*
 * Purpose
 * =======
 *
 * Mjoin( PATL, pttrsm )  solves one of the matrix equations
 *
 *    op( A ) * X = alpha * B,   or  X * op( A ) = alpha * B,
 *
 * where alpha is a scalar, X and B are m by n matrices, A is a unit, or
 * non-unit, upper or lower triangular matrix and op( A ) is one of
 *
 *    op( A ) = A   or   op( A ) = A'   or   op( A ) = conjg( A' ).
 *
 * The matrix X is overwritten on B.
 *
 * For a  more  detailed description of  the arguments of this function,
 * see the reference implementation in the  ATLAS/src/blas/reference di-
 * rectory.
 *
 * ---------------------------------------------------------------------
 */
/*
 * .. Local Variables ..
 */
   pthread_attr_t             attr;
   PT_TREE_T                  root = NULL;
#ifdef TREAL
   TYPE                       alpha0 = ALPHA;
#endif
/* ..
 * .. Executable Statements ..
 *
 */
   if( ( M == 0 ) || ( N == 0 ) ) return;

   if( SCALAR_IS_ZERO( ALPHA ) )
   { Mjoin( PATL, ptgezero )( M, N, B, LDB ); return; }

   ATL_thread_init( &attr );
#ifdef TREAL
   root = Mjoin( PATL, pttrsm_nt )( ATL_NTHREADS, &attr, SIDE, UPLO,
                                    TRANS, DIAG, M, N, (void *)(&alpha0),
                                    (void *)(A), LDA, (void *)(B), LDB );
#else
   root = Mjoin( PATL, pttrsm_nt )( ATL_NTHREADS, &attr, SIDE, UPLO,
                                    TRANS, DIAG, M, N, (void *)(ALPHA),
                                    (void *)(A), LDA, (void *)(B), LDB );
#endif
   ATL_join_tree  ( root );
   ATL_free_tree  ( root );
   ATL_thread_exit( &attr );
/*
 * End of Mjoin( PATL, pttrsm )
 */
}

PT_TREE_T Mjoin( PATL, pttrsm_nt )
(
   const unsigned int         NTHREADS,
   pthread_attr_t             * ATTR,
   const enum ATLAS_SIDE      SIDE,
   const enum ATLAS_UPLO      UPLO,
   const enum ATLAS_TRANS     TRANS,
   const enum ATLAS_DIAG      DIAG,
   const int                  M,
   const int                  N,
   const void                 * ALPHA,
   const void                 * A,
   const int                  LDA,
   void                       * B,
   const int                  LDB
)
{
/*
 * Purpose
 * =======
 *
 * Mjoin( PATL, pttrsm_nt )  solves one of the matrix equations
 *
 *    op( A ) * X = alpha * B,   or  X * op( A ) = alpha * B,
 *
 * where alpha is a scalar, X and B are m by n matrices, A is a unit, or
 * non-unit, upper or lower triangular matrix and op( A ) is one of
 *
 *    op( A ) = A   or   op( A ) = A'   or   op( A ) = conjg( A' ).
 *
 * The matrix X is overwritten on B.
 *
 * For a  more  detailed description of  the arguments of this function,
 * see the reference implementation in the  ATLAS/src/blas/reference di-
 * rectory.
 *
 * ---------------------------------------------------------------------
 */
/*
 * .. Local Variables ..
 */
   PT_TREE_T                  root = NULL;
   PT_LVL3_TYPE_T             type;
   double                     tblks;
#ifdef TREAL
   TYPE                       alpha;
#else
   TYPE                       alpha[2];
#endif
   unsigned int               nthreads;
   int                        nb, nbm1;
/* ..
 * .. Executable Statements ..
 *
 */
   nbm1  = ( nb = Mjoin( PATL, GetNB )() ) - 1;
   tblks = (double)( (M+nbm1) / nb ) * (double)( (N+nbm1) / nb );
   if( SIDE == AtlasLeft ) { tblks *= (double)( (M+nbm1) / nb ); }
   else                    { tblks *= (double)( (N+nbm1) / nb ); }

   if( ( M <= nb ) || ( N <= nb ) || ( NTHREADS <= 1 ) ||
       ( tblks <= (double)(ATL_XOVER_L3_DEFAULT) ) )
   {
#ifdef TREAL
      alpha    = *((TYPE *)(ALPHA));
#else
      alpha[0] = ((TYPE *)(ALPHA))[0]; alpha[1] = ((TYPE *)(ALPHA))[1];
#endif
      Mjoin( PATL, trsm )( SIDE, UPLO, TRANS, DIAG, M, N, alpha,
                           (TYPE *)(A), LDA, (TYPE *)(B), LDB );
      return( root );
   }

   if( tblks >= (double)(NTHREADS) ) { nthreads = NTHREADS; }
   else    { nthreads = (unsigned int)floor( tblks + 0.5 ); }

   Mjoin( PATL, ptl3settype )( &type );
   root = ATL_Strsm( &type, 0, nthreads, ATTR, nb, SIDE, UPLO, TRANS,
                     DIAG, M, N, ALPHA, A, LDA, B, LDB );
   ATL_thread_tree( root, ATTR );

   return( root );
/*
 * End of Mjoin( PATL, pttrsm_nt )
 */
}
