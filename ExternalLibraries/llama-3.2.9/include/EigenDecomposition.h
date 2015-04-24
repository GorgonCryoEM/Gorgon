// -*- C++ -*-

#ifndef   _LLAMA_EIGEN_DECOMPOSITION_H_
#define   _LLAMA_EIGEN_DECOMPOSITION_H_

#include "Decls.h"
#include "General.h"
#include "Exception.h"
#include "Matrix.h"
#include "f77fun.h"

namespace Llama {

  /**
     Compute the eigenvalues of a real matrix without the
     eigenvectors.
  */
  template <class Scalar>
  Matrix<Complex> * eigenvals (const Matrix<Scalar>&);

  /**
     Compute the full eigenvalue-eigenvector decomposition of 
     a real matrix.  The constructor is basically a wrapper 
     around the LAPACK routine DGEEVX.
  */
  template <class Scalar>
  class EigenDecomposition {

  private:

    Matrix<Complex> *vals, *leftvecs, *rightvecs;
    Matrix<Scalar> *scale, *rconde, *rcondv;
    int ilo, ihi;

  public:
    // Constructor
    EigenDecomposition (Matrix<Scalar>&);

    // Destructor
    ~EigenDecomposition (void)
    {
      delete leftvecs;
      delete rightvecs;
      delete vals;
      delete scale;
      delete rconde;
      delete rcondv;
    };

    /**
       Return a pointer to a matrix (in column-vector format)
       containing the eigenvalues.
    */
    const Matrix<Complex> * eigenvalues (void)
    {
      return this->vals;
    };

    /**
       Return a pointer to a matrix containing the left 
       eigenvectors.
    */
    const Matrix<Complex> * left_eigenvectors (void)
    {
      return this->leftvecs;
    };

    /**
       Return a pointer to a matrix containing the right
       eigenvectors.
    */
    const Matrix<Complex> * right_eigenvectors (void)
    {
      return this->rightvecs;
    };

  };

}

#endif
