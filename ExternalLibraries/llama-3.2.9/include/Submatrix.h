// -*- C++ -*-

#ifndef   _LLAMA_SUBMATRIX_H_
#define   _LLAMA_SUBMATRIX_H_

#include "Decls.h"
#include "Range.h"
#include "AbstMatrix.h"
#include "Matrix.h"
#include "f77fun.h"

namespace Llama {

  /**
     Submatrix extraction / manipulation closure.
  */
  template <class Scalar>
  class Submatrix : public AbstMatrix<Scalar> {

  private:

    // Implementation
    Matrix<Scalar> *_m;
    Llama::_slice _r, _c;
    // Dimension checker
    void _sizecheck (const Index y, const Index dim, Llama::_slice *s) {
      if (y >= dim)
	throw IndexError("submatrix constructor");
      s->use_slice = True;
      s->start = y;
      s->size = 1;
      s->stride = 0;
      s->indices = 0;
    };

    void _sizecheck (const Range& y, const Index dim, Llama::_slice *s) {
      Integer d = dim;
      if (!y.all() &&
	  (
	   (y.start() < 0) ||
	   (y.start() >= d) ||
	   (y.stop() < 0) ||
	   (y.stop() >= d)
	   )
	  )
	throw IndexError("submatrix constructor");

      s->use_slice = True;
      if (y.all()) {
	s->start = 0;
	s->size = (Index) dim;
	s->stride = 1;
      } else {
	s->start = y.start();
	s->size = y.size();
	s->stride = y.stride();
      }

    };

    void _sizecheck (const Matrix<Index> *y, const Index dim, Llama::_slice *s) {
      for (Index j = 0; j < y->size(); j++)
	if ((*y)(j) >= dim)
	  throw IndexError("submatrix constructor");
    
      s->use_slice = False;
      s->size = y->size();
      s->indices = y;
    };

    Matrix<Scalar> * matrix (void) const {
      return _m;
    };

    Index row (const Index j) const {
      if (_r.use_slice) {
	return _r.start + j * _r.stride;
      } else {
	return (*(_r.indices))(j);
      }
    };

    Index col (const Index j) const {
      if (_c.use_slice) {
	return _c.start + j * _c.stride;
      } else {
	return (*(_c.indices))(j);
      }
    };

    // Basic constructors

    Submatrix (void);

    Submatrix (Matrix<Scalar> *x, const Range& r, const Range& c) {
      _m = x;
      _sizecheck(r, x->rows(), &_r);
      _sizecheck(c, x->cols(), &_c);
    };

    Submatrix (Matrix<Scalar> *x, const Matrix<Index> *r, const Matrix<Index> *c) {
      _m = x;
      _sizecheck(r, x->rows(), &_r);
      _sizecheck(c, x->cols(), &_c);
    };

    Submatrix (Matrix<Scalar> *x, const Range& r, const Matrix<Index> *c) {
      _m = x;
      _sizecheck(r, x->rows(), &_r);
      _sizecheck(c, x->cols(), &_c);
    };

    Submatrix (Matrix<Scalar> *x, const Matrix<Index> *r, const Range& c) {
      _m = x;
      _sizecheck(r, x->rows(), &_r);
      _sizecheck(c, x->cols(), &_c);
    };

    Submatrix (Matrix<Scalar> *x, const Index r, const Range& c) {
      _m = x;
      _sizecheck(r, x->rows(), &_r);
      _sizecheck(c, x->cols(), &_c);
    };

    Submatrix (Matrix<Scalar> *x, const Range& r, const Index c) {
      _m = x;
      _sizecheck(r, x->rows(), &_r);
      _sizecheck(c, x->cols(), &_c);
    };

    Submatrix (Matrix<Scalar> *x, const Index r, const Matrix<Index> *c) {
      _m = x;
      _sizecheck(r, x->rows(), &_r);
      _sizecheck(c, x->cols(), &_c);
    };

    Submatrix (Matrix<Scalar> *x, const Matrix<Index> *r, const Index c) {
      _m = x;
      _sizecheck(r, x->rows(), &_r);
      _sizecheck(c, x->cols(), &_c);
    };

  public:

    // Copy constructor
    Submatrix (const Submatrix& x) {
      _m = x._m;
      _r = x._r;
      _c = x._c;
    };

    // Dimensions
    Index rows (void) const {
      return _r.size;
    };

    Index cols (void) const {
      return _c.size;
    };

    Index size (void) const {
      return this->rows() * this->cols();
    };

    // Access elements of the submatrix
    Scalar operator () (const Index j, const Index k) const {
      return (*(this->matrix()))(this->row(j), this->col(k));
    };

    Scalar& operator () (const Index j, const Index k) {
      return (*(this->matrix()))(this->row(j), this->col(k));
    };

    Scalar& operator () (const Index k) {
      return (*this)(k % this->rows(), k / this->rows());
    };

    Scalar operator () (const Index k) const {
      return (*this)(k % this->rows(), k / this->rows());
    };

    /**
       Assign values from a submatrix to a submatrix.
    */
    Submatrix& operator = (const Submatrix& x) {
      if (
	  (this->rows() != x.rows()) ||
	  (this->cols() != x.cols())
	  )
	throw DimError("submatrix =");
      for (Index k = 0; k < x.cols(); k++)
	for (Index j = 0; j < x.rows(); j++)
	  (*this)(j,k) = x(j,k);

      return *this;
    };

    /**
       Randomize the submatrix.
    */
    Submatrix& operator = (Random & r)
    {
      for (Index j = 0; j < this->rows(); j++) 
	for (Index k = 0; k < this->cols(); k++) 
	  (*this)(j,k) = r();
      return *this;
    };

    /**
       Set equal to a scalar multiple of the identity matrix.  If the
       matrix is nonsquare, only the diagonal elements will be nonzero.  
    */
    Submatrix& operator = (const Scalar& a) {
      for (Index k = 0; k < this->size(); k++) 
	(*this)(k) = 0;
      for (Index k = 0; k < min(this->rows(), this->cols()); k++)
	(*this)(k,k) = a;
      return *this;
    };

    /**
       Add a scalar multiple of the identity matrix
    */
    Submatrix& operator += (const Scalar& a) {
      for (Index k = 0; k < min(this->rows(), this->cols()); k++)
	(*this)(k,k) += a;
      return *this;
    };

    /**
       Subtract a scalar multiple of the identity matrix
    */
    Submatrix& operator -= (const Scalar& a) {
      for (Index k = 0; k < min(this->rows(), this->cols()); k++)
	(*this)(k,k) -= a;
      return *this;
    };

    /**
       Multiply by a scalar.
    */
    Submatrix& operator *= (const Scalar& a) {
      for (Index k = 0; k < this->size(); k++)
	(*this)(k) *= a;
      return *this;
    };

    /**
       Divide by a scalar.
    */
    Submatrix& operator /= (const Scalar& a) {
      if (a == 0) throw DivideError("submatrix /=");
      for (Index k = 0; k < this->size(); k++)
	(*this)(k) /= a;
      return *this;
    };

    Submatrix& operator = (const CAbstMatrix<Scalar>& x) {
      if (
	  (this->rows() != x.rows()) ||
	  (this->cols() != x.cols())
	  )
	throw DimError("submatrix =");
      for (Index k = 0; k < x.size(); k++)
	(*this)(k) = x(k);
      return *this;
    };

    /**
       Add values from an abstract matrix to a submatrix.  
    */
    Submatrix& operator += (const CAbstMatrix<Scalar>& x) {
      if (
	  (this->rows() != x.rows()) ||
	  (this->cols() != x.cols())
	  )
	throw DimError("submatrix +=");
      for (Index k = 0; k < x.size(); k++)
	(*this)(k) += x(k);
      return *this;
    };

    /**
       Subtract values from an abstract matrix from a submatrix.  
    */
    Submatrix& operator -= (const CAbstMatrix<Scalar>& x) {
      if (
	  (this->rows() != x.rows()) ||
	  (this->cols() != x.cols())
	  )
	throw DimError("submatrix -=");
      for (Index k = 0; k < x.size(); k++)
	(*this)(k) -= x(k);
      return *this;
    };

    /**
       Binary input
    */
    Submatrix& operator << (std::istream& i) {
      Matrix<Scalar> v;

      try {

	v << i;

	if (
	    (v.rows() != this->rows()) ||
	    (v.cols() != this->cols())
	    )
	  throw DimError();

	for (Index k = 0; k < this->cols(); k++)
	  for (Index j = 0; j < this->rows(); j++)
	    (*this)(j,k) = v(j,k);

      } catch (Exception e) {
	std::cerr << e;
	throw Exception("submatrix binary read");
      }
      return *this;
    };

    Submatrix<Scalar> t (void) {
      Submatrix<Scalar> x = *this;
      Llama::_slice swap;
      swap = x._r;
      x._r = x._c;
      x._c = swap;
      return x;
    };

    friend class Matrix<Scalar>;

  };

}

#endif
