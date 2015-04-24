#include "atlas_buildinfo.h"
#include "atlas_cacheedge.h"

void ATL_buildinfo(void)
{
   printf("ATLAS version %s built by %s on %s:\n",
          ATL_VERS, ATL_UNAM, ATL_DATE);
   printf("   UNAME    : %s\n", ATL_SYSINFO);
   printf("   INSTFLG  : %s\n", ATL_INSTFLAGS);
   printf("   MMDEF    : %s\n", ATL_MMDEF);
   printf("   ARCHDEF  : %s\n", ATL_ARCHDEF);
   printf("   F2CDEFS  : %s\n", ATL_F2CDEFS);
   #ifdef CacheEdge
      printf("   CACHEEDGE: %d\n", CacheEdge);
   #else
      printf("   CACHEEDGE: UNDEFINED\n");
   #endif
   printf("   %s : %s, version %s\n", "F77     ", ATL_F77, ATL_F77VERS);
   printf("   F77FLAGS : %s\n", ATL_F77FLAGS);
   printf("   %s : %s, version %s\n", "CC      ", ATL_CC, ATL_CCVERS);
   printf("   CC FLAGS : %s\n", ATL_CCFLAGS);
   printf("   %s : %s, version %s\n", "MCC     ", ATL_MCC, ATL_MCCVERS);
   printf("   MCCFLAGS : %s\n", ATL_MMFLAGS);
}
