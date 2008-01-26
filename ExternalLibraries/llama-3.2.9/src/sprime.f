c-----------------------------------------------------------------------
      SUBROUTINE sprime (neq, y, yh, nyh, nrow, ncol, wm, iwm,
     1  ewt, savf, ftem, dfdp, par, f, jac, df, pjac, pdf)
      IMPLICIT DOUBLE PRECISION (a-h,o-z)
      DIMENSION neq(*), y(*), yh(nrow,ncol,*), wm(*), iwm(*),
     1  ewt(*), savf(*), ftem(*), dfdp(nrow,*), par(*)
      EXTERNAL f, jac, df, pjac, pdf
      PARAMETER (one=1.0d0,zero=0.0d0)
      COMMON /ode001/ rownd, rowns(173),
     1  rdum1(37),el0, h, rdum2(6),
     2  iownd1(14), iowns(4),
     3  idum1(3), ierpj, idum2(6),
     4  miter, idum3(4), n, idum4(5)
      COMMON /ode002/ rdum3(3),
     1  iownd2(3), idum5, nsv, idum6, nspe, idum7, iersp, jopt, idum8
c-----------------------------------------------------------------------
c sprime is called by odessa to initialize the yh array. it is also
c called by stode to reevaluate first order derivatives when kflag
c .le. -3. sprime computes the first derivatives of the sensitivity
c coefficients with respect to the independent variable t...
c
c        sprime = d(dy/dp)/dt = jac*dy/dp + df/dp
c                   where jac = jacobian matrix
c                       dy/dp = sensitivity matrix
c                       df/dp = inhomogeneity matrix
c this routine uses the common variables el0, h, ierpj, miter, n,
c nsv, nspe, iersp, jopt
c-----------------------------------------------------------------------
c call prepj with jopt = 1.
c if miter = 2 or 5, el0 is temporarily set to -1.0 and h is
c temporarily set to 1.0d0.
c-----------------------------------------------------------------------
      nspe = nspe + 1
      jopt = 1
      if (miter .eq. 1 .or. miter .eq. 4) go to 10
      htemp = h
      etemp = el0
      h = one
      el0 = -one
 10   call pjac (neq, y, yh, nyh, wm, iwm, ewt, savf, ftem,
     1   par, f, jac, jopt)
      if (ierpj .ne. 0) go to 300
      jopt = 0
      if (miter .eq. 1 .or. miter .eq. 4) go to 20
      h = htemp
      el0 = etemp
c-----------------------------------------------------------------------
c call prepdf and load dfdp(*,jpar).
c-----------------------------------------------------------------------
 20   do 30 j = 2,nsv
        jpar = j - 1
        call pdf (neq, y, wm, savf, ftem, dfdp(1,jpar), par,
     1     f, df, jpar)
 30   continue
c-----------------------------------------------------------------------
c compute jac*dy/dp and store results in yh(*,*,2).
c-----------------------------------------------------------------------
      go to (40,40,310,100,100) miter
c the jacobian is full.------------------------------------------------
c for each row of the jacobian..
 40   do 70 irow = 1,n
c and each column of the sensitivity matrix..
        do 60 j = 2,nsv
          sum = zero
c take the vector dot product..
          do 50 i = 1,n
            ipd = irow + n*(i-1) + 2
            sum = sum + wm(ipd)*yh(i,j,1)
 50       continue
          yh(irow,j,2) = sum
 60     continue
 70   continue
      go to 200
c the jacobian is banded.-----------------------------------------------
 100  ml = iwm(1)
      mu = iwm(2)
      icount = 1
      mband = ml + mu + 1
      meband = mband + ml
      nmu = n - mu
      ml1 = ml + 1
c for each row of the jacobian..
      do 160 irow = 1,n
        if (irow .gt. ml1) go to 110
        ipd = mband + irow + 1
        iyh = 1
        lband = mu + irow
        go to 120
 110    icount = icount + 1
        ipd = icount*meband + 2
        iyh = iyh + 1
        lband = lband - 1
        if (irow .le. nmu) lband = mband
c and each column of the sensitivity matrix..
 120    do 150 j = 2,nsv
          sum = zero
          i1 = ipd
          i2 = iyh
c take the vector dot product.
          do 140 i = 1,lband
            sum = sum + wm(i1)*yh(i2,j,1)
            i1 = i1 + meband - 1
            i2 = i2 + 1
 140      continue
          yh(irow,j,2) = sum
 150    continue
 160  continue
c-----------------------------------------------------------------------
c add the inhomogeneity term, i.e., add dfdp(*,jpar) to yh(*,jpar+1,2).
c-----------------------------------------------------------------------
 200  do 220 j = 2,nsv
        jpar = j - 1
        do 210 i = 1,n
          yh(i,j,2) = yh(i,j,2) + dfdp(i,jpar)
 210    continue
 220  continue
      return
c-----------------------------------------------------------------------
c error returns.
c-----------------------------------------------------------------------
 300  iersp = -1
      return
 310  iersp = -2
      return
      END
c-----------------------------------------------------------------------
