#include "atlas_misc.h"
#include <assert.h>
#include "atlas_tst.h"

#ifndef TEST_ROT
void ATL_ROT(const int, TYPE *, const int, TYPE *, const int,
             const TYPE, const TYPE);
   #define TEST_ROT(N_, alpha_, X_, ix_, beta_, Y_, iy_) \
      ATL_ROT(N_, X_, ix_, Y_, iy_, alpha_, beta_)
#endif

#ifdef TREAL

static void good_rot0(const int N, TYPE *X, const int incX,
                      TYPE *Y, const int incY, const TYPE c, const TYPE s)

{
   int i;
   TYPE tmp;

   for (i=N; i; i--, Y += incY, X += incX)
   {
      tmp = c * *X + s * *Y;
      *Y = c * *Y - s * *X;
      *X = tmp;
   }
}
void good_rot(const int N, const TYPE alpha, TYPE *X, const int incX,
              const TYPE beta, TYPE *Y, const int incY)
{
   good_rot0(N, X, incX, Y, incY, alpha, beta);
}

int CheckPad(int npad, TYPE padval, int N, TYPE *Y, int incY)
{
   int i, iret=0;

   incY = Mabs(incY);
   for (i=0; i < npad; i++)
   {
      if (Y[i] != padval)
      {
         iret = i;
         fprintf(stderr, "OVERWRITE %f IN PREPAD %d before beginning of Y!!\n",
                 Y[i], npad-i);
      }
   }
   Y += npad;
   if (incY != 1)
   {
      for (i=0; i < N*incY; i++)
      {
         if (i%incY)
         {
            if (Y[i] != padval)
            {
               iret = i;
               fprintf(stderr, "INTERNAL OVERWRITE %f AT POSITION %d!!\n",
                       Y[i], i);
            }
         }
      }
   }
   Y += 1 + (N-1)*incY;
   for (i=0; i < npad; i++)
   {
      if (Y[i] != padval)
      {
         iret = i;
         fprintf(stderr, "OVERWRITE %f IN POSTPAD %d past end of Y!!\n",
                 Y[i], i+1);
      }
   }
   return(iret);
}
#else
static void good_rot0
   (const int N, TYPE *X, const int incx, TYPE *Y, const int incy,
    const TYPE c0, const TYPE s0)
{
   const register TYPE c = c0, s = s0;
   register TYPE rx, ix, ry, iy;
   const int incX = incx<<1, incY = incy<<1;
   int i;

   if (N > 0)
   {
      for (i=N; i; i--, X += incX, Y += incY)  /* maybe compiler unrolls */
      {
         rx = *X;  ix = X[1];
         ry = *Y;  iy = Y[1];
         *X   = c * rx + s * ry;
         X[1] = c * ix + s * iy;
         *Y   = c * ry - s * rx;
         Y[1] = c * iy - s * ix;
      }
   }
}

void good_rot(const int N, const TYPE alpha, TYPE *X, const int incX,
              const TYPE beta, TYPE *Y, const int incY)
{
   good_rot0(N, X, incX, Y, incY, alpha, beta);
}
int CheckPad(int npad, TYPE padval, int N, TYPE *Y, int incY)
{
   int i, n, iret=0;

   incY = Mabs(incY);
   npad *= 2;
   for (i=0; i < npad; i++)
   {
      if (Y[i] != padval)
      {
         iret = i;
         fprintf(stderr, "OVERWRITE %f IN PREPAD %d before beginning of Y!!\n",
                 Y[i], npad-i);
      }
   }
   Y += npad;
   if (incY != 1)
   {
      for (i=0; i < N*incY; i++)
      {
         if (i%incY)
         {
            if (Y[2*i] != padval)
            {
               iret = i;
               fprintf(stderr, "INTERNAL REAL OVERWRITE %f AT POSITION %d!!\n",
                       Y[2*i], i);
            }
            if (Y[2*i+1] != padval)
            {
               iret = i+1;
               fprintf(stderr, "INTERNAL IMAG OVERWRITE %f AT POSITION %d!!\n",
                       Y[2*i+1], i);
            }
         }
      }
   }
   Y += 2 + 2*(N-1)*incY;
   for (i=0; i < npad; i++)
   {
      if (Y[i] != padval)
      {
         iret = i;
         fprintf(stderr, "OVERWRITE %f IN POSTPAD %d past end of Y!!\n",
                 Y[i], i+1);
      }
   }
   return(iret);
}
#endif

static TYPE my_infnrm(const int N, const TYPE *X, const int incX)
{
   int i, incx = incX SHIFT;
   TYPE max=ATL_rzero, tmp;
   for (i=N; i; i--, X += incx)
   {
      #ifdef TREAL
         tmp = Mabs(*X);
      #else
         tmp = Mabs(*X) + Mabs(X[1]);
      #endif
      if (tmp > max) max = tmp;
   }
   return(max);
}

int checkXY(int N, TYPE *Xg, int incXg, TYPE *Xc, int incXc,
            TYPE *Yg, int incYg, TYPE *Yc, int incYc,
            TYPE normX, TYPE normY)
{
   TYPE *diff;
   TYPE normDX, normDY, resid;
   TYPE eps, THRESH=50.0;
   TYPE Mjoin(PATL, epsilon)(void);
   int iret=0;

   eps = Mjoin(PATL,epsilon)();
   diff = malloc(sizeof(TYPE)*(N SHIFT));
   assert(diff);

   Mjoin(PATL,vdiff)(N, Xc, incXc, Xg, incXg, diff, 1);
   normDX = my_infnrm(N, diff, 1);
   Mjoin(PATL,vdiff)(N, Yc, incYc, Yg, incYg, diff, 1);
   normDY = my_infnrm(N, diff, 1);

   resid = Mmax(normDX, normDY) /
           (eps * Mmax(normX, ATL_rone) * Mmax(normY, ATL_rone) * N);
   if (resid > THRESH || resid != resid)
   {
      fprintf(stderr,
              "ROT resid=%e (normX=%e, normY=%e, normDX=%e, normDY=%e)!!!\n",
              resid, normX, normY, normDX, normDY);
      iret = -1;
   }
   free(diff);
   return(iret);
}
static int CheckXY(int npad, TYPE padvalX, TYPE padvalY,
                   int N, TYPE *Xg, TYPE *Yg, TYPE normX, TYPE normY,
                   TYPE *Xt, int incXt, TYPE *Yt, int incYt)
/*
 * Xg & Yg are known to be inc=1, with no padding
 */
{
   TYPE *xt = Xt + (npad SHIFT), *yt = Yt + (npad SHIFT);
   int i0, i1, i2;
   if (incXt < 0) xt -= ((N-1)SHIFT)*incXt;
   if (incYt < 0) yt -= ((N-1)SHIFT)*incYt;
   i0 = checkXY(N, Xg, 1, xt, incXt, Yg, 1, yt, incYt, normX, normY);
   i1 = CheckPad(npad, padvalX, N, Xt, incXt);
   i2 = CheckPad(npad, padvalY, N, Yt, incYt);
   if (i0 || i1 || i2) return(1);
   return(0);
}

static void vecset(int N, TYPE alpha, TYPE *X)
{
   int i;
   #ifdef TCPLX
      N *= 2;
   #endif
   for (i=0; i < N; i++) X[i] = alpha;
}

static TYPE *getvec(int npad, TYPE padval, int N, int incX)
{
   TYPE *X, *x;
   int i, n;

   if (N <= 0) return(NULL);
   incX = Mabs(incX);
   n = 2*npad + 1+(N-1)*incX;
   X = malloc( ATL_sizeof*n );
   assert(X);
   vecset(n, padval, X);
   #ifdef TCPLX
      npad *= 2;
      incX *= 2;
   #endif
   x = X + npad;
   for (i=0; i < N; i++, x += incX)
   {
      #ifdef TREAL
         *x = dumb_rand();
      #else
         *x   = dumb_rand();
         x[1] = dumb_rand();
      #endif
   }
   return(X);
}

static void copyvec(int N, const TYPE *X, int incX, TYPE *Y, int incY)
{
   int i;
   #ifdef TREAL
      for (i=0; i < N; i++, X += incX, Y += incY) *Y = *X;
   #else
      incX *= 2; incY *= 2;
      for (i=0; i < N; i++, X += incX, Y += incY) { *Y = *X; Y[1] = X[1]; }
   #endif
}

static TYPE *dupvec(int npad, int N, TYPE *X, int incX)
{
   int i, n;
   TYPE *y;

   incX = Mabs(incX);
   n = 1+(N-1)*incX + 2*npad;
   y = malloc(ATL_sizeof*n);
   assert(y);
   #ifdef TCPLX
      n *= 2;
   #endif
   for (i=0; i < n; i++) y[i] = X[i];
   return(y);
}

static TYPE *gen_dupvec(int N, TYPE padval, int npadX, TYPE *X, int incX,
                        int npadY, int incY)
{
   int i, n;
   TYPE *y, *yy, *xx=X+(npadX SHIFT);

   y = getvec(npadY, padval, N, incY);
   yy = y + (npadY SHIFT);
   if (incY < 1) yy -= ((N-1)SHIFT) * incY;
   if (incX < 1) xx -= ((N-1)SHIFT) * incX;
   copyvec(N, xx, incX, yy, incY);
   return(y);
}

int DoTest(int N, TYPE *alpha0, TYPE *beta0, int incX, int incY)
{
   int iret;
   const int npad=Mmax(4*Mmax(Mabs(incY), Mabs(incX)), 16);
   const TYPE padvalX=(-2271.0), padvalY=(-3389.0);
   TYPE *Xg, *Yg, *Xt, *Yt, *x, *y;
   TYPE normX, normY;
   TYPE alpha=(*alpha0), beta=(*beta0);

   Xg = getvec(0, padvalX, N, 1);
   Yg = getvec(0, padvalY, N, 1);
   Xt = gen_dupvec(N, padvalX, 0, Xg, 1, npad, incX);
   Yt = gen_dupvec(N, padvalY, 0, Yg, 1, npad, incY);

   normX = my_infnrm(N, Xg, 1);
   normY = my_infnrm(N, Yg, 1);
   good_rot(N, alpha, Xg, 1, beta, Yg, 1);

   x = Xt + (npad SHIFT);
   y = Yt + (npad SHIFT);
   if (incX < 1) x -= ((N-1)SHIFT) * incX;
   if (incY < 1) y -= ((N-1)SHIFT) * incY;
   TEST_ROT(N, alpha, x, incX, beta, y, incY);

   iret = CheckXY(npad, padvalX, padvalY, N, Xg, Yg, normX, normY,
                  Xt, incX, Yt, incY);
   free(Xg);
   free(Yg);
   free(Xt);
   free(Yt);
   return(iret);
}
int DoAllTests(int nN, int *Ns, int nX, int *Xs, int nY, int *Ys,
               int nalpha, TYPE *alp, int nbeta, TYPE *bet)
{
   int in, ix, iy, ia, ib, iret=0, i=0, j, k;
   char *passfail;

#ifdef TREAL
   char *t1="  ITST         N     alpha      beta  incX  incY    TEST";
   char *t2="======  ========  ========  ========  ====  ====  ======";
#else
   char *t1 =
   "  ITST         N    ralpha   ialpha     rbeta    ibeta  incX  incY    TEST";
   char *t2 =
   "======  ========  ======== ========  ======== ========  ====  ====  ======";
#endif
   fprintf(stdout, "%s\n", t1);
   fprintf(stdout, "%s\n", t2);
   for (in=0; in < nN; in++)
   {
      for (ix=0; ix < nX; ix++)
      {
         for (iy=0; iy < nY; iy++)
         {
            for (ia=0; ia < nalpha; ia++)
            {
               for (ib=0; ib < nbeta; ib++)
               {
                  j = DoTest(Ns[in], alp+(ia SHIFT), bet+(ib SHIFT),
                             Xs[ix], Ys[iy]);
                  iret += j;
                  if (j == 0) passfail = "PASSED";
                  else passfail = "FAILED";
                  #ifdef TREAL
                     fprintf(stdout, "%6d %9d %9.2f %9.2f %5d %5d  %s\n", i,
                            Ns[in], alp[ia], bet[ib], Xs[ix], Ys[iy], passfail);
                  #else
                     fprintf(stdout,
                             "%6d %9d  %8.2f %8.2f  %8.2f %8.2f %5d %5d  %s\n",
                             i, Ns[in], alp[2*ia], alp[2*ia+1],
                             bet[2*ib], bet[2*ib+1], Xs[ix], Ys[iy], passfail);
                  #endif
                  i++;
               }
            }
         }
      }
   }
   if (iret == 0) fprintf(stdout, "ALL ROT SANITY TESTS PASSED.\n\n");
   else fprintf(stdout, "%d of %d ROT TESTS FAILED!!\n\n", iret, i);
   return(iret);
}

void PrintUsage(char *nam)
{
   fprintf(stderr,
      "USAGE: %s -N # n1 ... n# -n <n> -X # x1 ... x# -Y # y1 ... y# -a # alpha1 ... alpha# -b # beta1 ... beta#\n", nam);
   exit(-1);
}

void GetFlags(int nargs, char **args, int *nN, int **Ns, int *nX, int **incXs,
              int *nY, int **incYs, int *nal, TYPE **alphas,
              int *nbe, TYPE **betas)
{
   int i, j;

   *nal = *nbe = -1;
   *nY = *nX = *nN = -1;

   for (i=1; i < nargs; i++)
   {
      if (args[i][0] != '-') PrintUsage(args[0]);
      if (i == nargs-1) PrintUsage(args[0]);
      switch(args[i][1])
      {
      case 'a':
         *nal = atoi(args[++i]);
         if (*nal > nargs-i) PrintUsage(args[0]);
         *alphas = malloc(ATL_MulBySize(*nal));
         assert(*alphas);
         for (j=0; j < *nal SHIFT; j++) (*alphas)[j] = atof(args[++i]);
         break;
      case 'b':
         *nbe = atoi(args[++i]);
         if (*nbe > nargs-i) PrintUsage(args[0]);
         *betas = malloc(ATL_MulBySize(*nbe));
         assert(*betas);
         for (j=0; j < *nbe SHIFT; j++) (*betas)[j] = atof(args[++i]);
         break;
      case 'Y':
         *nY = atoi(args[++i]);
         if (*nY > nargs-i) PrintUsage(args[0]);
         *incYs = malloc((*nY)*sizeof(int));
         assert(*incYs);
         for (j=0; j < *nY; j++) (*incYs)[j] = atoi(args[++i]);
         break;
      case 'X':
         *nX = atoi(args[++i]);
         if (*nX > nargs-i) PrintUsage(args[0]);
         *incXs = malloc((*nX)*sizeof(int));
         assert(*incXs);
         for (j=0; j < *nX; j++) (*incXs)[j] = atoi(args[++i]);
         break;
      case 'N':
         *nN = atoi(args[++i]);
         if (*nN > nargs-i) PrintUsage(args[0]);
         *Ns = malloc((*nN)*sizeof(int));
         assert(*Ns);
         for (j=0; j < *nN; j++) (*Ns)[j] = atoi(args[++i]);
         break;
      case 'y':
         *nY = 1;
         *incYs = malloc(sizeof(int));
         assert(*incYs);
         **incYs = atoi(args[++i]);
         break;
      case 'x':
         *nX = 1;
         *incXs = malloc(sizeof(int));
         assert(*incXs);
         **incXs = atoi(args[++i]);
         break;
      case 'n':
         *nN = 1;
         *Ns = malloc(sizeof(int));
         assert(*Ns);
         **Ns = atoi(args[++i]);
         break;
      default:
         PrintUsage(args[0]);
      }
   }
   if (*nN < 0)
   {
      *nN = 4;
      *Ns = malloc((*nN)*sizeof(int));
      assert(*Ns);
      **Ns = 777;
      (*Ns)[1] = 1;
      (*Ns)[2] = 3;
      (*Ns)[3] = 7;
   }
   if (*nX < 0)
   {
      *nX = 1;
      *incXs = malloc((*nX)*sizeof(int));
      assert(*incXs);
      **incXs = 1;
   }
   if (*nY < 0)
   {
      *nY = 1;
      *incYs = malloc((*nY)*sizeof(int));
      assert(*incYs);
      **incYs = 1;
   }
   if (*nal < 1)
   {
      #ifdef TREAL
         *nal = 3;
         *alphas = malloc(ATL_MulBySize(3));
         assert(*alphas);
         (*alphas)[0] = 1.0;
         (*alphas)[1] = -1.0;
         (*alphas)[2] = 0.9;
      #else
         *nal = 4;
         *alphas = malloc(ATL_MulBySize(4));
         assert(*alphas);
         (*alphas)[0] = 1.0;
         (*alphas)[1] = 0.0;
         (*alphas)[2] = -1.0;
         (*alphas)[3] = 0.0;
         (*alphas)[4] = 1.3;
         (*alphas)[5] = 0.0;
         (*alphas)[6] = 0.9;
         (*alphas)[7] = 1.1;
      #endif
   }
   if (*nbe < 1)
   {
      #ifdef TREAL
         *nbe = 3;
         *betas = malloc(ATL_MulBySize(3));
         assert(*betas);
         (*betas)[0] = 1.0;
         (*betas)[1] = -1.0;
         (*betas)[2] = 0.8;
      #else
         *nbe = 4;
         *betas = malloc(ATL_MulBySize(4));
         assert(*betas);
         (*betas)[0] = 1.0;
         (*betas)[1] = 0.0;
         (*betas)[2] = -1.0;
         (*betas)[3] = 0.0;
         (*betas)[4] = 1.2;
         (*betas)[5] = 0.0;
         (*betas)[6] = 1.1;
         (*betas)[7] = 0.8;
      #endif
   }
}

main(int nargs, char **args)
{
   int nN, nX, nY, nal, nbe;
   int *Ns, *incXs, *incYs;
   TYPE *alphas=NULL, *betas=NULL;
   int ierr;
   GetFlags(nargs, args, &nN, &Ns, &nX, &incXs, &nY, &incYs,
            &nal, &alphas, &nbe, &betas);
   ierr = DoAllTests(nN, Ns, nX, incXs, nY, incYs, nal, alphas, nbe, betas);
   if (alphas) free(alphas);
   if (betas) free(betas);
   free(incXs);
   free(incYs);
   exit(ierr);
}
