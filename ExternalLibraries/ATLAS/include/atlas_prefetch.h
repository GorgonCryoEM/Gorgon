#ifndef ATLAS_PREFETCH_H
#define ATLAS_PREFETCH_H
/*
 * Altivec prefetch model not well utilized by SSE-like prefetch, so have
 * special commands for it.
 */
#if defined(ATL_AltiVec)
   #include "atlas_altivec.h"
#endif
/*
 *
 * ATL_pfl1R(mem)  : fetch location mem to L1, with intent to read *only*
 * ATL_pfl1W(mem)  : fetch location mem to L1, with intent to read/write
 * ATL_pfl1WO(mem) : fetch location mem to L1, with intent to write ONLY
 */

#if defined(ATL_3DNow1) || defined(ATL_3DNow2) || ( defined(ATL_SSE1) && defined(ATL_ARCH_ATHLON) )
   #ifdef __GNUC__
      #define ATL_pfl1R(mem) \
         __asm__ __volatile__ ("prefetch %0" : : "m" (*((char *)(mem))))
      #define ATL_pfl1W(mem) \
         __asm__ __volatile__ ("prefetchw %0" : : "m" (*((char *)(mem))))
      #define ATL_pfl1WO ATL_pfl1W
      #define ATL_GOT_L1PREFETCH
      #ifdef ATL_SSE1
         #define ATL_pfl2R(mem) \
            __asm__ __volatile__ ("prefetcht1 %0" : : "m" (*((char *)(mem))))
         #define ATL_pfl2W(mem) \
            __asm__ __volatile__ ("prefetcht1 %0" : : "m" (*((char *)(mem))))
         #define ATL_pfl2WO ATL_pfl2W
         #define ATL_GOT_L2PREFETCH
      #endif
   #endif
#elif defined(ATL_SSE1) || defined (ATL_SSE2) /* SSE prefetch is available */
   #ifdef __GNUC__
      #define ATL_pfl1R(mem) \
         __asm__ __volatile__ ("prefetchnta %0" : : "m" (*((char *)(mem))))
      #define ATL_pfl1W(mem) \
         __asm__ __volatile__ ("prefetchnta %0" : : "m" (*((char *)(mem))))
      #define ATL_pfl1WO ATL_pfl1W
      #define ATL_GOT_L1PREFETCH

      #define ATL_pfl2R(mem) \
         __asm__ __volatile__ ("prefetcht1 %0" : : "m" (*((char *)(mem))))
      #define ATL_pfl2W(mem) \
         __asm__ __volatile__ ("prefetcht1 %0" : : "m" (*((char *)(mem))))
      #define ATL_pfl2WO ATL_pfl2W
      #define ATL_GOT_L2PREFETCH
   #endif
#elif defined(__SUNPRO_C) && defined(__sparc)
   #include <sun_prefetch.h>
   #define ATL_pfl1R(mem) sparc_prefetch_read_many((void*)(mem))
   #define ATL_pfl1W(mem) sparc_prefetch_write_many((void*)(mem))
   #define ATL_GOT_L1PREFETCH
   #define ATL_pfl2R(mem) sparc_prefetch_read_many((void*)(mem))
   #define ATL_pfl2W(mem) sparc_prefetch_write_many((void*)(mem))
   #define ATL_GOT_L2PREFETCH
#elif defined(ATL_ARCH_21264)
   #ifdef __GNUC__
      #define ATL_pfl1R(mem) \
         __asm__ __volatile__ ("ldt $f31, %0" : : "m" (*((char *)(mem))))
      #define ATL_pfl1W(mem) \
         __asm__ __volatile__ ("lds $f31, %0" : : "m" (*((char *)(mem))))
      #define ATL_pfl1WO(mem) \
         __asm__ __volatile__ ("wh64 %0" : : "m" (*((char *)(mem))))
      #define ATL_GOT_L1PREFETCH
   #elif defined(__DECC)
      #include "c_asm.h"
      #define ATL_pfl1R(mem) asm ("ldt %f31,(%a0) ;", mem)
      #define ATL_pfl1W(mem) asm ("lds %f31,(%a0) ;", mem)
      #define ATL_pfl1WO(mem) asm ("wh64 (%a0) ;", mem)
      #define ATL_GOT_L1PREFETCH
   #endif
/*
 * Note: SunUS5/10 seems to get no benefit from prefetch, so don't enable
 */
#elif defined(ATL_ARCH_SunUS4) || defined(ATL_ARCH_SunUS2) || \
      defined(ATL_ARCH_SunUSIII) || defined(ATL_ARCH_SunUSX)
   #ifdef __GNUC__
      #define ATL_pfl1R(mem) \
         __asm__ __volatile__ ("prefetch %0,0" : : "m" (*((char *)(mem))))
      #define ATL_pfl1W(mem) \
         __asm__ __volatile__ ("prefetch %0,2" : : "m" (*((char *)(mem))))
      #define ATL_GOT_L1PREFETCH
   #endif
#elif defined(ATL_ARCH_IA64Itan) || defined(ATL_ARCH_IA64Itan2)
/*
 * Have to use nt2, 'cause fpu ignored L1.
 * NOTE: just let icc to prefetch, keep inst here for reference
 */
   #if defined(__ECC) && 0
      #include "ia64intrin.h"
      #define ATL_pfl1R(mem) __lfetch(2, (mem))
      #define ATL_pfl1W(mem)  __lfetch_excl(2, (mem))
      #define ATL_GOT_L1PREFETCH
   #elif defined(__GNUC__) && !defined(__ECC)
      #define ATL_pfl1R(mem) \
         __asm__ ("    lfetch.nt2  [%0]": : "r"((void *)(mem)))
      #define ATL_pfl1W(mem) \
         __asm__ ("    lfetch.excl     [%0]": : "r"((void *)(mem)))
      #define ATL_GOT_L1PREFETCH
   #endif
#elif defined(ATL_AltiVec)
   #ifndef ATL_NoFakePF
      /* 33619968 is ATL_GetCtrl(0, 1, 2), or fetch 1 32-byte block */
      #define ATL_pfl1R(mem) ATL_pfavR(mem, 33619968, 3)
      #define ATL_pfl1W(mem) ATL_pfavW(mem, 33619968, 2)
      #define ATL_GOT_L1PREFETCH
   #endif
#endif
#if defined(ATL_pfl1W) && !defined(ATL_pfl1WO)
   #define ATL_pfl1WO ATL_pfl1W
#endif

#ifdef ATL_NOL1PREFETCH
   #ifdef ATL_GOT_L1PREFETCH
      #undef ATL_pfl1R
      #undef ATL_pfl1W
      #undef ATL_pfl1WO
      #undef ATL_GOT_L1PREFETCH
   #endif
#endif
#ifdef ATL_NOL2PREFETCH
   #ifdef ATL_GOT_L2PREFETCH
      #undef ATL_pfl2R
      #undef ATL_pfl2W
      #undef ATL_pfl2WO
      #undef ATL_GOT_L2PREFETCH
   #endif
#endif
#ifndef ATL_GOT_L1PREFETCH  /* dummy calls cpp takes out of code */
   #define ATL_pfl1R(mem)
   #define ATL_pfl1W(mem)
   #define ATL_pfl1WO(mem)
#endif
#ifndef ATL_GOT_L2PREFETCH  /* dummy calls cpp takes out of code */
   #define ATL_pfl2R(mem)
   #define ATL_pfl2W(mem)
#endif

#endif
