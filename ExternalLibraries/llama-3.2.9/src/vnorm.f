c-----------------------------------------------------------------------
      DOUBLE PRECISION FUNCTION vnorm (n, v, w)
c-----------------------------------------------------------------------
c this function routine computes the weighted root-mean-square norm
c of the vector of length n contained in the array v, with weights
c contained in the array w of length n..
c  vnorm = sqrt( (1/n) * sum( v(i)*w(i) )**2 )
c protection for underflow/overflow is accomplished using two
c constants which are hopefully applicable for all machines.
c these are:
c     cutlo = maximum of sqrt(u/eps)  over all known machines
c     cuthi = minimum of sqrt(z)      over all known machines
c where
c     eps = smallest number s.t. eps + 1 .gt. 1
c     u   = smallest positive number (underflow limit)
c     z   = largest number (overflow limit)
c
c details of the algorithm and of values of cutlo and cuthi are
c found in the blas routine snrm2 (see also algorithm 539, trans.
c math. software, vol. 5 no. 3, 1979, 308-323.
c for single precision, the following values should be universal:
c     data cutlo,cuthi /4.441e-16,1.304e19/
c for double precision, use
c     data cutlo,cuthi /8.232d-11,1.304d19/
c-----------------------------------------------------------------------
      IMPLICIT DOUBLE PRECISION (a-h,o-z)
      INTEGER next,i,j,n
      DIMENSION v(n),w(n)
      DATA cutlo,cuthi /8.232d-11,1.304d19/
      DATA zero,one/0.0d0,1.0d0/
c  blas algorithm
      next = 1
      sum = zero
      i = 1
20    sx = v(i)*w(i)
      go to (30,40,70,80),next
30    if (abs(sx).gt.cutlo) go to 110
      next = 2
      xmax = zero
40    if (sx.eq.zero) go to 130
      if (abs(sx).gt.cutlo) go to 110
      next = 3
      go to 60
50    i=j
      next = 4
      sum = (sum/sx)/sx
60    xmax = abs(sx)
      go to 90
70    if(abs(sx).gt.cutlo) go to 100
80    if(abs(sx).le.xmax) go to 90
      sum = one + sum * (xmax/sx)**2
      xmax = abs(sx)
      go to 130
90    sum = sum + (sx/xmax)**2
      go to 130
100   sum = (sum*xmax)*xmax
110   hitest = cuthi/real(n)
      do 120 j = i,n
         sx = v(j)*w(j)
         if(abs(sx).ge.hitest) go to 50
         sum = sum + sx**2
120   continue
      vnorm = sqrt(sum)
      go to 140
130   continue
      i = i + 1
      if (i.le.n) go to 20
      vnorm = xmax * sqrt(sum)
140   continue
      return
      END
c-----------------------------------------------------------------------
