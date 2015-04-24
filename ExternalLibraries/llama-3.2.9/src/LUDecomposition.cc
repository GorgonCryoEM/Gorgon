#include "LUDecomposition.h"

using namespace Llama;

namespace Llama {

  LUDecomposition<Real>::LUDecomposition (const Matrix<Real>& x) :
    A(x)
  {
    try {

      int m = x.rows(), n = x.cols(), info;
      pivots = new int[(m < n) ? m : n];
      F77_FUN(dgetrf)(&m, &n, A.data(), &m, pivots, &info);
      if (info < 0) {
	delete[] pivots;
	throw Exception("LU decomposition", "illegal input");
      } else if (info > 0) {
	std::cerr << "singular matrix detected in LU Decomposition" 
		  << NewLine 
		  << "pivot " << info << " is zero" << NewLine;
      }

      _comp_condn = False;
      sumnorm = x.norm1();

      _inverse_computed = False;
      inv = 0;

    } catch (Exception e) {
      std::cerr << e;
      throw Exception("LU decomposition");
    }
  }

  void
  LUDecomposition<Real>::solve (Matrix<Real> *x)
  {
    int info = 0, n = A.cols(), lda = A.rows(), 
      nrhs = x->cols(), ldx = x->rows();
    if ((n != lda) || (lda != ldx)) {
      throw DimError("LU solve");
    }

    F77_FUN(dgetrs)("N", &n, &nrhs, A.data(), &lda,
		    pivots, x->data(), &ldx, &info);
    if (info < 0) throw Exception("LU solve", "illegal input");

    return;
  }

  void
  LUDecomposition<Real>::tsolve (Matrix<Real> *x)
  {
    int info = 0, n = A.cols(), lda = A.rows(), 
      nrhs = x->cols(), ldx = x->rows();
    if ((n != lda) || (lda != ldx)) {
      throw DimError("LU transpose-solve");
    }

    F77_FUN(dgetrs)("T", &n, &nrhs, A.data(), &lda,
		    pivots, x->data(), &ldx, &info);
    if (info < 0) throw Exception("LU solve", "illegal input");

    return;
  }

  Real 
  LUDecomposition<Real>::_lu_condition ()
  {
    Real rcond, *work;
    int info = 0, n = A.cols(), lda = A.rows(), *iwork;
  
    try {
      work = new Real[4*n];
      iwork = new int[n];
    } catch (std::bad_alloc) {
      throw OutOfMemory("LU condition");
    }

    F77_FUN(dgecon)("1", &n, A.data(), &lda, &sumnorm, &rcond, 
		    work, iwork, &info);

    if (info < 0) {
      delete[] work;
      delete[] iwork;
      throw Exception("LU condition", "illegal input");
    }

    delete[] work;
    delete[] iwork;
    return rcond;
  }

  void
  LUDecomposition<Real>::compute_inverse (void)
  {

    try {

      inv = new Matrix<Real>(A);
      int n = inv->rows(), lwork = -1, info = 0;
      Real query = 0;

      F77_FUN(dgetri)(&n, inv->data(), &n, pivots, &query, &lwork, &info); // Get optimal workspace size

      if (info < 0) {
	delete inv;
	throw Exception("matrix inverse", "illegal input");
      } else if (info > 0) {
	delete inv;
	throw SingularMatrix("matrix inverse");
      }
  
      lwork = (int) query;
      Real *work;

      try {
	work = new Real[lwork];
      } catch (std::bad_alloc) {
	throw OutOfMemory("matrix inverse");
      }

      F77_FUN(dgetri)(&n, inv->data(), &n, pivots, work, &lwork, &info); // Compute inverse

      delete[] work;

      if (info < 0) {
	delete inv;
	throw Exception("matrix inverse", "illegal input");
      } else if (info > 0) {
	delete inv;
	throw SingularMatrix("matrix inverse");
      }
  
      _inverse_computed = True;

    } catch (Exception e) {
      std::cerr << e;
      throw Exception("matrix inverse");
    }

  }

}
