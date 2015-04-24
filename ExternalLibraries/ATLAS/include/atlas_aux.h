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
/*
 * Header file for ATLAS's auxiliary routines
 */
#ifndef ATLAS_AUX_H
#define ATLAS_AUX_H
#include "atlas_misc.h"

void ATL_xerbla(int p, char *rout, char *form, ...);
int ATL_lcm(const int M, const int N);
double ATL_walltime();
double ATL_cputime();

/*
 * Auxiliary routines that come in all four types
 */
void ATL_sgeadd(const int M, const int N, const float alpha,
                const float *A, const int lda, const float beta,
                float *C, const int ldc);

void ATL_sgemove(const int M, const int N, const float alpha,
                 const float *A, const int lda, float *C, const int ldc);

void ATL_sgecopy(const int M, const int N, const float *A, const int lda,
                 float *C, const int ldc);

void ATL_sgescal(const int M, const int N, const float beta,
                 float *C, const int ldc);
void ATL_strscal
   (const enum ATLAS_UPLO Uplo, const int M, const int N, const float alpha,
    float *A, const int lda);
void ATL_shescal
   (const enum ATLAS_UPLO Uplo, const int M, const int N, const float alpha,
    float *A, const int lda);

void ATL_sgezero(const int M, const int N, float *C, const int ldc);

void ATL_szero(const int N, float *X, const int incX);
void ATL_sset(const int N, const float alpha, float *X, const int incX);
void ATL_sscal(const int N, const float alpha, float *X, const int incX);
void ATL_scopy(const int N, const float *X, const int incX,
               float *Y, const int incY);
void ATL_scpsc(const int N, const float alpha, const float *X,
               const int incX, float *Y, const int incY);
void ATL_saxpy(const int N, const float alpha, const float *X,
               const int incX, float *Y, const int incY);
void ATL_saxpy_x1_y1(const int N, const float alpha, const float *X,
                     const int incX, float *Y, const int incY);
void ATL_saxpby(const int N, const float alpha, const float *X,
                const int incX, const float beta, float *Y, const int incY);

void ATL_sgeadd_a1_b1
   (const int M, const int N, const float alpha, const float *A,
    const int lda, const float beta, float *C, const int ldc);
void ATL_saxpby_a1_b1
   (const int N, const float alpha, const float *X, const int incX,
    const float beta, float *Y, const int incY);
void ATL_sgeadd_a0_b1
   (const int M, const int N, const float alpha, const float *A,
    const int lda, const float beta, float *C, const int ldc);
void ATL_saxpby_a0_b1
   (const int N, const float alpha, const float *X, const int incX,
    const float beta, float *Y, const int incY);
void ATL_sgeadd_aX_b1
   (const int M, const int N, const float alpha, const float *A,
    const int lda, const float beta, float *C, const int ldc);
void ATL_saxpby_aX_b1
   (const int N, const float alpha, const float *X, const int incX,
    const float beta, float *Y, const int incY);
void ATL_sgeadd_a1_b0
   (const int M, const int N, const float alpha, const float *A,
    const int lda, const float beta, float *C, const int ldc);
void ATL_saxpby_a1_b0
   (const int N, const float alpha, const float *X, const int incX,
    const float beta, float *Y, const int incY);
void ATL_sgeadd_a0_b0
   (const int M, const int N, const float alpha, const float *A,
    const int lda, const float beta, float *C, const int ldc);
void ATL_saxpby_a0_b0
   (const int N, const float alpha, const float *X, const int incX,
    const float beta, float *Y, const int incY);
void ATL_sgeadd_aX_b0
   (const int M, const int N, const float alpha, const float *A,
    const int lda, const float beta, float *C, const int ldc);
void ATL_saxpby_aX_b0
   (const int N, const float alpha, const float *X, const int incX,
    const float beta, float *Y, const int incY);
void ATL_sgeadd_a1_bX
   (const int M, const int N, const float alpha, const float *A,
    const int lda, const float beta, float *C, const int ldc);
void ATL_saxpby_a1_bX
   (const int N, const float alpha, const float *X, const int incX,
    const float beta, float *Y, const int incY);
void ATL_sgeadd_a0_bX
   (const int M, const int N, const float alpha, const float *A,
    const int lda, const float beta, float *C, const int ldc);
void ATL_saxpby_a0_bX
   (const int N, const float alpha, const float *X, const int incX,
    const float beta, float *Y, const int incY);
void ATL_sgeadd_aX_bX
   (const int M, const int N, const float alpha, const float *A,
    const int lda, const float beta, float *C, const int ldc);
void ATL_saxpby_aX_bX
   (const int N, const float alpha, const float *X, const int incX,
    const float beta, float *Y, const int incY);

void ATL_sgemove_a1
   (const int M, const int N, const float alpha, const float *A,
    const int lda, float *C, const int ldc);
void ATL_sgemove_a0
   (const int M, const int N, const float alpha, const float *A,
    const int lda, float *C, const int ldc);
void ATL_sgemove_aX
   (const int M, const int N, const float alpha, const float *A,
    const int lda, float *C, const int ldc);

void ATL_sgescal_b1
   (const int M, const int N, const float beta, float *C, const int ldc);
void ATL_sgescal_b0
   (const int M, const int N, const float beta, float *C, const int ldc);
void ATL_sgescal_bX
   (const int M, const int N, const float beta, float *C, const int ldc);

void ATL_dgeadd(const int M, const int N, const double alpha,
                const double *A, const int lda, const double beta,
                double *C, const int ldc);

void ATL_dgemove(const int M, const int N, const double alpha,
                 const double *A, const int lda, double *C, const int ldc);

void ATL_dgecopy(const int M, const int N, const double *A, const int lda,
                 double *C, const int ldc);

void ATL_dgescal(const int M, const int N, const double beta,
                 double *C, const int ldc);
void ATL_dtrscal
   (const enum ATLAS_UPLO Uplo, const int M, const int N, const double alpha,
    double *A, const int lda);
void ATL_dhescal
   (const enum ATLAS_UPLO Uplo, const int M, const int N, const double alpha,
    double *A, const int lda);

void ATL_dgezero(const int M, const int N, double *C, const int ldc);

void ATL_dzero(const int N, double *X, const int incX);
void ATL_dset(const int N, const double alpha, double *X, const int incX);
void ATL_dscal(const int N, const double alpha, double *X, const int incX);
void ATL_dcopy(const int N, const double *X, const int incX,
               double *Y, const int incY);
void ATL_dcpsc(const int N, const double alpha, const double *X,
               const int incX, double *Y, const int incY);
void ATL_daxpy(const int N, const double alpha, const double *X,
               const int incX, double *Y, const int incY);
void ATL_daxpy_x1_y1(const int N, const double alpha, const double *X,
                     const int incX, double *Y, const int incY);
void ATL_daxpby(const int N, const double alpha, const double *X,
                const int incX, const double beta, double *Y, const int incY);

void ATL_dgeadd_a1_b1
   (const int M, const int N, const double alpha, const double *A,
    const int lda, const double beta, double *C, const int ldc);
void ATL_daxpby_a1_b1
   (const int N, const double alpha, const double *X, const int incX,
    const double beta, double *Y, const int incY);
void ATL_dgeadd_a0_b1
   (const int M, const int N, const double alpha, const double *A,
    const int lda, const double beta, double *C, const int ldc);
void ATL_daxpby_a0_b1
   (const int N, const double alpha, const double *X, const int incX,
    const double beta, double *Y, const int incY);
void ATL_dgeadd_aX_b1
   (const int M, const int N, const double alpha, const double *A,
    const int lda, const double beta, double *C, const int ldc);
void ATL_daxpby_aX_b1
   (const int N, const double alpha, const double *X, const int incX,
    const double beta, double *Y, const int incY);
void ATL_dgeadd_a1_b0
   (const int M, const int N, const double alpha, const double *A,
    const int lda, const double beta, double *C, const int ldc);
void ATL_daxpby_a1_b0
   (const int N, const double alpha, const double *X, const int incX,
    const double beta, double *Y, const int incY);
void ATL_dgeadd_a0_b0
   (const int M, const int N, const double alpha, const double *A,
    const int lda, const double beta, double *C, const int ldc);
void ATL_daxpby_a0_b0
   (const int N, const double alpha, const double *X, const int incX,
    const double beta, double *Y, const int incY);
void ATL_dgeadd_aX_b0
   (const int M, const int N, const double alpha, const double *A,
    const int lda, const double beta, double *C, const int ldc);
void ATL_daxpby_aX_b0
   (const int N, const double alpha, const double *X, const int incX,
    const double beta, double *Y, const int incY);
void ATL_dgeadd_a1_bX
   (const int M, const int N, const double alpha, const double *A,
    const int lda, const double beta, double *C, const int ldc);
void ATL_daxpby_a1_bX
   (const int N, const double alpha, const double *X, const int incX,
    const double beta, double *Y, const int incY);
void ATL_dgeadd_a0_bX
   (const int M, const int N, const double alpha, const double *A,
    const int lda, const double beta, double *C, const int ldc);
void ATL_daxpby_a0_bX
   (const int N, const double alpha, const double *X, const int incX,
    const double beta, double *Y, const int incY);
void ATL_dgeadd_aX_bX
   (const int M, const int N, const double alpha, const double *A,
    const int lda, const double beta, double *C, const int ldc);
void ATL_daxpby_aX_bX
   (const int N, const double alpha, const double *X, const int incX,
    const double beta, double *Y, const int incY);

void ATL_dgemove_a1
   (const int M, const int N, const double alpha, const double *A,
    const int lda, double *C, const int ldc);
void ATL_dgemove_a0
   (const int M, const int N, const double alpha, const double *A,
    const int lda, double *C, const int ldc);
void ATL_dgemove_aX
   (const int M, const int N, const double alpha, const double *A,
    const int lda, double *C, const int ldc);

void ATL_dgescal_b1
   (const int M, const int N, const double beta, double *C, const int ldc);
void ATL_dgescal_b0
   (const int M, const int N, const double beta, double *C, const int ldc);
void ATL_dgescal_bX
   (const int M, const int N, const double beta, double *C, const int ldc);

void ATL_cgeadd(const int M, const int N, const float *alpha,
                const float *A, const int lda, const float *beta,
                float *C, const int ldc);

void ATL_cgemove(const int M, const int N, const float *alpha,
                 const float *A, const int lda, float *C, const int ldc);

void ATL_cgecopy(const int M, const int N, const float *A, const int lda,
                 float *C, const int ldc);

void ATL_cgescal(const int M, const int N, const float *beta,
                 float *C, const int ldc);
void ATL_ctrscal
   (const enum ATLAS_UPLO Uplo, const int M, const int N, const float *alpha,
    float *A, const int lda);
void ATL_chescal
   (const enum ATLAS_UPLO Uplo, const int M, const int N, const float alpha,
    float *A, const int lda);

void ATL_cgezero(const int M, const int N, float *C, const int ldc);

void ATL_czero(const int N, float *X, const int incX);
void ATL_cset(const int N, const float *alpha, float *X, const int incX);
void ATL_cscal(const int N, const float *alpha, float *X, const int incX);
void ATL_ccopy(const int N, const float *X, const int incX,
               float *Y, const int incY);
void ATL_ccpsc(const int N, const float *alpha, const float *X,
               const int incX, float *Y, const int incY);
void ATL_caxpy(const int N, const float *alpha, const float *X,
               const int incX, float *Y, const int incY);
void ATL_caxpy_x1_y1(const int N, const float *alpha, const float *X,
                     const int incX, float *Y, const int incY);
void ATL_caxpby(const int N, const float *alpha, const float *X,
                const int incX, const float *beta, float *Y, const int incY);

void ATL_cgeadd_a1_b1
   (const int M, const int N, const float *alpha, const float *A,
    const int lda, const float *beta, float *C, const int ldc);
void ATL_caxpby_a1_b1
   (const int N, const float *alpha, const float *X, const int incX,
    const float *beta, float *Y, const int incY);
void ATL_cgeadd_a0_b1
   (const int M, const int N, const float *alpha, const float *A,
    const int lda, const float *beta, float *C, const int ldc);
void ATL_caxpby_a0_b1
   (const int N, const float *alpha, const float *X, const int incX,
    const float *beta, float *Y, const int incY);
void ATL_cgeadd_aX_b1
   (const int M, const int N, const float *alpha, const float *A,
    const int lda, const float *beta, float *C, const int ldc);
void ATL_caxpby_aX_b1
   (const int N, const float *alpha, const float *X, const int incX,
    const float *beta, float *Y, const int incY);
void ATL_cgeadd_a1_b0
   (const int M, const int N, const float *alpha, const float *A,
    const int lda, const float *beta, float *C, const int ldc);
void ATL_caxpby_a1_b0
   (const int N, const float *alpha, const float *X, const int incX,
    const float *beta, float *Y, const int incY);
void ATL_cgeadd_a0_b0
   (const int M, const int N, const float *alpha, const float *A,
    const int lda, const float *beta, float *C, const int ldc);
void ATL_caxpby_a0_b0
   (const int N, const float *alpha, const float *X, const int incX,
    const float *beta, float *Y, const int incY);
void ATL_cgeadd_aX_b0
   (const int M, const int N, const float *alpha, const float *A,
    const int lda, const float *beta, float *C, const int ldc);
void ATL_caxpby_aX_b0
   (const int N, const float *alpha, const float *X, const int incX,
    const float *beta, float *Y, const int incY);
void ATL_cgeadd_a1_bX
   (const int M, const int N, const float *alpha, const float *A,
    const int lda, const float *beta, float *C, const int ldc);
void ATL_caxpby_a1_bX
   (const int N, const float *alpha, const float *X, const int incX,
    const float *beta, float *Y, const int incY);
void ATL_cgeadd_a0_bX
   (const int M, const int N, const float *alpha, const float *A,
    const int lda, const float *beta, float *C, const int ldc);
void ATL_caxpby_a0_bX
   (const int N, const float *alpha, const float *X, const int incX,
    const float *beta, float *Y, const int incY);
void ATL_cgeadd_aX_bX
   (const int M, const int N, const float *alpha, const float *A,
    const int lda, const float *beta, float *C, const int ldc);
void ATL_caxpby_aX_bX
   (const int N, const float *alpha, const float *X, const int incX,
    const float *beta, float *Y, const int incY);

void ATL_cgemove_a1
   (const int M, const int N, const float *alpha, const float *A,
    const int lda, float *C, const int ldc);
void ATL_cgemove_a0
   (const int M, const int N, const float *alpha, const float *A,
    const int lda, float *C, const int ldc);
void ATL_cgemove_aX
   (const int M, const int N, const float *alpha, const float *A,
    const int lda, float *C, const int ldc);

void ATL_cgescal_b1
   (const int M, const int N, const float *beta, float *C, const int ldc);
void ATL_cgescal_b0
   (const int M, const int N, const float *beta, float *C, const int ldc);
void ATL_cgescal_bX
   (const int M, const int N, const float *beta, float *C, const int ldc);

void ATL_zgeadd(const int M, const int N, const double *alpha,
                const double *A, const int lda, const double *beta,
                double *C, const int ldc);

void ATL_zgemove(const int M, const int N, const double *alpha,
                 const double *A, const int lda, double *C, const int ldc);

void ATL_zgecopy(const int M, const int N, const double *A, const int lda,
                 double *C, const int ldc);

void ATL_zgescal(const int M, const int N, const double *beta,
                 double *C, const int ldc);
void ATL_ztrscal
   (const enum ATLAS_UPLO Uplo, const int M, const int N, const double *alpha,
    double *A, const int lda);
void ATL_zhescal
   (const enum ATLAS_UPLO Uplo, const int M, const int N, const double alpha,
    double *A, const int lda);

void ATL_zgezero(const int M, const int N, double *C, const int ldc);

void ATL_zzero(const int N, double *X, const int incX);
void ATL_zset(const int N, const double *alpha, double *X, const int incX);
void ATL_zscal(const int N, const double *alpha, double *X, const int incX);
void ATL_zcopy(const int N, const double *X, const int incX,
               double *Y, const int incY);
void ATL_zcpsc(const int N, const double *alpha, const double *X,
               const int incX, double *Y, const int incY);
void ATL_zaxpy(const int N, const double *alpha, const double *X,
               const int incX, double *Y, const int incY);
void ATL_zaxpy_x1_y1(const int N, const double *alpha, const double *X,
                     const int incX, double *Y, const int incY);
void ATL_zaxpby(const int N, const double *alpha, const double *X,
                const int incX, const double *beta, double *Y, const int incY);

void ATL_zgeadd_a1_b1
   (const int M, const int N, const double *alpha, const double *A,
    const int lda, const double *beta, double *C, const int ldc);
void ATL_zaxpby_a1_b1
   (const int N, const double *alpha, const double *X, const int incX,
    const double *beta, double *Y, const int incY);
void ATL_zgeadd_a0_b1
   (const int M, const int N, const double *alpha, const double *A,
    const int lda, const double *beta, double *C, const int ldc);
void ATL_zaxpby_a0_b1
   (const int N, const double *alpha, const double *X, const int incX,
    const double *beta, double *Y, const int incY);
void ATL_zgeadd_aX_b1
   (const int M, const int N, const double *alpha, const double *A,
    const int lda, const double *beta, double *C, const int ldc);
void ATL_zaxpby_aX_b1
   (const int N, const double *alpha, const double *X, const int incX,
    const double *beta, double *Y, const int incY);
void ATL_zgeadd_a1_b0
   (const int M, const int N, const double *alpha, const double *A,
    const int lda, const double *beta, double *C, const int ldc);
void ATL_zaxpby_a1_b0
   (const int N, const double *alpha, const double *X, const int incX,
    const double *beta, double *Y, const int incY);
void ATL_zgeadd_a0_b0
   (const int M, const int N, const double *alpha, const double *A,
    const int lda, const double *beta, double *C, const int ldc);
void ATL_zaxpby_a0_b0
   (const int N, const double *alpha, const double *X, const int incX,
    const double *beta, double *Y, const int incY);
void ATL_zgeadd_aX_b0
   (const int M, const int N, const double *alpha, const double *A,
    const int lda, const double *beta, double *C, const int ldc);
void ATL_zaxpby_aX_b0
   (const int N, const double *alpha, const double *X, const int incX,
    const double *beta, double *Y, const int incY);
void ATL_zgeadd_a1_bX
   (const int M, const int N, const double *alpha, const double *A,
    const int lda, const double *beta, double *C, const int ldc);
void ATL_zaxpby_a1_bX
   (const int N, const double *alpha, const double *X, const int incX,
    const double *beta, double *Y, const int incY);
void ATL_zgeadd_a0_bX
   (const int M, const int N, const double *alpha, const double *A,
    const int lda, const double *beta, double *C, const int ldc);
void ATL_zaxpby_a0_bX
   (const int N, const double *alpha, const double *X, const int incX,
    const double *beta, double *Y, const int incY);
void ATL_zgeadd_aX_bX
   (const int M, const int N, const double *alpha, const double *A,
    const int lda, const double *beta, double *C, const int ldc);
void ATL_zaxpby_aX_bX
   (const int N, const double *alpha, const double *X, const int incX,
    const double *beta, double *Y, const int incY);

void ATL_zgemove_a1
   (const int M, const int N, const double *alpha, const double *A,
    const int lda, double *C, const int ldc);
void ATL_zgemove_a0
   (const int M, const int N, const double *alpha, const double *A,
    const int lda, double *C, const int ldc);
void ATL_zgemove_aX
   (const int M, const int N, const double *alpha, const double *A,
    const int lda, double *C, const int ldc);

void ATL_zgescal_b1
   (const int M, const int N, const double *beta, double *C, const int ldc);
void ATL_zgescal_b0
   (const int M, const int N, const double *beta, double *C, const int ldc);
void ATL_zgescal_bX
   (const int M, const int N, const double *beta, double *C, const int ldc);

/*
 * Specialized complex auxiliary routines
 */

void ATL_ccplxinvert
   (const int N, float *X, const int incX, float *Y, const int incY);

void ATL_cscalConj
   (const int N, const float *alpha, float *X, const int incX);
void ATL_ccopyConj
   (const int N, const float *X, const int incX, float *Y, const int incY);
void ATL_cmoveConj
   (const int N, const float *alpha, const float *X, const int incX,
    float *Y, const int incY);
void ATL_caxpyConj
   (const int N, const float *alpha, const float *X, const int incX,
    float *Y, const int incY);
void ATL_caxpyConj_x1_y1(const int N, const float *alpha, const float *X,
                         const int incX, float *Y, const int incY);
void ATL_caxpbyConj
   (const int N, const float *alpha, const float *X, const int incX,
    const float *beta, float *Y, const int incY);

void ATL_cgeaddConj_aXi0_b1
   (const int M, const int N, const float *alpha, const float *A,
    const int lda, const float *beta, float *C, const int ldc);
void ATL_cgeaddConj_a1_b1
   (const int M, const int N, const float *alpha, const float *A,
    const int lda, const float *beta, float *C, const int ldc);
void ATL_cgeaddConj_a0_b1
   (const int M, const int N, const float *alpha, const float *A,
    const int lda, const float *beta, float *C, const int ldc);
void ATL_cgeaddConj_aXi0_b1
   (const int M, const int N, const float *alpha, const float *A,
    const int lda, const float *beta, float *C, const int ldc);
void ATL_cgeaddConj_aX_b1
   (const int M, const int N, const float *alpha, const float *A,
    const int lda, const float *beta, float *C, const int ldc);
void ATL_cgeaddConj_aXi0_b0
   (const int M, const int N, const float *alpha, const float *A,
    const int lda, const float *beta, float *C, const int ldc);
void ATL_cgeaddConj_a1_b0
   (const int M, const int N, const float *alpha, const float *A,
    const int lda, const float *beta, float *C, const int ldc);
void ATL_cgeaddConj_a0_b0
   (const int M, const int N, const float *alpha, const float *A,
    const int lda, const float *beta, float *C, const int ldc);
void ATL_cgeaddConj_aXi0_b0
   (const int M, const int N, const float *alpha, const float *A,
    const int lda, const float *beta, float *C, const int ldc);
void ATL_cgeaddConj_aX_b0
   (const int M, const int N, const float *alpha, const float *A,
    const int lda, const float *beta, float *C, const int ldc);
void ATL_cgeaddConj_aXi0_bXi0
   (const int M, const int N, const float *alpha, const float *A,
    const int lda, const float *beta, float *C, const int ldc);
void ATL_cgeaddConj_a1_bXi0
   (const int M, const int N, const float *alpha, const float *A,
    const int lda, const float *beta, float *C, const int ldc);
void ATL_cgeaddConj_a0_bXi0
   (const int M, const int N, const float *alpha, const float *A,
    const int lda, const float *beta, float *C, const int ldc);
void ATL_cgeaddConj_aXi0_bXi0
   (const int M, const int N, const float *alpha, const float *A,
    const int lda, const float *beta, float *C, const int ldc);
void ATL_cgeaddConj_aX_bXi0
   (const int M, const int N, const float *alpha, const float *A,
    const int lda, const float *beta, float *C, const int ldc);
void ATL_cgeaddConj_aXi0_bX
   (const int M, const int N, const float *alpha, const float *A,
    const int lda, const float *beta, float *C, const int ldc);
void ATL_cgeaddConj_a1_bX
   (const int M, const int N, const float *alpha, const float *A,
    const int lda, const float *beta, float *C, const int ldc);
void ATL_cgeaddConj_a0_bX
   (const int M, const int N, const float *alpha, const float *A,
    const int lda, const float *beta, float *C, const int ldc);
void ATL_cgeaddConj_aXi0_bX
   (const int M, const int N, const float *alpha, const float *A,
    const int lda, const float *beta, float *C, const int ldc);
void ATL_cgeaddConj_aX_bX
   (const int M, const int N, const float *alpha, const float *A,
    const int lda, const float *beta, float *C, const int ldc);
void ATL_caxpby_aXi0_b1
   (const int N, const float *alpha, const float *X, const int incX,
    const float *beta, float *Y, const int incY);
void ATL_caxpby_aXi0_b1
   (const int N, const float *alpha, const float *X, const int incX,
    const float *beta, float *Y, const int incY);
void ATL_cgeadd_aXi0_b1
   (const int M, const int N, const float *alpha, const float *A,
    const int lda, const float *beta, float *C, const int ldc);
void ATL_caxpby_aXi0_b0
   (const int N, const float *alpha, const float *X, const int incX,
    const float *beta, float *Y, const int incY);
void ATL_caxpby_aXi0_b0
   (const int N, const float *alpha, const float *X, const int incX,
    const float *beta, float *Y, const int incY);
void ATL_cgeadd_aXi0_b0
   (const int M, const int N, const float *alpha, const float *A,
    const int lda, const float *beta, float *C, const int ldc);
void ATL_caxpby_aXi0_bXi0
   (const int N, const float *alpha, const float *X, const int incX,
    const float *beta, float *Y, const int incY);
void ATL_caxpby_aXi0_bXi0
   (const int N, const float *alpha, const float *X, const int incX,
    const float *beta, float *Y, const int incY);
void ATL_cgeadd_aXi0_bXi0
   (const int M, const int N, const float *alpha, const float *A,
    const int lda, const float *beta, float *C, const int ldc);
void ATL_caxpby_aXi0_bX
   (const int N, const float *alpha, const float *X, const int incX,
    const float *beta, float *Y, const int incY);
void ATL_caxpby_aXi0_bX
   (const int N, const float *alpha, const float *X, const int incX,
    const float *beta, float *Y, const int incY);
void ATL_cgeadd_aXi0_bX
   (const int M, const int N, const float *alpha, const float *A,
    const int lda, const float *beta, float *C, const int ldc);
void ATL_caxpby_a1_bXi0
   (const int N, const float *alpha, const float *X, const int incX,
    const float *beta, float *Y, const int incY);
void ATL_cgeadd_a1_bXi0
   (const int M, const int N, const float *alpha, const float *A,
    const int lda, const float *beta, float *C, const int ldc);
void ATL_caxpby_a0_bXi0
   (const int N, const float *alpha, const float *X, const int incX,
    const float *beta, float *Y, const int incY);
void ATL_cgeadd_a0_bXi0
   (const int M, const int N, const float *alpha, const float *A,
    const int lda, const float *beta, float *C, const int ldc);
void ATL_caxpby_aX_bXi0
   (const int N, const float *alpha, const float *X, const int incX,
    const float *beta, float *Y, const int incY);
void ATL_cgeadd_aX_bXi0
   (const int M, const int N, const float *alpha, const float *A,
    const int lda, const float *beta, float *C, const int ldc);

void ATL_cgemove_aXi0
   (const int M, const int N, const float *alpha0, const float *A,
    const int lda, float *C, const int ldc);

void ATL_cgescal_bXi0
   (const int M, const int N, const float *beta, float *C, const int ldc);

void ATL_zcplxinvert
   (const int N, double *X, const int incX, double *Y, const int incY);

void ATL_zscalConj
   (const int N, const double *alpha, double *X, const int incX);
void ATL_zcopyConj
   (const int N, const double *X, const int incX, double *Y, const int incY);
void ATL_zmoveConj
   (const int N, const double *alpha, const double *X, const int incX,
    double *Y, const int incY);
void ATL_zaxpyConj
   (const int N, const double *alpha, const double *X, const int incX,
    double *Y, const int incY);
void ATL_zaxpyConj_x1_y1(const int N, const double *alpha, const double *X,
                         const int incX, double *Y, const int incY);
void ATL_zaxpbyConj
   (const int N, const double *alpha, const double *X, const int incX,
    const double *beta, double *Y, const int incY);

void ATL_zgeaddConj_aXi0_b1
   (const int M, const int N, const double *alpha, const double *A,
    const int lda, const double *beta, double *C, const int ldc);
void ATL_zgeaddConj_a1_b1
   (const int M, const int N, const double *alpha, const double *A,
    const int lda, const double *beta, double *C, const int ldc);
void ATL_zgeaddConj_a0_b1
   (const int M, const int N, const double *alpha, const double *A,
    const int lda, const double *beta, double *C, const int ldc);
void ATL_zgeaddConj_aXi0_b1
   (const int M, const int N, const double *alpha, const double *A,
    const int lda, const double *beta, double *C, const int ldc);
void ATL_zgeaddConj_aX_b1
   (const int M, const int N, const double *alpha, const double *A,
    const int lda, const double *beta, double *C, const int ldc);
void ATL_zgeaddConj_aXi0_b0
   (const int M, const int N, const double *alpha, const double *A,
    const int lda, const double *beta, double *C, const int ldc);
void ATL_zgeaddConj_a1_b0
   (const int M, const int N, const double *alpha, const double *A,
    const int lda, const double *beta, double *C, const int ldc);
void ATL_zgeaddConj_a0_b0
   (const int M, const int N, const double *alpha, const double *A,
    const int lda, const double *beta, double *C, const int ldc);
void ATL_zgeaddConj_aXi0_b0
   (const int M, const int N, const double *alpha, const double *A,
    const int lda, const double *beta, double *C, const int ldc);
void ATL_zgeaddConj_aX_b0
   (const int M, const int N, const double *alpha, const double *A,
    const int lda, const double *beta, double *C, const int ldc);
void ATL_zgeaddConj_aXi0_bXi0
   (const int M, const int N, const double *alpha, const double *A,
    const int lda, const double *beta, double *C, const int ldc);
void ATL_zgeaddConj_a1_bXi0
   (const int M, const int N, const double *alpha, const double *A,
    const int lda, const double *beta, double *C, const int ldc);
void ATL_zgeaddConj_a0_bXi0
   (const int M, const int N, const double *alpha, const double *A,
    const int lda, const double *beta, double *C, const int ldc);
void ATL_zgeaddConj_aXi0_bXi0
   (const int M, const int N, const double *alpha, const double *A,
    const int lda, const double *beta, double *C, const int ldc);
void ATL_zgeaddConj_aX_bXi0
   (const int M, const int N, const double *alpha, const double *A,
    const int lda, const double *beta, double *C, const int ldc);
void ATL_zgeaddConj_aXi0_bX
   (const int M, const int N, const double *alpha, const double *A,
    const int lda, const double *beta, double *C, const int ldc);
void ATL_zgeaddConj_a1_bX
   (const int M, const int N, const double *alpha, const double *A,
    const int lda, const double *beta, double *C, const int ldc);
void ATL_zgeaddConj_a0_bX
   (const int M, const int N, const double *alpha, const double *A,
    const int lda, const double *beta, double *C, const int ldc);
void ATL_zgeaddConj_aXi0_bX
   (const int M, const int N, const double *alpha, const double *A,
    const int lda, const double *beta, double *C, const int ldc);
void ATL_zgeaddConj_aX_bX
   (const int M, const int N, const double *alpha, const double *A,
    const int lda, const double *beta, double *C, const int ldc);
void ATL_zaxpby_aXi0_b1
   (const int N, const double *alpha, const double *X, const int incX,
    const double *beta, double *Y, const int incY);
void ATL_zaxpby_aXi0_b1
   (const int N, const double *alpha, const double *X, const int incX,
    const double *beta, double *Y, const int incY);
void ATL_zgeadd_aXi0_b1
   (const int M, const int N, const double *alpha, const double *A,
    const int lda, const double *beta, double *C, const int ldc);
void ATL_zaxpby_aXi0_b0
   (const int N, const double *alpha, const double *X, const int incX,
    const double *beta, double *Y, const int incY);
void ATL_zaxpby_aXi0_b0
   (const int N, const double *alpha, const double *X, const int incX,
    const double *beta, double *Y, const int incY);
void ATL_zgeadd_aXi0_b0
   (const int M, const int N, const double *alpha, const double *A,
    const int lda, const double *beta, double *C, const int ldc);
void ATL_zaxpby_aXi0_bXi0
   (const int N, const double *alpha, const double *X, const int incX,
    const double *beta, double *Y, const int incY);
void ATL_zaxpby_aXi0_bXi0
   (const int N, const double *alpha, const double *X, const int incX,
    const double *beta, double *Y, const int incY);
void ATL_zgeadd_aXi0_bXi0
   (const int M, const int N, const double *alpha, const double *A,
    const int lda, const double *beta, double *C, const int ldc);
void ATL_zaxpby_aXi0_bX
   (const int N, const double *alpha, const double *X, const int incX,
    const double *beta, double *Y, const int incY);
void ATL_zaxpby_aXi0_bX
   (const int N, const double *alpha, const double *X, const int incX,
    const double *beta, double *Y, const int incY);
void ATL_zgeadd_aXi0_bX
   (const int M, const int N, const double *alpha, const double *A,
    const int lda, const double *beta, double *C, const int ldc);
void ATL_zaxpby_a1_bXi0
   (const int N, const double *alpha, const double *X, const int incX,
    const double *beta, double *Y, const int incY);
void ATL_zgeadd_a1_bXi0
   (const int M, const int N, const double *alpha, const double *A,
    const int lda, const double *beta, double *C, const int ldc);
void ATL_zaxpby_a0_bXi0
   (const int N, const double *alpha, const double *X, const int incX,
    const double *beta, double *Y, const int incY);
void ATL_zgeadd_a0_bXi0
   (const int M, const int N, const double *alpha, const double *A,
    const int lda, const double *beta, double *C, const int ldc);
void ATL_zaxpby_aX_bXi0
   (const int N, const double *alpha, const double *X, const int incX,
    const double *beta, double *Y, const int incY);
void ATL_zgeadd_aX_bXi0
   (const int M, const int N, const double *alpha, const double *A,
    const int lda, const double *beta, double *C, const int ldc);

void ATL_zgemove_aXi0
   (const int M, const int N, const double *alpha0, const double *A,
    const int lda, double *C, const int ldc);

void ATL_zgescal_bXi0
   (const int M, const int N, const double *beta, double *C, const int ldc);


#if defined(ATL_USEPTHREADS) && !defined(ATL_flushcache)
   #include "atlas_pthreads.h"
   #define ATL_flushcache ATL_ptflushcache
   #define ATL_PTCACHEMUL * ATL_NTHREADS
#else
   #define ATL_PTCACHEMUL
#endif
double ATL_flushcache(int size);

#endif
