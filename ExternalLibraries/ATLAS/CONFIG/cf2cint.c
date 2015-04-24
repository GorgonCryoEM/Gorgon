
#if defined(Add_) || defined(Add__)
   #define c2fint c2fint_
#elif defined(UpCase)
   #define c2fint C2FINT
#endif
void c2fint(void *vp)
{
   int *ip=vp;
   long *lp=vp;
   short *sp=vp;

   if ( (sizeof(long) != sizeof(int)) && (*lp == 1) )
      printf("F77 INTEGER -> C long\n");
   else if (*ip == 1) printf("F77 INTEGER -> C int\n");
   else if (*sp == 1) printf("F77 INTEGER -> C short\n");
}
