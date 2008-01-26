
#if defined(Add_) || defined(Add__)
   #define crout crout_
#elif defined(UpCase)
   #define crout CROUT
#endif
#ifdef SunStyle

void crout(char *str1, F77_INTEGER *n1, char *str2, F77_INTEGER *n2,
           F77_INTEGER three, F77_INTEGER five)
{
   if ( (*n1 != -1) || (*n2 != -2) || (three != 3) || (five != 5) ) exit(-1);
   if (str1[0] != '1' || str1[1] != '2' || str1[2] != '3') exit(-1);
   if (str2[0] != '1' || str2[1] != '2' || str2[2] != '3' ||
       str2[3] != '4' || str2[4] != '5') exit(-1);
   printf("-DSunStyle\n");
}

#elif defined(CrayStyle)

#include <fortran.h>
void crout(_fcd str1, F77_INTEGER *n1, _fcd str2, F77_INTEGER *n2)
{
   if ( (*n1 != -1) || (*n2 != -2) ) exit(-1);
   if (*(_fcdtocp(str1)) != '1' || *(_fcdtocp(str2)) != '1' ) exit(-1);
   printf("-DCrayStyle\n");
}

#elif defined(StructVal)

typedef struct {char *cp; F77_INTEGER len;} F77_CHAR;
void crout(F77_CHAR str1, F77_INTEGER *n1, F77_CHAR str2, F77_INTEGER *n2)
{
   if ( (*n1 != -1) || (*n2 != -2) || (str1.len != 3) || (str2.len != 5) )
      exit(-1);
   if (str1.cp[0] != '1' || str1.cp[1] != '2' || str1.cp[2] != '3') exit(-1);
   if (str2.cp[0] != '1' || str2.cp[1] != '2' || str2.cp[2] != '3' ||
       str2.cp[3] != '4' || str2.cp[4] != '5') exit(-1);
   printf("-DStringStructVal\n");
}
#elif defined(StructPtr)
typedef struct {char *cp; F77_INTEGER len;} *F77_CHAR;
void crout(F77_CHAR str1, F77_INTEGER *n1, F77_CHAR str2, F77_INTEGER *n2)
{
   if ( (*n1 != -1) || (*n2 != -2) || (str1->len != 3) || (str2->len != 5) )
      exit(-1);
   if (str1->cp[0] != '1' || str1->cp[1] != '2' || str1->cp[2] != '3') exit(-1);
   if (str2->cp[0] != '1' || str2->cp[1] != '2' || str2->cp[2] != '3' ||
       str2->cp[3] != '4' || str2->cp[4] != '5') exit(-1);
   printf("-DStringStructPtr\n");
}
#endif
