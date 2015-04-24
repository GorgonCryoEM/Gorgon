// -*- C++ -*-

#ifndef   _LLAMA_ABST_MAPPING_H_
#define   _LLAMA_ABST_MAPPING_H_

#include "Decls.h"
#include "Matrix.h"
#include "AbstDiffeomorphism.h"

namespace Llama {

  /**
     Abstract mapping class.  Represents a mapping from R^N x R^P -> R^N */
  template <class Scalar>
  class AbstMapping : public AbstDiffeomorphism<Scalar> {

  public:

    // Destructor
    virtual ~AbstMapping (void) { };

    /**
       How many equations (functions) are there?
       This is equal to the number of state variables.
    */
    virtual const Index neqs (void) const = 0;

    /**
       How many variables are there?
    */
    virtual const Index nvar (void) const = 0;

    /**
       How many parameters are there?  */
    virtual const Index npar (void) const {
      return this->nvar() - this->neqs();
    };

    /**
       Iterate the map forward once */
    virtual void advance (Matrix<Scalar> *x) const = 0;

    /**
       Evaluate the state-variable portion of the Jacobian only */
    virtual void deriv (Matrix<Scalar> *df, const Matrix<Scalar>& x) const = 0;

    /**
       How many times is the mapping to be composed with itself?
    */
    virtual Index itmap (void) const = 0;

    /**
       Set the number of times is the mapping to be composed with
       itself.  
    */
    virtual void itmap (Index i) = 0;

    /**
       Apply the mapping, overwriting the input with the output.
    */
    virtual void operator () (Matrix<Scalar> *x) const = 0;

    /**
       Evaluate the mapping at point x. Store the result in y. Assume
       that x & y are of the appropriate size.  */
    virtual void operator () (Matrix<Scalar> *f, const Matrix<Scalar>& x) const = 0;

    /**
       Evaluate the mapping at x.  Create a new vector to hold the
       result.  */
    virtual Matrix<Scalar> * operator () (const Matrix<Scalar>& x) const = 0;

    /**
       Evaluate the Jacobian of the mapping at x.  Store the result in matrix df.
    */
    virtual void operator () (Matrix<Scalar> *df, const Matrix<Scalar>& x, 
			      const Matrix<Index>& i) const = 0;

    /**
       Evaluate the Jacobian of the mapping.  Create a new matrix
       to hold the result.  
    */
    virtual Matrix<Scalar> * operator () (const Matrix<Scalar>& x, 
					  const Matrix<Index>& i) const = 0;

    /**
       Compute an itinerary of length n beginning at x.
    */
    virtual Matrix<Scalar> * itinerary (const Matrix<Scalar>& x0, Index n) const = 0;

    /**
       Find a fixed point, using x as an initial guess.  Allow the
       variables indexed by iact to vary.  */
    virtual void fixed_point (Matrix<Scalar> *x, const Matrix<Index>& iact) = 0;

  };

}

#endif
