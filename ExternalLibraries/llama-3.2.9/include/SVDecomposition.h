// -*- C++ -*-

#ifndef   _LLAMA_SV_DECOMPOSITION_H_
#define   _LLAMA_SV_DECOMPOSITION_H_

#include "Decls.h"
#include "General.h"
#include "Exception.h"
#include "Matrix.h"
#include "f77fun.h"

namespace Llama {

  /**
     Compute the singular values without computing the singular vectors.
  */
  Matrix<Real> * singvals (const Matrix<Real>&);

  /**
     Perform singular-value decomposition.  The constructor is a wrapper around the LAPACK
     routine DGEEVX.  The singular values are computed as a vector, the left eigenvectors
     are computed as a matrix, and the right eigenvectors as the Transpose of a matrix.
  */
  template <class Scalar>
  class SVDecomposition {

  private:
    // Implementation
    Matrix<Scalar> *s, *u, *vt;

  public:
    // Constructor
    SVDecomposition (const Matrix<Scalar>&);

    // Destructor
    ~SVDecomposition (void) {
      delete vt;
      delete u;
      delete s;
    };

    /**
       Return a pointer to the matrix of left singular vectors (as columns).
    */
    const Matrix<Scalar> *left_singular_vectors (void) {
      return u;
    };

    /**
       Return a pointer to the matrix of singular values (in vector format).
    */
    const Matrix<Scalar> *singular_values (void) {
      return s;
    };

    /**
       Return a pointer to a matrix containing the 
       right singular vectors (as columns).
    */
    const Matrix<Scalar> *rsvt (void) {
      return vt;
    };

    /**
       Return a Transpose<Scalar> containing the right
       singular vectors (as columns).  
    */
    Transpose<Scalar> right_singular_vectors (void) {
      return vt->t();
    };
  
    /**
       The matrix operator 2-norm.
    */
    Scalar norm2 (void) const {
      return (*s)(0);
    };

    /**
       Return an estimate of the condition number.
    */
    Scalar condition (void) const {
      return (*s)(0) / (*s)(s->size() - 1);
    };

    /**
       The estimated reciprocal condition number 
       (compare to machine precision).
    */
    Scalar rcondn (void) const {
      return (*s)(s->size() - 1) / (*s)(0);
    };

  };

}

#endif
