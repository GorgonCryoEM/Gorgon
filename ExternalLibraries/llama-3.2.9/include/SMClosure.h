// -*- C++ -*-

#ifndef _LLAMA_SMCLOSURE_H_
#define _LLAMA_SMCLOSURE_H_

#include "Decls.h"
#include "General.h"
#include "Exception.h"
#include "Matrix.h"

namespace Llama {

  /**
     Scalar-matrix multiplication closure.
  */

  template <class Scalar>
  class SMClosure {

  private:

    Scalar _a;
    const Matrix<Scalar> *_v;

    Scalar alpha (void) const {
      return _a;
    };

    const Matrix<Scalar> *matrix (void) const {
      return _v;
    };

    Index size (void) const {
      return this->matrix()->size();
    };

    Index rows (void) const {
      return this->matrix()->rows();
    };

    Index cols (void) const {
      return this->matrix()->cols();
    };

    SMClosure (void) : _a(0), _v(0) { };

    SMClosure& operator = (const SMClosure&);

  public:

    SMClosure (const Scalar a, const Matrix<Scalar> *v) : _a(a), _v(v) { };

    SMClosure (const SMClosure& x) : _a(x.alpha()), _v(x.matrix()) { };

    ~SMClosure (void) { };

    friend class Matrix<Scalar>;

  };

  // Rules of scalar/matrix arithmetic

  template <class Scalar>
  inline SMClosure<Scalar>
  operator * (const Matrix<Scalar>& x, const Scalar a)
  {
    return SMClosure<Scalar>(a, &x);
  }

  template <class Scalar>
  inline SMClosure<Scalar>
  operator * (const Scalar a, const Matrix<Scalar>& x)
  {
    return SMClosure<Scalar>(a, &x);
  }

  template <class Scalar>
  inline SMClosure<Scalar>
  operator * (const Scalar a, const SMClosure<Scalar>& x)
  {
    return SMClosure<Scalar>(a * x.alpha(), x.matrix());
  }

  template <class Scalar>
  inline SMClosure<Scalar>
  operator * (const SMClosure<Scalar>& x, const Scalar a)
  {
    return SMClosure<Scalar>(x.alpha() * a, x.matrix());
  }

  inline SMClosure<Complex>
  operator * (const Real a, const Matrix<Complex>& x)
  {
    return SMClosure<Complex>(a, &x);
  }

  inline SMClosure<Complex>
  operator * (const Matrix<Complex>& x, const Real a)
  {
    return SMClosure<Complex>(a, &x);
  }

}

#endif

