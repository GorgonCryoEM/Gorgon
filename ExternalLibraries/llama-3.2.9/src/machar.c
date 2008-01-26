/*
 * $Log$
 * Revision 1.1  2000/05/03 18:06:43  aking
 * Initial revision
 *
 * Revision 1.2  1999/07/08 18:10:30  aking
 * More precision in double-precision constants.
 * This eliminates possible misinterpretation of MACH_XMAX as Inf.
 *
 * Revision 1.1  1998/04/28 16:18:43  aking
 * Initial revision
 *
*/

#include <math.h>
#include <stdio.h>

#define REAL double

static void machar(int *, int *, int *, int *, int *, int *, int *,
		   int *, int *, REAL *, REAL *, REAL *, REAL *);

int main (void)
{
  int radix, it, irnd, ngrd, machep, negep, iexp, minexp, maxexp;
  REAL eps, epsneg, xmin, xmax;

  machar(&radix, &it, &irnd, &ngrd, &machep, &negep, &iexp, &minexp, &maxexp,
	 &eps, &epsneg, &xmin, &xmax);

  printf("#define MACH_RADIX \t% d\n", radix);
  printf("#define MACH_DIGITS \t% d\n", it);
  printf("#define MACH_ROUNDING \t% d\n", irnd);
  printf("#define MACH_NGUARD \t% d\n", ngrd);
  printf("#define MACH_MACHEP \t% d\n", machep);
  printf("#define MACH_NEGEP \t% d\n", negep);
  printf("#define MACH_IEXP \t% d\n", iexp);
  printf("#define MACH_MINEXP \t% d\n", minexp);
  printf("#define MACH_MAXEXP \t% d\n", maxexp);
  printf("#define MACH_EPS \t% .16e\n", eps);
  printf("#define MACH_EPSNEG \t% .16e\n", epsneg);
  printf("#define MACH_XMIN \t% .16e\n", xmin);
  printf("#define MACH_XMAX \t% .16e\n", xmax);
  printf("#define MACH_SEPS \t% .16e\n", sqrt(eps));
  printf("#define MACH_CEPS \t% .16e\n", pow(eps, 1.0/3.0));
  return 0;
}

#define CONV(i) ((REAL)(i))
#define ZERO 0.0e0
 
static void machar (int *ibeta, int *it, int *irnd, int *ngrd, int *machep, 
		    int *negep, int *iexp, int *minexp, int *maxexp, REAL *eps, 
		    REAL *epsneg, REAL *xmin, REAL *xmax)
{
  int i, itemp, iz, j, k, mx, nxres;
  REAL a, b, beta, betah, betain, one, t, temp, temp1, tempa, two, y, z, zero;

  one = CONV(1);
  two = one+one;
  zero = one-one;
  a = one;
  do {
    a +=  a;
    temp = a+one;
    temp1 = temp-a;
  } while (temp1-one == zero);
  b = one;
  do {
    b += b;
    temp = a+b;
    itemp = (int)(temp-a);
  } while (itemp == 0);
  *ibeta = itemp;
  beta = CONV(*ibeta);
  *it = 0;
  b = one;
  do {
    ++(*it);
    b *= beta;
    temp = b+one;
    temp1 = temp-b;
  } while (temp1-one == zero);
  *irnd = 0;
  betah = beta/two;
  temp = a+betah;
  if (temp-a != zero) *irnd = 1;
  tempa = a+beta;
  temp = tempa+betah;
  if (*irnd == 0 && temp-tempa != zero) *irnd = 2;
  *negep = (*it)+3;
  betain = one/beta;
  a = one;
  for (i = 1; i<=(*negep); i++) a *= betain;
  b = a;
  for (;;) {
    temp = one-a;
    if (temp-one != zero) break;
    a *= beta;
    --(*negep);
  }
  *negep = -(*negep);
  *epsneg = a;
  *machep = -(*it)-3;
  a = b;
  for (;;) {
    temp = one+a;
    if (temp-one != zero) break;
    a *=  beta;
    ++(*machep);
  }
  *eps = a;
  *ngrd = 0;
  temp = one+(*eps);
  if (*irnd == 0 && temp*one-one != zero) *ngrd = 1;
  i = 0;
  k = 1;
  z = betain;
  t = one+(*eps);
  nxres = 0;
  for (;;) {
    y = z;
    z = y*y;
    a = z*one;
    temp = z*t;
    if (a+a == zero || fabs(z) >= y) break;
    temp1 = temp*betain;
    if (temp1*beta == z) break;
    ++i;
    k += k;
  }
  if (*ibeta != 10) {
    *iexp = i+1;
    mx = k+k;
  } else {
    *iexp = 2;
    iz = (*ibeta);
    while (k >= iz) {
      iz *= *ibeta;
      ++(*iexp);
    }
    mx = iz+iz-1;
  }
  for (;;) {
    *xmin = y;
    y *= betain;
    a = y*one;
    temp = y*t;
    if (a+a != zero && fabs(y) < *xmin) {
      ++k;
      temp1 = temp*betain;
      if (temp1*beta == y && temp != y) {
	nxres = 3;
	*xmin = y;
	break;
      }
    }
    else break;
  }
  *minexp = -k;
  if (mx <= k+k-3 && *ibeta != 10) {
    mx += mx;
    ++(*iexp);
  }
  *maxexp = mx+(*minexp);
  *irnd += nxres;
  if (*irnd >= 2) *maxexp -= 2;
  i = (*maxexp)+(*minexp);
  if (*ibeta == 2 && !i) --(*maxexp);
  if (i > 20) --(*maxexp);
  if (a != y) *maxexp -= 2;
  *xmax = one-(*epsneg);
  if ((*xmax)*one != *xmax) *xmax = one-beta*(*epsneg);
  *xmax /= (*xmin*beta*beta*beta);
  i = (*maxexp)+(*minexp)+3;
  for (j = 1; j<=i; j++) {
    if (*ibeta == 2) *xmax += *xmax;
    else *xmax *= beta;
  }
}
#undef CONV

