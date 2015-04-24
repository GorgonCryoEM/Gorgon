c-----------------------------------------------------------------------
      SUBROUTINE xsetf (mflag)
c
c this routine resets the print control flag mflag.
c
      INTEGER mflag, mesflg, lunit
      COMMON /eh0001/ mesflg, lunit
c
      if (mflag .eq. 0 .or. mflag .eq. 1) mesflg = mflag
      return
      END
c-----------------------------------------------------------------------
