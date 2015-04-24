// -*- C++ -*-

#ifndef   _LLAMA_LU_DECOMPOSITION_H_
#define   _LLAMA_LU_DECOMPOSITION_H_

#include "Decls.h"
#include "General.h"
#include "Exception.h"
#include "Matrix.h"
#include "f77fun.h"

namespace Llama {

  /**
     LU decomposition (Gaussian elimination) on a matrix.
  */
  template <class Scalar>
  class LUDecomposition {

  private:

    Matrix<Scalar> A;
    int *pivots;
    Boolean _comp_condn;
    Real sumnorm, condn;

    Boolean _inverse_computed;
    Matrix<Scalar> *inv;

    Real _lu_condition (void);

    void compute_inverse (void);

  public:

    LUDecomposition (const Matrix<Scalar>&);

    ~LUDecomposition (void) {
      delete[] pivots;
      if (_inverse_computed) delete inv;
    };

    /**
       Return an estimate of the condition number.
    */
    Real condition (void) {
      if (_comp_condn) {
	return 1.0 / condn;
      } else {
	_comp_condn = True;
	condn = _lu_condition();
	return 1.0 / condn;
      }
    };

    /**
       The estimated reciprocal condition number 
       (compare to machine precision).
    */
    Real rcondn (void) {
      if (_comp_condn) {
	return condn;
      } else {
	_comp_condn = True;
	condn = _lu_condition();
	return condn;
      }
    };

    /**
       Solve the equation A . X = B by the sequence:

       Matrix<Scalar> A(n,n), B(n), X;
       X = B;
       LUDecomposition<Scalar> L(A);
       L.solve(&X);
    */
    void solve (Matrix<Scalar> *);

    /**
       Back solve the equation A^t . X = B by the sequence:

       Matrix<Scalar> A(n,n), B(n), X;
       X = B;
       LUDecomposition<Scalar> L(A);
       L.tsolve(&X);
    */
    void tsolve (Matrix<Scalar> *);

    const Matrix<Scalar> *inverse (void) {
      if (!_inverse_computed) {
	compute_inverse();
      }
      return inv;
    };

  };

}

#endif

