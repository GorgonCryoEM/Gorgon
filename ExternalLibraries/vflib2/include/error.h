/*----------------------------------------------------
 * error.h
 * Header of error.cc
 * Error handling functions
 *
 * Author: P. Foggia
 * $Id$
 *--------------------------------------------------*/

/*----------------------------------------------------
 * REVISION HISTORY
 *   $Log$
 *   Revision 1.2  1998/12/12 12:17:32  foggia
 *   Now supports full printf syntax
 *
 *   Revision 1.1  1998/09/16 17:35:14  foggia
 *   Initial revision
 *
 ---------------------------------------------------*/

#ifndef ERROR_H

#include <stddef.h>

void error(char *msg, ...);



#define FAIL(reason)    error("%s in %s:%d", (reason), __FILE__, \
                                                          (int)__LINE__)

#define OUT_OF_MEMORY()  FAIL("Out of memory")
#define CANT_HAPPEN()    FAIL("Can't happen")




#endif
