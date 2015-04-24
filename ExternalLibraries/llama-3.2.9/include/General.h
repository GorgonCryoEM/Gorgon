// -*- C++ -*-

#ifndef _LLAMA_GENERAL_H_
#define _LLAMA_GENERAL_H_

#include <cmath>
#include <memory>

#include "Decls.h"

namespace Llama {

  // Total order
  typedef int (*Order)(const void *, const void *);

  const Real Pi = 3.141592653589793238462643383280;
  const Real TwoPi = 6.283185307179586476925286766559;
  const Real PiHalf = 1.570796326794896619231321691640;

  const Boolean True  = true;
  const Boolean False = false;

  const char Blank = ' ';			// Blank character.
  const char NewLine = '\n';		// Linefeed character.
  const char Tab = '\t';			// Tab character.

#define EmptyString ""
#define UpperCase  "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
#define LowerCase  "abcdefghijklmnopqrstuvwxyz"
#define WhiteSpace "\t\n\v\f\r "

  inline Integer 
  min (Integer t1, Integer t2) {
    return (t1 < t2) ? t1 : t2;
  }

  inline Integer 
  max (Integer t1, Integer t2) {
    return (t1 > t2) ? t1 : t2;
  }

  //  inline Integer 
  //  abs (Integer x) {
  //    return (x > 0) ? x : -x;
  //  }

  inline Integer 
  signum (Integer x) {
    return ((x < 0) ? -1 : ((x == 0) ? 0 : 1));
  }

  inline Integer
  conj (Integer x) 
  {
    return x;
  }

  inline Index 
  min (Index t1, Index t2) {
    return (t1 < t2) ? t1 : t2;
  }

  inline Index 
  max (Index t1, Index t2) {
    return (t1 > t2) ? t1 : t2;
  }

  inline Index
  conj (Index x) 
  {
    return x;
  }

  inline Real 
  min (Real t1, Real t2) {
    return (t1 < t2) ? t1 : t2;
  }

  inline Real 
  max (Real t1, Real t2) {
    return (t1 > t2) ? t1 : t2;
  }

  // inline Real 
  // abs (Real& x) {
  //   return fabs(x);
  // }

  inline Real 
  abs (Real x) {
    return fabs(x);
  }

  inline Integer
  abs (Integer x) {
    return labs(x);
  }

  inline Index
  abs (Index x) {
    return x;
  }

  inline Real
  conj (Real x)
  {
    return x;
  }

  inline Real 
  signum (Real x) {
    return ((x < 0.0) ? -1.0 : ((x == 0.0) ? 0.0 : 1.0));
  }

  template <class Scalar>
  inline void
  swap (Scalar a, Scalar b)
  {
    Scalar s = a;
    a = b;
    b = s;
  }

  /**
     Safe hypotheneuse.
  */
  inline Real 
  pythag (Real a, Real b)
  {
    Real absa, absb;
    absa = abs(a); absb = abs(b);
    if (absa > absb) {
      return absa * hypot(1.0, absb / absa);
    } else {
      return (absb == 0.0 ? 0.0 : absb * hypot(1.0, absa / absb));
    }
  }

  /**
     Euclid's algorithm for integers 
  */
  inline Index
  GCD (Index p, Index q)
  {
    Index r;
    while (( r = p % q ) > 0) {
      p = q;
      q = r;
    }
    return q;
  }

}

#endif
