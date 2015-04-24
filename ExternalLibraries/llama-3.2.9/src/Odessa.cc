#include "Odessa.h"
#include "f77fun.h"
#include "machinu.h"

using namespace Llama;

namespace Llama {

  extern "C" {
    struct {		// global resource acquired upon construction of an odessa object
      int lock;
      void (*f)(double, const double *, const double *, double *);
      void (*j)(double, const double *, const double *, double *, Index);
    } _odessa_lockbox = {0, 0, 0};
  }

  static void 
  frhs (int *n, double *t, double *x, double *p, double *dxdt) 
  {
    (*(_odessa_lockbox.f))(*t, x, p + *n, dxdt);
  }
			
  static void 
  finhom (int *n, double *t, double *x, double *p, double *dfdp, int *j) {
    if (*j > *n) (*(_odessa_lockbox.j))(*t, x, p + *n, dfdp, *j - 1);
  };

  static void 
  fjac (int *n, double *t, double *x, double *p, int *ml, int *mu, 
	double *dfdx, int *nr) {
    p += *n;
    for (int j = 0; j < *n; j++) {
      (*(_odessa_lockbox.j))(*t, x, p, dfdx, j);
      dfdx += *nr;
    }
  }

  void
  Odessa::_odessa_workspace (void) 
  {
    _odessa_workspace(True);
  }

  void 
  Odessa::_odessa_workspace (Boolean sens_anal) 
  {

    int nyh, maxord, lwm = 0;
    maxord = ((meth == ADAMS) ? 12 : 5);
    nyh = ((sens_anal) ? (N * (N + P + 1)) : N);

    switch (miter) {
    case FUNCTIONAL:
      lwm = 0;
      liw = 20;
      break;
    case NEWTON:
      lwm = N * N + 2;
      liw = ((sens_anal) ? (21 + 2 * N + P) : (20 + N));
      break;
    }

    lrw = 20 + nyh * (maxord + 1) + 2 * nyh + lwm + N;

    iwork = new int[liw];
    rwork = new double[lrw];

    for (Index k = 4; k < 10; k++) {
      iwork[k] = 0;
      rwork[k] = 0.0;
    }
 
    iwork[5] = mx;
    iwork[6] = mxhnil;
    rwork[4] = 0.0;
    rwork[5] = hmax;
    rwork[6] = hmin;

  }

  void 
  Odessa::_odessa_twiddles (Boolean sens_anal, Boolean all_pars) 
  {

    // set the method flag
    mf = ((meth == GEAR) ? 20 : 10);
    mf += ((miter == FUNCTIONAL) ? 0 : ((have_jac) ? 1 : 2));

    // do we have optional inputs? (yes)
    iopt[0] = 1;
    // are we performing a sensitivity analysis?
    iopt[1] = (sens_anal) ? 1 : 0;
    // do we have an explicit jacobian?
    iopt[2] = (have_jac) ? 1 : 0;

    // set the dimensions of the problem for odessa
    // number of dimensions of the vectorfield
    n[0] = N;	
    // do we want the whole jacobian or
    // just the derivatives with respect
    // to the initial conditions?
    n[1] = (all_pars) ? N + P : N;

  }

  // Basic constructor
  Odessa::Odessa (odessa_func_t f, Index n, Index p, odessa_jac_t j, Method me, Miter mi) 
    : N(n), P(p), funk(f), jack(j), meth(me), miter(mi) {
  
    have_jac = (j != 0);	// do we have an explicit jacobian?

				// acquire global resources
    if (_odessa_lockbox.lock) 
      throw Exception("odessa", "resource locked");
    _odessa_lockbox.f = funk;
    _odessa_lockbox.j = (have_jac) ? jack : 0;

    // set some default values
    mx  = 10000;		// maximum number of integration steps per reporting step
    mxhnil =  1;		// maximum number of hnil messages printed

    itol = 1;			// double tolerances

    rtol = 1e-10;		// relative error tolerance
    atol = 1e-10;		// absolute error tolerance

    hmax = 0.0;			// maximum integrator step size
    hmin = 0.0;			// minimum integrator step size
 
    itask = 1;			// plain old integration is our task
    istate = 1;			// for initial call to odessa

    _odessa_workspace();	// get some workspace
    
    use_disposal = False;	// for later
    disposal = 0;

    // space to put the integrated variables
    y = new Matrix<double>(N, N + P + 1);

  }

  // Initialize for straight integration
  // (no sensitivity analysis)
  void 
  Odessa::initialize (const Matrix<double>& x) 
  {
    _odessa_twiddles(False, False);
    for (Index k = 0; k < N; k++)
      (*y)(k, 0) = x(k);
    p = x;
  }

  // Initialize the odessa object for
  // integration with simultaneous
  // sensitivity analysis
  void 
  Odessa::initialize (const Matrix<double>& x, const Matrix<double>& df) 
  {

    Boolean all_pars = (df.cols() == N + P);

    if (!all_pars && (df.cols() != N))
      throw DimError("odessa initialize");

    _odessa_twiddles(True, all_pars);

    for (Index k = 0; k < N; k++)
      (*y)(k, 0) = x(k);

    if (all_pars) {
      (*y)(Range(), Range(1, N + P)) = df;
    } else {
      (*y)(Range(), Range(1, N)) = df;
    }

    p = x;

  }

  // Destructor
  Odessa::~Odessa (void) 
  { 
    delete y;
    delete[] rwork;
    delete[] iwork;
    // release global resources
    _odessa_lockbox.f = 0;
    _odessa_lockbox.j = 0;
    _odessa_lockbox.lock = 0;

  }

  void 
  Odessa::integrate (double *t, double tout) 
  {

    odessa_(frhs, finhom, n, y->data(), p.data(), t, &tout, 
	    &itol, &rtol, &atol, &itask, &istate, iopt, 
	    rwork, &lrw, iwork, &liw, fjac, &mf);

    if (istate != 2) throw Exception("odessa");

  }

  void
  Odessa::solution (Matrix<double> *s) 
  {
    if (s->size() < N) 
      throw DimError("odessa solution");
    (*s)(Range(0,N-1), 0) = (*y)(Range(), 0);
  }

  void 
  Odessa::solution (Submatrix<double> s) 
  {
    if (s.size() < N)
      throw DimError("odessa solution");
    for (Index k = 0; k < N; k++) 
      s(k) = (*y)(k, 0);
  }

  void
  Odessa::floquet (Matrix<double> *df) 
  {
    (*df)(Range(0,N-1), Range(0,N-1)) = (*y)(Range(), Range(1,N));
  }

  void
  Odessa::sensitivity (Matrix<double> *df) 
  {
    (*df)(Range(0,N-1), Range(0,N+P-1)) = (*y)(Range(), Range(1,N+P));
  }

}
