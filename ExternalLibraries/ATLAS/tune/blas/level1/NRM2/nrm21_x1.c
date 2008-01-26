#include "atlas_misc.h"
#include <math.h>
TYPE ATL_UNRM2(const int N, const TYPE *X, const int incX)
/*
 * Only machines like x86 with extended precision (both arithmetic and sqrt)
 * will be able to use this kernel.  On machines with standard 64 bit
 * precision, this will fail the overflow/underflow tests.
 */
{
   int i;
   TYPE t0=ATL_rzero;
   for (i=0; i != N; i++) t0 += X[i]*X[i];
   t0 = sqrt(t0);
   return(t0);
}
