c-----------------------------------------------------------------------
      SUBROUTINE prepj (neq, y, yh, nyh, wm, iwm, ewt, savf, ftem,
     1   par, f, jac, jopt)
      IMPLICIT DOUBLE PRECISION (a-h,o-z)
      DIMENSION neq(*), y(*), yh(nyh,*), wm(*), iwm(*), ewt(*),
     1   savf(*), ftem(*), par(*)
      EXTERNAL f, jac
      PARAMETER (zero=0.0d0,one=1.0d0)
      COMMON /ode001/ rownd, rowns(173),
     2   rdum1(37), el0, h, rdum2(4), tn, uround,
     3   iownd(14), iowns(4),
     4   idum1(3), ierpj, idum2, jcur, idum3(4),
     5   miter, idum4(4), n, idum5(2), nfe, nje, idum6
c-----------------------------------------------------------------------
c prepj is called by stode to compute and process the matrix
c p = i - h*el(1)*j , where j is an approximation to the jacobian.
c if isopt = 1, prepj is also called by sprime with jopt = 1.
c here j is computed by the user-supplied routine jac if
c miter = 1 or 4, or by finite differencing if miter = 2, 3, or 5.
c if miter = 3, a diagonal approximation to j is used.
c j is stored in wm and replaced by p.  if miter .ne. 3, p is then
c subjected to lu decomposition (jopt = 0) in preparation for later
c solution of linear systems with p as coefficient matrix. this is
c done by dgefa if miter = 1 or 2, and by dgbfa if miter = 4 or 5.
c
c in addition to variables described previously, communication
c with prepj uses the following..
c y     = array containing predicted values on entry.
c ftem  = work array of length n (acor in stode).
c savf  = array containing f evaluated at predicted y.
c wm    = real work space for matrices.  on output it contains the
c         inverse diagonal matrix if miter = 3 and the lu decomposition
c         of p if miter is 1, 2 , 4, or 5.
c         storage of matrix elements starts at wm(3).
c         wm also contains the following matrix-related data..
c         wm(1) = sqrt(uround), used in numerical jacobian increments.
c         wm(2) = h*el0, saved for later use if miter = 3.
c iwm   = integer work space containing pivot information, starting at
c         iwm(21), if miter is 1, 2, 4, or 5.  iwm also contains band
c         parameters ml = iwm(1) and mu = iwm(2) if miter is 4 or 5.
c el0   = el(1) (input).
c ierpj = output error flag,  = 0 if no trouble, .gt. 0 if
c         p matrix found to be singular.
c jcur  = output flag = 1 to indicate that the jacobian matrix
c         (or approximation) is now current.
c jopt  = input jacobian option, = 1 if jac is desired only.
c this routine also uses the common variables el0, h, tn, uround,
c ierpj, jcur, miter, n, nfe, and nje.
c-----------------------------------------------------------------------
      nje = nje + 1
      ierpj = 0
      jcur = 1
      hl0 = h*el0
      go to (100, 200, 300, 400, 500), miter
c if miter = 1, call jac and multiply by scalar. -----------------------
 100   lenp = n*n
      do 110 i = 1,lenp
 110    wm(i+2) = zero
      call jac (neq, tn, y, par, 0, 0, wm(3), n)
      if (jopt .eq. 1) return
      con = -hl0
      do 120 i = 1,lenp
 120    wm(i+2) = wm(i+2)*con
      go to 240
c if miter = 2, make n calls to f to approximate j. --------------------
 200   fac = vnorm (n, savf, ewt)
      r0 = 1000.0d0*abs(h)*uround*real(n)*fac
      if (r0 .eq. zero) r0 = one
      srur = wm(1)
      j1 = 2
      do 230 j = 1,n
        yj = y(j)
        r = max(srur*abs(yj),r0/ewt(j))
        y(j) = y(j) + r
        fac = -hl0/r
        call f (neq, tn, y, par, ftem)
        do 220 i = 1,n
 220      wm(i+j1) = (ftem(i) - savf(i))*fac
        y(j) = yj
        j1 = j1 + n
 230    continue
      nfe = nfe + n
      if (jopt .eq. 1) return
c add identity matrix. -------------------------------------------------
 240   j = 3
      do 250 i = 1,n
        wm(j) = wm(j) + one
 250    j = j + (n + 1)
c do lu decomposition on p. --------------------------------------------
      call dgefa (wm(3), n, n, iwm(21), ier)
      if (ier .ne. 0) ierpj = 1
      return
c if miter = 3, construct a diagonal approximation to j and p. ---------
 300   wm(2) = hl0
      r = el0*0.1d0
      do 310 i = 1,n
 310    y(i) = y(i) + r*(h*savf(i) - yh(i,2))
      call f (neq, tn, y, par, wm(3))
      nfe = nfe + 1
      do 320 i = 1,n
        r0 = h*savf(i) - yh(i,2)
        di = 0.1d0*r0 - h*(wm(i+2) - savf(i))
        wm(i+2) = 1.0d0
        if (abs(r0) .lt. uround/ewt(i)) go to 320
        if (abs(di) .eq. zero) go to 330
        wm(i+2) = 0.1d0*r0/di
 320    continue
      return
 330   ierpj = 1
      return
c if miter = 4, call jac and multiply by scalar. -----------------------
 400   ml = iwm(1)
      mu = iwm(2)
      ml3 = ml + 3
      mband = ml + mu + 1
      meband = mband + ml
      lenp = meband*n
      do 410 i = 1,lenp
 410    wm(i+2) = zero
      call jac (neq, tn, y, par, ml, mu, wm(ml3), meband)
      if (jopt .eq. 1) return
      con = -hl0
      do 420 i = 1,lenp
 420    wm(i+2) = wm(i+2)*con
      go to 570
c if miter = 5, make mband calls to f to approximate j. ----------------
 500   ml = iwm(1)
      mu = iwm(2)
      mband = ml + mu + 1
      mba = min(mband,n)
      meband = mband + ml
      meb1 = meband - 1
      srur = wm(1)
      fac = vnorm (n, savf, ewt)
      r0 = 1000.0d0*abs(h)*uround*real(n)*fac
      if (r0 .eq. zero) r0 = one
      do 560 j = 1,mba
        do 530 i = j,n,mband
          yi = y(i)
          r = max(srur*abs(yi),r0/ewt(i))
 530      y(i) = y(i) + r
        call f (neq, tn, y, par, ftem)
        do 550 jj = j,n,mband
          y(jj) = yh(jj,1)
          yjj = y(jj)
          r = max(srur*abs(yjj),r0/ewt(jj))
          fac = -hl0/r
          i1 = max(jj-mu,1)
          i2 = min(jj+ml,n)
          ii = jj*meb1 - ml + 2
          do 540 i = i1,i2
 540        wm(ii+i) = (ftem(i) - savf(i))*fac
 550      continue
 560    continue
      nfe = nfe + mba
      if (jopt .eq. 1) return
c add identity matrix. -------------------------------------------------
 570   ii = mband + 2
      do 580 i = 1,n
        wm(ii) = wm(ii) + one
 580    ii = ii + meband
c do lu decomposition of p. --------------------------------------------
      call dgbfa (wm(3), meband, n, ml, mu, iwm(21), ier)
      if (ier .ne. 0) ierpj = 1
      return
      END
c-----------------------------------------------------------------------
