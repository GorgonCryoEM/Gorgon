c-----------------------------------------------------------------------
      SUBROUTINE intdy (t, k, yh, nyh, dky, iflag)
      IMPLICIT DOUBLE PRECISION (a-h,o-z)
      DIMENSION yh(nyh,1), dky(1)
      COMMON /ode001/ rownd, rowns(173),
     2   rdum1(38),h, rdum2(2), hu, rdum3, tn, uround,
     3   iownd(14), iowns(4),
     4   idum1(8), l, idum2,
     5   idum3(5), n, nq, idum4(4)
c-----------------------------------------------------------------------
c intdy computes interpolated values of the k-th derivative of the
c dependent variable vector y, and stores it in dky.  this routine
c is called within the package with k = 0 and t = tout, but may
c also be called by the user for any k up to the current order.
c (see detailed instructions in the usage documentation.)
c-----------------------------------------------------------------------
c the computed values in dky are gotten by interpolation using the
c nordsieck history array yh.  this array corresponds uniquely to a
c vector-valued polynomial of degree nqcur or less, and dky is set
c to the k-th derivative of this polynomial at t.
c the formula for dky is..
c             q
c  dky(i)  =  sum  c(j,k) * (t - tn)**(j-k) * h**(-j) * yh(i,j+1)
c            j=k
c where  c(j,k) = j*(j-1)*...*(j-k+1), q = nqcur, tn = tcur, h = hcur.
c the quantities  nq = nqcur, l = nq+1, n = neq, tn, and h are
c communicated by common.  the above sum is done in reverse order.
c iflag is returned negative if either k or t is out of bounds.
c-----------------------------------------------------------------------
      iflag = 0
      if (k .lt. 0 .or. k .gt. nq) go to 80
      tp = tn - hu*(1.0d0 + 100.0d0*uround)
      if ((t-tp)*(t-tn) .gt. 0.0d0) go to 90
c
      s = (t - tn)/h
      ic = 1
      if (k .eq. 0) go to 15
      jj1 = l - k
      do 10 jj = jj1,nq
 10     ic = ic*jj
 15   c = real(ic)
      do 20 i = 1,nyh
 20     dky(i) = c*yh(i,l)
      if (k .eq. nq) go to 55
      jb2 = nq - k
      do 50 jb = 1,jb2
        j = nq - jb
        jp1 = j + 1
        ic = 1
        if (k .eq. 0) go to 35
        jj1 = jp1 - k
        do 30 jj = jj1,j
 30       ic = ic*jj
 35     c = real(ic)
        do 40 i = 1,nyh
 40       dky(i) = c*yh(i,jp1) + s*dky(i)
 50     continue
      if (k .eq. 0) return
 55   r = h**(-k)
      do 60 i = 1,nyh
 60     dky(i) = r*dky(i)
      return
c
 80   call xerr('INTDY--  k (=i1) illegal',
     1  51, 1, 1, k, 0, 0, zero,zero)
      iflag = -1
      return
 90   call xerr ('INTDY--  t (=r1) illegal',
     1   52, 1, 0, 0, 0, 1, t, zero)
      call xerr('t not in interval tcur - hu (= r1) to tcur (=r2)',
     1   52, 1, 0, 0, 0, 2, tp, tn)
      iflag = -2
      return
      END
c-----------------------------------------------------------------------
