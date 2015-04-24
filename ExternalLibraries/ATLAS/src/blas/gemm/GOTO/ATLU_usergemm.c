#include "atlas_misc.h"
#include <sys/mman.h>

#ifdef EV6
   #ifdef ATL_OS_OSF1
      #define BUFFER_SIZE (16<<20)
      #define ALIGN_SIZE 0x7fffff     /* 8MB Align */
   #else
      #define BUFFER_SIZE (8<<20)
      #define ALIGN_SIZE 0x3fffff     /* 4MB Align */
   #endif
#else
   #define BUFFER_SIZE (1<<20)
   #define ALIGN_SIZE  0x3ffff     /* 256kB Align */
#endif
#ifdef TREAL
   int Mjoin(PATU,gotogemm_tt)
      (const int, const int, const int, const SCALAR, const TYPE *, const int,
       const TYPE *, const int, TYPE *, const int, TYPE *);
   int Mjoin(PATU,gotogemm_tn)
      (const int, const int, const int, const SCALAR, const TYPE *, const int,
       const TYPE *, const int, TYPE *, const int, TYPE *);
   int Mjoin(PATU,gotogemm_nt)
      (const int, const int, const int, const SCALAR, const TYPE *, const int,
       const TYPE *, const int, TYPE *, const int, TYPE *);
   int Mjoin(PATU,gotogemm_nn)
      (const int, const int, const int, const SCALAR, const TYPE *, const int,
       const TYPE *, const int, TYPE *, const int, TYPE *);
#else
   int Mjoin(PATU,gotogemm_cc)
      (const int, const int, const int, const SCALAR, const TYPE *, const int,
       const TYPE *, const int, TYPE *, const int, TYPE *);
   int Mjoin(PATU,gotogemm_ct)
      (const int, const int, const int, const SCALAR, const TYPE *, const int,
       const TYPE *, const int, TYPE *, const int, TYPE *);
   int Mjoin(PATU,gotogemm_cn)
      (const int, const int, const int, const SCALAR, const TYPE *, const int,
       const TYPE *, const int, TYPE *, const int, TYPE *);
   int Mjoin(PATU,gotogemm_tc)
      (const int, const int, const int, const SCALAR, const TYPE *, const int,
       const TYPE *, const int, TYPE *, const int, TYPE *);
   int Mjoin(PATU,gotogemm_tt)
      (const int, const int, const int, const SCALAR, const TYPE *, const int,
       const TYPE *, const int, TYPE *, const int, TYPE *);
   int Mjoin(PATU,gotogemm_tn)
      (const int, const int, const int, const SCALAR, const TYPE *, const int,
       const TYPE *, const int, TYPE *, const int, TYPE *);
   int Mjoin(PATU,gotogemm_nc)
      (const int, const int, const int, const SCALAR, const TYPE *, const int,
       const TYPE *, const int, TYPE *, const int, TYPE *);
   int Mjoin(PATU,gotogemm_nt)
      (const int, const int, const int, const SCALAR, const TYPE *, const int,
       const TYPE *, const int, TYPE *, const int, TYPE *);
   int Mjoin(PATU,gotogemm_nn)
      (const int, const int, const int, const SCALAR, const TYPE *, const int,
       const TYPE *, const int, TYPE *, const int, TYPE *);
#endif

int Mjoin(PATU,usergemm)(const enum ATLAS_TRANS TA, const enum ATLAS_TRANS TB,
                          const int M, const int N, const int K,
                          const SCALAR alpha, const TYPE *A, const int lda0,
                          const TYPE *B, const int ldb0, const SCALAR beta,
                          TYPE *C, const int ldc0)
{
   TYPE *buffer, *a_buf;
   #ifdef TREAL
      #define lda lda0
      #define ldb ldb0
      #define ldc ldc0
   #else
      const int lda = lda0<<1, ldb = ldb0<<1, ldc = ldc0<<1;
   #endif
/*
 * NOTE: this function pointer is critical.  If you call goto's assembly
 * matmul directly, you get seg fault for complex on the EV6, but it works
 * fine as long as you use a function pointer.  -- RCW
 */
   int (*gemm_xx)
      (const int, const int, const int, const SCALAR, const TYPE *, const int,
       const TYPE *, const int, TYPE *, const int, TYPE *);

   if ( !SCALAR_IS_ONE(beta) ) Mjoin(PATL,gescal)(M, N, beta, C, ldc0);
   #ifdef ATL_OS_OSF1
      buffer = (TYPE *) mmap(0, BUFFER_SIZE, PROT_READ | PROT_WRITE,
                             MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
   #else
      buffer = (TYPE *) malloc(BUFFER_SIZE);
   #endif
   if (!buffer) return(-1);

   a_buf = (TYPE *)(((size_t)buffer + ALIGN_SIZE) & ~ALIGN_SIZE);

   if (TA == AtlasNoTrans)
   {
      if (TB == AtlasNoTrans) gemm_xx = Mjoin(PATU,gotogemm_nn);
      #ifdef TCPLX
         else if (TB == AtlasConjTrans) gemm_xx = Mjoin(PATU,gotogemm_nc);
      #endif
      else gemm_xx = Mjoin(PATU,gotogemm_nt);

   }
   #ifdef TCPLX
      else if (TA == AtlasConjTrans)
      {
         if (TB == AtlasNoTrans) gemm_xx = Mjoin(PATU,gotogemm_cn);
         else if (TB == AtlasConjTrans) gemm_xx = Mjoin(PATU,gotogemm_cc);
         else gemm_xx = Mjoin(PATU,gotogemm_ct);
      }
   #endif
   else
   {
      if (TB == AtlasNoTrans) gemm_xx = Mjoin(PATU,gotogemm_tn);
      #ifdef TCPLX
         else if (TB == AtlasConjTrans) gemm_xx = Mjoin(PATU,gotogemm_tc);
      #endif
      else gemm_xx = Mjoin(PATU,gotogemm_tt);
   }
   gemm_xx(M, N, K, alpha, A, lda, B, ldb, C, ldc, a_buf);
   #ifdef ATL_OS_OSF1
      munmap((void*)buffer, BUFFER_SIZE);
   #else
      free(buffer);
   #endif
   return(0);
}
