c-----------------------------------------------------------------------
      SUBROUTINE prepdf (neq, y, srur, savf, ftem, dfdp, par,
     1   f, df, jpar)
      IMPLICIT DOUBLE PRECISION (a-h,o-z)
      EXTERNAL f, df
      DIMENSION neq(*), y(*), savf(*), ftem(*), dfdp(*), par(*)
      COMMON /ode001/ rownd, rowns(173),
     1   rdum1(43), tn, rdum2,
     2   iownd1(14), iowns(4),
     3   idum1(10), miter, idum2(4), n, idum3(2), nfe, idum4(2)
      COMMON /ode002/ rdum3(3),
     1   iownd2(3), idum5(2), ndfe, idum6, idf, idum7(3)
c-----------------------------------------------------------------------
c prepdf is called by sprime and stesa to compute the inhomogeneity
c vectors df(i)/dp(jpar). here df/dp is computed by the user-supplied
c routine df if idf = 1, or by finite differencing if idf = 0.
c
c in addition to variables described previously, communication with
c prepdf uses the following..
c y     = real array of length nyh containing dependent variables.
c         prepdf uses only the first n entries of y(*).
c srur  = sqrt(uround) (= wm(1)).
c savf  = real array of length n containing derivatives dy/dt.
c ftem  = real array of length n used to temporarily store dy/dt for
c         numerical differentiation.
c dfdp  = real array of length n used to store df(i)/dp(jpar), i = 1,n.
c par   = real array of length npar containing equation parameters
c         of interest.
c jpar  = input parameter, 2 .le. jpar .le. nsv, designating the
c         appropriate solution vector corresponding to par(jpar).
c this routine also uses the common variables tn, miter, n, nfe, ndfe,
c and idf.
c-----------------------------------------------------------------------
      ndfe = ndfe + 1
      idf1 = idf + 1
      go to (100, 200), idf1
c idf = 0, call f to approximate dfdp. ---------------------------------
 100  rpar = par(jpar)
      r = max(srur*abs(rpar),srur)
      par(jpar) = rpar + r
      fac = 1.0d0/r
      call f (neq, tn, y, par, ftem)
      do 110 i = 1,n
 110    dfdp(i) = (ftem(i) - savf(i))*fac
      par(jpar) = rpar
      nfe = nfe + 1
      return
c idf = 1, call user supplied df. --------------------------------------
 200  do 210 i = 1,n
 210    dfdp(i) = 0.0d0
      call df (neq, tn, y, par, dfdp, jpar)
      return
      END
c-----------------------------------------------------------------------
