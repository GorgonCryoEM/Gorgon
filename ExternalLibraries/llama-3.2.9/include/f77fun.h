#ifndef _LLAMA_F77FUN_H_
#define _LLAMA_F77FUN_H_

#define CBLAS_FUN(F) cblas_##F
#define F77_FUN(F) F##_

typedef struct { double r, i; } f77_complex;

extern "C" {

#include <cblas.h>

					/* BLAS routine declarations */

  void F77_FUN(dcopy)(int *, double *, int *, double *, int *);
  void F77_FUN(zcopy)(int *, void *, int *, void *, int *);
  void F77_FUN(daxpy)(int *, double *, double *, int *, double *, int *);
  void F77_FUN(zaxpy)(int *, void *, void *, int *, void *, int *);
  double F77_FUN(ddot)(int *, double *, int *, double *, int *);
  f77_complex F77_FUN(zdotc)(int *, void *, int *, void *, int *);
  double F77_FUN(dnrm2)(int *, double *, int *);
  double F77_FUN(dznrm2)(int *, void *, int *);
  double F77_FUN(dasum)(int *, double *, int *);
  double F77_FUN(dzasum)(int *, void *, int *);
  void F77_FUN(dscal)(int *, double *, double *, int *);
  void F77_FUN(zscal)(int *, void *, void *, int *);

					/* LAPACK routine declarations */

  void F77_FUN(dgetrf)(int *, int *, double *, int *, int *, int *);
  void F77_FUN(dgetrs)(char *, int *, int *, double *, int *, int *, double *, int *, int *);
  void F77_FUN(dgecon)(char *, int *, double *, int *, double *, double *, double *, int *, int *);
  double F77_FUN(dlange)(char *, int *, int *, const double *, int *, double *);
  double F77_FUN(zlange)(char *, int *, int *, const void *, int *, double *);
  void F77_FUN(dgetri)(int *, double *, int *, int *, double *, int *, int *);
  void F77_FUN(dgesvd)(char *, char *, int *, int *, double *, int *, double *, 
		       double *, int *, double *, int *, double *, int *, int *);
  void F77_FUN(dgeevx)(char *, char *, char *, char *, int *, double *, int *, 
		       double *, double *, double *, int *, double *, int *, int *, int *, 
		       double *, double *, double *, double *, double *, int *, int *, int *);
  void F77_FUN(dpotrf)(char *, int *, double *, int *, int *);
  void F77_FUN(dpotrs)(char *, int *, int *, double *, int *, double *, int *, int *);
  void F77_FUN(zpotrf)(char *, int *, void *, int *, int *);
  void F77_FUN(zpotrs)(char *, int *, int *, void *, int *, void *, int *, int *);
  void F77_FUN(dgeqrf)(int *, int *, double *, int *, double *, double *, int *, int *);
  void F77_FUN(dorgqr)(int *, int *, int *, double *, int *, double *, double *, int *, int *);

  typedef void (*odessa_f)(int *, double *, double *, double *, double *);
  typedef void (*odessa_df)(int *, double *, double *, double *, double *, int *);
  typedef void (*odessa_jac)(int *, double *, double *, double *, int *, int *, double *, int *);

  void F77_FUN(odessa) (odessa_f, odessa_df, int *, double *, double *, double *, double *, 
			int *, double *, double *, int *, int *, int *, double *, int *, int *, 
			int *, odessa_jac, int *);

  void F77_FUN(xsetun)(int *);
}

typedef enum CBLAS_TRANSPOSE Transpose_t;

#define NOTRANSPOSE CblasNoTrans
#define TRANSPOSE   CblasTrans
#define CONJTRANSPOSE CblasConjTrans
#define COLMAJOR CblasColMajor
#define ROWMAJOR CblasRowMajor

#endif
