c-----------------------------------------------------------------------
      SUBROUTINE xsetun (lun)
c
c this routine resets the logical unit number for messages.
c
      INTEGER lun, mesflg, lunit
      COMMON /eh0001/ mesflg, lunit
c
      if (lun .gt. 0) lunit = lun
      return
      END
c-----------------------------------------------------------------------
