// -*- C++ -*-

#ifndef   _LLAMA_ABST_DIFFEOMORPHISM_H_
#define   _LLAMA_ABST_DIFFEOMORPHISM_H_

#include "Decls.h"
#include "Matrix.h"

namespace Llama {

  /**
     Abstract diffeomorphism class.  Represents a C^1 mapping from R^M
     to R^N.  */
  template <class Scalar>
  class AbstDiffeomorphism {

  public:

    // Destructor
    virtual ~AbstDiffeomorphism (void) { };

    /**
       How many equations (functions) are there?
    */
    virtual const Index neqs (void) const = 0;

    /**
       How many variables are there?
    */
    virtual const Index nvar (void) const = 0;

    /**
       Evaluate the diffeomorphism at point x.  Store the result in y.
       Assume that x & y are of the appropriate size.  */
    virtual void operator () (Matrix<Scalar> *y, const Matrix<Scalar>& x) const = 0;

    /**
       Evaluate the diffeomorphism at x.  Create a new vector to hold
       the result.  */
    virtual Matrix<Scalar> * operator () (const Matrix<Scalar>& x) const = 0;

    /**
       Evaluate the jacobian of the diffeomorphism.  Store the result in
       matrix df (assumed to be of the appropriate size and shape.  */
    virtual void operator () (Matrix<Scalar> *df, const Matrix<Scalar>& x, 
			      const Matrix<Index>& i) const = 0;

    /**
       Evaluate the jacobian of the diffeomorphism.  Create a new matrix
       to hold the result.  */
    virtual Matrix<Scalar> * operator () (const Matrix<Scalar>& x, 
					  const Matrix<Index>& i) const = 0;

  };

}

#endif
