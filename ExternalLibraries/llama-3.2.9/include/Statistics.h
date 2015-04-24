// -*- C++ -*-

#ifndef _LLAMA_STATISTICS_H_
#define _LLAMA_STATISTICS_H_

#include "Decls.h"
#include "General.h"
#include "Exception.h"
#include "Matrix.h"

namespace Llama {

  /**
     Bin the data in x into n equal bins.  Return a pointer to an n x 2
     matrix: bin centers are in column 0, corresponding counts in column
     1.  
  */
  inline Matrix<Real> *
  histogram (const CAbstMatrix<Real>& x, const Index n)
  {

    Matrix<Real> *h = new Matrix<Real>(n,2);
    Real xmax = x(0), xmin = x(0), dx;
    Index p;

    for (Index k = 1; k < x.size(); k++) {
      if (x(k) > xmax) xmax = x(k);
      if (x(k) < xmin) xmin = x(k);
    }

    dx = (xmax - xmin) / (n-1);

    for (Index k = 0; k < n; k++) {
      (*h)(k,0) = xmin + k * dx;
      (*h)(k,1) = 0.0;
    }

    xmin -= dx/2;

    for (Index k = 0; k < x.size(); k++) {
      p = (Index) floor((x(k) - xmin) / dx);
      (*h)(p,1) += 1.0;
    }

    return h;

  }

  /**
     Bin the data in x into n equal bins.  Return a pointer to an n x 2
     matrix: bin centers are in column 0, corresponding counts in column
     1.  
  */
  inline Matrix<Real> *
  histogram (const CAbstMatrix<Real>& x, const Index n, Real xmin, Real xmax)
  {

    Matrix<Real> *h = new Matrix<Real>(n,2);
    Index p;
    Real dx = (xmax - xmin) / (n-1);

    for (Index k = 0; k < n; k++) {
      (*h)(k,0) = xmin + k * dx;
      (*h)(k,1) = 0.0;
    }

    xmin -= dx/2;

    for (Index k = 0; k < x.size(); k++) {
      p = (Index) floor((x(k) - xmin) / dx);
      p = max (0, min (n-1, p));
      (*h)(p,1) += 1.0;
    }

    return h;

  }

  /**
     Make a bar chart corresponding to the histogram x.
  */
  inline Matrix<Real> *
  barchart (const CAbstMatrix<Real>& x)
  {

    Index k = 0, n = 3 * x.rows() + 1;
    Matrix<Real> *c = new Matrix<Real>(n,2);

    (*c)(0,0) = (3 * x(0,0) - x(1,0)) / 2;
    (*c)(0,1) = 0;

    for (k = 0; k < x.rows() - 1; k++) {

      (*c)(3*k+1,0) = (*c)(3*k,0);
      (*c)(3*k+2,0) = (x(k+1,0) + x(k,0)) / 2;
      (*c)(3*k+3,0) = (*c)(3*k+2, 0);

      (*c)(3*k+1,1) = x(k,1);
      (*c)(3*k+2,1) = x(k,1);
      (*c)(3*k+3,1) = 0;
    
    }

    k = x.rows() - 1;

    (*c)(3*k+1,0) = (*c)(3*k,0);
    (*c)(3*k+2,0) = (3 * x(k,0) - x(k-1,0)) / 2;
    (*c)(3*k+3,0) = (*c)(3*k+2, 0);

    (*c)(3*k+1,1) = x(k,1);
    (*c)(3*k+2,1) = x(k,1);
    (*c)(3*k+3,1) = 0;

    return c;

  }

}

#endif
