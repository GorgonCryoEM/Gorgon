// -*- C++ -*-

#ifndef _LLAMA_DIAGONAL_H_
#define _LLAMA_DIAGONAL_H_

#include "Decls.h"
#include "AbstMatrix.h"
#include "Matrix.h"
#include "f77fun.h"

#include <iostream>

namespace Llama {

  template <class Scalar>
  class Diagonal : public virtual AbstMatrix<Scalar> {

  private:

    // Implementation
    AbstMatrix<Scalar> *_mat;
    Integer _supd;
    Index _dim;

    // Default constructor
    Diagonal(void);

    AbstMatrix<Scalar> *matrix (void) const {
      return _mat;
    };

    Integer superdiag (void) const {
      return _supd;
    };

  public:

    Index size (void) const {
      return _dim;
    };

    Index rows (void) const {
      return _dim;
    };

    Index cols (void) const {
      return 1;
    };

    Scalar operator () (const Index k) const {
      if (superdiag() == 0) {
	return (*_mat)(k,k);
      } else if (superdiag() > 0) {
	return (*_mat)(k,k+superdiag());
      } else {
	return (*_mat)(k-superdiag(),k);
      }
    };

    Scalar& operator () (const Index k) {
      if (superdiag() == 0) {
	return (*_mat)(k,k);
      } else if (superdiag() > 0) {
	return (*_mat)(k,k+superdiag());
      } else {
	return (*_mat)(k-superdiag(),k);
      }
    };

    Scalar operator () (const Index j, const Index k) const {
      if (k > 0) 
	throw IndexError();
      return (*this)(j);
    };

    Scalar& operator () (const Index j, const Index k) {
      if (k > 0) 
	throw IndexError();
      return (*this)(j);
    };

    ~Diagonal (void) { };

    Diagonal (const Diagonal& x) {
      _mat = x.matrix();
      _supd = x.superdiag();
      _dim = x.size();
    };

    Diagonal (AbstMatrix<Scalar> *x, Integer k = 0) {
      _mat = x;
      _supd = k;
      Integer n = x->cols() - x->rows(), 
	r = x->rows(), c = x->cols();
      if (k > -r && k < c) {
	if (n >= 0) {
	  _dim = (k < 0 || k > n) ? r - abs(k) : r;
	} else {
	  _dim = (k > 0 || k < n) ? c - abs(k) : c;
	}
      } else {
	throw DimError("diagonal extraction");
      }
    };

    Diagonal& operator = (const Diagonal& x) {
      _mat = x.matrix();
      _supd = x.superdiag();
      _dim = x.size();
    };

    Diagonal& operator *= (const Scalar& a) {
      for (Index k = 0; k < size(); k++)
	(*this)(k) *= a;
      return *this;
    };

    Diagonal& operator /= (const Scalar& a) {
      if (a == 0)
	throw DivideError("diagonal /=");
      for (Index k = 0; k < size(); k++)
	(*this)(k) /= a;
      return *this;
    };

    /**
       Assign elements from an abstract matrix.
    */
    Diagonal& operator = (const CAbstMatrix<Scalar>& x) {
      if (this->size() != x.size())
	throw DimError("diagonal =");
      for (Index k = 0; k < x.size(); k++) (*this)(k) += x(k);
      return *this;    
    };

    /**
       Add elements from an abstract matrix.
    */
    Diagonal& operator += (const CAbstMatrix<Scalar>& x) {
      if (this->size() != x.size())
	throw DimError("diagonal +=");
      for (Index k = 0; k < x.size(); k++) (*this)(k) += x(k);
      return *this;    
    };

    /**
       Subtract elements from an abstract matrix.
    */
    Diagonal& operator -= (const CAbstMatrix<Scalar>& x) {
      if (this->size() != x.size())
	throw DimError("diagonal -=");
      for (Index k = 0; k < x.size(); k++) (*this)(k) -= x(k);
      return *this;    
    };
 
    /**
       Binary input.
    */
    Diagonal& operator << (std::istream& i) {
      Matrix<Scalar> v;
      try {
	v << i;
	if (v.size() != this->size())
	  throw DimError();
	for (Index k = 0; k < this->size(); k++)
	  (*this)(k) = v(k);
      } catch (Exception e) {
	std::cerr << e;
	throw Exception("diagonal binary read");
      }
      return *this;
    };

  };

  template <class Scalar>
  class DiagonalMatrix : public virtual CAbstMatrix<Scalar> {

  private:

    // Implementation
    const CAbstMatrix<Scalar> *_vec;

    const CAbstMatrix<Scalar> *vector (void) const {
      return _vec;
    }

    // Default constructor
    DiagonalMatrix (void);
  
  public:

    // Dimensions
    Index size (void) const {
      return _vec->size() * _vec->size();
    };

    Index rows (void) const {
      return _vec->size();
    }

    Index cols (void) const {
      return _vec->size();
    }

    // Elementwise access: matrix format
    Scalar operator () (const Index j, const Index k) const {
      if (j != k) {
	return 0;
      } else {
	return (*_vec)(k);
      }
    };

    Scalar operator () (const Index k) const {
      return (*this)(k % this->rows(), k / this->rows());
    };

    ~DiagonalMatrix (void) { };

    DiagonalMatrix (const DiagonalMatrix& x) {
      _vec = x.vector();
    };

    DiagonalMatrix (const CAbstMatrix<Scalar> *x) {
      _vec = x;
    };

    DiagonalMatrix& operator = (const DiagonalMatrix& x) {
      _vec = x.vector();
    };

  };

  /**
     If x is a vector, then diag(x) is a pointer to a new square matrix
     with x on the diagonal and zeros elsewhere.  
  */
  template <class Scalar>
  inline DiagonalMatrix<Scalar>
  diag (const CAbstMatrix<Scalar>& x)
  {
    return DiagonalMatrix<Scalar>(&x);
  }

}

#endif
