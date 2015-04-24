#include <math.h>
#include "machinu.h"

double d1mach_ (int *i)
     /* Double precision machine constants */
{
  double dmach[5];

  dmach[0] = MACH_XMIN;                    /* tiny: smallest possible magnitude*/
  dmach[1] = MACH_XMAX;                    /* big: largest magnitude  */
  dmach[2] = MACH_EPSNEG;                  /* epsneg: smalest relative spacing */
  dmach[3] = MACH_EPS;                     /* eps: largest relative spacing*/
  dmach[4] = log10((double) MACH_RADIX);   /* log10 of the machine radix */

  return dmach[*i-1];
}

