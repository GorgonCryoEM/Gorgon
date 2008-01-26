// -*- C++ -*-

#ifndef _LLAMA_REAL_MATRIX_H_
#define _LLAMA_REAL_MATRIX_H_

#include "Matrix.h"
#include "f77fun.h"

namespace Llama {

  inline void 
  Matrix<Real>::_copy (Real *dest, const Index incd, const Real *src, 
		       const Index incs, const Index size)
  {
    CBLAS_FUN(dcopy)(size, src, incs, dest, incd);
  }

  inline void 
  Matrix<Real>::_axpy (Real *y, const Index incy, const Real a, const Real *x, 
		       const Index incx, const Index size)
  {
    CBLAS_FUN(daxpy)(size, a, x, incx, y, incy);
  }

  inline void
  Matrix<Real>::_scal (Real *x, const Index inc, const Real a, const Index size)
  {
    CBLAS_FUN(dscal)(size, a, x, inc);
  }

  inline void
  Matrix<Real>::_gemm (const Transpose_t lt, const Transpose_t rt, 
		       const Index m, const Index n, const Index k,
		       const Real alpha, 
		       const Real *a, const Index lda,
		       const Real *b, const Index ldb,
		       const Real beta,
		       Real *c, const Index ldc)
  {

    CBLAS_FUN(dgemm)(COLMAJOR, lt, rt, m, n, k, alpha, a, lda, b, ldb, 
		     beta, c, ldc );

  };


  inline Real
  Matrix<Real>::frobnorm (void) const
  {
    char norm = 'F';
    int m = this->rows(), n = this->cols();
    Real t = F77_FUN(dlange)(&norm, &m, &n, this->data(), &m, 0);
    return t;
  }

  inline Real
  Matrix<Real>::norm1 (void) const
  {
    char norm = '1';
    int m = this->rows(), n = this->cols();
    Real t = F77_FUN(dlange)(&norm, &m, &n, this->data(), &m, 0);
    return t;
  }

  inline Real
  Matrix<Real>::norminf (void) const
  {
    try {
      char norm = 'I';
      int m = this->rows(), n = this->cols();
      Real *work = 0, t;
      try {
	work = new Real[m];
      } catch (std::bad_alloc) {
	throw OutOfMemory();
      }
      t = F77_FUN(dlange)(&norm, &m, &n, this->data(), &m, work);
      delete[] work;
      return t;
    } catch (Exception e) {
      std::cerr << e;
      throw Exception("matrix infinity norm");
    }
  }

  inline Real
  Matrix<Real>::norm (void) const
  {
    Real t = CBLAS_FUN(dnrm2)(this->size(), this->data(), 1);
    return t;
  }

  inline Real
  Matrix<Real>::sumnorm (void) const
  {
    Real t = CBLAS_FUN(dasum)(this->size(), this->data(), 1);
    return t;
  }

  /**
     Ordinary inner product with another vector.
  */
  inline Real
  Matrix<Real>::dot (const Matrix<Real>& x) const 
  {
    Real d = 0;
    if (this->size() != x.size())
      throw DimError("real dot product");
    d = CBLAS_FUN(ddot)(this->size(), this->data(), 1, x.data(), 1);
    return d;
  }

  /**
     Construct the companion matrix of a univariate polynomial of degree
     n, the coefficients of which are stored in the matrix a, which is
     in column-vector format and has length n+1.  The polynomial corresponding
     to a is a(0) x^n + a(1) x^(n-1) + ... + a(n-2) x + a(n).
  */
  inline Matrix<Real> *
  companion (const Matrix<Real>& a)
  {
    Index m = a.size(), n = 0;
    while (a(n) == 0) {
      n++;
    }
    m -= n + 1;

    Matrix<Real> *c = new Matrix<Real>(m, m);

    for (Index k = 0; k < c->size(); k++) (*c)(k) = 0;
    for (Index k = 0; k < c->rows()-1; k++) (*c)(k+1, k) = 1.0;

    for (Index k = 0; k < c->cols(); k++) 
      (*c)((Index) 0, k) = - a(k + n + 1) / a(n);

    return c;
  }

}

#endif
