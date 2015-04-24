// -*- C++ -*-

#ifndef   _LLAMA_VECTORFIELD_H_
#define   _LLAMA_VECTORFIELD_H_

#include "Decls.h"
#include "General.h"
#include "Exception.h"
#include "Matrix.h"
#include "Range.h"
#include "Submatrix.h"
#include "SMClosure.h"
#include "MMClosure.h"
#include "AbstVectorfield.h"
#include "f77fun.h"
#include "Odessa.h"
#include "machinu.h"

namespace Llama {

  /**
     Vectorfield class.  Implements a vectorfield on R^N with parameters
     in R^P */
  template <class Scalar>
  class Vectorfield : public AbstVectorfield<Scalar> {

  public:

    typedef void (*vectorfield_fcn)(Real, const Scalar *, const Scalar *, Scalar *);
    typedef void (*vectorfield_jac)(Real, const Scalar *, const Scalar *, Scalar *, Index);

  private:

    // implementation
    std::string _what;
    Index _neqs, _npar;
    vectorfield_fcn _f;
    vectorfield_jac _jac;

    struct {			// control parameters for 
      Index njac, nchord;		// Newton-Raphson iterations
      Real tol, mina, safe;
    } _nr_controls;

    struct {			// control parameters for odessa integration
      Index mx, itask, mxhnil; 
      Real rtol, atol, hmax, hmin;
    } _ode_controls;

    // Default constructor
    Vectorfield (void) { };

  public:

    // Basic constructor
    /**
       Construct a new vectorfield.  The arguments are:
       1) a string descriptive of the vectorfield,
       2) the dimension of the space (number of dynamical variables),
       3) the number of parameters,
       4) the routine which implements the vectorfield,
       5) (optional) a routine which calculates the Jacobian of the vectorfield.
    */
    Vectorfield (const std::string& what, Index neq, Index npar, vectorfield_fcn f, vectorfield_jac j = 0) 
      : _what(what), _neqs(neq), _npar(npar), _f(f), _jac(j) {

      // Set default values of control parameters
      _nr_controls.njac   =      20;
      _nr_controls.nchord =      10;
      _nr_controls.tol   = 1.0e-12;
      _nr_controls.mina   = 1.0e-9;
      _nr_controls.safe   = 10.0 * MACH_EPS;

      _ode_controls.mx  = 10000;	// maximum number of integration steps per reporting step
      _ode_controls.mxhnil =  1;	// maximum number of hnil messages printed

      _ode_controls.rtol = 1e-10;	// relative error tolerance
      _ode_controls.atol = 1e-10;	// absolute error tolerance
      _ode_controls.hmax = 0.0;	// maximum integrator step size
      _ode_controls.hmin = MACH_EPS;	// minimum integrator step size
 
    };

    // Copy constructor
    Vectorfield (const Vectorfield& m) 
      : _what(m.what()), _f(m._f), _jac(m._jac) { };

    // Destructor
    virtual ~Vectorfield (void) { };

    virtual const std::string& what (void) const {
      return this->_what;
    };

    virtual Index neqs (void) const {
      return _neqs;
    };

    virtual Index nvar (void) const {
      return _neqs + _npar;
    };

    virtual Index npar (void) const {
      return _npar;
    };

    /**
       Return a pointer to the function implementing the vectorfield itself.
       This function is of type void (*)(Real, const Scalar *, const Scalar *, Scalar *) */
    virtual vectorfield_fcn func (void) const {
      return _f;
    };

    /**
       Return a pointer to the function implementing the jacobian of the
       vectorfield.  This function is of type 
       void (*)(Real, const Scalar *, const Scalar *, Scalar *, Index) */
    virtual vectorfield_jac jacobian (void) const {
      if (_jac == 0) 
	throw LogicError("vectorfield jacobian", "no jacobian defined");
      return _jac;
    };

    /**
       Evaluate the vectofield at point x.  Store the result in y.
       Assume that x & y are of the appropriate size.  */
    virtual void operator () (Matrix<Scalar> *y, const Matrix<Scalar>& x, Real t = 0) const {
      (*_f)(t, x.data(), x.data() + this->neqs(), y->data());
    };

    /**
       Evaluate the vectofield at x.  Create a new vector to hold
       the result.  */
    virtual Matrix<Scalar> * operator () (const Matrix<Scalar>& x, Real t = 0) const {
      Matrix<Scalar> *y = new Matrix<Scalar>(this->neqs());
      (*this)(y, x, t);
      return y;
    };

    /**
       Evaluate the jacobian of the vectofield.  Store the result in
       matrix df (assumed to be of the appropriate size and shape.  */
    virtual void operator () (Matrix<Scalar> *df, const Matrix<Scalar>& x, 
			      const Matrix<Index>& i, Real t = 0) const {
      try {
	vectorfield_jac jac = this->jacobian();
	for (Index j = 0; j < i.size(); j++)
	  (*jac)(t, x.data(), x.data() + this->neqs(), &(*df)(0,j), i(j));
      } catch (Exception e) {
	std::cerr << e;
	throw Exception("vectorfield jacobian evaluation");
      }
    };

    /**
       Evaluate the jacobian of the vectofield.  Create a new matrix
       to hold the result.  */
    virtual Matrix<Scalar> * operator () (const Matrix<Scalar>& x, 
					  const Matrix<Index>& i, 
					  Real t = 0) const {
      Matrix<Scalar> *df = new Matrix<Scalar>(this->neqs(), i.size());
      (*this)(df, x, i, t);
      return df;
    };

    /**
       Integrate the vectorfield from time 0 to time t.  */
    void flow (Matrix<Scalar> *y, const Matrix<Scalar>& x, Real t, Real t0 = 0) {
    
      try {

	Odessa ode(this->func(), this->neqs(), this->npar(), this->jacobian(), ADAMS, FUNCTIONAL);
	Real tt = t0;

	ode.initialize(x);

	ode.integrate(&tt, t);
    
	ode.solution(y);

      } catch (Exception e) {
	std::cerr << e;
	throw Exception("vectorfield flow");
      }

    };

    /**
       Compute an orbit at times t beginning at x.
    */
    virtual Matrix<Scalar> * orbit (const Matrix<Scalar>& x, const Matrix<Real>& t) {
      try {

	if (x.size() != this->nvar()) 
	  throw DimError();

	Odessa ode(this->func(), this->neqs(), this->npar(), this->jacobian(), ADAMS, FUNCTIONAL);
	Real tt = t(0);

	ode.initialize(x);

	Matrix<Real> *y = new Matrix<Scalar>(t.size(), this->neqs());

	ode.solution((*y)(0, Range()));

	for (Index j = 1; j < t.size(); j++) {

	  ode.integrate(&tt, t(j));
    
	  ode.solution((*y)(j, Range()));

	}
  
	return y;

      } catch (Exception e) {
	std::cerr << e;
	throw Exception("vectorfield orbit");
      }

    };

    /**
       Integrate the vectorfield and the equations of first variation.
       Fill df with the Floquet matrix.  */
    void floquet (Matrix<Scalar> *df, Matrix<Scalar> *y, 
		  const Matrix<Scalar>& x, Real t, Real t0 = 0) {

      try {

	Odessa ode(this->func(), this->neqs(), this->npar(), this->jacobian(), ADAMS, NEWTON);
	Real tt = t0;

	*df = 1;

	ode.initialize(x, *df);

	ode.integrate(&tt, t);
    
	ode.solution(y);
	ode.floquet(df);

      } catch (Exception e) {
	std::cerr << e;
	throw Exception("vectorfield floquet");
      }

    };

    /**
       Integrate the vectorfield and the equations of first variation.
       Fill df with the full sensitivity matrix.  */
    void sensitivity (Matrix<Scalar> *df, Matrix<Scalar> *y, 
		      const Matrix<Scalar>& x, Real t, Real t0 = 0) {

      try {

	Odessa ode(this->func(), this->neqs(), this->npar(), this->jacobian(), ADAMS, NEWTON);
	Real tt = t0;

	*df = 1;

	ode.initialize(x, *df);

	ode.integrate(&tt, t);
    
	ode.solution(y);
	ode.sensitivity(df);

      } catch (Exception e) {
	std::cerr << e;
	throw Exception("vectorfield sensitivity");
      }

    };

  private:

    int chord_root_iteration (Matrix<Scalar> *x, const Matrix<Index>& i, Real *a, Real t0 = 0) {
      try {

	Matrix<Scalar> xp = *x, f(this->neqs());

	(*this)(&f, xp, t0);	// evaluate the vectorfield
	Real errf = f.maxnorm();	// compute the error

	Matrix<Scalar> df(this->neqs(),this->neqs());	// evaluate the jacobian
	(*this)(&df, xp, i, t0);

	// LU decomposition
	LUDecomposition<Real> T(df);

	// make sure the problem is well-conditioned
	if (T.rcondn() < _nr_controls.safe)
	  throw SingularMatrix("vectorfield chord-root-iteration", T.rcondn());

	// Now do the chord iterations.
	for (Index k = 1; k <= _nr_controls.nchord; k++) {

	  Real errfs = errf;

	  T.solve(&f);		// backsolve
	  f *= *a;		// adjust by relaxation parameter
	  xp(i,0) -= f;		// update the point

	  (*this)(&f, xp, t0);	// evaluate the vectorfield

	  if ((errf = f.maxnorm()) < _nr_controls.tol) {

	    *x = xp;
	    return k;

	  } else if (errfs <= errf) {

	    *a /= 2.0;
	    if (*a < _nr_controls.mina) return -1;

	  }
	
	}

	*x = xp;
	return 0;

      } catch (Exception e) {

	std::cerr << e;
	throw Exception("chord-root iteration");

      }
  
    };

  public:

    /**
       Find a singular point, using x as an initial guess.  Allow the
       variables indexed by iact to vary.  */
    virtual void singular_point (Matrix<Scalar> *x, const Matrix<Index>& iact, Real t0 = 0) {
      try {  

	if (iact.size() != this->neqs())
	  throw Exception("vectorfield singular-point", 
			  "wrong number of active variables");

	Real a = 1.0;
	int status = 0;

	for (Index count = 0; (status >= 0) && (count < _nr_controls.njac); count++)
	  status = chord_root_iteration (x, iact, &a, t0);

	if (status == 0) 
	  throw Exception("vectorfield singular-point",
			  "excessively many steps");

	if (status < 0) 
	  throw Exception("vectorfield singular-point",
			  "excessively small steps");

      } catch (Exception e) {
	std::cerr << e;
	throw Exception("vectorfield singular-point");
      }

    };

  };

}

#endif
