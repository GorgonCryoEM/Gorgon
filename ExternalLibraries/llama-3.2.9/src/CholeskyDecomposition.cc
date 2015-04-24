#include "CholeskyDecomposition.h"

using namespace Llama;

namespace Llama {

  CholeskyDecomposition<Real>::CholeskyDecomposition (const Matrix<Real>& x)
  {
    try {

      uplo = 'L';

      L = 0;

      if (x.rows() != x.cols())
	throw Exception("Cholesky decomposition", "nonsquare matrix");

      try {
	A = new Matrix<Real>(x);
      } catch (std::bad_alloc) {
	throw OutOfMemory();
      }

      int n = x.rows(), info = 0;

      F77_FUN(dpotrf)(&uplo, &n, A->data(), &n, &info);

      if (info < 0) {

	throw Exception("Cholesky decomposition", "illegal input");

      } else if (info > 0) {

	std::cerr << "singular matrix detected in Cholesky Decomposition" << NewLine
		  << "leading minor of order " << info 
		  << " is not positive definite" << NewLine;

	throw Exception("Cholesky decomposition", 
			"factorization cannot be completed");

      }

    } catch (Exception e) {
      std::cerr << e;
      throw Exception("Cholesky decomposition");
    }
  }

  void
  CholeskyDecomposition<Real>::solve (Matrix<Real> *x)
  {

    if ((A->cols() != A->rows()) || (A->rows() != x->rows())) {
      throw DimError("Cholesky solve");
    }

    int info = 0, n = A->cols(), lda = A->rows(), 
      nrhs = x->cols(), ldx = x->rows();

    F77_FUN(dpotrs)(&uplo, &n, &nrhs, A->data(), &lda,
		    x->data(), &ldx, &info);

    if (info < 0) 
      throw Exception("Cholesky solve", "illegal input");

    return;
  }

  CholeskyDecomposition<Complex>::CholeskyDecomposition (const Matrix<Complex>& x)
  {
    try {

      uplo = 'L';

      L = 0;

      if (x.rows() != x.cols()) 
	throw Exception("Cholesky decomposition", "nonsquare matrix");

      try {
	A = new Matrix<Complex>(x);
      } catch (std::bad_alloc) {
	throw OutOfMemory();
      }

      int n = x.rows(), info = 0;

      F77_FUN(zpotrf)(&uplo, &n, A->data(), &n, &info);

      if (info < 0) {

	throw Exception("Cholesky decomposition", "illegal input");

      } else if (info > 0) {

	std::cerr << "singular matrix detected in Cholesky Decomposition" << NewLine
		  << "leading minor of order " << info 
		  << " is not positive definite" << NewLine;

	throw Exception("Cholesky decomposition", 
			"factorization cannot be completed");

      }

    } catch (Exception e) {
      std::cerr << e;
      throw Exception("Cholesky decomposition");
    }
  }


  void
  CholeskyDecomposition<Complex>::solve (Matrix<Complex> *x)
  {

    if ((A->cols() != A->rows()) || (A->rows() != x->rows())) {
      throw DimError("Cholesky solve");
    }

    int info = 0, n = A->cols(), lda = A->rows(), 
      nrhs = x->cols(), ldx = x->rows();

    F77_FUN(zpotrs)(&uplo, &n, &nrhs, A->data(), &lda,
		    x->data(), &ldx, &info);

    if (info < 0) 
      throw Exception("Cholesky solve", "illegal input");

    return;
  }

}
