    /*        Fast GEMM routine for Alpha 21164/21264      */
    /*         on  Linux, Digital UNIX                     */
    /*        by Kazushige Goto <goto@statabo.rim.or.jp>   */

#define ASSEMBLER
#include "common.h"
#include "version.h"

#ifdef EV6
#include "gemm_EV6_k.S"
#else
#include "gemm_EV5_k.S"
#endif
