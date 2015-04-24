    /*        Fast GEMM routine for Alpha 21164/21264      */
    /*         on  Linux, Digital UNIX                     */
    /*        by Kazushige Goto <goto@statabo.rim.or.jp>   */
/*
 * Modified by R. Clint Whaley (rwhaley@cs.utk.edu) on 03/24/00
 * for inclusion in ATLAS.
 * CHANGES:
 *   (1) Added support for ATLAS's data type macros
 *   (2) Changed routine name macros to match ATLAS's naming scheme
 *       and got rid of unused naming macros
 *   (3) Got rid of unused prototypes
 * To see all changes, the author's original tarfile is in :
 *    libgemm-20000228.tar.bz
 */

#ifndef COMMON_H
#define COMMON_H

#define ZERO 0.0000
#define ONE  1.0000

#if defined(DREAL) || defined(DCPLX)
   #ifndef DGEMM
      #define DGEMM
   #endif
#elif defined(SREAL) || defined(SCPLX)
   #ifdef DGEMM
      #undef DGEMM
   #endif
#endif

#ifdef DGEMM

#define GEMM_TT ATLU_dgotogemm_tt
#define GEMM_TN ATLU_dgotogemm_tn
#define GEMM_NT ATLU_dgotogemm_nt
#define GEMM_NN ATLU_dgotogemm_nn

#define ZGEMM_CC ATLU_zgotogemm_cc
#define ZGEMM_CT ATLU_zgotogemm_ct
#define ZGEMM_CN ATLU_zgotogemm_cn
#define ZGEMM_TC ATLU_zgotogemm_tc
#define ZGEMM_TT ATLU_zgotogemm_tt
#define ZGEMM_TN ATLU_zgotogemm_tn
#define ZGEMM_NC ATLU_zgotogemm_nc
#define ZGEMM_NT ATLU_zgotogemm_nt
#define ZGEMM_NN ATLU_zgotogemm_nn

#define FLOAT	double

#define SIZE	8
#define LD	ldt
#define ST	stt
#define SXADDQ	s8addq
#define MUL	mult
#define ADD	addt
#define SUB	subt

#define GEMM_PARAM_T  dgemm_param_t
#define ZGEMM_PARAM_T zgemm_param_t
#define GEMM_THREAD   dgemm_thread
#define ZGEMM_THREAD  zgemm_thread

#else

#define GEMM_TT ATLU_sgotogemm_tt
#define GEMM_TN ATLU_sgotogemm_tn
#define GEMM_NT ATLU_sgotogemm_nt
#define GEMM_NN ATLU_sgotogemm_nn

#define ZGEMM_CC ATLU_cgotogemm_cc
#define ZGEMM_CT ATLU_cgotogemm_ct
#define ZGEMM_CN ATLU_cgotogemm_cn
#define ZGEMM_TC ATLU_cgotogemm_tc
#define ZGEMM_TT ATLU_cgotogemm_tt
#define ZGEMM_TN ATLU_cgotogemm_tn
#define ZGEMM_NC ATLU_cgotogemm_nc
#define ZGEMM_NT ATLU_cgotogemm_nt
#define ZGEMM_NN ATLU_cgotogemm_nn

#define FLOAT	float

#define SIZE	4
#define LD	lds
#define ST	sts
#define SXADDQ	s4addq
#define MUL	muls
#define ADD	adds
#define SUB	subs

#define GEMM_PARAM_T  sgemm_param_t
#define ZGEMM_PARAM_T cgemm_param_t
#define GEMM_THREAD   sgemm_thread
#define ZGEMM_THREAD  cgemm_thread

#endif

#if 0
#ifdef linux
#define WH64(a)		.long (0x18<<26 | 0x1f << 21 | (a)<<16 | 0xf800)
#else
#define WH64(a)		wh64	($##a)
#endif
#else
#define WH64(a)		lds	$f31, 0($##a)
#endif

/* for x >= 0, split the address x into x_h and x_l
   such that
        x = x_h * 65536 + x_l
   where x_l in [-32768, 32767] that is [-(1<<15), (1<<15)-1]
   the formula acturally used is
        x + (1<<15) = ( x_h<<16 ) + ( x_l + (1<<15) )
*/
#define Address_H(x) (((x)+(1<<15))>>16)
#define Address_L(x) ((x)-((Address_H(x))<<16))

#ifndef ASSEMBLER

#define MIN(a,b)   (a>b? b:a)
#define MAX(a,b)   (a<b? b:a)

void xerbla_(char *, int *info, long);

int GEMM_TT(int, int, int, FLOAT, FLOAT *, int, FLOAT* ,
	      int, FLOAT *, int, FLOAT *);
int GEMM_TN(int, int, int, FLOAT, FLOAT *, int, FLOAT* ,
	      int, FLOAT *, int, FLOAT *);
int GEMM_NT(int, int, int, FLOAT, FLOAT *, int, FLOAT* ,
	      int, FLOAT *, int, FLOAT *);
int GEMM_NN(int, int, int, FLOAT, FLOAT *, int, FLOAT* ,
	      int, FLOAT *, int, FLOAT *);

int ZGEMM_CC(int m, int n, int k, FLOAT *alpha, FLOAT *a,
             int lda, FLOAT *b, int ldb, FLOAT *c, int ldc, FLOAT *buffer);
int ZGEMM_CT(int m, int n, int k, FLOAT *alpha, FLOAT *a,
             int lda, FLOAT *b, int ldb, FLOAT *c, int ldc, FLOAT *buffer);
int ZGEMM_CN(int m, int n, int k, FLOAT *alpha, FLOAT *a,
             int lda, FLOAT *b, int ldb, FLOAT *c, int ldc, FLOAT *buffer);
int ZGEMM_TC(int m, int n, int k, FLOAT *alpha, FLOAT *a,
             int lda, FLOAT *b, int ldb, FLOAT *c, int ldc, FLOAT *buffer);
int ZGEMM_TT(int m, int n, int k, FLOAT *alpha, FLOAT *a,
             int lda, FLOAT *b, int ldb, FLOAT *c, int ldc, FLOAT *buffer);
int ZGEMM_TN(int m, int n, int k, FLOAT *alpha, FLOAT *a,
             int lda, FLOAT *b, int ldb, FLOAT *c, int ldc, FLOAT *buffer);
int ZGEMM_NC(int m, int n, int k, FLOAT *alpha, FLOAT *a,
             int lda, FLOAT *b, int ldb, FLOAT *c, int ldc, FLOAT *buffer);
int ZGEMM_NT(int m, int n, int k, FLOAT *alpha, FLOAT *a,
             int lda, FLOAT *b, int ldb, FLOAT *c, int ldc, FLOAT *buffer);
int ZGEMM_NN(int m, int n, int k, FLOAT *alpha, FLOAT *a,
             int lda, FLOAT *b, int ldb, FLOAT *c, int ldc, FLOAT *buffer);

/* the info number used to differentiate abnormal exit from Goto's code */
#define INFO_NUM   99
#endif

#endif
