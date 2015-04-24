
#include <stdio.h>
#include <sys/systemcfg.h>

/*
 * ifdef guards for older systemcfg.h's:
 */
#ifndef POWER_620
   #define POWER_620 0x0040
#endif
#ifndef POWER_630
   #define POWER_630 0x0080
#endif
#ifndef POWER_A35
   #define POWER_A35 0x0100
#endif
#ifndef POWER_RS64II
   #define POWER_RS64II 0x0200
#endif
#ifndef POWER_RS64III
   #define POWER_RS64III 0x0400
#endif
#ifndef IA64
   #define IA64 0x0003
#endif
#ifndef IA64_M1
   #define IA64_M1 0x0001
#endif
#ifndef IA64_M2
   #define IA64_M2 0x0002
#endif

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
   char *cpu;
   int l1;

   what = GetFlags(nargs, args);
   l1 = _system_configuration.dcache_size;

   switch(what)
   {
   default:
      what = Pall;
   case Pall:
   case Pcpu:
      switch(_system_configuration.architecture)
      {
      case POWER_RS:
      case POWER_PC:
         switch(_system_configuration.implementation)
         { /* need to differintiate betweein Thin & Wide Pwr2 */
         case POWER_RS1:
            cpu = "Power1";
            break;
         case POWER_RSC:
            cpu = "POWER_RSC";
            break;
         case POWER_RS2:
            cpu = "Power2";
            break;
         case POWER_601:
            cpu = "PowerPC 601";
            break;
         case POWER_603:
            cpu = "PowerPC 603";
            break;
         case POWER_604:
            if (l1 == 32*1024) cpu = "PowerPC 604e";
            else if (l1 == 16*1024) cpu = "PowerPC 604";
            else cpu = "PowerPC 604 UNKNOWN";
            break;
         case POWER_620:
            cpu = "PowerPC 620";
            break;
         case POWER_630:
            cpu = "PowerPC 630";
            break;
         case POWER_A35:
            cpu = "PowerPC A35";
            break;
         case POWER_RS64II:
            cpu = "Power RS64II";
            break;
         case POWER_RS64III:
            cpu = "Power RS64III";
            break;
#ifdef POWER_4
         case POWER_4:
            cpu = "Power4";
            break;
#endif
         default:
            if (__power_rs())  /* power1, 2 or 3 */
               cpu = "UNKNOWN Power architecture";
            else if (__power_pc())  /* PPC of some type */
               cpu = "UNKNOWN PowerPC architecture";
            else cpu = "UNKNOWN";
         }
         break;
      case IA64:
         switch(_system_configuration.implementation)
         {
         case IA64_M1:
            cpu = "IA64 M1";
            break;
         case IA64_M2:
            cpu = "IA64 M2";
            break;
         default:
            cpu = "UNKNOWN IA64";
         }
         break;
      }
      printf("CPU type = %s\n", cpu);
      if (what != Pall) break;
   case Pnproc:
      if (what != Pall) break;
      printf("ncpus = %d\n", _system_configuration.ncpus);
   case Pl1:
      if (what != Pall) break;
      printf("L1 cache: size=%d, associativity=%d\n",
             _system_configuration.dcache_size,
             _system_configuration.dcache_asc);
   case Pl2:
      printf("L2 cache: size=%d, associativity=%d\n",
             _system_configuration.L2_cache_size,
             _system_configuration.L2_cache_asc);
      if (what != Pall) break;
   case Ptlb:
      printf("Number of TLB entries = %d\n", _system_configuration.dtlb_size);
   }
   exit(0);
}
