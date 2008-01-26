// -*- C++ -*-

#ifndef   _LLAMA_DIFFEOMORPHISM_H_
#define   _LLAMA_DIFFEOMORPHISM_H_

#include "Decls.h"
#include "General.h"
#include "Exception.h"
#include "Matrix.h"
#include "AbstDiffeomorphism.h"
#include "Range.h"
#include "Submatrix.h"
#include "SMClosure.h"
#include "MMClosure.h"

namespace Llama {

  /**
     Diffeomorphism class.  Implements a C^1 mapping from R^M to R^N.
  */
  template <class Scalar>
  class Diffeomorphism : public AbstDiffeomorphism<Scalar> {

  public:

    typedef void (*diffeo_fcn)(const Scalar *, Scalar *);
    typedef void (*diffeo_jac)(const Scalar *, Scalar *, Index);

  private:

    // Implementation
    std::string _what;
    Index _m, _n;
    diffeo_fcn _f;
    diffeo_jac _jac;

  public:

    /**
       Construct a new diffeomorphism object.  The arguments are:
       1) a string descriptive of the diffeomorphism,
       2) the routine which implements the diffeomorphism,
       3) (optional) a routine which calculates the jacobian of the diffeomorphism,
    */
    Diffeomorphism (const std::string& what, Index M, Index N, diffeo_fcn f, diffeo_jac j = 0) 
      : _what(what), _m(M), _n(N), _f(f), _jac(j) { };

    /**
       Copy constructor
    */
    Diffeomorphism (const Diffeomorphism& m) 
      : _what(m.what()), _m(m._m), _n(m._n), _f(m._f), _jac(m._jac) { };

    /**
       Copy a Diffeomorphism object.
    */

    Diffeomorphism& operator = (const Diffeomorphism& x) {
      _what = x.what();
      _m = x.nvar();
      _n = x.neqs();
      _f = x.func();
      _jac = x.jacobian();
      return *this;
    };

    // Destructor
    virtual ~Diffeomorphism (void) { };

    /**
       Information about the diffeomorphism.
    */
    const std::string& what (void) const {
      return this->_what;
    };

    /**
       Dimension of source space.
    */
    virtual const Index nvar (void) const {
      return this->_m;
    };

    /**
       Dimension of target space.
    */
    virtual const Index neqs (void) const {
      return this->_n;
    };

    /**
       Return a pointer to the function implementing the diffeomorphism.
       This function is of type void (*)(Scalar, const Scalar *, Scalar *) */
    virtual diffeo_fcn func (void) const {
      return _f;
    };

    /**
       Return a pointer to the function implementing the jacobian of the
       diffeomorphism.  This function is of type 
       void (*)(Scalar, const Scalar *, Scalar *, Index) */
    virtual diffeo_jac jacobian (void) const {
      if (_jac == 0) 
	throw LogicError("diffeomorphism jacobian", "no jacobian defined");
      return _jac;
    };

    /**
       Evaluate the diffeomorphism at point x.  Store the result in y.
       It is assumed that x & y are of the appropriate size.  */
    virtual void operator () (Matrix<Scalar> *y, const Matrix<Scalar>& x) const {
      (*_f)(x.data(), y->data());
    }

    /**
       Evaluate the diffeomorphism at x.  Create a new vector to hold
       the result.  */
    virtual Matrix<Scalar> * operator () (const Matrix<Scalar>& x) const {
      Matrix<Scalar> *y = new Matrix<Scalar>(this->neqs());
      (*this)(y, x);
      return y;
    };

    /**
       Evaluate the jacobian of the diffeomorphism.  Store the result in
       matrix df (assumed to be of the appropriate size and shape.  */
    virtual void operator () (Matrix<Scalar> *df, const Matrix<Scalar>& x, 
			      const Matrix<Index>& i) const {
      try {
	for (Index j = 0; j < i.size(); j++)
	  (*_jac)(x.data(), &(*df)(0,j), i(j));
      } catch (Exception e) {
	std::cerr << e;
	throw Exception("diffeomorphism jacobian evaluation");
      }
    };

    /**
       Evaluate the jacobian of the diffeomorphism.  Create a new matrix
       to hold the result.  */
    virtual Matrix<Scalar> * operator () (const Matrix<Scalar>& x, 
					  const Matrix<Index>& i) const {
      Matrix<Scalar> *df = new Matrix<Scalar>(this->neqs(), i.size());
      (*this)(df, x, i);
      return df;
    };

  };

}

#endif
