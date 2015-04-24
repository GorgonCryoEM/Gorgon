#ifndef _LLAMA_DECLS_H_
#define _LLAMA_DECLS_H_

#include <complex>

namespace Llama {

  typedef bool Boolean;
  typedef size_t Index;
  typedef long Integer;
  typedef double Real;
  typedef std::complex<Real> Complex;

  template <class Scalar, Index N> class TinyVector;
  template <class Scalar> class CAbstMatrix;
  template <class Scalar> class AbstMatrix;
  template <class Scalar> class Matrix;
  class Random;
  class UniformRandom;
  class NormalRandom;
  template <class Scalar> class SMClosure;
  template <class Scalar> class MMClosure;
  template <class Scalar> class Transpose;
  class Range;
  template <class Scalar> class Submatrix;
  template <class OScalar, class IScalar> class FAMClosure;
  template <class Scalar> class Diagonal;
  template <class Scalar> class DiagonalMatrix;
  template <class Scalar> class EigenDecomposition;
  template <class Scalar> class LUDecomposition;
  template <class Scalar> class SVDecomposition;
  template <class Scalar> class QRDecomposition;
  template <class Scalar> class CholeskyDecomposition;
  template <class Scalar> class AbstDiffeomorphism;
  template <class Scalar> class Diffeomorphism;
  template <class Scalar> class AbstMapping;
  template <class Scalar> class Mapping;
  template <class Scalar> class AbstVectorfield;
  template <class Scalar> class Vectorfield;
  template <class Scalar> class FourierTransform;

}

#endif
