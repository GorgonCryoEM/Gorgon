// -*- C++ -*-

#ifndef _LLAMA_FA_CLOSURE_H_
#define _LLAMA_FA_CLOSURE_H_

#include "Matrix.h"
#include "f77fun.h"

namespace Llama {

  /**
     Function applied elementwise closure.  Applying a function to a
     matrix creates an FAMClosure object to hold pointers to the matrix
     and the function.  The function application is evaluated only when
     the object is assigned to a matrix.  */
  template <class OScalar, class IScalar>
  class FAMClosure : public CAbstMatrix<OScalar> {

  private:

    typedef OScalar (*funk)(IScalar);

    // Implementation
    const Matrix<IScalar> *_v;
    funk _f;

    FAMClosure& operator = (const FAMClosure&);

  public:

    // Default constructor
    FAMClosure (Matrix<IScalar> *v, funk f) {
      _v = v;
      _f = f;
    };

    const Matrix<IScalar> *matrix (void) const {
      return _v;
    };

    funk fun (void) const {
      return _f;
    };

    Index size (void) const {
      return _v->size();
    };

    Index rows (void) const {
      return _v->rows();
    };

    Index cols (void) const {
      return _v->cols();
    };

    // Copy constructor
    FAMClosure (const FAMClosure& x) {
      _v = x.matrix();
      _f = x.fun();
    };

    OScalar operator () (const Index j) const {
      return _f((*_v)(j));
    };

    OScalar operator () (const Index j, const Index k) const {
      return _f((*_v)(j,k));
    };

    friend class Matrix<IScalar>;

  };

  static Real
  __llama_d_exp (Real x)
  {
    return exp(x);
  }

  /**
     Exponential function applied elementwise to a real matrix
  */
  inline FAMClosure<Real, Real>
  exp (Matrix<Real>& x)
  {
    return FAMClosure<Real, Real>(&x, __llama_d_exp);
  }

  static Real
  __llama_d_sqrt (Real x)
  {
    return sqrt(x);
  }

  /**
     Square-root function applied elementwise to a real matrix
  */
  inline FAMClosure<Real, Real>
  sqrt (Matrix<Real>& x)
  {
    return FAMClosure<Real, Real>(&x, __llama_d_sqrt);
  }

  static Real
  __llama_d_abs (Real x)
  {
    return abs(x);
  }

  /**
     Absolute-value function applied elementwise to a real matrix
  */
  inline FAMClosure<Real, Real>
  abs (Matrix<Real>& x)
  {
    return FAMClosure<Real, Real>(&x, __llama_d_abs);
  }

  static Real
  __llama_d_log (Real x)
  {
    return log(x);
  }

  /**
     Logarithm function applied elementwise to a real matrix
  */
  inline FAMClosure<Real, Real>
  log (Matrix<Real>& x)
  {
    return FAMClosure<Real, Real>(&x, __llama_d_log);
  }

  static Real
  __llama_d_sin (Real x)
  {
    return sin(x);
  }

  /**
     Sine function applied elementwise to a real matrix
  */
  inline FAMClosure<Real, Real>
  sin (Matrix<Real>& x)
  {
    return FAMClosure<Real, Real>(&x, __llama_d_sin);
  }

  static Real
  __llama_d_cos (Real x)
  {
    return cos(x);
  }

  /**
     Cosine function applied elementwise to a real matrix
  */
  inline FAMClosure<Real, Real>
  cos (Matrix<Real>& x)
  {
    return FAMClosure<Real, Real>(&x, __llama_d_cos);
  }

  static Real
  __llama_dc_abs (Complex x)
  {
    return abs(x);
  }

  /**
     Absolute-value function applied elementwise to a complex matrix
  */
  inline FAMClosure<Real, Complex>
  abs (Matrix<Complex>& x)
  {
    return FAMClosure<Real, Complex>(&x, __llama_dc_abs);
  }

}

#endif
