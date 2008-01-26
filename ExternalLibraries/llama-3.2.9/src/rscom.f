c-----------------------------------------------------------------------
      SUBROUTINE rscom (rsav, isav)
c-----------------------------------------------------------------------
c this routine restores from rsav and isav the contents of common blocks
c ode001, ode002 and eh0001, which are used internally in the odesssa
c package.  this presumes that rsav and isav were loaded by means
c of subroutine svcom or the equivalent.
c-----------------------------------------------------------------------
      IMPLICIT DOUBLE PRECISION (a-h,o-z)
      DIMENSION rsav(*), isav(*)
      COMMON /ode001/ rode1(219), iode1(39)
      COMMON /ode002/ rode2(3), iode2(11)
      COMMON /eh0001/ ieh(2)
      DATA lrode1/219/, liode1/39/, lrode2/3/, liode2/11/
c
      do 10 i = 1,lrode1
 10     rode1(i) = rsav(i)
      do 20 i = 1,lrode2
        j = lrode1 + i
 20     rode2(i) = rsav(j)
      do 30 i = 1,liode1
 30     iode1(i) = isav(i)
      do 40 i = 1,lode2
        j = liode1 + i
 40     iode2(i) = isav(j)
      ieh(1) = isav(liode1+liode2+1)
      ieh(2) = isav(liode1+liode2+2)
      return
      END
c-----------------------------------------------------------------------
