// -*- C++ -*-

#ifndef _LLAMA_RANGE_H_
#define _LLAMA_RANGE_H_

#include "General.h"
#include "Exception.h"

namespace Llama {

  /**
     Range of integers closure.
  */
  class Range {
  private:
    Boolean _all;			// all?
    Integer _start;		// beginning index
    Integer _stop;		// end index
    Integer _stride;		// stride unit
    Integer _size;		// size
  public:
    // constructors
    Range ();
    Range (Integer, Integer, Integer = 1);
    // basic information
    Boolean all() const;
    Integer start () const;
    Integer stop () const;
    Integer stride () const;
    Integer size () const;
    // the i-th element in the range
    Integer operator () (Integer) const;
  };

  inline Range::Range () : _all(True) {}

  inline Range::Range (Integer start, Integer stop, Integer stride)
    : _all(False), _start(start), _stop(stop), _stride(stride)
  {
    _size = (_stop - _start) / _stride + 1;
    if (_size <= 0) 
      throw Exception("range constructor", "empty range");
  }

  inline Boolean
  Range::all () const
  { return _all; }

  inline Integer
  Range::start () const
  { return _start; }

  inline Integer
  Range::stop () const
  { return _stop; }

  inline Integer
  Range::stride () const
  { return _stride; }

  inline Integer
  Range::size () const
  {
    return _size; 
  }

  inline Integer
  Range::operator () (Integer k) const
  {
    if (k > _size) 
      throw IndexError("range ()");
    if (this->all()) {
      return k;
    } else {
      return _start + _stride * k;
    }
  }

  /**
     Llama slice object
  */
  typedef struct {
    Boolean use_slice;
    Index start, size;
    Integer stride;
    const Matrix<Index> *indices;
  } _slice;

}

#endif
