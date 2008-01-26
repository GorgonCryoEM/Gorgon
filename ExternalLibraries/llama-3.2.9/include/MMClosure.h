// -*- C++ -*-

#ifndef _LLAMA_MM_CLOSURE_H_
#define _LLAMA_MM_CLOSURE_H_

#include "Matrix.h"
#include "f77fun.h"

namespace Llama {

  /**
     Matrix multiplication closure.
  */
  template <class Scalar>
  class MMClosure {

  private:

    Scalar _alpha;
    Index _rows, _cols, _suml, _lld, _rld;
    const Matrix<Scalar> *_left, *_right;
    Transpose_t _lt, _rt;

    Scalar alpha (void) const { 
      return _alpha;
    };

    Index sumlength (void) const {
      return _suml;
    };

    Index lld (void) const {
      return _lld;
    };

    Index rld (void) const {
      return _rld;
    };

    const Matrix<Scalar> *lmatrix (void) const {
      return _left;
    };

    const Matrix<Scalar> *rmatrix (void) const {
      return _right;
    };

    Transpose_t ltrans (void) const {
      return _lt;
    };

    Transpose_t rtrans (void) const {
      return _rt;
    };

    Index rows (void) const {
      return _rows;
    };

    Index cols (void) const {
      return _cols;
    };

    MMClosure& operator = (const MMClosure&);

    //    Index size (void) const {
    //      return _rows * _cols;
    //    };

    //    Scalar operator () (const Index i, const Index j) const {
    //      Scalar s = 0;
    //      for (Index k = 0; k < this->sumlength(); k++) 
    //        s += (*(this->lmatrix()))(i,k) * (*(this->rmatrix()))(k,j);
    //      s *= this->alpha();
    //      return s;
    //    };

  public:

    // Basic constructor
    MMClosure (const Scalar a, const Matrix<Scalar> *l, const Matrix<Scalar> *r, 
	       const Transpose_t lt, const Transpose_t rt)
    {

      _alpha = a;

      _left = l;
      _right = r;

      _lt = lt;
      _rt = rt;

      _rows   = (lt == NOTRANSPOSE) ? l->rows() : l->cols();
      _suml   = (lt == NOTRANSPOSE) ? l->cols() : l->rows();
      _cols   = (rt == NOTRANSPOSE) ? r->cols() : r->rows();

      if (_suml != ((rt == NOTRANSPOSE) ? r->rows() : r->cols()))
	throw DimError("matrix multiply");
    
      _lld = l->rows();
      _rld = r->rows();

    };

    MMClosure (const MMClosure& x);

    ~MMClosure (void) { };

    friend class Matrix<Scalar>;

  };

  // Rules of Matrix Algebra

  template <class Scalar>
  inline MMClosure<Scalar>
  operator * (const Matrix<Scalar>& l, const Matrix<Scalar>& r)
  {
    return MMClosure<Scalar>(1, &l, &r, l.trans(), r.trans());
  }

  template <class Scalar>
  inline MMClosure<Scalar>
  operator * (const Scalar a, const MMClosure<Scalar>& c)
  {
    return MMClosure<Scalar>(a * c.alpha(), c.lmatrix(), c.rmatrix(), 
			     c.lmatrix()->trans(), c.rmatrix()->trans());
  }

  template <class Scalar>
  inline MMClosure<Scalar>
  operator * (const MMClosure<Scalar>& c, const Scalar a)
  {
    return MMClosure<Scalar>(c.alpha() * a, c.lmatrix(), c.rmatrix(), 
			     c.lmatrix()->trans(), c.rmatrix()->trans());
  }

  template <class Scalar>
  inline MMClosure<Scalar>
  operator * (const SMClosure<Scalar>& c, const Matrix<Scalar>& r)
  {
    return MMClosure<Scalar>(c.alpha(), c.matrix(), &r, c.trans(), r.trans());
  }

  template <class Scalar>
  inline MMClosure<Scalar>
  operator * (const Matrix<Scalar>& l, const SMClosure<Scalar>& c)
  {
    return MMClosure<Scalar>(c.alpha(), &l, c.matrix(), l.trans(), c.trans());
  }

  template <class Scalar>
  inline MMClosure<Scalar>
  operator * (const Transpose<Scalar>& l, const Matrix<Scalar>& r)
  {
    return MMClosure<Scalar>(1, l.matrix(), &r, l.trans(), r.trans());
  }

  template <class Scalar>
  inline MMClosure<Scalar>
  operator * (const Matrix<Scalar>& l, const Transpose<Scalar>& r)
  {
    return MMClosure<Scalar>(1, &l, r.matrix(), l.trans(), r.trans());
  }

  template <class Scalar>
  inline MMClosure<Scalar>
  operator * (const Transpose<Scalar>& l, const Transpose<Scalar>& r)
  {
    return MMClosure<Scalar>(1, l.matrix(), r.matrix(), l.trans(), r.trans());
  }

  template <class Scalar>
  inline MMClosure<Scalar>
  operator * (const SMClosure<Scalar>& c, const Transpose<Scalar>& r)
  {
    return MMClosure<Scalar>(c.alpha(), c.matrix(), r.matrix(), l.trans(), r.trans());
  }

  template <class Scalar>
  inline MMClosure<Scalar>
  operator * (const Transpose<Scalar>& l, const SMClosure<Scalar>& c)
  {
    return MMClosure<Scalar>(c.alpha(), l.matrix(), c.matrix(), l.trans(), c.trans());
  }

}

#endif
