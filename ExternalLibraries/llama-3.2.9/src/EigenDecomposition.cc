#include "EigenDecomposition.h"

using namespace Llama;

namespace Llama {

  static void eigsort (Matrix<Complex> *);
  static void eigsort (Matrix<Complex> *, Matrix<Complex> *, Matrix<Complex> *);

  EigenDecomposition<Real>::EigenDecomposition (Matrix<Real>& x)
  {

    try {

      if (x.cols() != x.rows()) 
	throw Exception("Eigendecomposition", "nonsquare matrix");

      char balanc = 'B', jobvl = 'V', jobvr = 'V', sense = 'B';
      int n = x.cols(), lda = x.rows(), ldvl = n, ldvr = n, 
	lwork = -1, *iwork = 0, info;
      Real *wr, *wi, abnrm, query, *work;
      Matrix<Real> *schur, *vl, *vr;

      try {

	schur = new Matrix<Real>(x);

	scale = new Matrix<Real>(n);
	rconde = new Matrix<Real>(n);
	rcondv = new Matrix<Real>(n);

	vl = new Matrix<Real>(ldvl,n);
	vr = new Matrix<Real>(ldvr,n);
	wr = new Real[n];
	wi = new Real[n];

      } catch (std::bad_alloc) {
	throw OutOfMemory();
      }

      F77_FUN(dgeevx)(&balanc, &jobvl, &jobvr, &sense, &n, schur->data(), &lda, 
		      wr, wi, vl->data(), &ldvl, vr->data(), &ldvr, &ilo, &ihi, 
		      scale->data(), &abnrm, rconde->data(), rcondv->data(), 
		      &query, &lwork, iwork, &info);
      if (info < 0) throw Exception("eigendecomposition", "illegal input");
      if (info > 0) throw Exception("eigendecomposition", "failed to compute all eigenvalues");

      try {

	lwork = (int) query;
	work = new Real[lwork];
	iwork = new int[2 * n - 2];

      } catch (std::bad_alloc) {
	throw OutOfMemory();
      }

      F77_FUN(dgeevx)(&balanc, &jobvl, &jobvr, &sense, &n, schur->data(), &lda, 
		      wr, wi, vl->data(), &ldvl, vr->data(), &ldvr, &ilo, &ihi, 
		      scale->data(), &abnrm, rconde->data(), rcondv->data(), 
		      work, &lwork, iwork, &info);
      if (info < 0) throw Exception("eigendecomposition", "illegal input");
      if (info > 0) throw Exception("eigendecomposition", "failed to compute all eigenvalues");

      delete[] iwork;
      delete[] work;

      try {

	vals = new Matrix<Complex>(n);
	leftvecs = new Matrix<Complex>(n,n);
	rightvecs = new Matrix<Complex>(n,n);

      } catch (std::bad_alloc) {
	throw OutOfMemory();
      }

      Index j, k = 0;
      while (k < x.cols()) {
	if (wi[k] > 0) {
	  (*vals)(k) = Complex(wr[k], wi[k]);
	  (*vals)(k+1) = Complex(wr[k+1], wi[k+1]);
	  for (j = 0; j < x.rows(); j++) {
	    (*leftvecs)(j,k) = Complex((*vl)(j,k), (*vl)(j,k+1));
	    (*rightvecs)(j,k) = Complex((*vr)(j,k), (*vr)(j,k+1));
	    (*leftvecs)(j,k+1) = Complex((*vl)(j,k), -(*vl)(j,k+1));
	    (*rightvecs)(j,k+1) = Complex((*vr)(j,k), -(*vr)(j,k+1));
	  }
	  k += 2;
	} else {
	  (*vals)(k) = Complex(wr[k], wi[k]);
	  for (j = 0; j < x.rows(); j++) {
	    (*leftvecs)(j,k) = (*vl)(j,k);
	    (*rightvecs)(j,k) = (*vr)(j,k);
	  }
	  k++;
	}
      }

      delete[] wi;
      delete[] wr;
      delete vl;
      delete vr;
      delete schur;

      eigsort(vals, leftvecs, rightvecs);

    } catch (Exception e) {
      std::cerr << e;
      throw Exception("EigenDecomposition");
    }

  }

  Matrix<Complex> * eigenvals (const Matrix<Real>& x)
  {

    try {

      if (x.cols() != x.rows()) 
	throw Exception("eigenvals", "nonsquare matrix");

      char balanc = 'B', jobvl = 'N', jobvr = 'N', sense = 'N';
      int n = x.cols(), lda = x.rows(), ldvl = 1, ldvr = 1, ilo, ihi, 
	lwork = -1, *iwork = 0, info;
      Real *wr, *wi, *vl = 0, *vr = 0, *scale, abnrm, *rconde = 0, *rcondv = 0, 
	query, *work;
      Matrix<Real> *a;

      try {

	a = new Matrix<Real>(x);
	wr = new Real[n];
	wi = new Real[n];
	scale = new Real[n];

      } catch (std::bad_alloc) {
	throw OutOfMemory();
      }

      F77_FUN(dgeevx)(&balanc, &jobvl, &jobvr, &sense, &n, a->data(), &lda, 
		      wr, wi, vl, &ldvl, vr, &ldvr, &ilo, &ihi, 
		      scale, &abnrm, rconde, rcondv, &query, &lwork, iwork, &info);
      if (info < 0) throw Exception("eigenvals", "illegal input");
      if (info > 0) throw Exception("eigenvals", "failed to compute all eigenvalues");

      try {

	lwork = (int) query;
	work = new Real[lwork];

      } catch (std::bad_alloc) {
	throw OutOfMemory();
      }

      F77_FUN(dgeevx)(&balanc, &jobvl, &jobvr, &sense, &n, a->data(), &lda, 
		      wr, wi, vl, &ldvl, vr, &ldvr, &ilo, &ihi, 
		      scale, &abnrm, rconde, rcondv, work, &lwork, iwork, &info);
      if (info < 0) throw Exception("eigenvals", "illegal input");
      if (info > 0) throw Exception("eigenvals", "failed to compute all eigenvalues");

      delete[] work;
      delete[] scale;
      delete a;

      Matrix<Complex> *w;

      try {

	w = new Matrix<Complex>(n);
	for (Index k = 0; k < x.rows(); k++) 
	  (*w)(k) = Complex(wr[k], wi[k]);
    
      } catch (std::bad_alloc) {
	throw OutOfMemory();
      }

      delete[] wi;
      delete[] wr;

      eigsort(w);

      return w;

    } catch (Exception e) {

      std::cerr << e;
      throw Exception("eigenvals");

    }

  }

  static void eigsort (Matrix<Complex> *z)
  {
    Index i, j, k, n = z->size();
    Real p;
    Complex w;

    for (i = 0; i < n; i++) {
      p = abs((*z)(k = i));
      for (j = i + 1; j < n; j++) { // Find maximum modulus
	if (abs((*z)(j)) >= p) p = abs((*z)(k = j));
      }
      if (k != i) {		// Swap values & vectors
	w = (*z)(k);
	(*z)(k) = (*z)(i);
	(*z)(i) = w;
      }
    }
  }

  static void eigsort (Matrix<Complex> *z, Matrix<Complex> *left, 
		       Matrix<Complex> *right)
  {
    Index i, j, k, n = z->size();
    Real p;
    Complex w;

    for (i = 0; i < n; i++) {
      p = abs((*z)(k = i));
      for (j = i + 1; j < n; j++) { // Find maximum modulus
	if (abs((*z)(j)) >= p) p = abs((*z)(k = j));
      }
      if (k != i) {		// Swap values & vectors
	w = (*z)(k);
	(*z)(k) = (*z)(i);
	(*z)(i) = w;

	for (j = 0; j < n; j++) {
	  w = (*left)(j,k);
	  (*left)(j,k) = (*left)(j,i);
	  (*left)(j,i) = w;

	  w = (*right)(j,k);
	  (*right)(j,k) = (*right)(j,i);
	  (*right)(j,i) = w;
	}
      }
    }
  }

}
