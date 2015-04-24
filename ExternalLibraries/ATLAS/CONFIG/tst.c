
#include <stdio.h>
#include <stdlib.h>

void hello(void *p, int i)
{
   if (p) fprintf(stdout, "hello, i=%d\n", i);
   else fprintf(stdout, "helloNULL, i=%d\n", i);
}

main(int nargs, char *args)
{
   hello(NULL, 2);
}
