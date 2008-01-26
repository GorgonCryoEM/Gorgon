// -*- C++ -*-

#ifndef _LLAMA_GEN_MATRIX_H_
#define _LLAMA_GEN_MATRIX_H_

#include "Matrix.h"
#include "Exception.h"
#include <iostream>

namespace Llama {

  /**
     Trace
  */
  template <class Scalar>
  inline Scalar
  trace (const CAbstMatrix<Scalar>& x)
  {
    Index m = min(x.cols(), x.rows());
    Scalar t(0);
    for (Index k = 0; k < m; k++) t += x(k,k);
    return t;
  }

  /**
     Column-wise sum of matrix entries
  */
  template <class Scalar>
  inline Matrix<Scalar> *
  sum (const CAbstMatrix<Scalar>& x)
  {
    Matrix<Scalar> *s = new Matrix<Scalar>(1,x.cols());
    for (Index k = 0; k < x.cols(); k++) {
      (*s)(k) = 0;
      for (Index j = 0; j < x.rows(); j++) (*s)(k) += x(j,k);
    }
    return s;
  }

  /**
     Column-wise product of matrix entries
  */
  template <class Scalar>
  inline Matrix<Scalar> *
  product (const CAbstMatrix<Scalar>& x)
  {
    Matrix<Scalar> *s = new Matrix<Scalar>(1,x.cols());
    for (Index k = 0; k < x.cols(); k++) {
      (*s)(k) = 1;
      for (Index j = 0; j < x.rows(); j++) (*s)(k) *= x(j,k);
    }
    return s;
  }

  /**
     Column-wise arithmetic mean (average) of matrix entries
  */
  template <class Scalar>
  inline Matrix<Scalar> *
  mean (const CAbstMatrix<Scalar>& x)
  {
    Matrix<Scalar> *s = sum(x);
    for (Index j = 0; j < x.cols(); j++) 
      (*s)(j) /= (Real) x.rows();
    return s;
  }

  /**
     Column-wise consecutive differences of matrix entries.
     The matrix to which the return value points is of the same
     dimensions as the argument.  The first row is unchanged.
     This differs from the convention used by MATLAB and OCTAVE
     and ensures that

     cumsum o diff == diff o cumsum == identity.
  */
  template <class Scalar>
  inline Matrix<Scalar> *
  diff (const CAbstMatrix<Scalar>& x)
  {
    Matrix<Scalar> *d = new Matrix<Scalar>(x);
    for (Index k = 0; k < x.cols(); k++) {
      for (Index j = 1; j < x.rows(); j++)
	(*d)(j,k) -= x(j-1,k);
    }
    return d;
  }

  /**
     Column-wise cumulative sum of matrix entries.  The matrix to which
     the return value points is of the same dimensions as the argument.
     The first row is unchanged.  This differs from the convention
     used by MATLAB and OCTAVE and ensures that

     cumsum o diff == diff o cumsum == identity.  
  */
  template <class Scalar>
  inline Matrix<Scalar> *
  cumsum (const CAbstMatrix<Scalar>& x)
  {
    Matrix<Scalar> *s = new Matrix<Scalar>(x);
    for (Index k = 0; k < x.cols(); k++)
      for (Index j = 1; j < x.rows(); j++)
	(*s)(j,k) += (*s)(j-1,k);
    return s;
  }

  /**
     Hilbert matrix.  */
  class Hilbert : public CAbstMatrix<Real> {

  private:
    Index n;

  public:
    Hilbert (void) : n(0) { };
    Hilbert (Index k = 0) : n(k) { };
    ~Hilbert (void) { };
    Index rows (void) const {
      return n;
    };
    Index cols (void) const {
      return n;
    };
    Index size (void) const {
      return n * n;
    };
    Real operator () (Index j, Index k) const {
      return 1.0 / ((Real) (j + k + 1));
    };
    Real operator () (Index k) const {
      return (*this)(k % n, k / n);
    };
  };

  /**
     Vector of linearly spaced elements
  */

  class linspace : public CAbstMatrix<Real> {

  private:
    Real _a, _da;
    Index _n;

  public:
    /**
       A vector of n elements from a to b.
    */
    linspace (Real a = 0, Real b = 1, Index n = 2) : _a(a), _da((b-a)/(n-1)), _n(n) { };
    /**
       A vector of elements from a to b with spacing da.
    */
    linspace (Real a, Real b, Real da) : _a(a), _da(da) {
      _n = (Index) floor((b - a)/da);
    };
    ~linspace (void) { };
    Index size (void) const { return _n; };
    Index rows (void) const { return _n; };
    Index cols (void) const { return 1; };
    Real operator () (Index k) const {
      return _a + k * _da;
    };
    Real operator () (Index j, Index k) const {
      if (k > 0) 
	throw IndexError("linspace");
      return _a + j * _da;
    };
  };

  class complexify : public CAbstMatrix<Complex> {

  private:

    const CAbstMatrix<Real> *_r, *_i;

    complexify (void);

  public:
    complexify (const Matrix<Real>& r, const Matrix<Real>& i) {
      _r = &r;
      _i = &i;
    };
    complexify (const Matrix<Real>& r) {
      _r = &r;
      _i = 0;
    };
    ~complexify (void) { };
    Index size (void) const { return _r->size(); };
    Index rows (void) const { return _r->rows(); };
    Index cols (void) const { return _r->cols(); };
    Complex operator () (Index j, Index k) const {
      if (_i != 0)
	return Complex((*_r)(j,k), (*_i)(j,k));
      else
	return Complex((*_r)(j,k), 0);
    };
    Complex operator () (Index k) const {
      if (_i != 0)
	return Complex((*_r)(k), (*_i)(k));
      else
	return Complex((*_r)(k), 0);
    };
  };

  /**
     The Schur (elementwise) product.
  */
  template <class Scalar>
  class SchurPClosure : public CAbstMatrix<Scalar> {

  private:
    const CAbstMatrix<Scalar> *_left, *_right;

  public:
    SchurPClosure (const CAbstMatrix<Scalar> *a, const CAbstMatrix<Scalar> *b) 
      : _left(a), _right(b) { 
      if ((a->rows() != b->rows()) || (a->cols() != b->cols()))
	throw DimError("Schur product");
    };
    ~SchurPClosure (void) { };
    Index size (void) const { return _left->size(); };
    Index rows (void) const { return _left->rows(); };
    Index cols (void) const { return _left->cols(); };
    Scalar operator () (Index k) const {
      return (*_left)(k) * (*_right)(k);
    };
    Scalar operator () (Index j, Index k) const {
      return (*_left)(j,k) * (*_right)(j,k);
    };
  };

  /**
     The Schur (elementwise) quotient.
  */
  template <class Scalar>
  class SchurQClosure : public CAbstMatrix<Scalar> {

  private:
    const CAbstMatrix<Scalar> *_dividend, *_divisor;

  public:
    SchurQClosure (const CAbstMatrix<Scalar> *a, const CAbstMatrix<Scalar> *b) 
      : _dividend(a), _divisor(b) { 
      if ((a->rows() != b->rows()) || (a->cols() != b->cols()))
	throw DimError("Schur quotient");
    };
    ~SchurQClosure (void) { };
    Index size (void) const { return _dividend->size(); };
    Index rows (void) const { return _dividend->rows(); };
    Index cols (void) const { return _dividend->cols(); };
    Scalar operator () (Index k) const {
      return (*_dividend)(k) / (*_divisor)(k);
    };
    Scalar operator () (Index j, Index k) const {
      return (*_dividend)(j,k) / (*_divisor)(j,k);
    };
  };

  /**
     Multiply two matrices or vectors element-by-element.
  */
  template <class Scalar>
  inline SchurPClosure<Scalar>
  schur_product (const CAbstMatrix<Scalar>& a, const CAbstMatrix<Scalar>& b)
  {
    return SchurPClosure<Scalar>(&a, &b);
  }

  /**
     Divide two matrices or vectors element-by-element.
  */
  template <class Scalar>
  inline SchurQClosure<Scalar>
  schur_quotient (const CAbstMatrix<Scalar>& dividend, const CAbstMatrix<Scalar>& divisor)
  {
    return SchurQClosure<Scalar>(&dividend, &divisor);
  }

}

#endif
