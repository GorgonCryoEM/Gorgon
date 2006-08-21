/*-----------------------------------------------------
 * error.cc
 * Implementation of error handling functions
 *
 * Author: P. Foggia
 * $Id$
 ----------------------------------------------------*/

/*----------------------------------------------------
 * REVISION HISTORY
 *   $Log$
 *   Revision 1.2  1998/12/12 12:18:07  foggia
 *   Now supports full printf syntax
 *
 *   Revision 1.1  1998/09/16 17:35:14  foggia
 *   Initial revision
 *
 ---------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "error.h"


/*------------------------------------------
 * void error(msg, ...)
 * Prints an error message and exits 
 * the program.
 * The syntax is the same of printf, 
 * except that a trailing \n is automatically
 * appended.
 -----------------------------------------*/
void error(char *msg, ...)
  { va_list ap;
    va_start(ap, msg);
    fprintf(stderr, "ERROR: ");
    vfprintf(stderr, msg, ap);
    fprintf(stderr, "\n");
    va_end(ap);
    exit(1);
  }

