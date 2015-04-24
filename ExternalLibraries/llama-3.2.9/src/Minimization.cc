#include "Minimization.h"

using namespace Llama;

namespace Llama {

  static Real nmtry (Matrix<Real> *p, Matrix<Real> *y, Matrix<Real> *psum, 
		     Real (*funk)(const CAbstMatrix<Real>&), 
		     const Index ihi, const Real fac)
  {
    try {

      Index ndim = p->rows();
      Real fac1, fac2, ytry = 0;
      Matrix<Real> ptry;

      fac1 = (1.0 - fac) / ndim;
      fac2 = fac1 - fac;

      ptry = (*p)(Range(), ihi);
      ptry *= -fac2;
      ptry += fac1 * (*psum);

      ytry = (*funk)(ptry);

      if (ytry < (*y)(ihi)) {
	(*y)(ihi) = ytry;
	*psum += ptry;
	*psum -= (*p)(Range(), ihi);
	(*p)(Range(), ihi) = ptry;
      }
	
      return ytry;

    } catch (Exception e) {
      std::cerr << e;
      throw Exception("nmtry");
    }

  }


  inline void 
  rowsum (const Matrix<Real>& x, Matrix<Real> *s)
  {
    for (Index k = 0; k < x.rows(); k++) {
      (*s)(k) = 0.0;
      for (Index j = 0; j < x.cols(); j++) (*s)(k) += x(k,j);
    }
  }

  Index neldermead (Matrix<Real> *p, Matrix<Real> *y, 
		    Real (*funk)(const CAbstMatrix<Real>&), 
		    Real ftol, Index nmax)
  {

    Index nfunk = 0;

    try {

      Index i, ihi, ilo, inhi, ndim = p->rows(), mpts = p->cols();
      Real rtol, ysave, ytry;
      Matrix<Real> psum(ndim);

      if (mpts != (ndim + 1))
	throw DimError("Nelder-Mead minimization");

      rowsum(*p, &psum);

      for (;;) {
	ilo = 0;
	ihi = ((*y)(0) > (*y)(1)) ? (inhi = 1, 0) : (inhi = 0, 1);
	for (i = 0; i < mpts; i++) {
	  if ((*y)(i) <= (*y)(ilo)) ilo = i;
	  if ((*y)(i) > (*y)(ihi)) {
	    inhi = ihi;
	    ihi = i;
	  } else if ((*y)(i) > (*y)(inhi) && i != ihi) inhi = i;
	}

	rtol = 2.0 * abs((*y)(ihi) - (*y)(ilo)) / (abs((*y)(ihi)) + abs((*y)(ilo)));

	if (rtol < ftol) {	// Successful exit

	  swap((*y)(0), (*y)(ilo));
	  for (i = 0; i < ndim; i++) {
	    swap((*p)(i,0), (*p)(i,ilo));
	  }
	  break;

	}

	if (nfunk >= nmax) {
	  std::ostringstream err;
	  err << "maximum number of function evaluations (" << nmax << ") exceeded";
	  throw Exception("Nelder-Mead minimization", err.str());
	}

	// Reflect the simplex from the high point
	ytry = nmtry(p, y, &psum, funk, ihi, -1.0);
	nfunk++;

	if (ytry <= (*y)(ilo)) {

	  // If this works, try an additional extrapolation
	  ytry = nmtry(p, y, &psum, funk, ihi, 2.0);
	  nfunk++;

	} else if (ytry >= (*y)(inhi)) {

	  // Else, do a one-dimensional contraction
	  ysave = (*y)(ihi);
	  ytry = nmtry(p, y, &psum, funk, ihi, 0.5);
	  nfunk++;

	  if (ytry >= ysave) {
	    // Cannot seem to get rid of the high point, contract about the low point
	    for (i = 0; i < mpts; i++) {
	      if (i != ilo) {
		for (Index j = 0; j < ndim; j++)
		  psum(j) = 0.5 * ((*p)(j,ilo) + (*p)(j,i));
		(*p)(Range(),i) = psum;
		(*y)(i)=(*funk)(psum);
	      }
	    }
	    nfunk += ndim;
	    rowsum(*p, &psum);
	  }
	}
      }

      return nfunk;

    } catch (Exception e) {
      std::cerr << e;
      throw Exception("Nelder-Mead minimization");
    }

  }

  Index minimize (const Matrix<Real>& x, 
		  Real (*funk)(const CAbstMatrix<Real>&), 
		  Real ftol, Index nmax, Matrix<Real> *ans)
  {
    try {
      Index nfunk = 0;
      Matrix<Real> xx = x, y(x.cols());

      for (Index k = 0; k < x.cols(); k++)
	y(k) = (*funk)(xx(Range(), k));

      nfunk = neldermead(&xx, &y, funk, ftol, nmax);
      *ans = xx(Range(), 0);

      return nfunk;

    } catch (Exception e) {
      std::cerr << e;
      throw Exception("minimize");
    }

  }

}
