// -*- C++ -*-

#ifndef   _LLAMA_QR_DECOMPOSITION_H_
#define   _LLAMA_QR_DECOMPOSITION_H_

#include "Decls.h"
#include "General.h"
#include "Exception.h"
#include "Matrix.h"
#include "f77fun.h"

namespace Llama {

  /**
     QR (orthogonal) decomposition of a matrix.
  */
  template <class Scalar>
  class QRDecomposition {

  private:

    Matrix<Scalar> *mat;
    Scalar *tau;
    Boolean q_computed, r_computed;
    Matrix<Scalar> *q, *r;

    void compute_orth (void);
    void compute_rect (void);

  public:

    QRDecomposition (const Matrix<Scalar>&);

    ~QRDecomposition (void) { 
      delete[] tau;
      delete mat;
      if (q_computed) delete q;
      if (r_computed) delete r;
    };

    /**
       Return a pointer to the Q (orthogonal) matrix of the
       decomposition.  
    */
    const Matrix<Scalar> *Q (void) {
      if (!q_computed) {
	compute_orth();
      }
      return q;
    };

    /**
       Return a pointer to the R (right-upper triangular) matrix of the
       decomposition.  
    */
    const Matrix<Scalar> *R (void) {
      if (!r_computed) {
	compute_rect();
      }
      return r;
    };

    /**
       Solve the equation A . X = B by the sequence:

       Matrix<Scalar> A(n,n), B(n), X;
       X = B;
       QRDecomposition<Scalar> L(A);
       L.solve(&X);
    */
    void solve (Matrix<Scalar> *);

    /**
       Back solve the equation A^t . X = B by the sequence:

       Matrix<Scalar> A(n,n), B(n), X;
       X = B;
       QRDecomposition<Scalar> L(A);
       L.tsolve(&X);
    */
    void tsolve (Matrix<Scalar> *);

  };

}

#endif

