// -*- C++ -*-

#ifndef   _LLAMA_ABST_VECTORFIELD_H_
#define   _LLAMA_ABST_VECTORFIELD_H_

#include "Decls.h"
#include "Matrix.h"

namespace Llama {

  /**
     Abstract vectorfield class.  Represents a C^1 vectorfield on R^N
     with parameters in R^P */
  template <class Scalar>
  class AbstVectorfield {

  public:

    // Destructor
    virtual ~AbstVectorfield (void) { };

    /**
       How many equations are there?
    */
    virtual Index neqs (void) const = 0;

    /**
       How many variables are there?
    */
    virtual Index nvar (void) const = 0;

    /**
       How many parameters are there?
    */
    virtual Index npar (void) const = 0;

    /**
       Integrate the vectorfield from time t0 to time t.
    */
    virtual void flow (Matrix<Scalar> *y, const Matrix<Scalar>& x, Real t, Real t0 = 0) = 0;

    /**
       Integrate the vectorfield and the equations of first variation.
    */
    virtual void floquet (Matrix<Scalar> *df, Matrix<Scalar> *y, 
			  const Matrix<Scalar>& x, Real t, Real t0 = 0) = 0;

    /**
       Find a singular point, using x as an initial guess.  Allow the
       variables indexed by iact to vary.  */
    virtual void singular_point (Matrix<Scalar> *x, const Matrix<Index>& iact, Real t0 = 0) = 0;

    /**
       Compute an orbit of the flow.  */
    virtual Matrix<Scalar> * orbit (const Matrix<Scalar>& x, const Matrix<Real>& t) = 0;

  };

}

#endif

