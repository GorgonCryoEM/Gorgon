// -*- C++ -*-

#ifndef   _LLAMA_MINIMIZATION_H_
#define   _LLAMA_MINIMIZATION_H_

#include "Decls.h"
#include "General.h"
#include "Exception.h"
#include "Matrix.h"
#include "SMClosure.h"
#include "MMClosure.h"
#include "f77fun.h"

namespace Llama {

  /**
     Minimize a scalar-valued function of several variables using the
     Nelder-Mead algorithm.  The function to be minimized is f, a
     function of n variables.  On input, x should point to a matrix of
     dimension n x (n+1), the columns of which are co-ordinates of a
     simplex, and y should point to an (n+1) x 1 matrix containing the
     values of f at these points.  On exit, the first column of the
     matrix to which x points contains an approximation of the minimum,
     with (*y)(0) the value of f at that point.  The minimum is computed
     to a fractional tolerance of tol, and no more than nmax function
     evaluations are performed.  The number of function evaluations
     performed is returned.  
  */
  Index neldermead (Matrix<Real> *x, Matrix<Real> *y, Real (*f)(const CAbstMatrix<Real>&), Real tol, Index nmax);

  /**
     Minimize a scalar-valued function of several variables using the
     Nelder-Mead algorithm.  The function to be minimized is f, a
     function of n variables.  x should point to a matrix of dimension n
     x (n+1), the columns of which are co-ordinates of a simplex.  The
     minimum is computed to a fractional tolerance of tol, no more than
     nmax function evaluations are performed, and the approximate
     minimum is returned in the matrix to which min points.  NOTE: min
     must be the address of a valid matrix.  The number of function
     evaluations performed is returned.
  */
  Index minimize (const Matrix<Real>& x, Real (*f)(const CAbstMatrix<Real>&), Real tol, Index nmax, Matrix<Real> *min);

}

#endif
