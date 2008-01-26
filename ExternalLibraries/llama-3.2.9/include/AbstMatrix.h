// -*- C++ -*-

#ifndef   _LLAMA_ABST_MATRIX_H_
#define   _LLAMA_ABST_MATRIX_H_

#include "Decls.h"

#include <iostream>

namespace Llama {

  /**
     This is a constant abstract matrix class.
  */
  template <class Scalar>
  class CAbstMatrix {

  public:

    // Destructor
    virtual ~CAbstMatrix (void) { };

    // Dimensions
    virtual Index size (void) const = 0;
    virtual Index rows (void) const = 0;
    virtual Index cols (void) const = 0;

    // Elementwise access.
    virtual Scalar operator () (const Index) const = 0;
    virtual Scalar operator () (const Index, const Index) const = 0;

    /**
       Binary output.
    */
    virtual void operator >> (std::ostream& o) const {
      Index dims[2];
      Scalar tmp;
      dims[0] = this->rows();
      dims[1] = this->cols();
      o.write((char *) &dims[0], 2 * sizeof(Index));
      for (Index k = 0; k < this->size(); k++)
	tmp = (*this)(k);
      o.write((char *) &tmp, sizeof(Scalar));
    };
  
  };

  /**
     This is an abstract matrix class.
  */
  template <class Scalar>
  class AbstMatrix : public virtual CAbstMatrix<Scalar> {

  public:

    virtual ~AbstMatrix (void) { };

    virtual Scalar& operator () (const Index) = 0;
    virtual Scalar& operator () (const Index, const Index) = 0;

  };

  /**
     Formatted output
  */
  template <class Scalar>
  inline std::ostream&
  operator << (std::ostream& o, const CAbstMatrix<Scalar>& x) {
    o << NewLine;
    for (Index j = 0; j < x.rows(); j++) {
      for (Index k = 0; k < x.cols(); k++) o << Blank << Blank << x(j,k);
      o << NewLine;
    }
    return o;
  };

}

#endif
