
#include <stdio.h>
enum WHATPROBE {Pcpu, Pnproc, Pl1, Pl2, Ptlb, Pall};

PrintUsage(char *nam)
{
   fprintf(stderr, "USAGE: %s -c (cpu) -n (#procs) -1 (L1 cache) -2 (L2 cache) -t (# of tlb)\n\n", nam);
   exit(1);
}

enum WHATPROBE GetFlags(int nargs, char **args)
{
   int i;
   enum WHATPROBE ret=Pall;

   if (nargs > 2) PrintUsage(args[0]);
   if (nargs > 1)
   {
      if (args[1][0] != '-') PrintUsage(args[0]);
      switch(args[1][1])
      {
      case 'c':
         ret = Pcpu;
         break;
      case 'n':
         ret = Pnproc;
         break;
      case '1':
         ret = Pl1;
         break;
      case '2':
         ret = Pl2;
         break;
      case 't':
         ret = Ptlb;
         break;
      default:
         ret = Pall;
      }
   }
   return(ret);
}

main(int nargs, char **args)
{
   enum WHATPROBE what;
   int ncpu;

   what = GetFlags(nargs, args);
   switch(what)
   {
   case Pnproc:
      ncpu = syscall(315, 1);
      printf("ncpus = %d\n", ncpu);
      break;
   default:
      fprintf(stderr, "Not implemented\n");
      exit(-1);
   }
   exit(0);
}
