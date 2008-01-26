c-----------------------------------------------------------------------
      DOUBLE PRECISION FUNCTION addx(a,b)
      DOUBLE PRECISION a,b
c
c  this function is necessary to force optimizing compilers to
c  execute and store a sum, for successful execution of the
c  test a + b = b.
c
      addx = a + b
      return
      END
c-----------------------------------------------------------------------
