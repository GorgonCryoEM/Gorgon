c-----------------------------------------------------------------------
      SUBROUTINE stesa (neq, y, nrow, ncol, yh, wm, iwm, ewt, savf,
     1   acor, par, nrs, f, jac, df, pjac, pdf, solve)
      IMPLICIT DOUBLE PRECISION (a-h,o-z)
      EXTERNAL f, jac, df, pjac, pdf, solve
      DIMENSION neq(*), y(nrow,*), yh(nrow,ncol,*), wm(*), iwm(*),
     1   ewt(nrow,*), savf(*), acor(nrow,*), par(*), nrs(*)
      PARAMETER (one=1.0d0,zero=0.0d0)
      COMMON /ode001/ rownd, rowns(173),
     1   tesco(3,12), rdum1, el0, h, rdum2(4), tn, rdum3,
     2   iownd1(14), iowns(4),
     3   ialth, lmax, idum1, ierpj, iersl, jcur, idum2, kflag, l, idum3,
     4   miter, idum4(4), n, nq, idum5, nfe, idum6(2)
      COMMON /ode002/ dups, dsms, ddns,
     1   iownd2(3), idum7, nsv, idum8(2), idf, idum9, jopt, kflags
c-----------------------------------------------------------------------
c stesa is called by stode to perform an explicit calculation for the
c first-order sensitivity coefficients dy(i)/dp(j), i = 1,n; j = 1,npar.
c
c in addition to the variables described previously, communication
c with stesa uses the following..
c y      = an nrow (=n) by ncol (=nsv) real array containing the
c          corrected dependent variables on output..
c                  y(i,1) , i = 1,n = state variables (input);
c                  y(i,j) , i = 1,n , j = 2,nsv ,
c                           = sensitivity coefficients, dy(i)/dp(j).
c yh     = an n by nsv by lmax real array containing the predicted
c          dependent variables and their approximate scaled derivatives.
c savf   = a real array of length n used to store first derivatives
c          of dependent variables if miter = 2 or 5.
c par    = a real array of length npar containing the equation
c          parameters of interest.
c nrs    = an integer array of length npar + 1 containing the number
c          of repeated steps (kflags .lt. 0) due to the sensitivity
c          calculations..
c                  nrs(1) = total number of repeated steps
c                  nrs(i) , i = 2,npar = number of repeated steps due
c                                        to parameter i.
c nsv    = number of solution vectors = npar + 1.
c kflags = local error test flag, = 0 if test passes, .lt. 0 if test
c          fails, and step needs to be repeated. error test is applied
c          to each solution vector independently.
c dups, dsms, ddns = real scalars used for computing rhup, rhsm, rhdn,
c                    on return to stode (ialth .eq. 1).
c this routine also uses the common variables el0, h, tn, ialth, lmax,
c ierpj, iersl, jcur, kflag, l, miter, n, nq, nfe, and jopt.
c-----------------------------------------------------------------------
      dups = zero
      dsms = zero
      ddns = zero
      hl0 = h*el0
      el0i = one/el0
      ti2 = one/tesco(2,nq)
      ti3 = one/tesco(3,nq)
c if miter = 2 or 5 (or idf = 0), supply derivatives at corrected
c y(*,1) values for numerical differentiation in pjac and/or pdf.
      if (miter .eq. 2  .or.  miter .eq. 5  .or.  idf .eq. 0)  go to 10
      go to 15
 10   call f (neq, tn, y, par, savf)
      nfe = nfe + 1
c if jcur = 0, update the jacobian matrix.
c if miter = 5, load corrected y(*,1) values into y(*,2).
 15   if (jcur .eq. 1) go to 30
      if (miter .ne. 5) go to 25
      do 20 i = 1,n
 20     y(i,2) = y(i,1)
 25   call pjac (neq, y, y(1,2), n, wm, iwm, ewt, savf, acor(1,2),
     1   par, f, jac, jopt)
      if (ierpj .ne. 0) return
c-----------------------------------------------------------------------
c this is a looping point for the sensitivity calculations.
c-----------------------------------------------------------------------
c for each parameter par(*), a sensitivity solution vector is computed
c using the same step size (h) and order (nq) as in stode.
c a local error test is applied independently to each solution vector.
c-----------------------------------------------------------------------
 30   do 100 j = 2,nsv
        jpar = j - 1
c evaluate inhomogeneity term, temporarily load into y(*,jpar+1). ------
        call pdf(neq, y, wm, savf, acor(1,j), y(1,j), par,
     1     f, df, jpar)
c-----------------------------------------------------------------------
c load rhs of sensitivity solution (corrector) equation..
c
c       rhs = dy/dp - el(1)*h*d(dy/dp)/dt + el(1)*h*df/dp
c
c-----------------------------------------------------------------------
        do 40 i = 1,n
 40       y(i,j) = yh(i,j,1) - el0*yh(i,j,2) + hl0*y(i,j)
c-----------------------------------------------------------------------
c solve corrector equation: the solutions are located in y(*,jpar+1).
c the explicit formula is..
c
c       (i - el(1)*h*jac) * dy/dp(corrected) = rhs
c
c-----------------------------------------------------------------------
        call solve (wm, iwm, y(1,j), dum)
        if (iersl .ne. 0) return
c estimate local truncation error. -------------------------------------
        do 50 i = 1,n
 50       acor(i,j) = (y(i,j) - yh(i,j,1))*el0i
        err = vnorm(n, acor(1,j), ewt(1,j))*ti2
        if (err .gt. one) go to 200
c-----------------------------------------------------------------------
c local error test passed. set kflags to 0 to indicate this.
c if ialth = 1, compute dsms, ddns, and dups (if l .lt. lmax).
c-----------------------------------------------------------------------
        kflags = 0
        if (ialth .gt. 1) go to 100
        if (l .eq. lmax) go to 70
        do 60 i= 1,n
 60       y(i,j) = acor(i,j) - yh(i,j,lmax)
        dups = max(dups,vnorm(n,y(1,j),ewt(1,j))*ti3)
 70     dsms = max(dsms,err)
 100  continue
      return
c-----------------------------------------------------------------------
c this section is reached if the error tolerance for sensitivity
c solution vector jpar has been violated. kflags is made negative to
c indicate this. if kflags = -1, set kflag equal to zero so that kflag
c is set to -1 on return to stode before repeating the step.
c increment nrs(1) (= total number of repeated steps due to all
c sensitivity solution vectors) by one.
c increment nrs(jpar+1) (= total number of repeated steps due to
c solution vector jpar+1) by one.
c load dsms for rh calculation in stode.
c-----------------------------------------------------------------------
 200  kflags = kflags - 1
      if (kflags .eq. -1) kflag = 0
      nrs(1) = nrs(1) + 1
      nrs(j) = nrs(j) + 1
      dsms = err
      return
      END
c-----------------------------------------------------------------------
