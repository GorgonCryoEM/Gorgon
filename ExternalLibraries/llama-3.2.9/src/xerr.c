#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

static void llerr (int fatal, char *format, ...)
{
  va_list arg;
  va_start(arg, format);
  vfprintf(stderr, format, arg);
  va_end(arg);
  if (fatal) exit(fatal);
}

void xerr_ (int *errno, int *i1, int *i2, 
	     double *r1, double *r2, double *r3)
{
#ifndef NOMSG
  switch (*errno) {
  case 101:
    llerr(0,"ODESSA Warning: internal t = %lg and h = %lg are such\nthat, \
in the machine, t + h = t on the next step.\n Solver will continue anyway.\n", *r1, *r2);
    break;
  case 102:
    llerr(0,"ODESSA Warning: above warning has been issued %d times\nand \
will not be issued again for this problem.\n", *i1);
    break;
  case 201:
    llerr(0,"ODESSA Warning: at current t = %lg, mxstep = %d steps\nhave \
been taken on this call before reaching tout\n", *r1, *i1);
    break;
  case 202:
    llerr(0,"ODESSA Warning: at t = %lg, ewt(%d) = %lg < 0.0\n", *r1, *i1, *r2);
    break;
  case 203:
    llerr(0,"ODESSA Warning: at t = %lg, too much accuracy\nrequested for \
precision of machine.  Tolerance scale factor = %lg\n", *r1, *r2);
    break;
  case 204:
    llerr(0,"ODESSA Warning: at t = %lg and step size h = %lg\n the error \
test failed repeatedly or with abs(h) = hmin\n", *r1, *r2);
    break;
  case 205:
    llerr(0,"ODESSA Warning: at t = %lg and step size h = %lg\n the \
corrector convergence failed repeatedly or with abs(h) = hmin\n", *r1, *r2);
    break;
  case 301:
    llerr(0,"ODESSA Warning: repeated calls with istate = 1 and tout = %lg\n", *r1);
    break;
  case 302:
    llerr(0,"ODESSA Warning: repeated occurrences of illegal input\n");
    break;
  case 303:
    llerr(1,"ODESSA: run aborted---apparent infinite loop\n");
    break;
  case 304:
    llerr(1,"ODESSA: run aborted\n");
    break;
  case 401:
    llerr(0,"LOCATE Warning: extrapolation used:\nx = %lg, xmin = %lg, xmax = %lg\n", *r1, *r2, *r3);
    break;
  case 402:
    llerr(0,"HUNT Warning: extrapolation used:\nx = %lg, xmin = %lg, xmax = %lg\n", *r1, *r2, *r3);
    break;
  case 1:
    llerr(0,"ODESSA Warning: istate = %d illegal\n", *i1);
    break;
  case 2:
    llerr(0,"ODESSA Warning: itask = %d illegal\n", *i1);
    break;
  case 3:
    llerr(0,"ODESSA Warning: istate > 1 but ODESSA not initialized\n");
    break;
  case 4:
    llerr(0,"ODESSA Warning: neq = %d < 1\n", *i1);
    break;
  case 5:
    llerr(0,"ODESSA Warning: istate = 3 and neq increased from %d to %d\n", *i1, *i2);
    break;
  case 6:
    llerr(0,"ODESSA Warning: itol = %d illegal\n", *i1);
    break;
  case 7:
    llerr(0,"ODESSA Warning: iopt = %d illegal\n", *i1);
    break;
  case 8:
    llerr(0,"ODESSA Warning: mf = %d illegal\n", *i1);
    break;
  case 9:
    llerr(0,"ODESSA Warning: ml = %d illegal ( < 0 or > neq = %d )\n", *i1, *i2);
    break;
  case 10:
    llerr(0,"ODESSA Warning: mu = %d illegal ( < 0 or > neq = %d )\n", *i1, *i2);
    break;
  case 11:
    llerr(0,"ODESSA Warning: maxord = %d < 0\n", *i1);
    break;
  case 12:
    llerr(0,"ODESSA Warning: mxstep = %d < 0\n", *i1);
    break;
  case 13:
    llerr(0,"ODESSA Warning: mxhnil = %d < 0\n", *i1);
    break;
  case 14:
    llerr(0,"ODESSA Warning: tout = %lg behind t = %lg, where integration direction\n\
is given by h0 = %lg\n", *r1, *r2, *r3);
    break;
  case 15:
    llerr(0,"ODESSA Warning: hmax = %lg < 0.0\n", *r1);
    break;
  case 16:
    llerr(0,"ODESSA Warning: hmin = %lg < 0.0\n", *r1);
    break;
  case 17:
    llerr(0,"ODESSA Warning: rwork length needed = %d exceeds lrw = %d\n", *i1, *i2);
    break;
  case 18:
    llerr(0,"ODESSA Warning: iwork length needed = %d exceeds liw = %d\n", *i1, *i2);
    break;
  case 19:
    llerr(0,"ODESSA Warning: rtol(%d) = %lg < 0.0\n", *i1, *r1);
    break;
  case 20:
    llerr(0,"ODESSA Warning: atol(%d) = %lg < 0.0\n", *i1, *r1);
    break;
  case 21:
    llerr(0,"ODESSA Warning: ewt(%d) = %lg <= 0.0\n", *i1, *r1);
    break;
  case 22:
    llerr(0,"ODESSA Warning: tout = %lg too close to t = %lg to start integration\n", *r1, *r2);
    break;
  case 23:
    llerr(0,"ODESSA Warning: itask = %d and tout = %lg is behind tcur - hu = %lg\n", *i1, *r1, *r2);
    break;
  case 24:
    llerr(0,"ODESSA Warning: itask = 4 or 5 and tcrit = %lg is behind tcur = %lg\n", *r1, *r2);
    break;
  case 25:
    llerr(0,"ODESSA Warning: itask = 4 or 5 and tcrit = %lg is behind tout = %lg\n", *r1);
    break;
  case 26:
    llerr(0,"ODESSA Warning: at start of problem, too much accuracy requested\n\
for precision of machine.  Tolerance scale factor = %lg\n", *r1);
    break;
  case 27:
    llerr(0,"ODESSA Warning: trouble from INTDY.  itask = %d, tout = %lg\n", *i1, *r1);
    break;
  case 28:
    llerr(0,"ODESSA Warning: npar = %d < 1\n", *i1);
    break;
  case 29:
    llerr(1,"ODESSA Error: istate = 3 and npar changed from %d to %d\n", *i1, *i2);
    break;
  case 30:
    llerr(0,"ODESSA Warning: miter = %d illegal\n", *i1);
    break;
  case 31:
    llerr(0,"ODESSA Warning: trouble in sprime (ierpj)\n");
    break;
  case 32:
    llerr(0,"ODESSA Warning: trouble in sprime (miter)\n");
    break;
  case 33:
    llerr(1,"ODESSA: Fatal error in stode (kflag = -3)\n");
    break;
  case 51:
    llerr(0,"INTDY Warning: k = %d illegal\n", *i1);
    break;
  case 52:
    llerr(0,"INTDY Warning: t = %lg illegal: not in interval\n tcur-hu = %lg to tcur = %lg\n", *r1, *r2, *r3);
    break;
  default:
    llerr(0,"YERR: Unrecognized error %d\n", *errno);
    break;
  }
#endif
  return;
}
