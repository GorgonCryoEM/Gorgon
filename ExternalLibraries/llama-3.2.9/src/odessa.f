c-----------------------------------------------------------------------
      SUBROUTINE odessa (f, df, neq, y, par, t, tout, itol, rtol, atol,
     1   itask, istate, iopt, rwork, lrw, iwork, liw, jac, mf)
      IMPLICIT DOUBLE PRECISION (a-h,o-z)
      LOGICAL ihit
      EXTERNAL f, df, jac, prepj, solsy, prepdf
      DIMENSION neq(*), y(*), par(*), rtol(*), atol(*), iopt(*),
     1   rwork(lrw), iwork(liw), mord(2)
c-----------------------------------------------------------------------
c this is the september 1, 1986 version of ODESSA..
c an ordinary differential equation solver with explicit simultaneous
c sensitivity analysis.
c
c this package is a modification of the august 13, 1981 version of
c lsode.. livermore solver for ordinary differential equations.
c this version is in double precision.
c
c ODESSA solves for the first-order sensitivity coefficients..
c    dy(i)/dp, for a single parameter, or,
c    dy(i)/dp(j), for multiple parameters,
c associated with a system of ordinary differential equations..
c    dy(t)/dt = f(y,t;p).
c-----------------------------------------------------------------------
c references...
c
c  1. jorge r. leis and mark a. kramer, the simultaneous solution and
c     explicit sensitivity analysis of systems described by ordinary
c     differential equations, submitted to acm trans. math. software,
c     (1985).
c
c  2. jorge r. leis and mark a. kramer, ODESSA - an ordinary
c     differential equation solver with explicit simultaneous
c     sensitivity analysis, submitted to acm trans. math. software.
c     (1985).
c
c  3. alan c. hindmarsh,  lsode and lsodi,  two new initial value
c     ordinary differential equation solvers, acm-signum newsletter,
c     vol. 15, no. 4 (1980), pp. 10-11.
c-----------------------------------------------------------------------
c the following internal common blocks contain
c (a) variables which are local to any subroutine but whose values must
c    be preserved between calls to the routine (own variables), and
c (b) variables which are communicated between subroutines.
c the structure of the blocks are as follows..  all real variables are
c listed first, followed by all integers.  within each type, the
c variables are grouped with those local to subroutine odessa first,
c then those local to subroutine stode, and finally those used
c for communication.  the blocks are declared in subroutines odessa
c intdy, stode, stesa, prepj, prepdf, and solsy.  groups of variables
c are replaced by dummy arrays in the common declarations in routines
c where those variables are not used.
c-----------------------------------------------------------------------
      COMMON /ode001/ tret, rowns(173),
     1   tesco(3,12), ccmax, el0, h, hmin, hmxi, hu, rc, tn, uround,
     2   illin, init, lyh, lewt, lacor, lsavf, lwm, liwm,
     3   mxstep, mxhnil, nhnil, ntrep, nslast, nyh, iowns(4),
     4   ialth, lmax, icf, ierpj, iersl, jcur, jstart, kflag, l, meth,
     5   miter, maxord, maxcor, msbp, mxncf, n, nq, nst, nfe, nje, nqu
      COMMON /ode002/ dups, dsms, ddns,
     1   npar, ldfdp, lnrs,
     2   isopt, nsv, ndfe, nspe, idf, iersp, jopt, kflags
      PARAMETER (zero=0.0d0,one=1.0d0,two=2.0d0,four=4.0d0)
      DATA illin,ntrep/0,0/
      DATA  mord(1),mord(2)/12,5/, mxstp0/500/, mxhnl0/10/
c-----------------------------------------------------------------------
c block a.
c this code block is executed on every call.
c it tests istate and itask for legality and branches appropiately.
c if istate .gt. 1 but the flag init shows that initialization has
c not yet been done, an error return occurs.
c if istate = 1 and tout = t, jump to block g and return immediately.
c-----------------------------------------------------------------------
      if (istate .lt. 1 .or. istate .gt. 3) go to 601
      if (itask .lt. 1 .or. itask .gt. 5) go to 602
      if (istate .eq. 1) go to 10
      if (init .eq. 0) go to 603
      if (istate .eq. 2) go to 200
      go to 20
 10   init = 0
      if (tout .eq. t) go to 430
 20   ntrep = 0
c-----------------------------------------------------------------------
c block b.
c the next code block is executed for the initial call (istate = 1),
c or for a continuation call with parameter changes (istate = 3).
c it contains checking of all inputs and various initializations.
c
c first check legality of the non-optional inputs neq, itol, iopt,
c mf, ml, and mu.
c-----------------------------------------------------------------------
      if (neq(1) .le. 0) go to 604
      if (istate .eq. 1) go to 25
      if (neq(1) .ne. n) go to 605
 25   n = neq(1)
      if (itol .lt. 1 .or. itol .gt. 4) go to 606
      do 26 i = 1,3
 26     if (iopt(i) .lt. 0 .or. iopt(i) .gt. 1) go to 607
      isopt = iopt(2)
      idf = iopt(3)
      nyh = n
      nsv = 1
      meth = mf/10
      miter = mf - 10*meth
      if (meth .lt. 1 .or. meth .gt. 2) go to 608
      if (miter .lt. 0 .or. miter .gt. 5) go to 608
      if (miter .le. 3) go to 30
      ml = iwork(1)
      mu = iwork(2)
      if (ml .lt. 0 .or. ml .ge. n) go to 609
      if (mu .lt. 0 .or. mu .ge. n) go to 610
 30   if (isopt .eq. 0) go to 32
c check legality of the non-optional inputs isopt, npar.
c compute number of solution vectors and total number of equations.
      if (neq(2) .le. 0) go to 628
      if (istate .eq. 1) go to 31
      if (neq(2) .ne. npar) go to 629
 31   npar = neq(2)
      nsv = npar + 1
      nyh = nsv * n
      if (miter .eq. 0 .or. miter .eq. 3) go to 630
c next process and check the optional inputs. --------------------------
 32   if (iopt(1) .eq. 1) go to 40
      maxord = mord(meth)
      mxstep = mxstp0
      mxhnil = mxhnl0
      if (istate .eq. 1) h0 = zero
      hmxi = zero
      hmin = zero
      go to 60
 40   maxord = iwork(5)
      if (maxord .lt. 0) go to 611
      if (maxord .eq. 0) maxord = 100
      maxord = min(maxord,mord(meth))
      mxstep = iwork(6)
      if (mxstep .lt. 0) go to 612
      if (mxstep .eq. 0) mxstep = mxstp0
      mxhnil = iwork(7)
      if (mxhnil .lt. 0) go to 613
      if (mxhnil .eq. 0) mxhnil = mxhnl0
      if (istate .ne. 1) go to 50
      h0 = rwork(5)
      if ((tout - t)*h0 .lt. zero) go to 614
 50   hmax = rwork(6)
      if (hmax .lt. zero) go to 615
      hmxi = zero
      if (hmax .gt. zero) hmxi = one/hmax
      hmin = rwork(7)
      if (hmin .lt. zero) go to 616
c-----------------------------------------------------------------------
c set work array pointers and check lengths lrw and liw.
c pointers to segments of rwork and iwork are named by prefixing l to
c the name of the segment.  e.g., the segment yh starts at rwork(lyh).
c segments of rwork (in order) are denoted  yh, wm, ewt, savf, acor.
c work space for dfdp is contained in acor.
c-----------------------------------------------------------------------
 60   lyh = 21
      lwm = lyh + (maxord + 1)*nyh
      if (miter .eq. 0) lenwm = 0
      if (miter .eq. 1 .or. miter .eq. 2) lenwm = n*n + 2
      if (miter .eq. 3) lenwm = n + 2
      if (miter .ge. 4) lenwm = (2*ml + mu + 1)*n + 2
      lewt = lwm + lenwm
      lsavf = lewt + nyh
      lacor = lsavf + n
      ldfdp = lacor + n
      lenrw = lacor + nyh - 1
      iwork(17) = lenrw
      liwm = 1
      leniw = 20 + n
      if (miter .eq. 0 .or. miter .eq. 3) leniw = 20
      lnrs = leniw + liwm
      if (isopt .eq. 1) leniw = lnrs + npar
      iwork(18) = leniw
      if (lenrw .gt. lrw) go to 617
      if (leniw .gt. liw) go to 618
c check rtol and atol for legality. ------------------------------------
      rtoli = rtol(1)
      atoli = atol(1)
      do 70 i = 1,nyh
        if (itol .ge. 3) rtoli = rtol(i)
        if (itol .eq. 2 .or. itol .eq. 4) atoli = atol(i)
        if (rtoli .lt. zero) go to 619
        if (atoli .lt. zero) go to 620
 70     continue
      if (istate .eq. 1) go to 100
c if istate = 3, set flag to signal PARAMETER changes to stode. --------
      jstart = -1
      if (nq .le. maxord) go to 90
c maxord was reduced below nq.  copy yh(*,maxord+2) into savf. ---------
      do 80 i = 1,n
 80     rwork(i+lsavf-1) = rwork(i+lwm-1)
c reload wm(1) = rwork(lwm), since lwm may have changed. ---------------
 90   if (miter .gt. 0) rwork(lwm) = sqrt(uround)
      go to 200
c-----------------------------------------------------------------------
c block c.
c the next block is for the initial call only (istate = 1).
c it contains all remaining initializations, the initial call to f,
c the initial call to sprime if isopt = 1,
c and the calculation of the initial step size.
c the error weights in ewt are inverted after being loaded.
c-----------------------------------------------------------------------
 100  uround = d1mach(4)
      tn = t
      if (itask .ne. 4 .and. itask .ne. 5) go to 105
      tcrit = rwork(1)
      if ((tcrit - tout)*(tout - t) .lt. zero) go to 625
      if (h0 .ne. zero .and. (t + h0 - tcrit)*h0 .gt. zero)
     1   h0 = tcrit - t
 105  jstart = 0
      if (miter .gt. 0) rwork(lwm) = sqrt(uround)
      nhnil = 0
      nst = 0
      nje = 0
      nslast = 0
      hu = zero
      nqu = 0
      ccmax = 0.3d0
      maxcor = 3
      if (isopt .eq. 1) maxcor = 4
      msbp = 20
      mxncf = 10
c initial call to f.  (lf0 points to yh(1,2) and loads in values).
      lf0 = lyh + nyh
      call f (neq, t, y, par, rwork(lf0))
      nfe = 1
      dups = zero
      dsms = zero
      ddns = zero
      ndfe = 0
      nspe = 0
      if (isopt .eq. 0) go to 114
c initialize counts for repeated steps due to sensitivity analysis.
      do 110 j = 1,nsv
 110    iwork(j + lnrs - 1) = 0
c load the initial value vector in yh. ---------------------------------
 114  do 115 i = 1,nyh
 115    rwork(i+lyh-1) = y(i)
c load and invert the ewt array.  (h is temporarily set to one.) -------
      nq = 1
      h = one
      call ewset (nyh, itol, rtol, atol, rwork(lyh), rwork(lewt))
      do 120 i = 1,nyh
        if (rwork(i+lewt-1) .le. zero) go to 621
 120    rwork(i+lewt-1) = one/rwork(i+lewt-1)
      if (isopt .eq. 0) go to 125
c call sprime to load first-order sensitivity derivatives into
c remaining yh(*,2) positions.
      call sprime (neq, y, rwork(lyh), nyh, n, nsv, rwork(lwm),
     1   iwork(liwm), rwork(lewt), rwork(lf0), rwork(lacor),
     2   rwork(ldfdp), par, f, jac, df, prepj, prepdf)
      if (iersp .eq. -1) go to 631
      if (iersp .eq. -2) go to 632
c-----------------------------------------------------------------------
c the coding below computes the step size, h0, to be attempted on the
c first step, unless the user has supplied a value for this.
c first check that tout - t differs significantly from zero.
c a scalar tolerance quantity tol is computed, as max(rtol(i))
c if this is positive, or max(atol(i)/abs(y(i))) otherwise, adjusted
c so as to be between 100*uround and 1.0e-3. only the original
c solution vector is considered in this calculation (isopt = 0 or 1).
c then the computed value h0 is given by..
c                                     neq
c  h0**2 = tol / ( w0**-2 + (1/neq) * sum ( f(i)/ywt(i) )**2  )
c                                      1
c where  w0     = max ( abs(t), abs(tout) ),
c        f(i)   = i-th component of initial value of f,
c        ywt(i) = ewt(i)/tol  (a weight for y(i)).
c the sign of h0 is inferred from the initial values of tout and t.
c-----------------------------------------------------------------------
 125  if (h0 .ne. zero) go to 180
      tdist = abs(tout - t)
      w0 = max(abs(t),abs(tout))
      if (tdist .lt. two*uround*w0) go to 622
      tol = rtol(1)
      if (itol .le. 2) go to 140
      do 130 i = 1,n
 130    tol = max(tol,rtol(i))
 140   if (tol .gt. zero) go to 160
      atoli = atol(1)
      do 150 i = 1,n
        if (itol .eq. 2 .or. itol .eq. 4) atoli = atol(i)
        ayi = abs(y(i))
        if (ayi .ne. zero) tol = max(tol,atoli/ayi)
 150    continue
 160   tol = max(tol,100.0d0*uround)
      tol = min(tol,0.001d0)
      sum = vnorm (n, rwork(lf0), rwork(lewt))
      sum = one/(tol*w0*w0) + tol*sum**2
      h0 = one/sqrt(sum)
      h0 = min(h0,tdist)
      h0 = sign(h0,tout-t)
c adjust h0 if necessary to meet hmax bound. ---------------------------
 180   rh = abs(h0)*hmxi
      if (rh .gt. one) h0 = h0/rh
c load h with h0 and scale yh(*,2) by h0. ------------------------------
      h = h0
      do 190 i = 1,nyh
 190    rwork(i+lf0-1) = h0*rwork(i+lf0-1)
      go to 270
c-----------------------------------------------------------------------
c block d.
c the next code block is for continuation calls only (istate = 2 or 3)
c and is to check stop conditions before taking a step.
c-----------------------------------------------------------------------
 200  nslast = nst
      go to (210, 250, 220, 230, 240), itask
 210  if ((tn - tout)*h .lt. zero) go to 250
      call intdy (tout, 0, rwork(lyh), nyh, y, iflag)
      if (iflag .ne. 0) go to 627
      t = tout
      go to 420
 220   tp = tn - hu*(one + 100.0d0*uround)
      if ((tp - tout)*h .gt. zero) go to 623
      if ((tn - tout)*h .lt. zero) go to 250
      go to 400
 230   tcrit = rwork(1)
      if ((tn - tcrit)*h .gt. zero) go to 624
      if ((tcrit - tout)*h .lt. zero) go to 625
      if ((tn - tout)*h .lt. zero) go to 245
      call intdy (tout, 0, rwork(lyh), nyh, y, iflag)
      if (iflag .ne. 0) go to 627
      t = tout
      go to 420
 240   tcrit = rwork(1)
      if ((tn - tcrit)*h .gt. zero) go to 624
 245   hmx = abs(tn) + abs(h)
      ihit = abs(tn - tcrit) .le. 100.0d0*uround*hmx
      if (ihit) go to 400
      tnext = tn + h*(one + four*uround)
      if ((tnext - tcrit)*h .le. zero) go to 250
      h = (tcrit - tn)*(one - four*uround)
      if (istate .eq. 2) jstart = -2
c-----------------------------------------------------------------------
c block e.
c the next block is normally executed for all calls and contains
c the call to the one-step core integrator stode.
c
c this is a looping point for the integration steps.
c
c first check for too many steps being taken, update ewt (if not at
c start of problem), check for too much accuracy being requested, and
c check for h below the roundoff level in t.
c tolsf is calculated considering all solution vectors.
c-----------------------------------------------------------------------
 250   continue
      if ((nst-nslast) .ge. mxstep) go to 500
      call ewset (nyh, itol, rtol, atol, rwork(lyh), rwork(lewt))
      do 260 i = 1,nyh
        if (rwork(i+lewt-1) .le. zero) go to 510
 260    rwork(i+lewt-1) = one/rwork(i+lewt-1)
 270   tolsf = uround*vnorm (nyh, rwork(lyh), rwork(lewt))
      if (tolsf .le. one) go to 280
      tolsf = tolsf*2.0d0
      if (nst .eq. 0) go to 626
      go to 520
 280   if (addx(tn,h) .ne. tn) go to 290
      nhnil = nhnil + 1
      if (nhnil .gt. mxhnil) go to 290
      call xerr ('ODESSA - warning..internal t (=r1) and h (=r2) are',
     1   101, 1, 0, 0, 0, 0, zero, zero)
      call xerr ('such that in the machine, t + h = t on the next step',
     1   101, 1, 0, 0, 0, 0, zero, zero)
      call xerr ('(h = step size). solver will continue anyway',
     1   101, 1, 0, 0, 0, 2, tn, h)
      if (nhnil .lt. mxhnil) go to 290
      call xerr ('ODESSA - above warning has been issued i1 times.',
     1   102, 1, 0, 0, 0, 0, zero, zero)
      call xerr ('it will not be issued again for this problem',
     1   102, 1, 1, mxhnil, 0, 0, zero,zero)
 290   continue
c-----------------------------------------------------------------------
c     call stode(neq,y,yh,nyh,yh,wm,iwm,ewt,savf,acor,par,nrs,
c    1   f,jac,df,prepj,prepdf,solsy)
c-----------------------------------------------------------------------
      call stode (neq, y, rwork(lyh), nyh, rwork(lyh), rwork(lwm),
     1   iwork(liwm), rwork(lewt), rwork(lsavf), rwork(lacor),
     2   par, iwork(lnrs), f, jac, df, prepj, prepdf, solsy)
      kgo = 1 - kflag
      go to (300, 530, 540, 633), kgo
c-----------------------------------------------------------------------
c block f.
c the following block handles the case of a successful return from the
c core integrator (kflag = 0).  test for stop conditions.
c-----------------------------------------------------------------------
 300   init = 1
      go to (310, 400, 330, 340, 350), itask
c itask = 1.  if tout has been reached, interpolate. -------------------
 310   if ((tn - tout)*h .lt. zero) go to 250
      call intdy (tout, 0, rwork(lyh), nyh, y, iflag)
      t = tout
      go to 420
c itask = 3.  jump to exit if tout was reached. ------------------------
 330   if ((tn - tout)*h .ge. zero) go to 400
      go to 250
c itask = 4.  see if tout or tcrit was reached.  adjust h if necessary.
 340   if ((tn - tout)*h .lt. zero) go to 345
      call intdy (tout, 0, rwork(lyh), nyh, y, iflag)
      t = tout
      go to 420
 345   hmx = abs(tn) + abs(h)
      ihit = abs(tn - tcrit) .le. 100.0d0*uround*hmx
      if (ihit) go to 400
      tnext = tn + h*(one + four*uround)
      if ((tnext - tcrit)*h .le. zero) go to 250
      h = (tcrit - tn)*(one - four*uround)
      jstart = -2
      go to 250
c itask = 5.  see if tcrit was reached and jump to exit. ---------------
 350   hmx = abs(tn) + abs(h)
      ihit = abs(tn - tcrit) .le. 100.0d0*uround*hmx
c-----------------------------------------------------------------------
c block g.
c the following block handles all successful returns from odessa.
c if itask .ne. 1, y is loaded from yh and t is set accordingly.
c istate is set to 2, the illegal input counter is zeroed, and the
c optional outputs are loaded into the work arrays before returning.
c if istate = 1 and tout = t, there is a return with no action taken,
c except that if this has happened repeatedly, the run is terminated.
c-----------------------------------------------------------------------
 400  do 410 i = 1,nyh
 410    y(i) = rwork(i+lyh-1)
      t = tn
      if (itask .ne. 4 .and. itask .ne. 5) go to 420
      if (ihit) t = tcrit
 420   istate = 2
      illin = 0
      rwork(11) = hu
      rwork(12) = h
      rwork(13) = tn
      iwork(11) = nst
      iwork(12) = nfe
      iwork(13) = nje
      iwork(14) = nqu
      iwork(15) = nq
      if (isopt .eq. 0) return
      iwork(19) = ndfe
      iwork(20) = nspe
      return
 430   ntrep = ntrep + 1
      if (ntrep .lt. 5) return
      call xerr ('ODESSA -- repeated calls with istate = 1 and
     1tout = t (=r1)', 301, 1, 0, 0, 0, 1, t, zero)
      go to 800
c-----------------------------------------------------------------------
c block h.
c the following block handles all unsuccessful returns other than
c those for illegal input.  first the error message routine is called.
c if there was an error test or convergence test failure, imxer is set.
c then y is loaded from yh, t is set to tn, and the illegal input
c counter illin is set to 0.  the optional outputs are loaded into
c the work arrays before returning.
c-----------------------------------------------------------------------
c the maximum number of steps was taken before reaching tout. ----------
 500  call xerr ('ODESSA - at current t (=r1), mxstep (=i1) steps',
     1   201, 1, 0, 0, 0, 0, zero, zero)
      call xerr ('taken on this call before reaching tout',
     1   201, 1, 1, mxstep, 0, 1, tn, zero)
      istate = -1
      go to 580
c ewt(i) .le. 0.0 for some i (not at start of problem). ----------------
 510   ewti = rwork(lewt+i-1)
      call xerr ('ODESSA - at t (=r1), ewt(i1) has become r2 .le. 0.',
     1   202, 1, 1, i, 0, 2, tn, ewti)
      istate = -6
      go to 580
c too much accuracy requested for machine precision. -------------------
 520  call xerr ('ODESSA - at t (=r1), too much accuracy requested',
     1  203, 1, 0, 0, 0, 0, zero,zero)
      call xerr ('for precision of machine..  see tolsf (=r2)',
     1  203, 1, 0, 0, 0, 2, tn, tolsf)
      rwork(14) = tolsf
      istate = -2
      go to 580
c kflag = -1.  error test failed repeatedly or with abs(h) = hmin. -----
 530  call xerr ('ODESSA - at t(=r1) and step size h(=r2), the error',
     1  204, 1, 0, 0, 0, 0, zero, zero)
      call xerr ('test failed repeatedly or with abs(h) = hmin',
     1  204, 1, 0, 0, 0, 2, tn, h)
      istate = -4
      go to 560
c kflag = -2.  convergence failed repeatedly or with abs(h) = hmin. ----
 540  call xerr ('ODESSA - at t (=r1) and step size h (=r2), the',
     1  205, 1, 0, 0, 0, 0, zero,zero)
      call xerr ('corrector convergence failed repeatedly',
     1   205, 1, 0, 0, 0, 0, zero,zero)
      call xerr ('or with abs(h) = hmin',
     1   205, 1, 0, 0, 0, 2, tn, h)
      istate = -5
c compute imxer if relevant. -------------------------------------------
 560   big = zero
      imxer = 1
      do 570 i = 1,nyh
        size = abs(rwork(i+lacor-1)*rwork(i+lewt-1))
        if (big .ge. size) go to 570
        big = size
        imxer = i
 570    continue
      iwork(16) = imxer
c set y vector, t, illin, and optional outputs. ------------------------
 580   do 590 i = 1,nyh
 590    y(i) = rwork(i+lyh-1)
      t = tn
      illin = 0
      rwork(11) = hu
      rwork(12) = h
      rwork(13) = tn
      iwork(11) = nst
      iwork(12) = nfe
      iwork(13) = nje
      iwork(14) = nqu
      iwork(15) = nq
      if (isopt .eq. 0) return
      iwork(19) = ndfe
      iwork(20) = nspe
      return
c-----------------------------------------------------------------------
c block i.
c the following block handles all error returns due to illegal input
c (istate = -3), as detected before calling the core integrator.
c first the error message routine is called.  then if there have been
c 5 consecutive such returns just before this call to the solver,
c the run is halted.
c-----------------------------------------------------------------------
 601   call xerr ('ODESSA - istate (=i1) illegal',
     1  1, 1, 1, istate, 0, 0, zero,zero)
      go to 700
 602   call xerr ('ODESSA - itask (=i1) illegal',
     1  2, 1, 1, itask, 0, 0, zero,zero)
      go to 700
 603  call xerr ('ODESSA - istate .gt. 1 but odessa not initialized',
     1  3, 1, 0, 0, 0, 0, zero,zero)
      go to 700
 604   call xerr ('ODESSA - neq (=i1) .lt. 1',
     1  4, 1, 1, neq(1), 0, 0, zero,zero)
      go to 700
 605  call xerr ('ODESSA - istate = 3 and neq changed.  (i1 to i2)',
     1  5, 1, 2, n, neq(1), 0, zero,zero)
      go to 700
 606   call xerr ('ODESSA - itol (=i1) illegal',
     1  6, 1, 1, itol, 0, 0, zero,zero)
      go to 700
 607   call xerr ('ODESSA - iopt (=i1) illegal',
     1   7, 1, 1, iopt, 0, 0, zero,zero)
      go to 700
 608   call xerr('ODESSA - mf (=i1) illegal',
     1   8, 1, 1, mf, 0, 0, zero,zero)
      go to 700
 609  call xerr('ODESSA - ml (=i1) illegal.. .lt.0 or .ge.neq (=i2)',
     1   9, 1, 2, ml, neq(1), 0, zero,zero)
      go to 700
 610  call xerr('ODESSA - mu (=i1) illegal.. .lt.0 or .ge.neq (=i2)',
     1   10, 1, 2, mu, neq(1), 0, zero,zero)
      go to 700
 611   call xerr('ODESSA - maxord (=i1) .lt. 0',
     1   11, 1, 1, maxord, 0, 0, zero,zero)
      go to 700
 612   call xerr('ODESSA - mxstep (=i1) .lt. 0',
     1   12, 1, 1, mxstep, 0, 0, zero,zero)
      go to 700
 613   call xerr('ODESSA - mxhnil (=i1) .lt. 0',
     1   13, 1, 1, mxhnil, 0, 0, zero,zero)
      go to 700
 614   call xerr('ODESSA - tout (=r1) behind t (=r2)',
     1   14, 1, 0, 0, 0, 2, tout, t)
      call xerr('integration direction is given by h0 (=r1)',
     1   14, 1, 0, 0, 0, 1, h0, zero)
      go to 700
 615   call xerr('ODESSA - hmax (=r1) .lt. 0.0',
     1   15, 1, 0, 0, 0, 1, hmax, zero)
      go to 700
 616   call xerr('ODESSA - hmin (=r1) .lt. 0.0',
     1   16, 1, 0, 0, 0, 1, hmin, zero)
      go to 700
 617  call xerr('ODESSA - rwork length needed, lenrw (=i1), exceeds
     1 lrw (=i2)', 17, 1, 2, lenrw, lrw, 0, zero,zero)
      go to 700
 618  call xerr('ODESSA - iwork length needed, leniw (=i1), exceeds
     1 liw (=i2)', 18, 1, 2, leniw, liw, 0, zero,zero)
      go to 700
 619   call xerr('ODESSA - rtol(i1) is r1 .lt. 0.0',
     1   19, 1, 1, i, 0, 1, rtoli, zreo)
      go to 700
 620   call xerr('ODESSA - atol(i1) is r1 .lt. 0.0',
     1   20, 1, 1, i, 0, 1, atoli, zero)
      go to 700
*
 621   ewti = rwork(lewt+i-1)
      call xerr('ODESSA - ewt(i1) is r1 .le. 0.0',
     1   21, 1, 1, i, 0, 1, ewti, zero)
      go to 700
 622  call xerr('ODESSA - tout (=r1) too close to t(=r2) to start
     1 integration', 22, 1, 0, 0, 0, 2, tout, t)
      go to 700
 623  call xerr('ODESSA - itask = i1 and tout (=r1) behind tcur - hu
     1 (= r2)', 23, 1, 1, itask, 0, 2, tout, tp)
      go to 700
 624  call xerr('ODESSA - itask = 4 or 5 and tcrit (=r1) behind tcur
     1 (=r2)', 24, 1, 0, 0, 0, 2, tcrit, tn)
      go to 700
 625   call xerr('ODESSA - itask = 4 or 5 and tcrit (=r1) behind tout
     1 (=r2)', 25, 1, 0, 0, 0, 2, tcrit, tout)
      go to 700
 626  call xerr('ODESSA - at start of problem, too much accuracy',
     1   26, 1, 0, 0, 0, 0, zero,zero)
      call xerr('requested for precision of machine. see tolsf (=r1)',
     1   26, 1, 0, 0, 0, 1, tolsf, zero)
      rwork(14) = tolsf
      go to 700
 627  call xerr('ODESSA - trouble from intdy. itask = i1, tout = r1',
     1   27, 1, 1, itask, 0, 1, tout, zero)
      go to 700
c error statements associated with sensitivity analysis.
 628  call xerr('ODESSA - npar (=i1) .lt. 1',
     1   28, 1, 1, npar, 0, 0, zero,zero)
      go to 700
 629  call xerr('ODESSA - istate = 3 and npar changed (i1 to i2)',
     1   29, 1, 2, np, npar, 0, zero,zero)
      go to 700
 630  call xerr('ODESSA - miter (=i1) illegal',
     1   30, 1, 1, miter, 0, 0, zero,zero)
      go to 700
 631  call xerr('ODESSA - trouble in sprime (ierpj)',
     1   31, 1, 0, 0, 0, 0, zero,zero)
      go to 700
 632  call xerr('ODESSA - trouble in sprime (miter)',
     1   32, 1, 0, 0, 0, 0, zero,zero)
      go to 700
 633  call xerr('ODESSA - fatal error in stode (kflag = -3)',
     1   33, 2, 0, 0, 0, 0, zero,zero)
      go to 801
c
 700   if (illin .eq. 5) go to 710
      illin = illin + 1
      istate = -3
      return
 710  call xerr('ODESSA - repeated occurrences of illegal input',
     1   302, 1, 0, 0, 0, 0, zero,zero)
c
 800  call xerr('ODESSA - run aborted.. apparent infinite loop',
     1   303, 2, 0, 0, 0, 0, zero,zero)
      return
 801  call xerr('ODESSA - run aborted',
     1   304, 2, 0, 0, 0, 0, zero,zero)
      return
      END
c-----------------------------------------------------------------------
