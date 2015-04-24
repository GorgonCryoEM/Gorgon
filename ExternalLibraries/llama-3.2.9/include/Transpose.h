// -*- C++ -*-

#ifndef _LLAMA_TRANSPOSE_H_
#define _LLAMA_TRANSPOSE_H_

#include "Matrix.h"
#include "f77fun.h"

namespace Llama {

  /**
     Matrix transposition closure.  Transposing a matrix creates a Transpose object
     to hold the matrix and the type of transposition.  Transpose objects can be 
     assigned to matrix objects (which involves only a single copy operation), or 
     multiplied by other matrices.
  */
  template <class Scalar>
  class Transpose : public CAbstMatrix<Scalar> {

  private:

    const Matrix<Scalar> *_m;
    Transpose_t _t;
  
    // Basic constructors
    Transpose (void);

    Transpose (const Matrix<Scalar> *m, const Transpose_t t = TRANSPOSE) {
      _m = m;
      _t = t;
    };

    Transpose& operator = (const Transpose&);

  public:

    const Matrix<Scalar> * matrix (void) const {
      return _m;
    };

    Transpose_t trans (void) const {
      return _t;
    };

    Index rows (void) const {
      return _m->cols();
    };

    Index cols (void) const {
      return _m->rows();
    };

    Index size (void) const {
      return _m->size();
    };

    // Copy constructor
    Transpose (const Transpose& x) {
      _m = x.matrix();
      _t = x.trans();
    };

    Scalar operator () (const Index j, const Index k) const {
      if (this->trans() == TRANSPOSE) {
	return (*(this->matrix()))(k,j);
      } else if (this->trans() == CONJTRANSPOSE) {
	return conj((*(this->matrix()))(k,j));
      } else if (this->trans() == NOTRANSPOSE) {
	return (*(this->matrix()))(j,k);
      } else {
	throw Exception("transpose op ()");
      }
    };

    Scalar operator () (const Index k) const {
      return (*this)(k % this->rows(), k / this->rows());
    };

    friend class Matrix<Scalar>;

  };

}

#endif

