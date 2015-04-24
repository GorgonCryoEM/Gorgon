// -*- C++ -*-

#ifndef   _LLAMA_ODESSA_H_
#define   _LLAMA_ODESSA_H_

#include "Decls.h"
#include "General.h"
#include "Exception.h"
#include "Matrix.h"
#include "Range.h"
#include "Submatrix.h"

namespace Llama {

  typedef enum {ADAMS, GEAR} Method;
  typedef enum {FUNCTIONAL, NEWTON} Miter;

  /**
     ODESSA interface. */
  class Odessa {

  public:

    typedef void (*disposal_t)(Matrix<double>*);
    typedef void (*odessa_func_t)(double, const double *, const double *, double *);
    typedef void (*odessa_jac_t)(double, const double *, const double *, double *, Index);

  private:

    // implementation

    Index N, P;			// dimensions of the problem (N state variables, P parameters)

    odessa_func_t funk;		// defines the vectorfield
    odessa_jac_t jack;		// defines the derivative of the vectorfield

    Method meth;			// integration method
    Miter miter;			// iteration method

    int n[2], itol;
    double rtol, atol, hmax, hmin;
    int itask, istate, iopt[3], lrw, liw, mf;
    int mx, mxhnil;

    double *rwork;
    int *iwork;

    Matrix<double> p, *y;

    Boolean have_jac;
    Boolean use_disposal;
    disposal_t disposal;

    // Forbidden methods
    Odessa (void) { };
    Odessa (const Odessa&);
    Odessa& operator = (const Odessa&);

    void _odessa_workspace (void);
    void _odessa_workspace (Boolean sens_anal);
    void _odessa_twiddles (Boolean sens_anal, Boolean all_pars);


  public:

    // Basic constructor
    /**
       Construct a new odessa integration object.  */
    Odessa (odessa_func_t f, Index n, Index p, odessa_jac_t j = 0, Method me = GEAR, Miter mi = NEWTON);

    virtual ~Odessa (void);	// Destructor

    /** 
	Initialize for straight integration (no sensitivity analysis) */
    void initialize (const Matrix<double>& x);

    /** 
	Initialize the odessa object for integration with simultaneous
	sensitivity analysis */
    void initialize (const Matrix<double>& x, const Matrix<double>& df);

    /**
       Integrate the initialized vectorfield from t to tout.  */
    void integrate (double *t, double tout);

    /**
       Extract the value of the state vector.  */
    void solution (Matrix<double> *s);

    /**
       Extract the value of the state vector.  */
    void solution (Submatrix<double> s);

    /**
       Extract the Floquet matrix (neqs x neqs) */
    void floquet (Matrix<double> *df);

    /**
       Extract the sensitivity matrix (neqs x nvar) */
    void sensitivity (Matrix<double> *df);

  };

}

#endif
