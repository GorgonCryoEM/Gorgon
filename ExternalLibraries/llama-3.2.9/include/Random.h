// -*- C++ -*-

#ifndef _LLAMA_RANDOM_H_
#define _LLAMA_RANDOM_H_

#include "General.h"
#include "Exception.h"
#include <cmath>

extern "C" {

  // routines defined in rand.c
  void ranf_array(double [], int);
  void ranf_start(long);
  double randdev(void);

}

namespace Llama {

  /**
     Base random number generator class.
  */
  class Random {
  private:
    // Seed for generator.
    long seed;
    // Method for generating uniform random numbers.
    short _meth;
    // One of the above C random number generators.
    double (*generator)(void);
    // Make these private so no copying can happen.
    Random (const Random &);
    void operator = (const Random &);

  protected:

    double draw (void) {
      return generator();
    };

  public:

    /**
       Return the number of the method being used for drawing uniform
       random numbers.
    */
    virtual Index method (void) {
      return _meth;
    };

    /**
       Set the method for choosing random numbers.
    */
    virtual void method (short m) {
      _meth = m;
      switch (_meth) {
      case 0:			// Knuth's ranf_array method
	generator = randdev;
	break;
      default:
	throw LogicError("random method", "unrecognized method number");
	break;
      }
    };

    /**
       Initialize with a seed and (optionally) a choice of method.
    */
    Random (long s, short m = 0) : seed(s) {
      ranf_start(seed);
      this->method(m);
    };

    virtual Real operator () (void) = 0;

  };

  /** 
      Uniform random number generator.
  */

  class UniformRandom : public Random {

  public:

    /**
       Initialize with (negative) seed.
    */
    UniformRandom (long s) 
      : Random(s) { };

    /**
       Generate a uniform random variate
    */
    Real operator () (void) {
      return (Real) this->draw();
    };

    /**
       Generate a uniform(a,b) random variate
    */
    Real operator () (Real a, Real b) {
      return a + (b - a) * (*this)();
    };

    /**
       Return an exponential random deviate (rate = lambda).
    */
    Real exponential (Real lambda) {
      return (-log((*this)()) / lambda);
    };

    /**
       Return a gamma random deviate (order = n).
    */
    Real gamma (Index n)
    {
      if (n < 1) throw LogicError("gamma random deviate", "order must be >= 1");
  
      if (n < 6) {

	Real x = 1.0;
	for (Index j = 0; j < n; j++) x *= this->draw();
	x = -log(x);
	return x;
      
      } else {

	Real am, e, s, v1, v2, x, y;
	do {
	  do {
	    do {
	      v1 = 2.0 * (*this)() - 1.0;
	      v2 = 2.0 * (*this)() - 1.0;
	    } while (v1 * v1 + v2 * v2 > 1.0);
	    y = v2 / v1;
	    am = n - 1;
	    s = sqrt(2.0 * am + 1.0);
	    x = s * y + am;
	  } while (x <= 0.0);
	  e = (1.0 + y * y) * exp(am * log(x / am) - s * y);
	} while ((*this)() > e);
	return x;
      }
    };

  };

  /**
     Normal (Gaussian) random number generator
  */
  class NormalRandom : public Random {

  private:

    Boolean set;
    double save;

  public:

    /**
       Initialize with (negative) seed.
    */
    NormalRandom (long s)
      : Random(s), set(False), save(0) { };

    /**
       Generate a normal(0,1) random variate.
    */
    Real operator () (void) {
      double fac, rsq, v1, v2;
      Real x;
      if (set) {
	set = False;
	x = (Real) save;
      } else {
	do {
	  v1 = 2.0 * draw() - 1.0;
	  v2 = 2.0 * draw() - 1.0;
	  rsq = v1 * v1 + v2 * v2;
	} while (rsq >= 1.0 || rsq == 0.0);
	fac = sqrt (-2.0 * log (rsq) / rsq);
	save = v1 * fac;
	set = True;
	x = (Real) (v2 * fac);
      }
      return x;
    }

    /**
       Generate a normal(m,s) random variate.
    */
    Real operator () (Real mean, Real std) {
      return mean + std * (*this)();
    }

  };

}

#endif
