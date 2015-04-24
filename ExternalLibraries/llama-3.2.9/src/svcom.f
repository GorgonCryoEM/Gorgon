c-----------------------------------------------------------------------
      SUBROUTINE svcom (rsav, isav)

c this routine stores in rsav and isav the contents of common blocks
c ode001, ode002 and eh0001, which are used internally in the odessa
c package.
c rsav = real array of length 222 or more.
c isav = integer array of length 52 or more.

      IMPLICIT DOUBLE PRECISION (a-h,o-z)
      DIMENSION rsav(*), isav(*)
      COMMON /ode001/ rode1(219), iode1(39)
      COMMON /ode002/ rode2(3), iode2(11)
      COMMON /eh0001/ ieh(2)
      DATA lrode1/219/, liode1/39/, lrode2/3/, liode2/11/
c
      do 10 i = 1,lrode1
 10     rsav(i) = rode1(i)
      do 20 i = 1,lrode2
        j = lrode1 + i
 20     rsav(j) = rode2(i)
      do 30 i = 1,liode1
 30     isav(i) = iode1(i)
      do 40 i = 1,liode2
        j = liode1 + i
 40     isav(j) = iode2(i)
      isav(liode1+liode2+1) = ieh(1)
      isav(liode1+liode2+2) = ieh(2)
      return
      END
c-----------------------------------------------------------------------
