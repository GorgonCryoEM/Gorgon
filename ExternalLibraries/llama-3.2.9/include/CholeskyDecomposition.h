// -*- C++ -*-

#ifndef   _LLAMA_CHOLESKY_DECOMPOSITION_H_
#define   _LLAMA_CHOLESKY_DECOMPOSITION_H_

#include "Decls.h"
#include "General.h"
#include "Exception.h"
#include "Matrix.h"
#include "f77fun.h"

namespace Llama {

  /**
     Cholesky decomposition of a real symmetric or complex Hermitian
     matrix, A.  A is assumed symmetric if real and Hermitian if
     complex.  The lower triangular part of A is the only part accessed.  
  */
  template <class Scalar>
  class CholeskyDecomposition {

  private:

    Matrix<Scalar> *A, *L;
    char uplo;

  public:
    // Constructor
    CholeskyDecomposition (const Matrix<Scalar>&);
    // Destructor
    ~CholeskyDecomposition (void) 
    {
      if (L != 0) delete L;
      delete A;
    };

    /**
       The lower triangular factor in the factorization
       A = L . L^t   (A = L . L^h if A is complex).
    */
    const Matrix<Scalar> *factor (void)
    {
      if (L == 0) {
      
	try {
	  L = new Matrix<Scalar>(A->rows(), A->rows());
	} catch (std::bad_alloc) {
	  throw OutOfMemory("Cholesky factor");
	}

	for (Index k = 0; k < L->cols(); k++)
	  for (Index j = 0; j < L->rows(); j++) 
	    (*L)(j,k) = ((j >= k) ? (*A)(j,k) : 0);
    
      }

      return L;

    };

    /**
       Back solve the equation A . X = B, for symmetric (Hermitian) A,
       by the sequence:

       Matrix<Scalar> A(n,n), B(n), X;
       X = B;
       CholeskyDecomposition<Scalar> L(A);
       L.solve(&X);
    */
    void solve (Matrix<Scalar> *);

  };

}

#endif

