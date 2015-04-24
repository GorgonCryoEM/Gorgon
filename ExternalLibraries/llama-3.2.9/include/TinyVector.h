// -*- C++ -*-

#ifndef _LLAMA_TINY_VECTOR_H_
#define _LLAMA_TINY_VECTOR_H_

#include "AbstMatrix.h"

namespace Llama {

  template <class Scalar, Index N>
  class TinyVector : public virtual AbstMatrix<Scalar> {

  private:
    // Implementation
    Scalar _data[N];

  public:

    // Default constructor
    TinyVector (void) { };

    // Destructor
    ~TinyVector (void) { };

    Scalar *data (void) {
      return _data;
    };

    Index size (void) const {
      return N;
    };

    Index rows (void) const {
      return N;
    };

    Index cols (void) const {
      return 1;
    };

    Scalar operator () (const Index k) const {
      return _data[k];
    };
  
    Scalar& operator () (const Index k) {
      return _data[k];
    };
  
    Scalar operator () (const Index j, const Index k) const {
      if (k > 0) 
	throw IndexError();
      return _data[j];
    };

    Scalar& operator () (const Index j, const Index k) {
      if (k > 0) 
	throw IndexError();
      return _data[j];
    };

  };

}

#endif
