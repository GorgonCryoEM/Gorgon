#include "SVDecomposition.h"

using namespace Llama;

namespace Llama {

  SVDecomposition<Real>::SVDecomposition (const Matrix<Real>& x)
  {
    try {

      char jobu = 'S', jobvt = 'S';
      int m = x.rows(), n = x.cols(), lda = m, ldu = m, 
	ls = min(x.rows(), x.cols()), ldvt = n, lwork = -1, info = 0;
      Real query, *work;
      Matrix<Real> *a;

      try {
	s = new Matrix<Real>(ls);
	a = new Matrix<Real>(x);
	u = new Matrix<Real>(m, ls);
	vt = new Matrix<Real>(ls, ls);
      } catch (std::bad_alloc) {
	throw OutOfMemory();
      }
  
      F77_FUN(dgesvd)(&jobu, &jobvt, &m, &n, a->data(), &lda, s->data(),
		      u->data(), &ldu, vt->data(), &ldvt, &query, &lwork,
		      &info);
      if (info < 0) throw Exception("SV decomposition", "illegal input");
      if (info > 0) throw Exception("SV decomposition", "non-convergence");

      try {
	lwork = (int) query;
	work = new Real[lwork];
      } catch (std::bad_alloc) {
	throw OutOfMemory();
      }

      F77_FUN(dgesvd)(&jobu, &jobvt, &m, &n, a->data(), &lda, s->data(),
		      u->data(), &ldu, vt->data(), &ldvt, work, &lwork,
		      &info);
      if (info < 0) throw Exception("SV decomposition", "illegal input");
      if (info > 0) throw Exception("SV decomposition", "non-convergence");

      delete a;
      delete[] work;

    } catch (Exception e) {
      std::cerr << e;
      throw Exception("SVDecomposition");
    }

  }

  Matrix<Real> *
  singvals (const Matrix<Real>& x)
  {
    try {
      char jobu = 'N', jobvt = 'N';
      int m = x.rows(), n = x.cols(), lda = m, ldu = m, 
	ls = min(x.rows(), x.cols()), ldvt = n, lwork = -1, info = 0;
      Real query, *work;
      Matrix<Real> *a, *s;
  
      try {
	a = new Matrix<Real>(x);
	s = new Matrix<Real>(ls);
      } catch (std::bad_alloc) {
	throw OutOfMemory();
      }
  
      F77_FUN(dgesvd)(&jobu, &jobvt, &m, &n, a->data(), &lda, s->data(),
		      0, &ldu, 0, &ldvt, &query, &lwork, &info);
      if (info < 0) throw Exception("singvals", "illegal input");
      if (info > 0) throw Exception("singvals", "non-convergence");

      try {
	lwork = (int) query;
	work = new Real[lwork];
      } catch (std::bad_alloc) {
	throw OutOfMemory();
      }

      F77_FUN(dgesvd)(&jobu, &jobvt, &m, &n, a->data(), &lda, s->data(),
		      0, &ldu, 0, &ldvt, work, &lwork, &info);
      if (info < 0) throw Exception("singvals", "illegal input");
      if (info > 0) throw Exception("singvals", "non-convergence");

      delete a;
      delete[] work;

      return s;

    } catch (Exception e) {
      std::cerr << e;
      throw Exception("singvals");
    }

  }

}
