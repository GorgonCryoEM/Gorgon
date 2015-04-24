// -*- C++ -*-

#ifndef _LLAMA_EXCEPTION_H_
#define _LLAMA_EXCEPTION_H_

#include "Decls.h"
#include "General.h"
#include <string>
#include <sstream>

namespace Llama {

  /**
     Llama exception-handling class.
  */
  class Exception { 

  private:

    // Implementation
    std::string _loc, _msg;

  public:

    /** 
	Create a new Exception, from (optional) location L,
	with (optional) additional message M.
    */
    Exception (const std::string& L = EmptyString, const std::string& M = EmptyString)
      : _loc(L), _msg(M) { }

    virtual ~Exception (void) { };

    virtual std::string& location () {return _loc;}

    virtual std::string& message () {return _msg;}

  };

  /**
     Print out the exception message.
  */
  inline std::ostream&
  operator << (std::ostream& o, Exception& e)
  {
    o << '\a' << "llama error encountered";
    if (e.location() != EmptyString) {
      o << " in " << e.location() << NewLine;
    } else {
      o << NewLine;
    }
    if (e.message() != EmptyString) {
      o << e.message() << NewLine;
    }
    return o;
  }

  class DivideError : public Exception {

  public:
  
    DivideError (const std::string& loc = EmptyString) 
      : Exception(loc, "division by zero") { };

  };

  class SingularMatrix : public Exception {

  public:

    SingularMatrix (const std::string& loc = EmptyString)
      : Exception(loc, "ill-conditioned or singular matrix encountered") { };

    SingularMatrix (const std::string& loc, const Real r) 
      : Exception(loc) {
      std::ostringstream o;
      o << "ill-conditioned or singular matrix encountered:" << NewLine
	<< "estimated reciprocal condition number = " << r;
      this->message() = o.str();
    }

  };

  class LogicError : public Exception {

  public:

    LogicError (const std::string& L = EmptyString) : Exception(L) { };

    LogicError (const std::string& L, const std::string& M) : Exception(L, M) { };

  };

  class DimError : public Exception {

  public:

    DimError (const std::string& loc = EmptyString)
      : Exception(loc, "incommensurate dimensions") { };

  };

  class IndexError : public Exception {

  public:

    IndexError (const std::string& loc = EmptyString)
      : Exception(loc, "index out of range") { };

  };

  class OutOfMemory : public Exception {

  public:

    OutOfMemory (const std::string& loc = EmptyString)
      : Exception(loc, "out of memory") { };

  };

  class IOError : public Exception {

  public:

    IOError (const std::string& loc = EmptyString)
      : Exception(loc, "llama i/o error") { };

  };

}

#endif
