// -*- C++ -*-

#ifndef   _LLAMA_FOURIER_H_
#define   _LLAMA_FOURIER_H_

#include "Decls.h"
#include "General.h"
#include "Exception.h"
#include "Matrix.h"
#include "f77fun.h"

#include <fftw.h>
#include <rfftw.h>
#include <cstdio>

namespace Llama {

  /**
     Fourier transform class.  Construction causes the computation of
     the discrete Fourier transform.  Currently, the implementation uses
     the "Fastest Fourier Transform in the West" algorithms.  */

  template <class Scalar>
  class FourierTransform {
  private:

    Index _n;
    void *_d_plan, *_i_plan;

    // Prevent copying
    FourierTransform (const FourierTransform&);
    FourierTransform& operator = (const FourierTransform&);

  public:
    /** Basic constructor.*/
    FourierTransform (const Index n);

    /** Basic constructor, use wisdom.*/
    FourierTransform (const Index n, const std::string& wisdom);

    /** Destructor */
    ~FourierTransform (void);

    /** Compute and return a pointer to the direct DFT. */
    Matrix<Complex> * fft (const Matrix<Scalar>& x);

    /** Compute and return a pointer to the inverse DFT. */
    Matrix<Scalar> * ifft (const Matrix<Complex>& x);

    /** Convolve two vectors */
    Matrix<Scalar> * convol (const Matrix<Scalar>& x, const Matrix<Scalar>& y);

  };

  inline
  FourierTransform<Real>::FourierTransform (const Index n) 
    : _n(n), _d_plan(0), _i_plan(0)
  {
    _d_plan = (void *) rfftw_create_plan(_n, FFTW_FORWARD,  FFTW_MEASURE);
    _i_plan = (void *) rfftw_create_plan(_n, FFTW_BACKWARD, FFTW_MEASURE);
  };

  inline
  FourierTransform<Real>::FourierTransform (const Index n, const std::string& wisdom) 
    : _n(n), _d_plan(0), _i_plan(0)
  {
    FILE *wisdom_file;
    if ((wisdom_file = fopen(wisdom.c_str(), "r")) &&
	(FFTW_FAILURE != fftw_import_wisdom_from_file(wisdom_file))) {
      fclose(wisdom_file);
    } else {
      std::cerr << "Error reading wisdom from file " << wisdom << NewLine;
    }

    _d_plan = (void *) rfftw_create_plan(_n, FFTW_FORWARD,  FFTW_MEASURE | FFTW_USE_WISDOM);
    _i_plan = (void *) rfftw_create_plan(_n, FFTW_BACKWARD, FFTW_MEASURE | FFTW_USE_WISDOM);

    if ((wisdom_file = fopen(wisdom.c_str(), "w")))
      fftw_export_wisdom_to_file(wisdom_file);
    else {
      std::cerr << "Error saving wisdom to file " << wisdom << NewLine;
    }
  };

  inline
  FourierTransform<Real>::~FourierTransform (void)
  {
    rfftw_destroy_plan((rfftw_plan) _i_plan);
    rfftw_destroy_plan((rfftw_plan) _d_plan);
  }

  inline Matrix<Complex> *
  FourierTransform<Real>::fft (const Matrix<Real>& x)
  {
    if ((max(x.cols(), x.rows()) != _n) || (min(x.cols(), x.rows()) != 1))
      throw DimError("Real fft");

    Matrix<Real> *y = new Matrix<Real>(x.rows(), x.cols());
    Matrix<Complex> *z = new Matrix<Complex>(x.rows(), x.cols());
  
    rfftw ((rfftw_plan) _d_plan, 1, (fftw_real *) x.data(), 1, 1, (fftw_real *) y->data(), 1, 1);

    // We must deconstruct the half-complex array.
    Index nhalf = x.size()/2;
    (*z)(0) = Complex((*y)(0), 0);
    for (Index k = 1; k < nhalf; k++) {
      (*z)(k) = Complex((*y)(k),(*y)(x.size()-k));
      (*z)(x.size()-k) = Complex((*y)(k), -(*y)(x.size()-k));
    }
    (*z)(nhalf) = ((x.size() % 2) != 0) ? Complex((*y)(nhalf), (*y)(nhalf+1)) : Complex((*y)(nhalf), 0);

    delete y;
    return z;
  }

  inline Matrix<Real> *
  FourierTransform<Real>::ifft (const Matrix<Complex>& x)
  {
    if ((max(x.cols(), x.rows()) != _n) || (min(x.cols(), x.rows()) != 1))
      throw DimError("Real ifft");

    Matrix<Real> *y = new Matrix<Real>(x.rows(), x.cols()), *z = new Matrix<Real>(x.rows(), x.cols());
  
    // We must construct the half-complex array.
    (*y)(0) = x(0).real();
    for (Index k = 1; k <= x.size()/2; k++) (*y)(k) = x(k).real();
    for (Index k = 1; k <= (x.size()+1)/2-1; k++) (*y)(x.size()-k) = x(k).imag();

    rfftw ((rfftw_plan) _i_plan, 1, (fftw_real *) y->data(), 1, 1, (fftw_real *) z->data(), 1, 1);

    delete y;

    *z /= ((Real) x.size());

    return z;
  }

  inline Matrix<Real> * 
  FourierTransform<Real>::convol (const Matrix<Real>& x, const Matrix<Real>& y)
  {
    if ((x.rows() != y.rows()) || (x.cols() != y.cols()) || 
	(x.size() != _n) || (y.size() != _n))
      throw DimError("real fft convolution");

    Matrix<Real> *v, *w = new Matrix<Real>(x.rows(), x.cols()), 
      *z = new Matrix<Real>(x.rows(), x.cols());
  
    rfftw ((rfftw_plan) _d_plan, 1, (fftw_real *) x.data(), 1, 1, (fftw_real *) w->data(), 1, 1);
    rfftw ((rfftw_plan) _d_plan, 1, (fftw_real *) y.data(), 1, 1, (fftw_real *) z->data(), 1, 1);

    v = new Matrix<Real>(x.rows(), x.cols());
    Index nhalf = x.size()/2;
    (*v)(0) = (*w)(0) * (*z)(0);
    for (Index k = 1; k < nhalf; k++) {
      (*v)(k) = (*w)(k) * (*z)(k) - (*w)(x.size()-k) * (*z)(x.size()-k);
      (*v)(x.size()-k) = (*w)(k) * (*z)(x.size()-k) + (*w)(x.size()-k) * (*z)(k);
    }
    if ((x.size() % 2) != 0) {
      (*v)(nhalf) = (*w)(nhalf) * (*z)(nhalf) - (*w)(nhalf+1) * (*z)(nhalf+1);
      (*v)(nhalf+1) = (*w)(nhalf) * (*z)(nhalf+1) + (*w)(nhalf+1) * (*z)(nhalf);
    } else {
      (*v)(nhalf) = (*w)(nhalf) * (*z)(nhalf);
    }

    delete w;

    rfftw ((rfftw_plan) _i_plan, 1, (fftw_real *) v->data(), 1, 1, (fftw_real *) z->data(), 1, 1);
    delete v;

    *z /= x.size();

    return z;
  }

}

#endif
