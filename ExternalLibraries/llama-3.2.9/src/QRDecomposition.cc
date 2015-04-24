#include "QRDecomposition.h"

using namespace Llama;

namespace Llama {

  QRDecomposition<Real>::QRDecomposition (const Matrix<Real>& a)
  {

    try {
      int m = a.rows(), n = a.cols(), lda = a.rows(), k = min(a.rows(),a.cols()), 
	info = 0, lwork = -1;
      Real *work, query = 0;

      try {
	mat = new Matrix<Real>(a);
	tau = new Real[k];
      } catch (std::bad_alloc) {
	throw OutOfMemory();
      }

      F77_FUN(dgeqrf)(&m, &n, mat->data(), &lda, tau, &query, &lwork, &info);
      if (info != 0) 
	throw Exception("LAPACK dgeqrf");

      lwork= (int) query;
      try {
	work = new Real[lwork];
      } catch (std::bad_alloc) {
	throw OutOfMemory();
      }

      F77_FUN(dgeqrf)(&m, &n, mat->data(), &lda, tau, work, &lwork, &info);
      if (info != 0) 
	throw Exception("LAPACK dgeqrf");

      delete[] work;
      q_computed = False;
      q = 0;
      r_computed = False;
      r = 0;

    } catch (Exception e) {
      std::cerr << e;
      throw Exception("QR decomposition");
    }  
  
  }

  void
  QRDecomposition<Real>::compute_orth (void)
  {

    try {
      int m = mat->rows(), n = mat->cols(), lda = mat->rows(), k = min(mat->rows(),mat->cols()), 
	info = 0, lwork = -1;
      Real *work, query = 0;

      try {
	q = new Matrix<Real>(*mat);
      } catch (std::bad_alloc) {
	throw OutOfMemory();
      }

      F77_FUN(dorgqr)(&m, &n, &k, q->data(), &lda, tau, &query, &lwork, &info);
      if (info != 0) 
	throw Exception("LAPACK dorgqr");

      lwork= (int) query;
      try {
	work = new Real[lwork];
      } catch (std::bad_alloc) {
	throw OutOfMemory();
      }

      F77_FUN(dorgqr)(&m, &n, &k, q->data(), &lda, tau, work, &lwork, &info);
      if (info != 0) 
	throw Exception("LAPACK dorgqr");

      delete[] work;
      q_computed = True;

    } catch (Exception e) {
      std::cerr << e;
      throw Exception("QR orthogonal matrix computation");
    }  
  
  }

  void
  QRDecomposition<Real>::compute_rect (void)
  {

    try {
      Index m = mat->rows(), n = mat->cols(), k = min(m,n);
      r = new Matrix<Real>(k,n);
      for (Index j = 0; j < n; j++)
	for (Index i = 0; i < k; i++)
	  (*r)(i,j) = ((i <= j) ? (*mat)(i,j) : 0);

      r_computed = True;

    } catch (Exception e) {
      std::cerr << e;
      throw Exception("QR orthogonal matrix computation");
    }  
  
  }

}
