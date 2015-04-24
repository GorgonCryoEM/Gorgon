
/*
 *             Automatically Tuned Linear Algebra Software v3.6.0
 *                    (C) Copyright 1998 R. Clint Whaley
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *   1. Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions, and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *   3. The name of the ATLAS group or the names of its contributers may
 *      not be used to endorse or promote products derived from this
 *      software without specific written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE ATLAS GROUP OR ITS CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>
#include <string.h>

#define MAXL2SIZE 4194304
#define Mciswspace(C) ( (((C) > 8) && ((C) < 14)) || ((C) == 32) )
#define Mlowcase(C) ( ((C) > 64 && (C) < 91) ? (C) | 32 : (C) )


char *machnam[38] = {"21264", "21164", "21064", "HPPA8K", "HP9735",
                     "POWER", "POWER2Thin", "POWER2Wide", "POWER3", "POWER4",
                     "P5", "P5MMX", "PPRO", "PII", "PIII", "P4", "ATHLON",
                     "HAMMER32", "HAMMER64", "IA64Itan", "IA64Itan2",
                     "SGIIP28", "SGIIP27", "SGIIP32", "SGIIP22", "SGIIP30",
                     "SunMS", "SunSS", "SunUS1", "SunUS2", "SunUS4",
                     "SunUS5", "SunUSIII","SunUSX", "PPC604e", "PPC604",
                     "PPCG4", "UNKNOWN"};
enum MACHTYPE {Dec21264, Dec21164, Dec21064, HPPA8K, HP9K735,
               IbmPwr,  IbmTPwr2, IbmWPwr2, IbmPwr3, IbmPwr4,
               IntP5, IntP5MMX, IntPPRO, IntPII, IntPIII, IntP4, AmdAthlon,
               AmdHammer32, AmdHammer64, IA64Itan, IA64Itan2,
               SgiIP28, SgiIP27, SgiIP32, SgiIP22, SgiIP30,
               SunMS, SunSS, SunUS1, SunUS2, SunUS4, SunUS5, SunUSIII, SunUSX,
               PPC604e, PPC604, PPCG4, MACHOther};

char *osnam[12] = {"Linux", "SunOS", "SunOS4", "OSF1", "IRIX", "AIX",
                  "Win9x", "WinNT", "HPUX", "FreeBSD", "OSX", "Other/UNKNOWN"};
enum OSTYPE {OSLinux=0, OSSunOS, OSSunOS4, OSOSF1, OSIRIX, OSAIX,
             OSWin9x, OSWinNT, OSHPUX, OSFreeBSD, OSOSX, OSOther};

enum LINUXARCH {LAPPC, LASPARC, LAALPHA, LAX86, LAIA64, LAOTHER};
char *wcnam[5] ={"vf", "vc", "icc", "ifort", "CRAP"};
enum WINCOMP   {WCvf, WCvc, WCic, WCif, WCgcc};

enum F2CNAME {f2c_Add_=0, f2c_Add__, f2c_NoChange, f2c_UpCase, f2c_NamErr};
enum F2CINT {FintCint=0, FintClong, FintCshort, f2c_IntErr};
enum F2CSTRING {fstrSun=0, fstrCray, fstrStructVal, fstrStructPtr, f2c_StrErr};

/* NOTE on ISA extensions, order is from most preferred to least */
#define NISA 5
#define NISAp1 6
char *ISAXNAM[NISAp1] = {"AltiVec", "SSE2", "SSE1", "3DNow2", "3DNow1", ""};
enum ISAEXT {ISA_AV=0, ISA_SSE2, ISA_SSE1, ISA_3DNow2, ISA_3DNow1, ISA_None};
#define NASMp1 6
enum ASMDIA {gas_x86_32, gas_x86_64, gas_sparc, osx_ppc, linux_ppc, ASM_None};
char *ASMNAM[NASMp1] = {"GAS_x8632", "GAS_x8664", "GAS_SPARC",
                        "AS_OSX_PPC", "GAS_LINUX_PPC", ""};
char *usermmnam[2] = {"", "GOTO"};
enum USERGEMM {UG_None=0, UG_GOTO};

int XCOMP=0, THREADS=0, USEWINF77=0, NLINES=0, ISWIN=0;
char TARGNAM[512];
enum MACHTYPE mach=MACHOther;

#define my_join(pre, nam) pre ## nam
#define Mstr2(m) # m
#define Mstr(m) Mstr2(m)
#define OSIsWin(OS_) ( ((OS_) == OSWin9x) || ((OS_) == OSWinNT) )
#define MachIsAlpha(mach_) \
   ( ((mach_) == Dec21264) || ((mach_) == Dec21164) || ((mach_) == Dec21064) )
#define MachIsX86(mach_)  \
   ( ((mach_) == IntP5) || ((mach_) == IntP5MMX) || ((mach_) == IntPPRO) || \
     ((mach_) == IntPII) || ((mach_) == IntPIII) || ((mach_) == IntP4) || \
     ((mach_) == AmdAthlon) || \
     ((mach_) == AmdHammer32) || ((mach_) == AmdHammer64))
#define MachIsUS(mach_) \
   ( ((mach_) == SunUS1) || ((mach_) == SunUS2) || ((mach_) == SunUS2) || \
     ((mach_) == SunUS4) || ((mach_) == SunUS5) || ((mach_) == SunUSIII) || \
     ((mach_) == SunUSX) )

#define WCIsF77(wc_) ( (wc_) == WCvf || (wc_) == WCif )

int QUERY=0;
FILE *fpI, *fparch;

#include <time.h>
void GetDate(int *month, int *day, int *year, int *hour, int *min)
{
   time_t tv;
   struct tm *tp;

   tv = time(NULL);
   tp = localtime(&tv);
   *month = tp->tm_mon + 1;
   *day = tp->tm_mday;
   *year = tp->tm_year + 1900;
   *hour = tp->tm_hour;
   *min = tp->tm_min;
}

long GetInt(FILE *fpin, long Default, char *spc, char *expstr)
/*
 * Gets a signed integral type from fpin.  If nothing or garbage is entered,
 * Default is returned.
 */
{
   char str[64];
   long iin;
   if (expstr) fprintf(stdout, "%sEnter %s [%d]: ", spc, expstr, Default);
   if (fgets(str, 64, fpin) == NULL) return(Default);
   if (sscanf(str, " %ld ", &iin) != 1) return(Default);
   return(iin);
}

long GetIntRange(long Default, long Min, long Max, char *spc, char *expstr)
{
   long i;
   int keepOn=0;
   do
   {
      i = GetInt(stdin, Default, spc, expstr);
      if (i > Max)
      {
         keepOn = 1;
         fprintf(stderr, "\n%d larger than max value of %d.  Try again.\n\n",
                 i, Max);
      }
      else if (i < Min)
      {
         keepOn = 1;
         fprintf(stderr, "\n%d smaller than min value of %d.  Try again.\n\n",
                 i, Min);
      }
      else keepOn = 0;
   }
   while (keepOn);
   return(i);
}

long GetIntVer(long Default, long Min, long Max, char *spc, char *expstr)
{
   long i, j;

   do
   {
      i = GetIntRange(Default, Min, Max, spc, expstr);
      fprintf(stdout, "%s   You entered: %d\n", spc, i);
      j = GetIntRange(0, 0, 1, spc, "1 to reenter, 0 accepts");
   }
   while(j);
   return(i);
}


void GetString(FILE *fpin, char *Default, char *spc, char *expstr,
               int len, char *str0)
/*
 * Get a string of length len, not including NULL terminator; pads
 * any extra len with NULLs
 */
{
   char str[512], *sp;
   int i;

   assert(len+1 <= 512);
   if (expstr)
   {
      if (Default) fprintf(stdout, "%sEnter %s [%s]: ", spc, expstr, Default);
      else fprintf(stdout, "%sEnter %s:", spc, expstr);
   }
   sp = fgets(str, 512, fpin);
   if ( (sp == NULL) || (str[0] == '\0') || (str[0] == '\n') )
   {
      if (Default) strcpy(str0, Default);
      else str0[0] = '\0';
      return;
   }
   str[len] = '\0';
   for (i=0; str0[i] = str[i]; i++);
   if (i) i--;
   while (Mciswspace(str0[i])) i--;
   while (++i < len) str0[i] = '\0';
   str0[i] = '\0';
}

void GetStrVer(char *def, char *spc, char *expstr, int len, char *str)
{
   int i;

   do
   {
      GetString(stdin, def, spc, expstr, len, str);
      fprintf(stdout, "%sYou have entered '%s'\n", spc, str);
      i = GetIntRange(0, 0, 1, spc, "1 to reenter, 0 to accept");
   }
   while(i);
}

int IsYes(char def, char *spc, char *expstr)
{
   char ch, ln[256];
   fprintf(stdout, "%s%s [%c]: ", spc, expstr, def);
   if (fgets(ln, 256, stdin) == NULL) ch=def;
   else if (ln[0] == '\0' || ln[0] == '\n') ch=def;
   else ch = ln[0];
   return( ((ch == 'y') || (ch == 'Y')) );
}

char GetChar(char def, char *spc, char *expstr)
{
   char ch, ln[256];
   fprintf(stdout, "%s%s [%c]: ", spc, expstr, def);
   if (fgets(ln, 256, stdin) == NULL) ch=def;
   else if (ln[0] == '\0' || ln[0] == '\n') ch=def;
   else ch = ln[0];
   return(ch);
}

int FileIsThere(char *nam)
{
   FILE *fp;

   fp = fopen(nam, "r");
   if (fp == NULL) return(0);
   fclose(fp);
   return(1);
}

#include <stdarg.h>
#define ATL_UseStringVarArgs
void ATL_mprintf(int np, ...)
/*
 * Prints same message to np output file streams
 */
{
   va_list argptr;
   FILE *fp[8];
   char *form;
   int i;
   #ifdef ATL_UseStringVarArgs
      char ln[1024];
   #endif

   if (np > 0)
   {
      va_start(argptr, np);
      assert(np <= 8);
      for (i=0; i < np; i++) fp[i] = va_arg(argptr, FILE *);
      form = va_arg(argptr, char *);
      #ifdef ATL_UseStringVarArgs
         vsprintf(ln, form, argptr);
         assert(strlen(ln) < 1024);/* sanity test only, will not stop overrun */
         va_end(argptr);
         for (i=0; i < np; i++) if (fp[i]) fprintf(fp[i], ln);
      #else
         for (i=0; i < np; i++) if (fp[i]) vfprintf(fp[i], form, argptr);
         va_end(argptr);
      #endif
   }
}

int GetFirstInt(char *ln)
{
   int i, iret=0;
   for (i=0; ln[i]; i++)
   {
      if (isdigit(ln[i]))
      {
         sscanf(ln+i, "%d", &iret);
         break;
      }
   }
   return(iret);
}

int GetLastInt(char *ln)
{
   int i, iret=0;
   for (i=0; ln[i]; i++);
   if (i > 0) for (i--; i > 0 && !isdigit(ln[i]); i--);
   if (i > 0 || (i == 0 && isdigit(ln[0])))
   {
      while(isdigit(ln[i]) && i > 0) i--;
      if (!isdigit(ln[i])) i++;
      sscanf(ln+i, "%d", &iret);
   }
   return(iret);
}

int fNumLines(char *fnam)
{
   FILE *fp;
   char ln[256];
   int i;

   fp = fopen(fnam, "r");
   assert(fp != NULL);
   for (i=0; fgets(ln, 256, fp); i++);
   return(i);
}

char *CmndResults(char *targ, char *cmnd)
{
   static char tnam[128];
   static int FirstTime=1;
   char ln[512];

   if (FirstTime)
   {
      FirstTime = 0;
      assert(tmpnam(tnam));
   }
   if (targ) sprintf(ln, "ssh %s \"%s\" > %s 2>&1 \n", targ, cmnd, tnam);
   else sprintf(ln, "%s > %s 2>&1\n", cmnd, tnam);
   if (!system(ln)) return(tnam);
   else return(NULL);
}

int CmndOneLine(char *targ, char *cmnd, char *ln)
/*
 * executes a system call with contents of cmnd, returns the output in ln;
 * Returns value returned by system call
 * if targ is set, we ssh to that machine
 */
{
   char ln2[512];
   int i;
   FILE *fp;
   char *tnam;

   ln[0] = '\0';
   tnam = CmndResults(targ, cmnd);
   if (tnam)
   {
      fp = fopen(tnam, "r");
      assert(fp);
      if (!fgets(ln, 512, fp)) ln[0] = '\0';
      fclose(fp);
      return(0);
   }
   else ln[0] = '\0';
   return(1);
}

#define BADINT -777938
int GetIntBeforeWord(char *word, char *ln)
/*
 * Finds integer before word in sentence.
 * RETURNS: integer on success, BADINT on failure
 */
{
   char *sp;
   sp = strstr(ln, word);
   if (sp == ln) return(BADINT);
   sp--;
   while(isspace(*sp) && sp != ln) sp--;
   if (sp == ln) return(BADINT);
   while (isdigit(*sp) && sp != ln) sp--;
   if (sp == ln) return(BADINT);
   return(atoi(sp));
}

int GetScreenHeight()
/*
 * Returns the number of vertical lines window has
 */
{
   int i;
   for (i=160; i; i--) fprintf(stdout, "%03d\n", i);
   i = GetIntRange(0, 0, 160, "", "number at top left of screen");
   return(i);
}

void GetEnter(FILE *fpout)
{
   char ln[128];
   fprintf(fpout, "---------- PRESS ENTER TO CONTINUE ---------- ");
   fgets(ln, 128, stdin);
}

int DisplayFile(char *fnam, FILE *fpout, int nlines)
{
   FILE *fp;
   char ln[256];
   int i, GoOn=1;

   fp = fopen(fnam, "r");
   if (fp == NULL)
   {
      fprintf(stderr, "Unable to open file '%s', continuing without display.\n",
              fnam);
      return(-1);
   }
   if (nlines)
   {
      do
      {
         for (i=0; i < nlines; i++)
         {
            GoOn = (int) fgets(ln, 256, fp);
            if (!GoOn) break;
            fprintf(fpout, "%s", ln);
         }
         if (GoOn) GetEnter(stdout);
         else break;
      }
      while(GoOn);
   }
   else while (fgets(ln, 256, fp)) fprintf(fpout, "%s", ln);
   i = ferror(fp);
   fclose(fp);
   return(i);
}

int DisplayFile0(char *fnam, FILE *fpout)
{
   FILE *fp;
   char ln[256];
   int i;

   fp = fopen(fnam, "r");
   if (fp == NULL)
   {
      fprintf(stderr, "Unable to open file '%s', continuing without display.\n",
              fnam);
      return(-1);
   }
   while (fgets(ln, 256, fp)) fprintf(fpout, "%s", ln);
   i = ferror(fp);
   fclose(fp);
   return(i);
}

int FoundInFile(char *fnam, char *str)
{
   FILE *fp;
   int found=0;
   char ln[256];

   fp = fopen(fnam, "r");
   assert(fp);
   while (fgets(ln, 256, fp))
   {
      if (strstr(ln, str))
      {
         found=1;
         break;
      }
   }
   fclose(fp);
   return(found);
}

int FindFiles(char *dir, char *fnam, char ***files0)
{
   int i, j;
   FILE *fp;
   char ln[256];
   static int FirstTime=1;
   static char *fpp[32], files[32][256], tnam[128], fnd[128];

   if (FirstTime)
   {
      if (ISWIN)
      {
         if (FileIsThere("/usr/bin/find.exe")) strcpy(fnd, "/usr/bin/find");
         else if (FileIsThere("/bin/find.exe")) strcpy(fnd, "/bin/find");
         else if (FileIsThere("/usr/local/bin/find.exe"))
            strcpy(fnd, "/usr/local/bin/find");
         else strcpy(fnd, "find");
      }
      else
      {
         if (FileIsThere("/usr/bin/find")) strcpy(fnd, "/usr/bin/find");
         else if (FileIsThere("/bin/find")) strcpy(fnd, "/bin/find");
         else if (FileIsThere("/usr/local/bin/find"))
            strcpy(fnd, "/usr/local/bin/find");
         else strcpy(fnd, "find");
      }
      for (i=0; i < 32; i++) fpp[i] = files[i];
      assert(tmpnam(tnam));
      FirstTime=0;
   }
   *files0 = fpp;
   sprintf(ln, "%s %s/ -name \'%s\' 2> /dev/null > %s\n", fnd, dir, fnam, tnam);
   i = system(ln);
   fp = fopen(tnam, "r");
   if (fp)
   {
      for (i=0; i < 32; i++)
      {
         if (fgets(files[i], 256, fp) == NULL) break;
         for (j=0; files[i][j]; j++) if (files[i][j] == '\n') files[i][j] = ' ';
      }
      fclose(fp);
   }
   else
   {
      fprintf(stderr, "Find not working, error code %d\n", i);
      return(0);
   }
   remove(tnam);
   if (ISWIN && i == 0) /* for windows, search for differing file extensions */
   {
      if (!strstr(fnam, "\\.")) /* add .exe to files with no extension */
      {
         sprintf(ln, "%s\\.exe", fnam);
         i = FindFiles(dir, ln, files0);
      }
      else if (strstr(fnam, "\\.a")) /* search for .lib for libraries */
      {
         for (i=0; fnam[i] != '.'; i++) ln[i] = fnam[i];
         ln[i] = '.'; ln[i+1] = 'l'; ln[i+2] = 'i'; ln[i+3] = 'b';
         ln[i+4] = '\0';
         i = FindFiles(dir, ln, files0);
      }
   }
   return(i);
}

#define ISSEP(ch_) ( (ch_) == ':' )
int GetAllPaths(char ***mypaths0)
{
   char sep = ':';
   int i, j=0, np=1;
   char **mpaths, *sp;

   sp = getenv("PATH");
   for (i=0; sp[i]; i++) if (ISSEP(sp[i])) np++;
   mpaths = malloc(np * sizeof(char *));
   assert(mpaths);
   for (j=0; j < np; j++)
   {
      for (i=0; ( sp[i] && !ISSEP(sp[i]) ); i++);
      mpaths[j] = malloc((i+1)*sizeof(char));
      assert(mpaths[j]);
      strncpy(mpaths[j], sp, i);
      mpaths[j][i] = '\0';
      sp += i+1;
   }
   *mypaths0 = mpaths;
   return(np);
}

char **AddToPaths(int npaths0, char **paths0, int nnew, ...)
{
   va_list argptr;
   char **paths, *pp;
   int i, j, np = npaths0 + nnew;

   assert(npaths0 >= 0 && nnew >= 0);
   va_start(argptr, nnew);
   paths = malloc(np * sizeof(char*));
   for (i=0; i != npaths0; i++) paths[i] = paths0[i];
   free(paths0);
   for (i=0; i != nnew; i++)
   {
      pp = va_arg(argptr, char *);
      j = strlen(pp)+1;
      paths[i+npaths0] = malloc(j*sizeof(char*));
      assert(paths[i+npaths0]);
      strcpy(paths[i+npaths0], pp);
   }
   va_end(argptr);
   return(paths);
}

void KillPaths(int npaths, char **paths)
{
   int i;

   for (i=0; i < npaths; i++) free(paths[i]);
   free(paths);
}

int FindFirstInPath(char *nam, char *fnd)
/*
 * finds first instance of nam in your path, returns 0 if not found, 1 else
 */
{
   char ln[256], *sp, *cp;
   FILE *fp;

   sp = getenv("PATH");
   cp = ln;
   do
   {
      while (*sp && *sp != ':') *cp++ = *sp++;
      if (*sp) sp++;
      if (cp[-1] != '/') *cp++ = '/';
      strcpy(cp, nam);
      fp = fopen(ln, "r");
      if (ISWIN) /* for Windows, look for default file extensions */
      {
         if (!strstr(ln, "."))  /* add exe to files with no extension */
         {
            strcat(ln, ".exe");
            fp = fopen(ln, "r");
         }
      }
      if (fp != NULL)
      {
         fclose(fp);
         strcpy(fnd, ln);
         return(1);
      }
      cp = ln;
   }
   while(*sp);
   return(0);
}


int GetIntVers(char *str, int *nskip)
{
   char ln[64];
   int i, j;

   *nskip = 0;
   for (i=0; str[i] && !isdigit(str[i]); i++);  /* skip non-digits */
   if (str[i])
   {
      for (j=0; j < 64 && str[j+i] && isdigit(str[j+i]); j++) ln[j] = str[j+i];
      ln[j] = '\0';
      if (j)
      {
         *nskip = i+j;
         return(atoi(ln));
      }
   }
   return(-1);
}

void GetGccVers(char *gcc, int *comp, int *major, int *minor, int *patch)
/*
 * comp: 0: gcc;  1: egcs;  2: pgcc
 */
{
   char ln[512], ln2[512];
   int i, j;

   *comp = *major = *minor = *patch = -1;
   sprintf(ln, "%s --version", gcc);
   if (CmndOneLine(NULL, ln, ln2) == 0)
   {
      *major = GetIntVers(ln2, &j);
      if (*major != -1)
      {
         *minor = GetIntVers(ln2+j, &i); j += i;
         if (*minor != -1)
         {
            *patch = GetIntVers(ln2+j, &i); j += i;
            if (strstr(ln2, "egcs")) *comp = 1;
            else if (strstr(ln2, "pgcc")) *comp = 2;
            else *comp = 0;
         }
      }
   }
}

int IsGoodGcc(enum OSTYPE OS, enum MACHTYPE mach, FILE *fpout, FILE *fplog,
              char *gcc)
{
   int iret = 0;
   int comp, major, minor, patch;

   GetGccVers(gcc, &comp, &major, &minor, &patch);
   ATL_mprintf(2, fplog, fpout, "   %s : v%d.%d.%d\n",
               gcc, major, minor, patch);
   if (major == -1)
      iret = 0;
   else if ( MachIsX86(mach) ) /* don't use gcc 2.96 - 3.0 */
   {
      if ((major == 2 && minor < 96) || (major == 3 && minor > 1))
         iret = 1;
   }
   else if (mach == IA64Itan || mach == IA64Itan2 || MachIsUS(mach) ||
            mach == Dec21164 || mach == Dec21264)
   {
      if (major == 3)
         iret = 1;
   }
/*
 * Need 3.2 or newer to make altivec work
 */
   else if (mach == PPCG4 && OS == OSLinux)
   {
      if (major >= 3 && minor >= 3)
         iret = 1;
   }
   else iret = 1; /* any gcc is fine, take first in path */
   return (iret);
}

int FindGoodGcc(enum OSTYPE OS, enum MACHTYPE mach, FILE *fpout, FILE *fplog,
                char *goodgcc)
/*
 * returns: 0: good gcc found;  1: bad gcc found
 */
{
   char *tmpc = "CONFIG/tst.c";
   char *tnam = "CONFIG/tmp.t";
   char **files;
   char *gccs[2] = {"gcc", "gcc-2"};
   char ln[512], ln2[512];
   char **dirs;
   int i, d, n, ND, comp, major, minor, patch, ig, ng;
   FILE *fp;

   remove(tnam);
   if (*goodgcc != '\0')
   {
      if (IsGoodGcc(OS, mach, fpout, fplog, goodgcc))
         return(0);
   }
   ND = GetAllPaths(&dirs);
   dirs = AddToPaths(ND, dirs, 2, "/usr/bin", "usr/local");
   ND += 2;
   *goodgcc = '\0';
   for (d=0; d < ND; d++)
   {
      for (ng = (OSIsWin(OS)) ? 2 : 1, ig=0; ig < ng; ig++)
      {
         n = FindFiles(dirs[d], gccs[ig], &files);
         for (i=0; i < n; i++)
         {
            if (IsGoodGcc(OS, mach, fpout, fplog, files[i]))
            {
               strcpy(goodgcc, files[i]);
               return(0);
            }
         }
      }
   }
   return(1);
}

void FindTarZip(char *tar, char *gzip, char *gunzip)
{
   char *fw[3] = {"tar", "gzip", "gunzip"};
   char *fn[3];
   char **dirs, **files;
   int i, n, d, ND;

   fn[0] = tar; fn[1] = gzip; fn[2] = gunzip;
   ND = GetAllPaths(&dirs);
   dirs = AddToPaths(ND, dirs, 3, "/bin", "/usr/bin", "usr/local");
   ND += 3;
   *tar = *gzip = *gunzip = '\0';
   for (d=0; d < ND; d++)
   {
      for (i=0; i < 3; i++)
      {
         if (fn[i][0] == '\0')
         {
            n = FindFiles(dirs[d], fw[i], &files);
            if (n) strcpy(fn[i], files[0]);
         }
      }
      if (fn[0][0] != '\0' && fn[1][0] != '\0' && fn[2][0] != '\0') break;
   }
   if (*tar == '\0') strcpy(tar, "tar");
   if (*gzip == '\0') strcpy(gzip, "gzip");
   if (*gunzip == '\0') strcpy(gunzip, "gunzip");
}

int Wstrfndsub(char *fnd, char *sub, char *in, char *out)
/*
 * copies IN to OUT, replacing the first occurrence of FND with SUB
 * 1 if FND was found, 0 otherwise
 */
{
   char *sp;
   int i, j;

   sp = strstr(in, fnd);
   if (sp)
   {
      for (i=0; &in[i] != sp; i++) out[i] = in[i];
      for (j=i; out[j] = sub[j-i]; j++);
      for (i += strlen(fnd); out[j] = in[i]; j++, i++);
   }
   else strcpy(out, in);
   return(sp != NULL);
}

#define F2C_assert(cond_) \
{ \
   if (!(cond_)) \
   { \
      fprintf(stderr, "Assertion %s failed, line %d of %s\n", \
              Mstr(cond_), __LINE__, __FILE__); \
      fprintf(stderr, "Unable to figure F2C data\n\n"); \
      return; \
   } \
}

int findSolErr(FILE *fplog, char *redir, char *TOPdir, char *CC, char *CCFLAGS,
               char *MCC, char *MMFLAGS, char *XCC, char *XCCFLAGS)
{
   char *sp, *tnam = "CONFIG/config.tmp";
   char compdef[512], ln[640];
   int i;
   FILE *fp;

   sprintf(ln, "cd CONFIG ; make IGetSunVers %s\n", redir);
   system(ln);
   fp = fopen(tnam, "r");
   fgets(ln, 256, fp);
   fclose(fp);
   remove(tnam);
   for (i=0; !isdigit(ln[i]) && ln[i]; i++);
   if (isdigit(ln[i]))
   {
      fprintf(stdout, "   cc major version number: %c\n", ln[i]);
      if (ln[i] == '4')
      {
         sp = strstr(MMFLAGS, "-xO2");
         if (sp) sp[3] = '5';
      }
   }
   else fprintf(stdout, "   unable to find version in \'%s\'\n", ln);
   DisplayFile("CONFIG/SolErr.txt", stdout, NLINES);
   return(IsYes('y', "", "Use gcc for Level 3 BLAS compilation?"));
}

void findF2C(char *redir, char *targ, char *TOPdir, char *F77, char *F77FLAGS,
             char *FLINK0, char *FLFLAGS0, char *CC, char *CCFLAGS,
             enum F2CNAME *f2cnam, enum F2CINT *f2cint, enum F2CSTRING *f2cstr)
{
   char *namdef[4] = {"Add_", "Add__", "NoChange", "UpCase"};
   char *intdef[4]={"F77_INTEGER=int", "F77_INTEGER=long", "F77_INTEGER=short"};
   char *strdef[4] = {"SunStyle", "CrayStyle", "StructVal", "StructPtr"};
   char compdef[1024], ln[1024];
   char FLINK[512], FLFLAGS[512];
   char *tnam = "CONFIG/config.tmp";
   int i;
   FILE *fp;

   *f2cnam = f2c_NamErr;
   *f2cint = f2c_IntErr;
   *f2cstr = f2c_StrErr;
   Wstrfndsub("$(F77)", F77, FLINK0, FLINK);
   Wstrfndsub("$(F77FLAGS)", F77FLAGS, FLFLAGS0, FLFLAGS);
/*
 * Find naming strategy
 */
   if (targ)
      sprintf(compdef, "F77=%s F77FLAGS=\"%s\" FLINK=%s FLFLAGS=\"%s\" CC=%s CCFLAGS=\"%s\" mydir=%s/CONFIG tnam=%s atlrun=atlas_runX targ=%s",
              F77, F77FLAGS, FLINK, FLFLAGS, CC, CCFLAGS, TOPdir, tnam, targ);
   else
      sprintf(compdef, "F77=%s F77FLAGS=\"%s\" FLINK=%s FLFLAGS=\"%s\" CC=%s CCFLAGS=\"%s\" mydir=%s/CONFIG tnam=%s",
              F77, F77FLAGS, FLINK, FLFLAGS, CC, CCFLAGS, TOPdir, tnam);
   sprintf(ln, "cd CONFIG ; make IRunName %s %s\n", compdef, redir);
   F2C_assert(system(ln) == 0);
   fp = fopen(tnam, "r");
   F2C_assert(fp);
   fgets(ln, 256, fp);
   fclose(fp);
   remove(tnam);
   if (strstr(ln, "Add__")) *f2cnam = f2c_Add__;
   else if (strstr(ln, "Add_")) *f2cnam = f2c_Add_;
   else if (strstr(ln, "NoChange")) *f2cnam = f2c_NoChange;
   else if (strstr(ln, "UpCase")) *f2cnam = f2c_UpCase;
   else return;
/*
 * Finding integer correspondence
 */
   if (targ)
      sprintf(compdef, "F77=%s F77FLAGS=\"%s\" FLINK=%s FLFLAGS=\"%s\" CC=%s CCFLAGS=\"%s -D%s\" mydir=%s/CONFIG tnam=%s atlrun=atlas_runX targ=%s",
              F77, F77FLAGS, FLINK, FLFLAGS, CC, CCFLAGS, namdef[*f2cnam],
              TOPdir, tnam, targ);
   else
      sprintf(compdef, "F77=%s F77FLAGS=\"%s\" FLINK=%s FLFLAGS=\"%s\" CC=%s CCFLAGS=\"%s -D%s\" mydir=%s/CONFIG tnam=%s",
              F77, F77FLAGS, FLINK, FLFLAGS, CC, CCFLAGS, namdef[*f2cnam],
              TOPdir, tnam);
   sprintf(ln, "cd CONFIG ; make IRunInt %s %s\n", compdef, redir);
   F2C_assert(system(ln) == 0);
   fp = fopen(tnam, "r");
   F2C_assert(fp);
   fgets(ln, 256, fp);
   fclose(fp);
   remove(tnam);
   if (strstr(ln, "short")) *f2cint = FintCshort;
   else if (strstr(ln, "int")) *f2cint = FintCint;
   else if (strstr(ln, "long")) *f2cint = FintClong;
   else return;
/*
 * Finding string handling
 */
   if (targ)
      sprintf(compdef, "F77=%s F77FLAGS=\"%s\" FLINK=%s FLFLAGS=\"%s\" CC=%s mydir=%s/CONFIG tnam=%s atlrun=atlas_runX targ=%s",
              F77, F77FLAGS, FLINK, FLFLAGS, CC, TOPdir, tnam, targ);
   else
      sprintf(compdef,
"F77=%s F77FLAGS=\"%s\" FLINK=%s FLFLAGS=\"%s\" CC=%s mydir=%s/CONFIG tnam=%s",
              F77, F77FLAGS, FLINK, FLFLAGS, CC, TOPdir, tnam);
   for (i=0; i < 5; i++)
   {
      sprintf(ln,
              "cd CONFIG ; make IRunStr %s CCFLAGS=\"%s -D%s -D%s -D%s\" %s\n",
              compdef, CCFLAGS, strdef[i], intdef[*f2cint], namdef[*f2cnam],
              redir);
      if (system(ln) == 0)
      {
         fp = fopen(tnam, "r");
         F2C_assert(fp);
         fgets(ln, 256, fp);
         fclose(fp);
         remove(tnam);
         if (strstr(ln, strdef[i]))
         {
            *f2cstr = i;
            return;
         }
      }
   }
}

#undef F2C_assert

#define ND 3
void FindBlas(FILE *fpout, FILE *fplog, char *redir, char *F77, char *F77FLAGS,
              char *BLASlib)
{
   char **files;
   char *dirs[ND] = {"/usr/lib", "/usr/local", ""};
   char ln[256];
   int i, j, n;

   ATL_mprintf(2, fplog, fpout, "Looking for BLAS (this may take a while):\n");
   if (*BLASlib)
   {
      sprintf(ln,
      "cd CONFIG ; make IBlasLink F77=\"%s\" F77FLAGS=\"%s\" BLASlib=\"%s\" %s\n",
              F77, F77FLAGS, BLASlib, redir);
      if (system(ln) == 0)
      {
         ATL_mprintf(2, fplog, fpout, "BLASlib set to %s.\n", BLASlib);
         return;
      }
      ATL_mprintf(2, fplog, fpout, "   Link failed, %s rejected\n", BLASlib);
   }
   dirs[ND-1] = getenv("HOME");
   for (j=0; j < ND; j++)
   {
      n = FindFiles(dirs[j], "lib*blas*.a", &files);
      if (n)
      {
         for (i=0; i < n; i++)
         {
            sprintf(ln,
      "cd CONFIG ; make IBlasLink F77=\"%s\" F77FLAGS=\"%s\" BLASlib=\"%s\" %s\n",
                    F77, F77FLAGS, files[i], redir);
            if (system(ln) == 0)
            {
               strcpy(BLASlib, files[i]);
               ATL_mprintf(2, fplog, fpout, "BLASlib set to %s.\n", BLASlib);
               return;
            }
            ATL_mprintf(2, fplog, fpout,
                        "   Link failed, %s rejected\n", files[i]);
         }
      }
   }
   ATL_mprintf(2, fplog, fpout,
               "Unable to find usable BLAS, BLASlib left blank.\n");
   BLASlib[0] = '\0';
}
#undef ND

char *FindUname()
{
   static int FirstTime=1;
   static char unam[64];
   if (FirstTime)
   {
      if (FileIsThere("/bin/uname")) strcpy(unam, "/bin/uname");
      else if (FileIsThere("/usr/bin/uname")) strcpy(unam, "/usr/bin/uname");
      else strcpy(unam, "uname");
      FirstTime = 0;
   }
   return(unam);
}

enum OSTYPE GetOS(FILE *fpout, FILE *fplog, char *targ)
{
   int ierr;
   char ln[512], ln2[128];
   enum OSTYPE i, OS;
   char *unam = FindUname();

   ATL_mprintf(2, fplog, fpout,
               "Probing to make operating system determination:\n");

   sprintf(ln2, "%s -s", unam);
   ierr = CmndOneLine(targ, ln2, ln);
   if (ierr == 0)
   {
      if(strstr(ln, "Linux")) OS = OSLinux;
      else if(strstr(ln, "FreeBSD")) OS = OSFreeBSD;
      else if (strstr(ln, "Darwin")) OS = OSOSX;
      else if(strstr(ln, "SunOS"))
      {
         sprintf(ln2, "%s -r", unam);
         CmndOneLine(targ, ln2, ln);
         if (ln[0] == '4') OS = OSSunOS4;
         else OS = OSSunOS;
      }
      else if(strstr(ln, "OSF1")) OS = OSOSF1;
      else if(strstr(ln, "IRIX")) OS = OSIRIX;
      else if(strstr(ln, "AIX")) OS = OSAIX;
      else if(strstr(ln, "WIN"))
      {
         if (strstr(ln, "95") || strstr(ln, "98") || strstr(ln, "_ME"))
            OS = OSWin9x;
         else if (strstr(ln, "NT")) OS = OSWinNT;  /* check this */
         else ierr = 1;
      }
      else if (strstr(ln, "HP-UX")) OS = OSHPUX;
      else ierr = 1;
   }
   if (ierr)
   {
      fprintf(stdout,
         "%s -s does not appear to work.\n", unam);
      for (i=0; i <= OSOther; i++) fprintf(stdout, " %3d. %s\n", i+1, osnam[i]);
      OS = (enum OSTYPE) GetIntRange(OSOther+1, 1, OSOther+1, "",
                                     "the number of your operating system") - 1;
   }
   ATL_mprintf(2, fplog, stdout,
               "Operating system configured as %s\n\n", osnam[OS]);

   return(OS);
}

char *TryComp(FILE *fpout, FILE *fplog, char *redir, char lang, int np,
              char **paths, char *comp, char *flags)
{
   static char ln2[512];
   char **matches, ln[512];
   int p, n, i;

   if (*comp != '/') /* if we don't already have explicit path */
   {
/*
 *    Try user's path first
 */
      if (FindFirstInPath(comp, ln2))
      {
         sprintf(ln, "cd CONFIG ; make %cTryComp COMP=\"%s\" FLAGS=\"%s\" %s\n",
                 lang, ln2, flags, redir);
         if (system(ln) == 0) return(ln2);
         ATL_mprintf(2, fplog, fpout, "   %s %s rejected\n", ln2, flags);
      }
/*
 *    A separate find on each directory, in order of precedence
 */
      for (p=0 ; p < np; p++)
      {
/*
 *       Try every compiler that is found, search backwards so bigger version
 *       numbers will be tried first
 */
         n = FindFiles(paths[p], comp, &matches);
         for (i=n-1; i >= 0; i--)
         {
            sprintf(ln,
                    "cd CONFIG ; make %cTryComp COMP=\"%s\" FLAGS=\"%s\" %s\n",
                    lang, matches[i], flags, redir);
            if (system(ln) == 0) return(matches[i]);
            ATL_mprintf(2, fplog, fpout, "   %s rejected\n", matches[i]);
         }
      }
   }
/*
 * Try without explicit path if nothing else works
 */
   sprintf(ln, "cd CONFIG ; make %cTryComp COMP=\"%s\" FLAGS=\"%s\" %s\n",
           lang, comp, flags, redir);
   if (system(ln) == 0) return(comp);
   ATL_mprintf(2, fplog, fpout, "   %s %s rejected\n", comp, flags);

   return(NULL);
}


int MakeWinComp(enum WINCOMP wc, char *def)
/*
 * returns: error return from make command
 */
{
   char ln[512], ln2[1024];

   DisplayFile("CONFIG/wincomp.txt", stdout, NLINES);
   GetStrVer(def, "", "path to windows compiler", 511, ln);
   sprintf(ln2,
"cd CONFIG; make win%s wcname=/usr/local/bin/ATLwin_%s wcexe='-DDEFDF=\"%s\"'",
           WCIsF77(wc) ? "f77" : "cc", wcnam[wc], ln);
   return(system(ln2));
}

int CheckWinComp(enum WINCOMP wc)
/*
 * Returns 0 if test pass, non-zero on failure
 */
{
   char ln[512];
   sprintf(ln, "cd CONFIG ; make %cTryComp COMP=\"ATLwin_%s\" FLAGS=\"\"",
           WCIsF77(wc) ? 'f' : 'c', wcnam[wc]);
   return(system(ln));
}

int CreateWinComp(enum WINCOMP wc)
/*
 * Returns 0 on success, 1 on failure
 */
{
   char ln[256];
   int ierr, FAIL;
   char *wcn[4] = {"DF", "CL", "IFORT", "ICL"};
   char *defp[4] =
      {"c:/Program Files/Microsoft Visual Studio/DF98/BIN/DF.EXE",
       "c:/Program Files/Microsoft Visual Studio/VC98/BIN/CL.EXE",
       "c:/Program Files/Intel/CPP/Compiler80/IA32/BIN/ICL.EXE",
       "c:/Program Files/Intel/Fortran/Compiler80/IA32/BIN/IFORT.EXE"};

   sprintf(ln, "/usr/local/bin/ATLwin_%s.exe", wcnam[wc]);
   if (FileIsThere(ln))
   {
      fprintf(stdout, "You've already got a %s:\n", ln);
      if (!GetIntRange(0, 0, 1, "", "0 uses it, 1 to overwrite with new"))
         return(0);
   }
   assert(wc < WCgcc);
   do
   {
      FAIL = 1;
      ierr = MakeWinComp(wc, defp[wc]);
      if (!ierr)
         ierr = CheckWinComp(wc);
      if (ierr)
      {
         fprintf(stdout, "\nCompiler not working:\n");
         ierr = GetIntRange(1, 0, 1, "", "0 skips, 1 to reenter/try again");
         sprintf(ln, "rm -f /usr/local/bin/ATLwin_%s.exe", wcnam[wc]);
         system(ln);
      }
      else FAIL = 0;
   }
   while (ierr);
   if (!FAIL)
      fprintf(stdout, "Compiler wrapper successfully created.\n\n");
   else
      fprintf(stdout, "Compiler wrapper not created.\n\n");
   return(FAIL);
}


void GetSyslib(enum OSTYPE OS, enum MACHTYPE mach, char *targ, char *LIBS)
{
   if (THREADS) strcpy(LIBS, "-lpthread -lm");
   else strcpy(LIBS, "-lm");

   switch(OS)
   {
   case OSOSX:  /* don't know answer */
   case OSFreeBSD:  /* don't know answer */
   case OSLinux:
      break;
   case OSSunOS:
      break;
   case OSSunOS4:
      assert(!THREADS);
      break;
   case OSOSF1:
      break;
   case OSIRIX:
      /* if (THREADS) strcpy(LIBS, "-lm"); */
      /* if (THREADS) strcpy(LIBS, "-lpthread -lexc -lm"); */ /* for gcc */
      break;
   case OSAIX:
      if (THREADS) strcpy(LIBS, "-lpthreads -lm");
      break;
   case OSHPUX:  /* don't know answer for threads */
      break;
   case OSWinNT:  /* don't know answer for threads */
      if (THREADS) strcpy(LIBS, "-lpthread");
      else strcpy(LIBS, "");
      break;
   case OSWin9x:
      assert(!THREADS);
      strcpy(LIBS, "");
      break;
   }
}

char *GetFCLFLAGS(enum OSTYPE OS, enum MACHTYPE mach, char *F77)
/*
 * returns flags necessary to make f77 compiler link when main is in C
 */
{
   static char ln[256];
   ln[0] = '\0';
   switch(OS)
   {
   case OSLinux:
   case OSWinNT:
      if (strstr(F77, "ifort")) strcpy(ln, "-nofor_main");
      break;
   case OSOSF1:
      if (!strstr(F77, "g77")) strcpy(ln, "-nofor_main");
      break;
   default:;
   }
   return(ln);
}

char *NameFromPath(char *path)
/*
 * Tries to get a executable name out of the path.
 * RETURNS: ptr to string containing executable name.
 */
{
   int i;
   static char name[128];

   if (!path) return(NULL);
   if (path[0] == '\0') return(NULL);

   for (i=0; path[i]; i++);
   for (; i && path[i] != '/'; i--);
   if (path[i] == '/') i++;
   path += i;
   for (i=0; name[i] = path[i]; i++);
/*
 * Strip trailing whitespace
 */
   for (i--; i >= 0 && isspace(name[i]); i--) name[i] = '\0';
/*
 * Remove .exe for windows
 */
   if (i >= 4 && tolower(name[i]) == 'e' && tolower(name[i-1]) == 'x' &&
       tolower(name[i-2] == 'e') && name[i-3] == '.')
      name[i-3] = '\0';

   return(name);
}

void KillBegEndSpaces(char *str)
{
   int i, j;
/*
 * Strip trailing blanks
 */
   for (i=0; str[i]; i++);
   for (i--; i >= 0 && isspace(str[i]); i--)
        str[i] = '\0';
/*
 * Strip leading blanks
 */
   if (isspace(*str))
   {
      for (j=1; isspace(str[j]); j++);
      for (i=0; str[i] = str[j]; i++,j++);
   }
}

void GetBlasTry(enum OSTYPE OS, enum MACHTYPE mach, char *blas)
{
   if (!blas) return;
   else if (*blas != '\0') return;
   switch(OS)
   {
   case OSAIX:
      if (mach == IbmTPwr2) strcpy(blas, "-esslp2");
      else strcpy(blas, "-essl");
      break;
   case OSSunOS:
      strcpy(blas, "-xlic_lib=sunperf");
      break;
   case OSOSF1:
      strcpy(blas, "-lcxml");
      break;
   case OSIRIX:
      strcpy(blas, "-lblas");
      break;
   default:
      *blas = '\0';
   }
/*
 * put a special case for MKL in here later
 */
}
char *GetLinkInfo(enum OSTYPE OS, enum MACHTYPE mach, char *F77, char *CC,
                  char *FLINK, char *FLFLAGS, char *FCLFLAGS,
                  char *CLINK, char *CLFLAGS)
{
/*
 * Generic linker settings
 */
   if (CLINK) strcpy(CLINK, "$(CC)");
   if (CLFLAGS) strcpy(CLFLAGS, "$(CCFLAGS)");
   if (FLINK) strcpy(FLINK, "$(F77)");
   if (FLFLAGS) strcpy(FLFLAGS, "$(F77FLAGS)");
   if (FCLFLAGS) strcpy(FCLFLAGS, "$(FLINKFLAGS)");
/*
 * Exceptions
 */
   if (mach == HP9K735 && OS == OSHPUX)
   {
      if (FLFLAGS && strstr(F77, "f77")) strcpy(FLFLAGS, "-Aa");
      if (CLFLAGS && !strstr(CC, "gcc") && strstr(CC, "cc"))
         strcpy(CLFLAGS, "-Aa");
   }
   if (FLFLAGS && strstr(F77, "xlf")) strcat(FLFLAGS, " -bmaxdata:0x70000000");
   if (CLFLAGS && strstr(CC, "xlc")) strcat(CLFLAGS, " -bmaxdata:0x70000000");
/*
 * See if we need special flags for linking C main progs using FLINK
 */
   if (FCLFLAGS)
   {
      if (strstr(F77, "ifort") || (OS == OSOSF1 && strstr(F77, "f77")))
         strcat(FCLFLAGS, " -nofor_main");
   }
}

#define COMPLEN 256
enum WCOMP {CPF77, CPCC, CPMM};
char *GuessCompInfo(enum OSTYPE OS, enum MACHTYPE mach, enum WCOMP which,
                    char *targ, char *sugg)
/*
 * Suggests decent generic default compiler and flags from whatever info
 * is available.  If (sugg) is set, return info about this compiler if
 * we know about it.  Note that this function does *not* replicate info
 * in GetDefCompInfo, so only call it when GetDefCompInfo fails.
 * RETURNS: Ptr to a string, with the first COMPLEN characters holding
 *          the compiler name, and the remainder the flags.
 */
{
   static char ln[1024];
   char *comp, *flag;
   char *sp;
   int i, WANTGCC=0;

   comp = ln;
   flag = ln + COMPLEN;

   if (sugg) WANTGCC = (strstr(sugg, "gcc") || strstr(sugg, "g77"));
/*
 * Universal wild-ass guess for everybody
 */
   strcpy(flag, "-O");
   if (which == CPF77) strcpy(comp, "g77");
   else strcpy(comp, "gcc");
/*
 * See if the user wants Intel compilers
 */
   if ( sugg && (strstr(sugg, "icc") || strstr(sugg, "ifort")) )
   {
      strcpy(comp, sugg);
      if (OSIsWin(OS))
      {
         if (mach == IA64Itan)
         {
            if (which == CPF77) strcpy(flag, "-G1 -O2");
            else if (which == CPCC) strcpy(flag, "-G1 -O3");
            else strcpy(flag, "-G1 -O3 -mp1 -Oa");
         }
         else
         {
            if (which == CPF77) strcpy(flag, "-O2 -mp1");
            else if (which == CPCC) strcpy(flag, "-O3 -mp1");
            else strcpy(flag, "-O3 -mp1 -Oa");
         }
      }
      else
      {
         if (mach == IA64Itan)
         {
            if (which == CPF77) strcpy(flag, "-tpp1 -O2 -mp1 -static");
            else if (which == CPCC) strcpy(flag, "-tpp1 -O3 -mp1 -static");
            else strcpy(flag, "-tpp1 -O3 -mp1 -static -fno-alias");
         }
         else
         {
            if (which == CPF77) strcpy(flag, "-O2 -mp1 -static");
            else if (which == CPCC) strcpy(flag, "-O3 -mp1 -static");
            else strcpy(flag, "-O3 -mp1 -static -fno-alias");
         }
      }
   }
/*
 * See if we are using CVF
 */
   else if ( sugg && strstr(sugg, "ATLwin_vf") )
   {
      assert(which == CPF77);
      strcpy(comp, sugg);
      strcpy(flag,
"-iface=nomixed_str_len_arg -fast -assume:accuracy_sensitive -fltconsistency");
   }
/*
 * See if we are using MVC
 */
   else if ( sugg && strstr(sugg, "ATLwin_vc") )
   {
      assert(which != CPF77);
      strcpy(comp, sugg);
      if (which == CPCC)
         strcpy(flag, "-O2 -Oy -Op -Za -nologo");
      else
         strcpy(flag, "-O1 -Oy -Oa -nologo");
   }
   else
   {
      switch(OS)
      {
      case OSFreeBSD:
         if (which == CPF77)
            strcpy(comp, "f77");
      case OSLinux:
         if (which == CPCC)
            strcpy(flag, "-fomit-frame-pointer -O3 -funroll-all-loops");
         else if (which == CPMM)
            strcpy(flag, "-fomit-frame-pointer -O");
         if (THREADS && OS == OSFreeBSD)
         {
            if (which == CPF77)
               strcat(flag, " -pthread");
            else
               strcat(flag, " -pthread -D_REENTRANT -D_THREAD_SAFE");
         }
         break;
      case OSSunOS4:
         if (which == CPF77)
         {
            if (!WANTGCC)
            {
               strcpy(comp, "f77");
               strcpy(flag, "-dalign -O4 -fast");
            }
         }
         else
         {
            strcpy(comp, "gcc");
            if (which == CPCC)
               strcpy(flag, "-fomit-frame-pointer -O3 -funroll-all-loops");
            else strcpy(flag, "-fomit-frame-pointer -O");
         }
         break;
      case OSSunOS:
         if (WANTGCC)
         {
            if (mach == SunUSX)
            {
               strcpy(flag,
                  "-mcpu=ultrasparc -mtune=ultrasparc -fomit-frame-pointer -O");
               if (which == CPCC) strcat(flag, "3");
            }
         }
         else
         {
            if (which == CPF77)
            {
               strcpy(comp, "f77");
               strcpy(flag, "-dalign -native -xO5");
            }
            else if (which == CPCC)
            {
               strcpy(comp, "cc");
               if (which == CPCC) strcpy(flag, "-dalign -fsingle -xO5 -native");
               else strcpy(flag, "-dalign -fsingle -xO2 -native");
            }
            if (THREADS) strcat(flag, " -mt");
         }
         break;
      case OSOSF1:
         if (!WANTGCC)
         {
            if (which == CPF77)
            {
               strcpy(comp, "f77");
               strcpy(flag, "-O5 -arch host -tune host");
            }
            else
            {
               strcpy(comp, "cc");
               strcpy(flag, "-arch host -tune host -std -O5");
            }
         }
         break;
      case OSIRIX:
         if (WANTGCC) break;
         sprintf(flag, "%s -m", FindUname());
         assert(CmndOneLine(targ, flag, comp) == 0);
         sp = strstr(ln, "IP");
         for (i=2; isdigit(sp[i]); i++);
         sp[i] = '\0';
         if (which == CPF77)
         {
            sprintf(flag,
         "-O3 -64 -OPT:Olimit=15000 -TARG:platform=%s -LNO:blocking=OFF", sp);
            strcpy(comp, "f77");
         }
         else
         {
            sprintf(flag, "-O3 -64 -OPT:Olimit=15000 -TARG:platform=%s -LNO:blocking=OFF -LOPT:alias=typed", sp);
            strcpy(comp, "cc");
         }
         break;
      case OSAIX :
         if (WANTGCC) break;
         if (which == CPF77) strcpy(comp, "xlf");
         else strcpy(comp, "xlc");
         if (which == CPMM) strcpy(flag, "-O -qmaxmem=-1 -qfloat=hsflt");
         else strcpy(flag, "-O3 -qmaxmem=-1 -qfloat=hsflt");
         if (THREADS) strcat(comp, "_r");
         break;
      case OSWin9x:
      case OSWinNT:
         if (which == CPF77)
            strcpy(flag, "-fomit-frame-pointer -funroll-all-loops -O3");
         else if (which == CPCC)
            strcpy(flag,
                   "-fomit-frame-pointer -O3 -funroll-all-loops");
         else strcpy(flag, "-fomit-frame-pointer -O");
      case OSHPUX:
         if (WANTGCC) break;
         if (which == CPF77)
         {
            strcpy(comp, "f77");
            strcpy(flag, "+O2");
         }
         else
         {
            strcpy(comp, "cc");
            if (which == CPCC)
               strcpy(flag, "-D_INCLUDE_POSIX_SOURCE -DUseTimes -Aa +O2");
            else
               strcpy(flag,
                  "-Aa +O2 +Onoinitcheck +Odataprefetch +Optrs_strongly_typed");
         }
      default:;
      }
   }
   return(ln);
}

char *GetDefCompInfo(enum OSTYPE OS, enum MACHTYPE mach, enum WCOMP which,
                     char *sugg)
/*
 * This function returns the compiler and flags used by ATLAS to build
 * the associated architecture defaults.  If the tarfile includes multiple
 * arch defaults and (sugg) is one of these compilers, returns the info for
 * (sugg).
 * RETURNS: NULL iff we don't have arch defaults for given which/OS/mach combo.
 *          Otherwise, ptr to a string, with the first COMPLEN characters
 *          holding the compiler name, and the remainder the flags.
 *
 * This function returns NULL if ATLAS does not presently have arch defaults
 * for the given which/OS/mach combo.
 *
 */
{
   static char comp[1024];
   char *flag;
   int i;
   int WANTGCC=0;

   flag = comp + COMPLEN;
   *flag = comp[0] = '\0';
   if (sugg)
     WANTGCC = (strstr(sugg, "gcc") || strstr(sugg, "g77"));
/*
 * See if the user wants Intel compilers
 */
   if ( sugg && (strstr(sugg, "icc") || strstr(sugg, "ifort")) )
   {
      strcpy(comp, sugg);
      if (OSIsWin(OS))
      {
         switch(mach)
         {
         case IntPIII:
            if (which == CPF77) strcpy(flag, "-QxK -O2");
            else if (which == CPCC) strcpy(flag, "-QxK -O3");
            else strcpy(flag, "-QxK -O2 -Oa");
            break;
         case IntP4:
            if (which == CPF77) strcpy(flag, "-QxN -O2");
            else if (which == CPCC) strcpy(flag, "-QxN -O3");
            else strcpy(flag, "-QxN -O3 -Oa");
            break;
         case IA64Itan2:
            if (which == CPF77) strcpy(flag, "-G2 -O2");
            else if (which == CPCC) strcpy(flag, "-G2 -O3");
            else strcpy(flag, "-G2 -O3 -Oa");
            break;
#if 0
         case IA64Itan:
            if (which == CPF77) strcpy(flag, "-G1 -O2");
            else if (which == CPCC) strcpy(flag, "-G1 -O3");
            else strcpy(flag, "-G1 -O3 -Oa");
            break;
#endif
         default:;
         }
      }
      else
      {
         switch(mach)
         {
         case IntPIII:
            if (which == CPF77) strcpy(flag, "-xK -O2 -mp1 -static");
            else if (which == CPCC) strcpy(flag, "-xK -O3 -mp1 -static");
            else strcpy(flag, "-xK -O2 -mp1 -static -fno-alias");
            break;
         case IntP4:
            if (which == CPF77) strcpy(flag, "-xN -O2 -mp1 -static");
            else if (which == CPCC) strcpy(flag, "-xN -O3 -mp1 -static");
            else strcpy(flag, "-xN -O3 -mp1 -static -fno-alias");
            break;
         case IA64Itan2:
            if (which == CPF77) strcpy(flag, "-tpp2 -O2 -mp1 -static");
            else if (which == CPCC) strcpy(flag, "-tpp2 -O3 -mp1 -static");
            else strcpy(flag, "-tpp2 -O3 -mp1 -static -fno-alias");
            break;
#if 0
         case IA64Itan:
            if (which == CPF77) strcpy(flag, "-tpp1 -O2 -mp1 -static");
            else if (which == CPCC) strcpy(flag, "-tpp1 -O3 -mp1 -static");
            else strcpy(flag, "-tpp1 -O3 -mp1 -static -fno-alias");
            break;
#endif
         default:;
         }
      }
   }
/*
 * See if we are using CVF
 */
   else if ( which == CPF77 && sugg && strstr(sugg, "ATLwin_vf") )
   {
      strcpy(comp, sugg);
      strcpy(flag,
"-iface=nomixed_str_len_arg -fast -assume:accuracy_sensitive -fltconsistency");
   }
/*
 * See if we are using MVC
 */
   else if ( which == CPCC && sugg && strstr(sugg, "ATLwin_vc") )
   {
      if (mach == IntP4)
      {
         strcpy(comp, sugg);
         strcpy(flag, "-O2 -Oy -Op -Za -nologo");
      }
   }
   else
   {
      switch(mach)
      {
      case Dec21164:
      case Dec21264:
         if (which == CPF77)
         {
            if (OS == OSOSF1)
            {
               strcpy(comp, "f77");
               strcpy(flag, "-O5 -arch host -tune host");
               if (THREADS) strcat(flag, " -pthread");
            }
         }
         else
         {
            strcpy(comp, "gcc");
            if (which == CPMM)
               strcpy(flag, "-O1 -fschedule-insns -fschedule-insns2");
            else strcpy(flag, "-O3 -fomit-frame-pointer");
         }
         break;
      case HPPA8K:
      case HP9K735:
         if (OS == OSHPUX)
         {
            if (which == CPF77)
            {
               strcpy(comp, "f77");
               strcpy(flag, "+O2");
            }
            else
            {
               strcpy(comp, "cc");
               if (which == CPMM)
                  strcpy(comp, "-Aa +O2");
               else
                  strcpy(comp, "-D_INCLUDE_POSIX_SOURCE -DUseTimes -Aa +O4");
            }
         }
         break;
      case IbmPwr:
         if (which == CPF77)
         {
            strcpy(comp, "xlf");
            strcpy(flag, "-O3 -qmaxmem=-1 -qfloat=hsflt");
         }
         else
         {
            strcpy(comp, "xlc");
            strcpy(flag, "-O3 -qarch=pwr -qtune=pwr -qmaxmem=-1 -qfloat=hsflt");
         }
         break;
      case IbmTPwr2:
         if (which == CPF77)
         {
            strcpy(comp, "xlf");
            strcpy(flag, "-O3 -qmaxmem=-1 -qfloat=hsflt");
         }
         else
         {
            strcpy(comp, "xlc");
            if (which == CPCC)
               strcpy(flag,
                      "-O3 -qarch=pwr2 -qtune=pwr2 -qmaxmem=-1 -qfloat=hsflt");
            else
               strcpy(flag,
    "-qarch=pwr2 -qtune=pwr2 -qmaxmem=-1 -qfloat=hsflt -qansialias -qfold -O");
         }
         break;
      case IbmPwr4:  /* yes, Pwr4 does better with pwr3 flags than with pwr4! */
      case IbmPwr3:
         if (which == CPF77)
         {
            strcpy(comp, "xlf");
            strcpy(flag,
                   "-qtune=pwr3 -qarch=pwr3 -O3 -qmaxmem=-1 -qfloat=hsflt");
         }
         else
         {
            strcpy(comp, "xlc");
            if (which == CPCC)
               strcpy(flag,
                      "-qtune=pwr3 -qarch=pwr3 -O3 -qmaxmem=-1 -qfloat=hsflt");
            else
               strcpy(flag,
          "-qtune=pwr3 -qarch=pwr3 -O3 -qmaxmem=-1 -qfloat=hsflt -qalias=allp");
         }
         break;
      case IntP5MMX:
      case IntPPRO:
      case IntPII:
      case IntPIII:
      case IntP4:
      case AmdAthlon:
      case AmdHammer32:
      case AmdHammer64:
         if (which == CPF77)
         {
            if (OS == OSFreeBSD) strcpy(comp, "f77");
            else strcpy(comp, "g77");
            strcpy(flag, "-fomit-frame-pointer -O");
         }
         else
         {
            strcpy(comp, "gcc");
            if (mach == AmdHammer64 || mach == AmdHammer32)
               strcpy(flag, "-fomit-frame-pointer -O -mfpmath=387");
            else
            {
               if (which == CPCC)
                  strcpy(flag, "-fomit-frame-pointer -O3 -funroll-all-loops");
               else strcpy(flag, "-fomit-frame-pointer -O");
            }
         }
         if (mach == AmdHammer32) strcat(flag, " -m32");
         else if (mach == AmdHammer64) strcat(flag, " -m64");
         break;
      case IA64Itan2:
/*
 *       If the user doesn't override, use Intel compiler on Itan2
 *       only override allowable is gcc/g77
 */
         if (sugg)
            if (!strstr(sugg, "g77") && !strstr(sugg, "gcc"))
               sugg = NULL;
         if (!sugg)
         {
            if (which == CPF77)
            {
               strcpy(comp, "ifort");
               strcpy(flag, "-tpp2 -O2 -mp1 -static");
            }
            else
            {
               strcpy(comp, "icc");
               if (which == CPCC) strcpy(flag, "-tpp2 -O3 -mp1 -static");
               else strcpy(flag, "-tpp2 -O3 -mp1 -static -fno-alias");
            }
            break;
         }
         /* If user override, fall thru for same gcc settings as Itan1 */
      case IA64Itan:
         if (which == CPF77)
         {
            strcpy(comp, "g77");
            strcpy(comp, "-O");
         }
         else
         {
            strcpy(comp, "gcc");
            strcpy(flag, "-O3 -funroll-all-loops");
         }
         break;
#if 0  /* no longer have arch defaults */
      case SgiIP22:
         if (which == CPF77) strcpy(comp, "f77");
         else strcpy(comp, "cc");
         strcpy(flag, "-O2 -mips2 -Olimit 15000");
         break;
      case SgiIP32:
         if (which == CPF77)
         {
            strcpy(comp, "f77");
            strcpy(flag,
  "-O3 -n32 -mips4 -OPT:Olimit=15000 -TARG:platform=ip32_5k -LNO:blocking=OFF");
         }
         else
         {
            strcpy(comp, "cc");
            strcpy(flag, "-O2 -n32 -mips4 -OPT:Olimit=15000 -TARG:platform=ip32_5k -LNO:blocking=OFF -LOPT:aliased=typed");
         }
         break;
      case SgiIP27:
#endif
      case SgiIP28:
      case SgiIP30:
         if (which == CPF77) strcpy(comp, "f77");
         else strcpy(comp, "cc");
         strcpy(flag,
                "-O3 -64 -OPT:Olimit=15000 -LNO:blocking=OFF -TARG:platform=");
         if (mach == SgiIP28) strcat(flag, "IP28");
         else if (mach == SgiIP27) strcat(flag, "IP27");
         else strcat(flag, "IP30");
         if (which == CPMM) strcat(flag, " -LOPT:alias=typed");
         break;
      case SunUS1:   /* used to be same as SunUS2 */
      case SunUS2:
      case SunUS4:
      case SunUS5:
      case SunUSIII:
         if (OS == OSSunOS && !WANTGCC)
         {
            if (which == CPF77)
            {
               strcpy(comp, "f77");
               strcpy(flag, "-dalign -native -xO5");
            }
            else
            {
               strcpy(comp, "cc");
               if (which == CPCC)
                  strcpy(flag, "-dalign -fsingle -xO5 -native");
               else
                  strcpy(flag, "-dalign -fsingle -xO2 -native");
            }
         }
         else
         {
            if (which == CPF77)
            {
               strcpy(comp, "g77");
               strcpy(flag, "-O");
            }
            else
            {
               strcpy(comp, "gcc");
               if (which == CPCC)
                  strcpy(flag,
                  "-mcpu=ultrasparc -mtune=ultrasparc -fomit-frame-pointer -O");
               else
                  strcpy(flag,
                 "-mcpu=ultrasparc -mtune=ultrasparc -fomit-frame-pointer -O3");
            }
         }
         break;
      case PPC604e:
      case PPC604:
         if (OS == OSAIX && !WANTGCC)
         {
            if (which == CPF77)
               strcpy(comp, "xlf");
            else
               strcpy(comp, "xlc");
            if (which == CPMM)
               strcpy(flag, "-O -qarch=ppc -qtune=604 -qmaxmem=-1 -qfloat=hsflt -qproto -qansialias -qfold");
            else
               strcpy(flag,
                      "-O3 -qarch=ppc -qtune=604 -qmaxmem=-1 -qfloat=hsflt");
         }
         else
         {
            if (which == CPF77)
            {
               strcpy(comp, "g77");
               strcpy(flag, "-O");
            }
            else
            {
               strcpy(comp, "gcc");
               if (which == CPCC)
                  strcpy(flag, "-fomit-frame-pointer -O3");
               else
                  strcpy(flag, "-fomit-frame-pointer -O");
            }
         }
         break;
      case PPCG4:
         if (which == CPF77)
         {
            strcpy(comp, "g77");
            strcpy(flag, "-O");
         }
         else
         {
            strcpy(comp, "gcc");
            if (which == CPCC)
               strcpy(flag, "-fomit-frame-pointer -O3");
            else
               strcpy(flag,
              "-fomit-frame-pointer -O2 -fschedule-insns -fno-schedule-insns2");
            if (OS == OSOSX && !WANTGCC)
            {
               strcpy(comp, "cc");
               strcat(flag, " -traditional-cpp -faltivec");
            }
         }
         break;
      default:;
      }
/*
 *    Don't actually give a crap about fortran, so allow user to suggest
 *    g77 without nullifying defaults
 */
      if (which == CPF77 && sugg)
      {
         strcpy(comp, sugg);
         strcpy(flag, "-O");
      }
   }
   if (THREADS)
   {
      if (strstr(comp, "xlc") || strstr(comp, "xlf")) strcat(comp, "_r");
      if (strstr(comp, "ifort"))
      {
         if (OSIsWin(OS))
            strcat(flag, " -reentrancy");
         else
            strcat(flag, " -reentrancy threaded");
      }
      if (OS == OSFreeBSD)
      {
         if (strstr(comp, "cc"))
            strcat(flag, " -pthread -D_REENTRANT -D_THREAD_SAFE");
         else strcat(flag, " -pthread");
      }
   }
   return(*comp ? comp : NULL);
}


void GimmeWinComp(enum WCOMP which, char *comp)
{
   enum WINCOMP wc;
   if (!comp) return;
   wc = WCgcc;
   if (!strcmp(comp, "cvf")) wc = WCvf;
   else if (!strcmp(comp, "mvc")) wc = WCvc;
   else if (!strcmp(comp, "icl") || !strcmp(comp, "icc")) wc = WCic;
   else if (!strcmp(comp, "ifort")) wc = WCif;
   if (wc != WCgcc)
      assert(!CreateWinComp(wc));
   sprintf(comp, "ATLwin_%s", wcnam[wc]);
}

void GimmeCompInfo(enum OSTYPE OS, enum MACHTYPE mach, enum WCOMP which,
                   char *targ, char *comp, char *flag)
{
   char *sp;
   if (!comp) return;
   sp = GetDefCompInfo(OS, mach, which, *comp ? comp : NULL);
   if (!sp)
      sp = GetDefCompInfo(OS, mach, which, which == CPF77 ? "g77" : "gcc");
   if (!sp)
      sp = GuessCompInfo(OS, mach, which, targ, *comp ? comp : NULL);
   if (*comp == '\0')
      strcpy(comp, sp);
   if (flag && *flag == '\0') strcpy(flag, sp+COMPLEN);
}

int GetCompInfo(FILE *fpout, FILE *fplog, enum OSTYPE OS, enum MACHTYPE mach,
                enum ISAEXT isaX, char *targ, char *redir, char *TOPdir,
                char *F77, char *F77FLAGS, char *FLINK, char *FLFLAGS,
                char *FCLFLAGS, char *goodgcc,
                char *CC, char *CCFLAGS, char *CLINK, char *CLFLAGS,
                char *MCC, char *MMFLAGS, char *BLASlib)
/*
 * Sets up good compiler flags for various OSs, and searches for the correct
 * compilers.  The compiler search should be improved so that it always takes
 * the newest release;  right now it simply takes the first compiler that
 * works with the desired flags.
 * CC & CCFLAGS are required;  other options may be set ptr to NULL, and
 * thus not probed.
 * RETURNS: 0 if no good gcc was found, 1 otherwise
 */
{
   int i, np;
   char *paths[16];
   char *CCG=NULL, *MCCG=NULL, *F77G=NULL;
   char gcc[256];
   int iret=1;

   np = 3;
   paths[0] = "/usr/bin/";
   paths[1] = "/usr/local/";
   paths[2] = "/opt/";

/*
 * Create windows compiler wrappers if required by user selection
 */
   if (OSIsWin(OS))
   {
      if (*CC)
         GimmeWinComp(CPCC, CC);
      if (MCC && *MCC)
         GimmeWinComp(CPMM, MCC);
      if (F77 && *F77)
         GimmeWinComp(CPF77, F77);
   }

   if (BLASlib && BLASlib[0] == '0')
   {
      *BLASlib = '\0';
      BLASlib = NULL;
   }
   if (goodgcc && goodgcc[0] != '\0') strcpy(gcc, goodgcc);
   else gcc[0] = '\0';
   GimmeCompInfo(OS, mach, CPCC, targ, CC, CCFLAGS);
   if (F77) GimmeCompInfo(OS, mach, CPF77, targ, F77, F77FLAGS);
   if (MCC) GimmeCompInfo(OS, mach, CPMM, targ, MCC, MMFLAGS);
/*
 * Tack on altivec flags to gcc iff altivec has been detected
 */
   if (isaX == ISA_AV)
   {
      if (strstr(CC, "gcc")) strcat(CCFLAGS, " -maltivec -mabi=altivec");
      if (MCC && MMFLAGS)
      {
         if (strstr(MCC, "gcc")) strcat(MMFLAGS, " -maltivec -mabi=altivec");
      }
   }
   ATL_mprintf(2, fpout, fplog,
               "Looking for compilers (this may take a while):\n");
   if ( FindGoodGcc(OS, mach, fpout, fplog, gcc) )
   {
      gcc[0] = '\0';
      iret = 0;
   }
   if (goodgcc) strcpy(goodgcc, gcc);
   if (gcc[0])
   {
      if ( strstr(CC, "gcc") ) strcpy(CC, gcc);
      if (MCC) { if ( strstr(MCC, "gcc") ) strcpy(MCC, gcc); }
   }
   if (CCG == NULL)
      CCG = TryComp(fpout, fplog, redir, 'c', np, paths, CC, CCFLAGS);
   if (CCG) strcpy(CC, CCG);
/*
 * If we've failed to find a working CC compiler, but we've found gcc, use
 * gcc instead
 */
   else if (*gcc)
   {
      strcpy(CC, "gcc");
      *CCFLAGS = '\0';
      GimmeCompInfo(OS, mach, CPCC, targ, CC, CCFLAGS);
      strcpy(CC, gcc);
   }

   if (MCC && MMFLAGS && MCCG == NULL)
   {
      MCCG = TryComp(fpout, fplog, redir, 'c', np, paths, MCC, MMFLAGS);
      if (MCCG) strcpy(MCC, MCCG);
      else if (*gcc)
      {
         *MMFLAGS = '\0';
         strcpy(MCC, "gcc");
         GimmeCompInfo(OS, mach, CPMM, targ, MCC, MMFLAGS);
         strcpy(MCC, gcc);
      }
   }
   if (F77G == NULL && F77)
   {
      F77G = TryComp(fpout, fplog, redir, 'f', np, paths, F77, F77FLAGS);
      if (F77G) strcpy(F77, F77G);
      else if (*gcc)
      {
         F77FLAGS = '\0';
         strcpy(F77, "g77");
         GimmeCompInfo(OS, mach, CPF77, targ, F77, F77FLAGS);
      }
   }
/*
 * Now that we have compilers, let's find out linker info
 */
   GetLinkInfo(OS, mach, F77, CC, FLINK, FLFLAGS, FCLFLAGS, CLINK, CLFLAGS);
/*
 * If requested, see if we can locate a comparison BLAS
 */
   if (F77 && BLASlib)
   {
      GetBlasTry(OS, mach, BLASlib);
      FindBlas(fpout, fplog, redir, F77, F77FLAGS, BLASlib);
   }
/*
 * If using a non-g77 Fortran under Windows, throw the -mno-cygwin for all gcc
 */
   if (OSIsWin(OS) && F77 && !strstr(F77, "g77"))
   {
      if (strstr(CC, "gcc"))
         strcat(CCFLAGS, " -mno-cygwin");
      if (MCC && strstr(MCC, "gcc"))
         strcat(MMFLAGS, " -mno-cygwin");
   }
/*
 * Strip stray whitespace from strings
 */
   KillBegEndSpaces(CC);
   KillBegEndSpaces(CCFLAGS);
   if (F77)
   {
      KillBegEndSpaces(F77);
      if (F77FLAGS) KillBegEndSpaces(F77FLAGS);
   }
   if (MCC)
   {
      KillBegEndSpaces(MCC);
      if (MMFLAGS) KillBegEndSpaces(MMFLAGS);
   }
   if (BLASlib) KillBegEndSpaces(BLASlib);

   if (F77) ATL_mprintf(2, fplog, fpout, "F77 = %s %s\n", F77, F77FLAGS);
   ATL_mprintf(2, fplog, fpout, "CC = %s %s\n", CC, CCFLAGS);
   if (MCC) ATL_mprintf(2, fplog, fpout, "MCC = %s %s\n\n", MCC, MMFLAGS);

   return(iret);
}

void GetArchDef(enum OSTYPE OS, enum MACHTYPE mach, char *arch,
                char *CC, char *CCFLAGS, char *MCC, char *MMFLAGS,
                char *mmdef, char *def)
{
   char *mmc=NULL, *syc=NULL;
   char *sp=NULL, *mp=NULL;
   int i;

/*
 * Make sure we have arch defaults for this compiler
 */
   mmdef[0] = def[0] = '\0';
   syc = NameFromPath(CC);
   if (!strcmp(syc, "xlc_r")) syc[3] = '\0';
   else if (strstr(syc, "ATLwin_"))
   {
      if (strstr(syc, "_icc")) strcpy(syc, "icc");
      else if (strstr(syc, "_vc")) strcpy(syc, "vc");
   }
   sprintf(  def, "$(TOPdir)/CONFIG/ARCHS/%s/%s/misc", arch, syc);
   if (!FileIsThere(def+10)) def[0] = '\0';

   mmc = NameFromPath(MCC);
   if (!strcmp(mmc, "xlc_r")) mmc[3] = '\0';
   else if (strstr(mmc, "ATLwin_"))
   {
      if (strstr(mmc, "_icc")) strcpy(mmc, "icc");
      else if (strstr(mmc, "_vc")) strcpy(mmc, "vc");
   }
   sprintf(mmdef, "$(TOPdir)/CONFIG/ARCHS/%s/%s/gemm", arch, mmc);
   if (!FileIsThere(mmdef+10)) mmdef[0] = '\0';
/*
 * If the user has changed flags on us, get reassurance arch defaults are
 * still valid
 */
   if (*def)
   {
      sp = GetDefCompInfo(OS, mach, CPCC, CC);
      assert(sp);
      sp += COMPLEN;
      if (strcmp(CCFLAGS, sp))
      {
         fprintf(stdout, "\nYour CCFLAGS      : %s\n", CCFLAGS);
         fprintf(stdout,   "Config recommends : %s\n", sp);
         DisplayFile("CONFIG/UseCCFlagsDef.txt", stdout, 200);
         fprintf(stdout, "1. Quit install\n");
         fprintf(stdout, "2. Continue install without arch defaults\n");
         fprintf(stdout, "3. Use arch defaults anyway\n");
         i = GetIntRange(3, 1, 3, "", "your choice");
         if (i == 1) exit(-1);
         else if (i == 2) *def = '\0';
      }
   }
   if (*mmdef)
   {
      sp = GetDefCompInfo(OS, mach, CPMM, MCC);
      assert(sp);
      sp += COMPLEN;
      if (strcmp(MMFLAGS, sp))
      {
         fprintf(stdout, "\nYour MMFLAGS      : %s\n", MMFLAGS);
         fprintf(stdout,   "Config recommends : %s\n", sp);
         DisplayFile("CONFIG/UseCCFlagsDef.txt", stdout, 200);
         fprintf(stdout, "1. Quit install\n");
         fprintf(stdout, "2. Continue install without arch defaults\n");
         fprintf(stdout, "3. Use arch defaults anyway\n");
         i = GetIntRange(3, 1, 3, "", "your choice");
         if (i == 1) exit(-1);
         else if (i == 2) *mmdef = '\0';
      }
/*
 *    atlas_install expects a default directory if we give it a MM dir,
 *    so put in bogus if we only have MM
 */
      if (!(*def))
         strcpy(def, mmdef);
   }
}

int RunASMProbe(char *asmnam, char *TOPdir, char *targ)
{
   char ln[512], ln2[512];
   if (targ)
      sprintf(ln2, "cd CONFIG ; make IRun_%s mydir=%s/CONFIG atlrun=atlas_runX targ=%s 2> /dev/null | fgrep SUCCESS",
              asmnam, TOPdir, targ);
   else
      sprintf(ln2,
      "cd CONFIG ; make IRun_%s mydir=%s/CONFIG 2> /dev/null | fgrep SUCCESS",
              asmnam, TOPdir);
   if(!CmndOneLine(NULL, ln2, ln))
   {
      if (strstr(ln, "SUCCESS"))
      {
         fprintf(stdout, "   %s: DETECTED!\n", asmnam);
         return(1);
      }
   }
   fprintf(stdout, "   %s: NO.\n", asmnam);
   return(0);
}
int RunISAProbe(char *isaxnam, char *TOPdir, char *targ, char *opt)
{
   char ln[512], ln2[512];
   if (targ)
      sprintf(ln2, "cd CONFIG ; make IRun_%s mydir=%s/CONFIG atlrun=atlas_runX targ=%s %s 2> /dev/null | fgrep SUCCESS",
              isaxnam, TOPdir, targ, opt);
   else
      sprintf(ln2,
              "cd CONFIG ; make IRun_%s mydir=%s/CONFIG %s 2> /dev/null | fgrep SUCCESS",
              isaxnam, TOPdir, opt);
   if(!CmndOneLine(NULL, ln2, ln))
   {
      if (strstr(ln, "SUCCESS"))
      {
         fprintf(stdout, "   %s: DETECTED!\n", isaxnam);
         return(1);
      }
   }
   fprintf(stdout, "   %s: NO.\n", isaxnam);
   return(0);
}

enum ISAEXT GetISAExt(char *targ, char *TOPdir)
{
   int i;

   fprintf(stdout, "\nProbing for supported ISA extensions:\n");

   if (RunISAProbe(ISAXNAM[0], TOPdir, targ, "CC=\"cc -faltivec\""))
      return(0);
   else if (RunISAProbe(ISAXNAM[0], TOPdir, targ, "CC=\"gcc -maltivec -mabi=altivec -DATL_AVgcc\""))
      return(0);
   else
   {
      for (i=1; i < ISA_None; i++)
      {
         if (RunISAProbe(ISAXNAM[i], TOPdir, targ, "")) return(i);
      }
   }
   return(ISA_None);
}

enum ASMDIA ProbeAsm(char *targ, char *TOPdir,
                     enum OSTYPE OS, enum MACHTYPE mach)
{
   enum ASMDIA iret=ASM_None;
   int i;

   if (MachIsX86(mach))
   {
      if (mach == AmdHammer64) iret = gas_x86_64;
      else iret = gas_x86_32;
   }
   else if (MachIsUS(mach)) iret = gas_sparc;
   else
   {
      for (i=0; i < ASM_None; i++)
         if (RunASMProbe(ASMNAM[i], TOPdir, targ))
           iret = i;
   }
   return(iret);
}

int FlushMul(enum OSTYPE OS, enum MACHTYPE mach, char *targ, char *TOPdir)
/*
 * returns the multiplication factor of true cache size necessary to do
 * complete flush; this is essentially the associativity of the cache
 * if not known, returns 0
 */
{
   int imul=0;
   char ln[256], ln2[256];

   switch(OS)
   {
   case OSAIX :
      if (targ)
         sprintf(ln2, "cd CONFIG ; make IRunAixInfo mydir=%s/CONFIG atlrun=atlas_runX targ=%s | fgrep L2",
                 TOPdir, targ);
      else
         sprintf(ln2, "cd CONFIG ; make IRunAixInfo mydir=%s/CONFIG | fgrep L2",
                 TOPdir);
      if(!CmndOneLine(NULL, ln2, ln))
         imul = GetLastInt(ln);
      break;
   }
   if (imul == 0)
   {
      switch(mach)
      {
      case Dec21164:
      case AmdAthlon:
      case AmdHammer32:
      case AmdHammer64:
      case SunUS1:
      case SunUS2:
      case SunUSIII:
      case SunUS4:
      case SunUS5:
      case SunUSX:
      case IntP4:
      case PPCG4:
      case IntPIII:
         imul = 2;
         break;
      case IntP5:
      case IntP5MMX:
      case IntPPRO:
      case IntPII:
         imul = 1;
         break;
      default:
         imul = 2;
      }
   }
   return(imul);
}

int ProbeCacheSize(enum OSTYPE OS, enum MACHTYPE mach, char *targ, char *TOPdir)
/*
 * Returns size (in KB) of largest cache, returns 0 if OS cannot provide
 */
{
   int i, size=0;
   char ln[256], ln2[256], *sp, s2;

   switch(OS)
   {
   case OSLinux:
      if (mach == IntPII || mach == IntPIII || mach == IntPPRO ||
          mach == AmdAthlon || mach == AmdHammer32 || mach == AmdHammer64)
      {
         if( !CmndOneLine(targ, "fgrep 'cache size' /proc/cpuinfo", ln) )
         {
            size = GetLastInt(ln);
            for (i=0; ln[i]; i++);
            i--;
            while (i > 0 && isspace(ln[i])) i--;
            while (i > 0 && !isspace(ln[i])) i--;
            if (i > 0)
            {
               if (ln[i+1] == 'M') size *= 1024;
               else if (ln[i+1] != 'K') size = 0;
            }
            else size = 0;
         }
      }
      else if (mach == PPCG4)
      {
         if( !CmndOneLine(targ, "fgrep 'L2 cache' /proc/cpuinfo", ln) )
         {
            size = GetLastInt(ln);
         }
         else size = 0;
      }
      break;
   case OSOSF1:
      break;
   case OSIRIX:
      if(!CmndOneLine(targ, "hinv | fgrep Secondary | fgrep 'cache size'", ln))
      {
         if (sp = strstr(ln, " Mbyte"))
         {
            s2 = 1;
            sp--;
            while(isdigit(*sp)) sp--;
            sp++;
            sscanf(sp, "%d", &size);
            size *= 1024;
         }
         else if (sp = strstr(ln, " Kbyte"))
         {
            s2 = 1;
            sp--;
            while(isdigit(*sp)) sp--;
            sp++;
            sscanf(sp, "%d", &size);
         }
      }
      break;
   case OSAIX :
      if (targ)
         sprintf(ln2, "cd CONFIG; make IRunAixInfo mydir=%s/CONFIG atlrun=atlas_runX targ=%s | fgrep L2",
                 TOPdir, targ);
      else
         sprintf(ln2, "cd CONFIG; make IRunAixInfo mydir=%s/CONFIG | fgrep L2",
                 TOPdir);
      if(!CmndOneLine(NULL, ln2, ln)) size = GetFirstInt(ln+5) / 1024;
      break;
   case OSHPUX :
   case OSSunOS:
   case OSSunOS4:
   case OSWin9x:
   case OSWinNT:
   case OSFreeBSD:
   case OSOSX:
   default:
      size = 0;
   }
   return(size);
}

int GetCacheSize(enum OSTYPE OS,  enum MACHTYPE mach, char *targ, char *TOPdir,
                 int lvl, int *AmSure)
{
/*
 * Returns size of requested level of cache.  If *AmSure is 0, this is a max,
 * otherwise that machine only comes with that size of cache.  If lvl is < 0,
 * give me a safe size to flush (handling associativity and so forth)
 */
   int l1, l2, s1, s2;
   int lf1, lf2;
   char ln[512], *sp;

   if (lvl == -2)
   {
      l2 = ProbeCacheSize(OS, mach, targ, TOPdir);
      if (l2 > 0)
      {
         *AmSure = 1;
         l1 = FlushMul(OS, mach, targ, TOPdir);
         if (!l1) l1 = 2;
         return(l2*l1);
      }
   }
   switch(mach)
   {
   case MACHOther:
      l1 = l2 = s1 = s2 = 0;
      lf2 = 4096;
      break;
   case SunUSIII:
   case SunUS2:
   case SunUS4:
   case SunUS5:
      lf1 = l1 = 16;
      lf2 = l2 = 4096;
      s1 = 1;
      s2 = 0;
      break;
   case SunUS1:
      lf1 = l1 = 16;
      lf2 = l2 = 1024;
      s1 = 1;
      s2 = 0;
      break;
   case SunSS:
      lf1 = l1 = 32;
      lf2 = l2 = 1024;
      s1 = 1;
      s2 = 0;
      break;
   case SunMS:
      lf1 = l1 = 8;
      lf2 = l2 = l1;
      s1 = 1;
      s2 = 1;
      break;
   case SgiIP22:    /* R4600 */
      if (l2 == -1) lf2 = l2 = 128;
      lf1 = l1 = 16;
      s1 = 1;
      break;
   case SgiIP32:   /* R5K */
      if (l2 == -1) lf2 = l2 = 1024;
      l1 = 32;
      lf1 = 2*l1;
      s1 = 1;
      break;
   case SgiIP27:
   case SgiIP28:
   case SgiIP30:
      if (l2 == -1) lf2 = l2 = 4096;
      l1 = 32;
      lf1 = 2*l1;
      s1 = 1;
      break;
   case AmdAthlon:
   case AmdHammer32:
   case AmdHammer64:
      lf1 = l1 = 64;
      lf2 = l2 = 4096;
      s1 = 1;
      s2 = 0;
      break;
   case IA64Itan2:
      lf1 = l1 = 256;
      lf2 = l2 = 4096;
      s1 = 1;
      s2 = 0;
      break;
   case IA64Itan:
      lf1 = l1 = 96;  /* ignore actual L1, 'cause fpu doesn't use it */
      lf2 = l2 = 4096;
      break;
   case IntPII:
   case IntPIII:
      lf1 = l1 = 16;
      lf2 = l2 = 512;
      s1 = 1;
      s2 = 0;
      break;
   case IntP4:
      lf1 = l1 = 64;
      lf2 = l2 = 512;
      s1 = 0;
      s2 = 0;
      break;
   case IntPPRO:
      lf1 = l1 = 8;
      lf2 = l2 = 1024;
      s1 = 1;
      s2 = 0;
      break;
   case IntP5MMX:
      lf1 = l1 = 16;
      lf2 = l2 = 1024;
      s1 = 1;
      s2 = 0;
      break;
   case IntP5:
      lf1 = l1 = 8;
      lf2 = l2 = 1024;
      s1 = 1;
      s2 = 0;
      break;
   case PPCG4:
   case PPC604e:
      l1 = 32;
      lf1 = 4*l1;
      lf2 = l2 = 4096;
      s1 = 1;
      s2 = 0;
      break;
   case PPC604:
      l1 = 16;
      lf1 = 4*l1;
      lf2 = l2 = 4096;
      s1 = 1;
      s2 = 0;
      break;
   case IbmPwr:
      l1 = 64;
      l2 = 64;
      lf2 = lf1 = 4*l1;
      s1 = 0;
      s2 = 0;
      break;
   case IbmWPwr2:
      l1 = 256;
      l2 = 256;
      lf2 = lf1 = 4*l1;
      s1 = 1;
      s2 = 1;
      break;
   case IbmTPwr2:
      l1 = 128;
      l2 = 128;
      lf2 = lf1 = 4*l1;
      s1 = 1;
      s2 = 1;
      break;
   case IbmPwr4:
      l1 = 32;
      lf2 = l2 = 8096;
      s1 = 1;
      s2 = 0;
      break;
   case IbmPwr3:
      l1 = 64;
      lf2 = l2 = 4096;
      s1 = 1;
      s2 = 0;
      break;
   case HP9K735:
      lf1 = l1 = 256;
      l2 = 256;
      lf2 = 4*l2;
      s1 = 1;
      s2 = 1;
      break;
   case HPPA8K:
      lf1 = l1 = 1024;
      lf2 = l2 = 1024;
      lf2 = 4*l2;
      s1 = 0;
      s2 = 0;
      break;
   case Dec21064:
      lf1 = l1 = 16;
      lf2 = l2 = 4096;
      s1 = 1;
      s2 = 0;
      break;
   case Dec21164:
      lf1 = l1 = 8;
      lf2 = l2 = 4096;
      s1 = 1;
      s2 = 0;
      break;
   default:
      l1 = l2 = s1 = s2 = 0;
      lf2 = 4096;
   }
   if (lvl == 1)
   {
      if (AmSure) *AmSure = s1;
      return(l1);
   }
   else if (lvl == -1)
   {
      if (AmSure) *AmSure = s1;
      return(lf1);
   }
   else if (lvl == -2)
   {
      if (AmSure) *AmSure = s2;
      return(lf2);
   }
   else
   {
      if (AmSure) *AmSure = s2;
      return(l2);
   }
}

int ProbeNCPU(enum OSTYPE OS,  enum MACHTYPE mach, char *targ, char *TOPdir)
/*
 * Probes the OS for the number of CPUs.  Returns 0 if it can't
 * figure it out
 */
{
   char ln[256], ln2[256], *fnam;
   char unam;
   int ncpu=0;

   switch(OS)
   {
   case OSSunOS4: /* SunOS4 single proc, need SunOS5 for SMP, I think */
      ncpu = 1;
      break;
   case OSSunOS:
      sprintf(ln2, "%s -X | fgrep NumCPU", FindUname());
      if (!CmndOneLine(targ, ln2, ln))
         ncpu = GetFirstInt(ln);
      break;
   case OSFreeBSD:
   case OSOSX:
         if ( !CmndOneLine(targ, "sysctl hw.ncpu", ln) ) ncpu = GetLastInt(ln);
         break;
   case OSLinux:
      if (mach == Dec21264 || mach == Dec21164 || mach == Dec21064)
      {
         if ( !CmndOneLine(targ, "fgrep 'cpus detected' /proc/cpuinfo", ln) )
            ncpu = GetLastInt(ln);
      }
      else
      {
         fnam = CmndResults(targ, "grep '^processor' /proc/cpuinfo");
         if (fnam) ncpu = fNumLines(fnam);
      }
      break;
   case OSOSF1:
      if (!CmndOneLine(targ, "/usr/sbin/psrinfo -n", ln))
         ncpu = GetFirstInt(ln);
      break;
   case OSIRIX:
      if (!CmndOneLine(targ, "hinv | fgrep Processor | fgrep MHZ", ln))
         ncpu = GetFirstInt(ln);
      break;
   case OSAIX :
      if (targ) sprintf(ln2, "cd CONFIG ; make IRunAixInfo mydir=%s/CONFIG atlrun=atlas_runX targ=%s | fgrep ncpus", TOPdir, targ);
      else sprintf(ln2, "cd CONFIG ; make IRunAixInfo mydir=%s/CONFIG | fgrep ncpus", TOPdir);
      if ( !CmndOneLine(NULL, ln2, ln) ) ncpu = GetLastInt(ln);
      break;
   case OSHPUX:
      if (targ) sprintf(ln2,"cd CONFIG ; make IRunHpuxInfo mydir=%s/CONFIG atlrun=atlas_runX targ=%s | fgrep ncpus", TOPdir, targ);
      else sprintf(ln2,"cd CONFIG ; make IRunHpuxInfo mydir=%s/CONFIG | fgrep ncpus", TOPdir);
      if ( !CmndOneLine(NULL, ln2 ,ln) ) ncpu = GetLastInt(ln);
      break;
   case OSWin9x:
      ncpu = 1;
      break;
   case OSWinNT:
   default:
      ncpu = 0;
   }
   return(ncpu);
}

int GetNCPU(enum OSTYPE OS, enum MACHTYPE mach, char *targ, char *TOPdir)
{
   int ncpu;

   ncpu = ProbeNCPU(OS, mach, targ, TOPdir);
   if (!ncpu)
   {
      DisplayFile("CONFIG/ncpu.txt", stdout, NLINES);
      ncpu = GetIntRange(0, 0, 64, "", "the number processors in system");
   }
   return(ncpu);
}

enum LINUXARCH ProbeLinuxArch(char *targ)
{
   enum LINUXARCH la=LAOTHER;
   char ln[256], ln2[256];
   char *unam;

   unam = FindUname();

   sprintf(ln2, "%s -m", unam);
   if ( !CmndOneLine(targ, ln2, ln) )
   {
      if (strstr(ln, "ppc") || strstr(ln, "Power Macintosh")) la = LAPPC;
      else if (strstr(ln, "sparc")) la = LASPARC;
      else if (strstr(ln, "alpha")) la = LAALPHA;
      else if (strstr(ln, "ia64")) la = LAIA64;
      else if ( strstr(ln, "i686") || strstr(ln, "i586") ||
                strstr(ln, "i486") || strstr(ln, "i386") ||
                strstr(ln, "x86_64") ) la = LAX86;
   }
   return(la);
}

enum MACHTYPE ProbeArch(enum OSTYPE OS, char *targ, char *TOPdir)
/*
 * Probes OS for machine type, returns MACHOther on failure
 */
{
   enum MACHTYPE mach=MACHOther;
   enum LINUXARCH la;
   char *unam;
   int ierr, i;
   char ln[256], ln2[256];

   unam = FindUname();
   sprintf(ln2, "%s -m", unam);
   switch(OS)
   {
   case OSFreeBSD:
   case OSOSX:
      la = ProbeLinuxArch(targ);
      switch(la)
      {
      case LAPPC: /* don't know */
         if (!CmndOneLine(targ, "sysctl hw.model", ln))
         {
            if (strstr(ln, "PowerMac"))
            {
               if (strstr(ln, "4")) mach = PPCG4;
               else if (strstr(ln, "3,3")) mach = PPCG4;
            }
         }
         break;
      case LASPARC: /* don't know */
         break;
      case LAALPHA:
         if (!CmndOneLine(targ, "sysctl hw.model", ln))
         {
            if (strstr(ln, "433au")) mach = Dec21164;
            else if (strstr(ln, "XP1000")) mach = Dec21264;
         }
         break;
      case LAIA64: /* don't know */
         break;
      case LAX86:
         if (!CmndOneLine(targ, "sysctl hw.model", ln))
         {
            if (strstr(ln, "Pentium Pro")) mach = IntPPRO;
            else if (strstr(ln, "Pentium III")) mach = IntPIII;
            else if (strstr(ln, "Pentium II ")) mach = IntPII;
            else if (strstr(ln, "Athlon")) mach = AmdAthlon;
            else if (strstr(ln, "AMD-K7")) mach = AmdAthlon;
            else if (strstr(ln, "32 bit Hammer")) mach = AmdHammer32;
            else if (strstr(ln, "64 bit Hammer")) mach = AmdHammer64;
            else if (strstr(ln, "Pentium/P55C")) mach = IntP5MMX; /* sent by */
            else if (strstr(ln, "Pentium")) mach=IntP5;       /* Nakata Maho */
         }
         break;
      default:;
      }
      break;
   case OSLinux:
      la = ProbeLinuxArch(targ);
      switch(la)
      {
      case LAPPC:
         if ( !CmndOneLine(targ, "cat /proc/cpuinfo | fgrep cpu", ln2) )
         {
            if (strstr(ln2, "604e")) mach = PPC604e;
            else if (strstr(ln2, "604")) mach = PPC604;
            else if (strstr(ln2, "G4")) mach = PPCG4;
            else if (strstr(ln2, "7400")) mach = PPCG4;
            else if (strstr(ln2, "7410")) mach = PPCG4;
            else if (strstr(ln2, "7455")) mach = PPCG4;
         }
         break;
      case LASPARC:  /* don't know here anymore */
         #if 0
         if ( !CmndOneLine(targ, "fgrep cpu /proc/cpuinfo", ln) )
         {
            if (strstr(ln, "UltraSparc II")) mach = SunUS2;
            else if (strstr(ln, "UltraSparc I")) mach = SunUS1;
            else if (strstr(ln, "UltraSparc")) mach = SunUSX;
         }
         #endif
         break;
      case LAALPHA:
         ln[0] = '\0';
         ierr = CmndOneLine(targ, "fgrep 'model name' /proc/cpuinfo", ln);
         if (ierr || ln[0] == '\0')
            ierr = CmndOneLine(targ, "fgrep model /proc/cpuinfo", ln);
         if (!ierr && ln[0] != '\0')
         {
            if (strstr(ln, "EV5")) mach = Dec21164;
            else if (strstr(ln, "EV4")) mach = Dec21064;
            else if (strstr(ln, "EV6")) mach = Dec21264;
         }
         break;
      case LAIA64:
         ln[0] = '\0';
         ierr = CmndOneLine(targ, "fgrep 'Itanium' /proc/cpuinfo", ln);
         if (ierr || ln[0] == '\0')
            ierr = CmndOneLine(targ, "fgrep model /proc/cpuinfo", ln);
         if (!ierr && ln[0] != '\0')
         {
            if (strstr(ln, "Itanium 2")) mach = IA64Itan2;
            else if (strstr(ln, "Itanium")) mach = IA64Itan;
         }
         break;
      case LAX86:
         ln[0] = '\0';
         ierr = CmndOneLine(targ, "fgrep 'model name' /proc/cpuinfo", ln);
         if (ierr || ln[0] == '\0')
            ierr = CmndOneLine(targ, "fgrep model /proc/cpuinfo", ln);
         if (!ierr && ln[0] != '\0')
         {
            if (strstr(ln, "Pentium")) /* Pentium of some flavor */
            {
               if (strstr(ln, " III ")) mach = IntPIII;
               else if (strstr(ln, " II ")) mach = IntPII;
               else if (strstr(ln, "Pro")) mach = IntPPRO;
               else if (strstr(ln, "MMX")) mach = IntP5MMX;
               else if (strstr(ln, " 4 ")) mach = IntP4;
            }
            else if (strstr(ln, "Athlon HX")) mach = AmdHammer64;
            else if (strstr(ln, "Opteron")) mach = AmdHammer64;
            else if (strstr(ln, "Hammer")) mach = AmdHammer64;
            else if (strstr(ln, "Athlon(tm) 64")) mach = AmdHammer64;
            else if (strstr(ln, "Athlon")) mach = AmdAthlon;
            else if (strstr(ln, "AMD-K7")) mach = AmdAthlon;
         }
         break;
      default:;
      }
      break;
   case OSSunOS:
      if (!CmndOneLine(targ, "/usr/sbin/psrinfo -v | fgrep sparcv9", ln))
      {
         if (!CmndOneLine(targ, "/usr/sbin/psrinfo -v | fgrep MHz", ln))
         {
            i = GetIntBeforeWord("MHz", ln);
            if (i != BADINT && i > 800) mach = SunUSIII;
         }
         else if (!CmndOneLine(targ, "/usr/sbin/psrinfo -v | fgrep GHz", ln))
         {
            if (strstr(ln, "GHz")) mach = SunUSIII;
         }
      }
      if (mach == MACHOther)
      {
         sprintf(ln2, "%s -i", unam);
         if( !CmndOneLine(targ, ln2, ln) )
         {
            if (strstr(ln, "SPARCstation-5")) mach = SunMS;
            else if (strstr(ln, "Ultra-1")) mach = SunUS1;
            else if (strstr(ln, "Ultra-2")) mach = SunUS2;
            else if (strstr(ln, "Ultra-4")) mach = SunUS4;
            else if (strstr(ln, "Ultra-5_")) mach = SunUS5;
            else if (strstr(ln, "Ultra-5\n")) mach = SunUS5;
            else if (strstr(ln, "Ultra")) mach = SunUSX;
         }
      }
      break;
   case OSSunOS4:
      if ( !CmndOneLine(targ, "sysinfo | fgrep CPU | fgrep MHz", ln) )
      {
         if (strstr(ln, "microSPARC")) mach = SunMS;
         else if (strstr(ln, "UltraSPARC"))
         {
            if( !CmndOneLine(targ, "sysinfo | fgrep Ultra", ln) )
            {
               if (strstr(ln, "Ultra-2")) mach = SunUS2;
               else if (strstr(ln, "Ultra-1")) mach = SunUS1;
               else if (strstr(ln, "Ultra")) mach = SunUSX;
            }
         }
         else if (strstr(ln, "Ultra-2")) mach = SunUS2;
         else if (strstr(ln, "Ultra-1")) mach = SunUS1;
         else if (strstr(ln, "Ultra")) mach = SunUSX;
      }
      break;
   case OSOSF1:
      if ( !CmndOneLine(targ, ln2, ln) )
      {
         if (strstr(ln, "alpha"))
         {
            ierr = CmndOneLine(targ, "/usr/sbin/pinfo -v | fgrep EV", ln);
            if (ierr)
            {
               ierr = CmndOneLine(targ, "/usr/sbin/psrinfo -v | fgrep EV", ln);
               if (ierr) ierr = CmndOneLine(targ, "psrinfo -v | fgrep EV", ln);
            }
            if (!ierr)
            {
               if (strstr(ln, "EV6")) mach = Dec21264;
               else if (strstr(ln, "EV5")) mach = Dec21164;
               else if (strstr(ln, "EV4")) mach = Dec21064;
            }
         }
      }
      break;
   case OSIRIX:
      if ( !CmndOneLine(targ, ln2, ln) )
      {
         if (strstr(ln, "IP28")) mach = SgiIP28;
         else if (strstr(ln, "IP27")) mach = SgiIP27;
         else if (strstr(ln, "IP22")) mach = SgiIP22;
         else if (strstr(ln, "IP32")) mach = SgiIP32;
         else if (strstr(ln, "IP30")) mach = SgiIP30;
      }
      break;
   case OSAIX :
      if (targ)
         sprintf(ln2, "cd CONFIG ; make IRunAixInfo mydir=%s/CONFIG atlrun=atlas_runX targ=%s | fgrep 'CPU type'", TOPdir, targ);
      else
         sprintf(ln2,
            "cd CONFIG ; make IRunAixInfo mydir=%s/CONFIG | fgrep 'CPU type'",
                 TOPdir);
      if ( !CmndOneLine(NULL, ln2, ln) )
      {
         if (strstr(ln, "PowerPC"))
         {
            if (strstr(ln, "604e")) mach = PPC604e;
            else if (strstr(ln, "604") && !strstr(ln, "UNKNOWN")) mach = PPC604;
            else if (strstr(ln, "PowerPC 630")) mach = IbmPwr3;
         }
         else if (strstr(ln, "Power"))
         {
            if (strstr(ln, "Power2")) mach= IbmTPwr2;
            else if (strstr(ln, "Power1")) mach = IbmPwr;
            else if (strstr(ln, "Power4")) mach = IbmPwr4;
         }
      }
      break;
   case OSHPUX:
      if(!CmndOneLine(targ, ln2, ln))
      {
         if (strstr(ln, "9000/735")) mach=HP9K735;
   /*      else if (strstr(ln, "9000/715")) mach=HP9K715; */
      }
      break;
   case OSWin9x:
   case OSWinNT:
   default:;
   }
   if (mach == SunUSX) mach =SunUS2;
   return(mach);
}

enum MACHTYPE Getx86Arch(char *targ, char *unam)
{
   char ln[256], ln2[256];
   int i, j;
   enum MACHTYPE mach=MACHOther;

   j = 0;
   sprintf(ln2, "%s -m", unam);
   ln[0] = '\0';
   i = CmndOneLine(targ, ln2, ln);
   if (i || ln[0] == '\0' || strstr(ln, "i586") || strstr(ln, "i486") ||
            strstr(ln, "i386"))
   {
      fprintf(stdout, "Enter your machine type:\n");
      fprintf(stdout, "   1. Other/UNKNOWN\n\n");
      fprintf(stdout, "   2. AMD Athlon\n");
      fprintf(stdout, "   3. 32 bit AMD Hammer\n");
      fprintf(stdout, "   4. 64 bit AMD Hammer\n");
      fprintf(stdout, "   5. Pentium PRO\n");
      fprintf(stdout, "   6. Pentium II\n");
      fprintf(stdout, "   7. Pentium III\n");
      fprintf(stdout, "   8. Pentium 4\n");
      fprintf(stdout, "   9. Pentium \n");
      fprintf(stdout, "  10. Pentium MMX\n");
      fprintf(stdout, "  11. IA-64 Itanium 1\n");
      fprintf(stdout, "  12. IA-64 Itanium 2\n");
      j = 11;
   }
   else if (strstr(ln, "ia64"))
   {
      fprintf(stdout, "Enter your machine type:\n");
      fprintf(stdout, "   1. Other/UNKNOWN\n");
      fprintf(stdout, "   2. IA-64 Itanium 1\n");
      fprintf(stdout, "   3. IA-64 Itanium 2\n");
      i = GetIntRange(1, 1, 3, "", "machine number");
      if (i == 2) mach = IA64Itan;
      else if (i == 3) mach = IA64Itan2;
      else mach = MACHOther;
   }
   else if (strstr(ln, "x86_64"))
   {
      fprintf(stdout, "Enter your machine type:\n");
      fprintf(stdout, "   1. Other/UNKNOWN\n");
      fprintf(stdout, "   2. 32 bit AMD Hammer\n");
      fprintf(stdout, "   3. 64 bit AMD Hammer\n");
      i = GetIntRange(1, 1, 3, "", "machine number");
      if (i == 2) mach = AmdHammer32;
      else if (i == 3) mach = AmdHammer64;
      else mach = MACHOther;
   }
   else if (strstr(ln, "i686"))
   {
      fprintf(stdout, "Enter your machine type:\n");
      fprintf(stdout, "   1. Other/UNKNOWN\n");
      fprintf(stdout, "   2. AMD Athlon\n");
      fprintf(stdout, "   3. 32 bit AMD Hammer\n");
      fprintf(stdout, "   4. 64 bit AMD Hammer\n");
      fprintf(stdout, "   5. Pentium PRO\n");
      fprintf(stdout, "   6. Pentium II\n");
      fprintf(stdout, "   7. Pentium III\n");
      fprintf(stdout, "   8. Pentium 4\n");
      j = 8;
   }
   if (j > 0)
   {
      i = GetIntRange(1, 1, j, "", "machine number");
      if (i == 1) mach = MACHOther;
      else if (i == 2) mach = AmdAthlon;
      else if (i == 3) mach = AmdHammer32;
      else if (i == 4) mach = AmdHammer64;
      else if (i == 5) mach = IntPPRO;
      else if (i == 6) mach = IntPII;
      else if (i == 7) mach = IntPIII;
      else if (i == 8) mach = IntP4;
      else if (i == 9) mach = IntP5;
      else if (i == 10) mach = IntP5MMX;
      else if (i == 11) mach = IA64Itan;
      else if (i == 12) mach = IA64Itan2;
      else mach = MACHOther;
   }
   return(mach);
}

enum MACHTYPE GetArch(FILE *fpout, FILE *fplog, enum OSTYPE OS, char *targ,
                      char *TOPdir)
{
   char ln[512], ln2[512];
   int i, j, ierr;
   enum MACHTYPE mach=MACHOther;
   enum LINUXARCH la;
   char *unam=FindUname();

   ATL_mprintf(2, fplog, fpout, "Probing for architecture:\n");
   mach = ProbeArch(OS, targ, TOPdir);
/*
 * If we're on a hammer, see if user wants 64 or 32 bit libraries
 */
   if (mach == AmdHammer64)
   {
      fprintf(stdout, "Select pointer width and ABI for x86-64 system:\n");
      fprintf(stdout, "   1. 32 bit libraries\n");
      fprintf(stdout, "   2. 64 bit libraries\n");
      i = GetIntRange(2, 1, 2, "", "bit number");
      if (i == 1) mach = AmdHammer32;
   }
   if (mach != MACHOther)
   {
      ATL_mprintf(2, fplog, fpout, "Architecture is set to %s\n\n",
                  machnam[mach]);
      return(mach);
   }

   switch(OS)
   {
   case OSFreeBSD:
   case OSLinux:
   case OSOSX:
      la = ProbeLinuxArch(targ);
      switch(la)
      {
      case LAIA64:
         fprintf(stdout, "Enter your machine type:\n");
         fprintf(stdout, "   1. IA-64 Itanium 1\n");
         fprintf(stdout, "   2. IA-64 Itanium 2\n");
         fprintf(stdout, "   3. Other/UNKNOWN\n");
         i = GetIntRange(3, 1, 2, "", "machine number");
         if (i == 1) mach = IA64Itan;
         else if (i == 2) mach = IA64Itan2;
         break;
      case LAPPC:
         fprintf(stdout, "Enter your machine type:\n");
         fprintf(stdout, "   1. PowerPC 604e\n");
         fprintf(stdout, "   2. PowerPC 604\n");
         fprintf(stdout, "   3. PowerPC G4 (7400)\n");
         fprintf(stdout, "   4. Other/UNKNOWN\n");
         i = GetIntRange(4, 1, 4, "", "machine number");
         if (i == 1) mach = PPC604e;
         else if (i == 2) mach = PPC604;
         else if (i == 3) mach = PPCG4;
         break;
      case LASPARC:
         fprintf(stdout, "Enter your machine type:\n");
         fprintf(stdout, "   1. Ultra-1\n");
         fprintf(stdout, "   2. Ultra-2\n");
         fprintf(stdout, "   3. Ultra-4\n");
         fprintf(stdout, "   4. Ultra-5/10\n");
         fprintf(stdout, "   5. UltraSparc III\n");
         fprintf(stdout, "   6. Other UltraSparc\n");
         fprintf(stdout, "   7. Other/UNKNOWN\n");
         i = GetIntRange(6, 1, 6, "", "machine number");
         if (i == 1) mach = SunUS1;
         else if (i == 2) mach = SunUS2;
         else if (i == 3) mach = SunUS4;
         else if (i == 4) mach = SunUS5;
         else if (i == 5) mach = SunUSIII;
         else if (i == 6) mach = SunUSX;
         break;
      case LAALPHA:
         fprintf(stdout, "Enter your machine type:\n");
         fprintf(stdout, "   1. DEC ALPHA 21064\n");
         fprintf(stdout, "   2. DEC ALPHA 21164 (ev5/ev56)\n");
         fprintf(stdout, "   3. DEC ALPHA 21264 (ev6)\n");
         fprintf(stdout, "   4. Other/UNKNOWN\n");
         i = GetIntRange(4, 1, 4, "", "machine number");
         if (i == 1) mach = Dec21064;
         else if (i == 2) mach = Dec21164;
         else if (i == 3) mach = Dec21264;
         break;
      case LAX86:
         mach = Getx86Arch(targ, unam);
      }
      break;
   case OSHPUX:
      if(CmndOneLine(targ, ln2, ln)) mach = MACHOther;
      else if (strstr(ln, "9000/735")) mach=HP9K735;
/*      else if (strstr(ln, "9000/715")) mach=HP9K715; */
      else mach = MACHOther;
      break;
   case OSSunOS4:
      fprintf(stdout, "Running trusty old SunOS 4 (hey, is that a Nehru jacket you're wearing), I see.\n");
      fprintf(stdout,
         "Since SunOS 4 is silent on the subject, what chip do you have:\n");
      fprintf(stdout,
"   1. UltraSparc\n   2. SuperSparc 1/2\n   3. MicroSparc\n  4. Other/UNKNOWN\n");
      i = GetIntRange(4, 1, 4, "", "chip number");
      if (i == 1) mach = SunUS2;
      else if (i == 2) mach = SunSS;
      else if (i == 3) mach = SunMS;
      break;
   case OSSunOS:
/*      assert(0); */
      break;
   case OSOSF1:
      mach = MACHOther;
         fprintf(stdout, "Looks like you are compiling OSF1/Tru64.\n");
         fprintf(stdout, "Choose model:\n");
         fprintf(stdout, "   1. Alpha 21064\n");
         fprintf(stdout, "   2. Alpha 21164 (ev5/ev56)\n");
         fprintf(stdout, "   3. Alpha 21264 (ev6)\n");
         fprintf(stdout, "   4. Other/UNKNOWN\n\n");
         i = GetIntRange(4, 1, 4, "", "machine number");
         if (i == 1) mach = Dec21064;
         else if (i == 2) mach = Dec21164;
         else if (i == 3) mach = Dec21264;
      break;
   case OSIRIX:
/*      assert(0); */
      break;
   case OSAIX:  /* don't even think about translating IBM's wild-ass uname */
      fprintf(stdout, "Here are the preconfigured ATLAS choices for AIX:\n");
      fprintf(stdout, "   1. IBM Power\n");
      fprintf(stdout, "   2. IBM Thin-node, Power2\n");
      fprintf(stdout, "   3. IBM PowerPC 604\n");
      fprintf(stdout, "   4. IBM PowerPC 604e (silver node)\n");
      fprintf(stdout, "   5. IBM Power3\n");
      fprintf(stdout, "   6. IBM Power4\n");
      fprintf(stdout, "   7. Other/UNKNOWN\n\n");
      i = GetIntRange(6, 1, 6, "", "machine number");
      if (i == 1) mach = IbmPwr;
      else if (i == 2) mach = IbmTPwr2;
      else if (i == 3) mach = PPC604;
      else if (i == 4) mach = PPC604e;
      else if (i == 5) mach = IbmPwr3;
      else if (i == 6) mach = IbmPwr4;
      else mach = MACHOther;
      break;
   case OSWin9x:
   case OSWinNT:
      mach = Getx86Arch(targ, unam);
      break;
   case OSOther:
      break;
   }
   ATL_mprintf(2, fplog, fpout, "Architecture is set to %s\n\n", machnam[mach]);
   return(mach);
}

void GetPmake(enum OSTYPE OS, int ncpu, char *pmake)
{
   if (ncpu == 0) ncpu = 1;
   if (!OSIsWin(OS)&&CmndResults(NULL, "make DoNothing --version | fgrep GNU"))
   { /* using gnu make */
      if (ncpu == 1) sprintf(pmake, "$(MAKE) -j 2");
      else sprintf(pmake, "$(MAKE) -j %d", ncpu);
   }
   else
   {  /* AIX, HP-UX, SunOS make do not have parallel option */
      switch(OS)
      {
      case OSIRIX:
         strcpy(pmake, "$(MAKE) -P");
         break;
      case OSFreeBSD:
         if (ncpu == 1) sprintf(pmake, "$(MAKE) -j 2");
         else sprintf(pmake, "$(MAKE) -j %d", ncpu);
         break;
      default:
         strcpy(pmake, "$(MAKE)");
      }
   }
}

char *GetXCompTarg(FILE *fpout0, FILE *fplog, char *targnam)
{
   char *targ=NULL;
   char ln[512];
   int ierr;
   FILE *fpout;

   fprintf(stdout, "\n\nI need to know if you are using a cross-compiler (i.e., you are compiling on\n");
   fprintf(stdout,
      "a different architecture than you want the library built for).\n\n");
   targnam[0] = '\0';
   if (IsYes('n', "", "Are you using a cross-compiler?"))
   {
      DisplayFile("CONFIG/xcomp.txt", stdout, NLINES);
      do
      {
         GetStrVer(targnam, "", "name of target machine", 511, targnam);
         fprintf(stdout, "\n\nTRYING TO ssh %s\n", targnam);
         sprintf(ln, "ssh %s \"ls\"", targnam);
         ierr = system(ln);
         if (ierr) DisplayFile("CONFIG/xcomp-err.txt", stdout, NLINES);
         else fprintf(stdout, "\n\nssh successful!\n\n");
         ATL_mprintf(2, stdout, fplog,
                     "Using cross compiler, spawning runs to %s.\n\n", targnam);
      }
      while (ierr);
      targ = targnam;
      fpout = fopen("CONFIG/Make.inc", "w");
      assert(fpout);
      fprintf(fpout,
         "atlas_run:\n\tssh %s \"cd $(atldir) ; ./$(exe) $(args) > $(redir)\"\n",
              targnam);
   }
   else
   {
      fpout = fopen("CONFIG/Make.inc", "w");
      assert(fpout);
      fprintf(fpout, "atlas_run:\n\tcd $(atldir) ; ./$(exe) $(args) > $(redir)\n");
   }
   fclose(fpout);
   return(targ);
}

enum USERGEMM GetUserMM(enum OSTYPE OS, enum MACHTYPE mach,
                        char *UMMdir0, char *UMMdef0)
{
   enum USERGEMM iret=UG_None;

   strcpy(UMMdir0, "$(GMMdir)");
   UMMdef0[0] = '\0';

   switch(mach)
   {
   case Dec21164 :
   case Dec21264 :
/*
 *    This is a cheap workaround to Linux_21164GOTO error, forcing use
 *    of the non-goto defaults on such a platform
 */
      if (mach != Dec21164 || OS != OSLinux)
      {
      DisplayFile("CONFIG/gotogemm.txt", stdout, NLINES);
      if (IsYes('y', "", "Use Goto GEMM for Compaq/DEC alpha speedup?"))
      {
         strcpy(UMMdir0, "$(TOPdir)/src/blas/gemm/GOTO/$(ARCH)");
         if (mach == Dec21164) strcpy(UMMdef0, "-DEV5");
         else if (mach == Dec21264) strcpy(UMMdef0, "-DEV6");
         iret = UG_GOTO;
      }
      }
      break;
   default:;
   }
   return(iret);
}

#define CF_ARCHDEF      0x1
#define CF_CREATEDIRS   0x2
#define CF_CREATEATLRUN 0x4
#define CF_CREATEWINF77 0x8
#define CF_STOREL1      0xA
#define CF_MOVELOG      0x10

void GoToTown(int flag, enum MACHTYPE mach, int ncpu, char *TOPdir, char *ARCH,
              char *BLASlib, char *F77, char *F77FLAGS, char *CC, char *CCFLAGS,
              char *MCC, char *MMFLAGS, char *GOODGCC,
              char *XCC, char *XCCFLAGS, char *UCDEF)
{
   char ARCH0[128], LIBdir[256], ATLASlib[256];
   char UMMdir[256], UMMdef[256];
   char CBLASlib[256], PTCBLASlib[256];
   char F77BLASlib[256], PTF77BLASlib[256];
   int L1SIZE=(-1), L2SIZE, BADMCC=0;
   char F2CDEFS[256];
   char FLINKER[128], CLINKER[128], ARCHIVER[128], RANLIB[128];
   char CLINKFLAGS[512], FLINKFLAGS[512], ARFLAGS[256], FCLINKFLAGS[512];
   char LIBS[256];
   char TAR[256], GZIP[256], GUNZIP[256], PMAKE[128];

   char comp[64], cflg[512], ln[512], tnam[256], archdef[256], mmdef[256];
   char BC[256], BCFLAGS[256];
   int GOGO=0, L2IsKnown=0, Use3DNow=0, TuneL1=1;
   int USEDEFL1=0;
   int ierr, i, j, SOLERR=0, np=2, delay=0;
   char *paths[2] = {"/usr/bin/", "/usr/local/"};
   char *sp, *targ;
   enum OSTYPE OS=OSOther, XOS=OSOther;
   enum MACHTYPE Xmach;
   enum F2CNAME f2cnam;
   enum F2CINT f2cint;
   enum F2CSTRING f2cstr;
   enum ISAEXT ISAX;
   enum ASMDIA ASMD;
   enum USERGEMM USERMM=UG_None;
   FILE *fpout, *fplog, *fps[2];
   const char *lognam="ConfSummary.log", *dmpnam="ConfDump.log";
   char redir[128];
   sprintf(redir, ">> %s 2>&1", dmpnam);

   F2CDEFS[0] = '\0';
   remove(dmpnam);
   fplog = fopen(lognam, "w");
   fps[0] = stdout;
   fps[1] = fplog;
   assert(fplog);
   assert(tmpnam(tnam));
   ATL_mprintf(2, fplog, stdout, "ATLAS3.6.0 configure started.\n\n");
   NLINES = GetScreenHeight();

   DisplayFile("CONFIG/errata.txt", stdout, NLINES);
   if (!IsYes('y', "", "Have you scoped the errata file?")) exit(-1);
   DisplayFile("CONFIG/init.txt", stdout, NLINES);
   if (!IsYes('y', "", "Are you ready to continue?")) exit(-1);

/*
 * It doesn't seem to quite work right anymore, rsh is no longer available,
 * and ssh-based is sloooooowww, so just disable cross-compilation for now
 * Need to replace with version that doesn't require shared filesystem anyway
 */
#if 0
   targ = GetXCompTarg(stdout, fplog, TARGNAM);
#else
   targ = NULL;
#endif
   if (targ) XCOMP = 1;
/*
 * Set up some good defaults
 */
   if (TOPdir[0] == '\0')
   {
      i = CmndOneLine(targ, "pwd", ln);
      if (i || ln[0] == '\0')
      {
         sp = getenv("PWD");
         if (sp) strcpy(TOPdir, sp);
         else strcpy(TOPdir, "$(HOME)/ATLAS");
      }
      else
      {
         for (i=0; TOPdir[i] = ln[i]; i++);
         i--;
         while(Mciswspace(TOPdir[i])) i--;
         TOPdir[i+1] = '\0';
      }
   }
   strcpy(LIBdir, "$(TOPdir)/lib/$(ARCH)");
   strcpy(ATLASlib, "libatlas.a");
   strcpy(F77BLASlib, "libf77blas.a");
   strcpy(CBLASlib, "libcblas.a");
   strcpy(PTCBLASlib, "libptcblas.a");
   strcpy(PTF77BLASlib, "libptf77blas.a");
   strcpy(ARCHIVER, "ar");
   strcpy(ARFLAGS, "r");
   strcpy(RANLIB, "echo");
   L2SIZE = MAXL2SIZE;

   OS = GetOS(stdout, fplog, targ);
   ISWIN = OSIsWin(OS);
   if (mach == MACHOther)
      mach = GetArch(stdout, fplog, OS, targ, TOPdir);
   ASMD = ProbeAsm(targ, TOPdir, OS, mach);
   ISAX = GetISAExt(targ, TOPdir);
   USERMM = GetUserMM(OS, mach, UMMdir, UMMdef);
   if (ISAX == ISA_3DNow1 || ISAX == ISA_3DNow2)
   {
      DisplayFile("CONFIG/3DNow.txt", stdout, NLINES);
      Use3DNow = IsYes('n', "   ", "Use 3DNow! for computation?");
   }
   i = ncpu;
   if (!ncpu) ncpu = ProbeNCPU(OS, mach, targ, TOPdir);
   if (ncpu != 1) /* user may want to thread */
   {
      DisplayFile("CONFIG/pthread.txt", stdout, NLINES);
      if ( IsYes(ncpu > 1 ? 'y' : 'n', "   ", "enable Posix threads support?") )
      {
         THREADS=1;
         if (!i) ncpu = GetNCPU(OS, mach, targ, TOPdir);
      }
      else ncpu = 1;
   }
   if (ncpu) ATL_mprintf(2, fplog, stdout, "Number of CPUs: %d\n\n", ncpu);
   else ATL_mprintf(2, fplog, stdout,
                    " Unable to determine number of CPUs.\n\n");

   strcpy(ARCH0, machnam[mach]);
   if (ISAX != ISA_None)
   {
      if (ISAX == ISA_3DNow1 || ISAX == ISA_3DNow2)
      {
         if (Use3DNow) strcat(ARCH0, ISAXNAM[ISAX]);
      }
      else strcat(ARCH0, ISAXNAM[ISAX]);
   }
   if (USERMM) strcat(ARCH0, usermmnam[USERMM]);
   L1SIZE = GetCacheSize(OS,  mach, targ, TOPdir, 1, &i);
   if (!i) L1SIZE = -1;

/*
 * Linux paging may screw up L1 cache on 604e, so force detection
 */
   if (OS == OSLinux && mach == PPC604e) L1SIZE = -1;

   L2SIZE = GetCacheSize(OS,  mach, targ, TOPdir, -2, &L2IsKnown) * 1024;
   if (L2IsKnown)
      ATL_mprintf(2, fplog, stdout,
                  "Required cache flush detected as : %d bytes\n", L2SIZE);
   i = GetCompInfo(stdout, fplog, OS, mach, ISAX, targ, redir, TOPdir,
                   F77, F77FLAGS, FLINKER, FLINKFLAGS, FCLINKFLAGS,
                   GOODGCC, CC, CCFLAGS, CLINKER, CLINKFLAGS,
                   MCC, MMFLAGS, BLASlib);
   if (!i) /* no good gcc is found, issue warning */
   {
      if (MachIsX86(mach)) /* bad gcc warnings */
      {
         BADMCC = 1;
         DisplayFile("CONFIG/gcc3x86.txt", stdout, NLINES);
         if (IsYes('y', "   ", "Stop ATLAS install?")) exit(-1);
      }
      else if (MachIsUS(mach))
      {
         BADMCC = 1;
         DisplayFile("CONFIG/gcc3US.txt", stdout, NLINES);
         if (IsYes('n', "   ", "Stop ATLAS install?")) exit(-1);
      }
      else if (mach == Dec21164 || mach == Dec21264)
      {
         BADMCC = 1;
         DisplayFile("CONFIG/gcc3evX.txt", stdout, NLINES);
         if (IsYes('y', "   ", "Stop ATLAS install?")) exit(-1);
      }
      else if (mach == IA64Itan || mach == IA64Itan2)
      {
         BADMCC = 1;
         DisplayFile("CONFIG/gcc3ia64.txt", stdout, NLINES);
         if (IsYes('y', "   ", "Stop ATLAS install?")) exit(-1);
      }
      else if (mach == PPCG4 && OS == OSLinux)
      {
         BADMCC = 1;
         DisplayFile("CONFIG/gcc33ppc.txt", stdout, NLINES);
         if (IsYes('y', "   ", "Stop ATLAS install?")) exit(-1);
      }
   }

   GetSyslib(OS, mach, targ, LIBS);
   if (XCOMP)
   {
      delay = 60;
      ATL_mprintf(2, fplog, stdout, "\n\n*******************************************************************************\n");
      ATL_mprintf(2, fplog, stdout, "****************  FINDING INFO ABOUT CROSS-COMPILING MACHINE  *****************\n");
      ATL_mprintf(2, fplog, stdout, "*******************************************************************************\n");
      XOS = GetOS(stdout, fplog, NULL);
      Xmach = GetArch(stdout, fplog, XOS, NULL, TOPdir);
      GetCompInfo(stdout, fplog, XOS, Xmach, ISA_None, NULL, redir, TOPdir,
                  NULL, NULL, NULL, NULL, NULL, XCC, XCCFLAGS, NULL, NULL,
                  NULL, NULL, NULL, NULL);
      ATL_mprintf(2, fplog, stdout, "\n\n*******************************************************************************\n");
      ATL_mprintf(2, fplog, stdout, "****************  DONE FINDING INFO ABOUT CROSS-COMPILING MACHINE  ************\n");
      ATL_mprintf(2, fplog, stdout, "*******************************************************************************\n");
   }
   else if (strstr(CC, "ATLwin_") || strstr(CCFLAGS, "-mno-cygwin"))
   {
      strcpy(XCC, GOODGCC);
      strcpy(XCCFLAGS, "-O");
   }
   else
   {
      strcpy(XCC, CC);
      strcpy(XCCFLAGS, CCFLAGS);
   }
   if (OS == OSSunOS4 || OS == OSOSX) strcpy(RANLIB, "ranlib");

   ATL_mprintf(2, fplog, stdout, "FINDING tar, gzip, AND gunzip\n");
   FindTarZip(TAR, GZIP, GUNZIP);
   ATL_mprintf(2, fplog, stdout, "   tar    : %s\n", TAR);
   ATL_mprintf(2, fplog, stdout, "   gzip   : %s\n", GZIP);
   ATL_mprintf(2, fplog, stdout, "   gunzip : %s\n", GUNZIP);
   GetPmake(OS, ncpu, PMAKE);

   if (OS != OSOther)
   {
      if (mach != MACHOther)
         fprintf(stdout,
         "\n\nATLAS has default parameters for OS=\'%s\' and system=\'%s\'.\n",
                 osnam[OS], machnam[mach]);
      else
         fprintf(stdout,
"\n\nAlthough your machine is not known, ATLAS has default parameters for OS=\'%s\'.\n",
                 osnam[OS]);
      fprintf(stdout,
"If you want to just trust these default values, you can use express setup,\n");
      fprintf(stdout,
"drastically reducing the amount of questions you are required to answer\n\n");
      GOGO = IsYes('y', "   ", "use express setup?");
      fprintf(stdout, "\n\n");
   }

   ierr = 0;
   DisplayFile("CONFIG/arch.txt", stdout, NLINES);
   if (*ARCH == '\0')
   {
      if (OS == OSOther) strcpy(ARCH, "UNKNOWN");
      else strcpy(ARCH, osnam[OS]);
      strcat(ARCH, "_");
      strcat(ARCH, ARCH0);
      ierr = 1;
   }
   if (THREADS) /* add ncpu to ARCH */
   {
      for (i=0; ARCH[i]; i++);
      sprintf(ARCH+i, "_%d", ncpu);
   }
   do
   {
      if (ierr)
      {
         GetString(stdin, ARCH, "   ", "Architecture name (ARCH)", 127, ARCH);
         sprintf(ln, "Make.%s", ARCH);
      }
      ierr = FileIsThere(ln);
      if (ierr)
      {
         fprintf(stderr, "File Make.%s already exists!\n", ARCH);
         ierr = !IsYes('n', "", "overwrite it?");
      }
   }
   while (ierr);
   ATL_mprintf(2, stdout, fplog, "<arch> set to \'%s\'\n\n", ARCH);

   if (!L2IsKnown)
   {
      DisplayFile("CONFIG/l2size.txt", stdout, NLINES);
      L2SIZE = GetIntRange(L2SIZE/1024, 64, 16*1024, "   ",
                           "Maximum cache size (KB)");
      L2SIZE *= 1024;
   }
   else if (L2SIZE > MAXL2SIZE)
   {
      DisplayFile("CONFIG/l2sizemax.txt", stdout, NLINES);
      L2SIZE = GetIntRange(L2SIZE/1024, 64, 16*1024, "   ",
                           "Maximum cache size (KB)");
      L2SIZE *= 1024;
   }
   DisplayFile("CONFIG/nfsdelay.txt", stdout, NLINES);
   delay = GetIntRange(delay, 0, 600, "   ", "File creation delay in seconds");
   if (!GOGO)
   {
      GetString(stdin, TOPdir, "   ", "Top level ATLAS directory", 255, TOPdir);
      GetString(stdin, LIBdir, "   ",
                "Directory to build libraries in", 255, LIBdir);

      GetString(stdin, ATLASlib, "   ",
                "Library name for ATLAS primitives", 255, ATLASlib);
      GetString(stdin, CBLASlib, "   ",
                "Library name for the C interface to BLAS", 255, CBLASlib);
      GetString(stdin, F77BLASlib, "   ",
                "Library name for the Fortran77 interface to BLAS",
                255, F77BLASlib);
      if (THREADS)
      {
         GetString(stdin, PTCBLASlib, "   ",
                   "Library name for the C interface to BLAS", 255, PTCBLASlib);
         GetString(stdin, PTF77BLASlib, "   ",
                   "Library name for the Fortran77 interface to BLAS",
                   255, PTF77BLASlib);
      }
   }

   DisplayFile("CONFIG/f77exp.txt", stdout, NLINES);
   ierr = 0;
   do
   {
      if (ierr != 0)
      {
         if (GOGO) DisplayFile("CONFIG/f77exp.txt", stdout, NLINES);
         GetString(stdin, F77, "   ", "f77 compiler", 127, F77);
         GetString(stdin, F77FLAGS, "   ", "F77 Flags", 511, F77FLAGS);
      }
      sprintf(ln, "%s -c CONFIG/tst.f %s\n", F77, redir);
      ierr = system(ln);
      if (ierr)
      {
         if (GOGO) DisplayFile("CONFIG/f77exp.txt", stdout, NLINES);
         fprintf(stderr, "F77 = \'%s %s\' doesn't seem to work for me.\n",
                 F77, F77FLAGS);
         ierr = GetIntRange(1, 0, 1, "   ",
                   "1 to enter a different F77, 0 to continue with none");
      }
   }
   while (ierr);
   if (!GOGO)
   {
      GetString(stdin, FLINKER, "   ", "F77 Linker ", 127, FLINKER);
      GetString(stdin, FLINKFLAGS, "   ", "F77 Link Flags ", 511, FLINKFLAGS);
      fprintf(stdout, "\n\n");
   }
   ATL_mprintf(2, fplog, stdout, "F77 & FLAGS: %s %s\n", F77, F77FLAGS);
   ATL_mprintf(2, fplog, stdout, "FLINKER & FLAGS: %s %s\n\n",
               FLINKER, FLINKFLAGS);

   ierr = !GOGO;
   if (!GOGO) DisplayFile("CONFIG/ccomp.txt", stdout, NLINES);
   do
   {
      if (ierr != 0)
      {
         if (GOGO) DisplayFile("CONFIG/ccomp.txt", stdout, NLINES);
         GetString(stdin, CC, "   ", "ANSI C compiler(CC)", 127, CC);
         GetString(stdin, CCFLAGS, "   ", "C Flags (CCFLAGS)", 511, CCFLAGS);
      }
      sprintf(ln, "%s %s -c CONFIG/tst.c %s\n", CC, CCFLAGS, redir);
      ierr = system(ln);
      if (ierr)
      {
         fprintf(stderr,
                 "Compiling with = \'%s %s\' doesn't seem to work for me.\n",
                 CC, CCFLAGS);
         ierr = GetIntRange(1, 0, 1, "   ",
                           "1 to try different CC/flags, 0 to continue anyway");
      }
   }
   while(ierr);
   ATL_mprintf(2, fplog, stdout, "CC & FLAGS: %s %s\n", CC, CCFLAGS);

   if (MachIsAlpha(mach)&& !GOGO)
      DisplayFile("CONFIG/ccomp-alpha.txt", stdout, NLINES);
   ierr = !GOGO;
   do
   {
      if (ierr != 0)
      {
         if (GOGO) DisplayFile("CONFIG/ccomp-alpha.txt", stdout, NLINES);
         GetString(stdin, MCC, "   ", "C compiler for generated code (MCC)",
                   127, MCC);
         GetString(stdin, MMFLAGS, "   ", "C FLAGS (MMFLAGS)", 511, MMFLAGS);
      }
      Wstrfndsub("$(CC)", CC, MCC, comp);
      Wstrfndsub("$(CCFLAGS)", CCFLAGS, MMFLAGS, ln);
      Wstrfndsub("$(CDEFS)", "", ln, cflg);
      sprintf(ln, "%s %s -c CONFIG/tst.c %s\n", comp, cflg, redir);
      ierr = system(ln);
      if (ierr)
      {
         fprintf(stderr,
                 "Compiling with \'%s %s\' doesn't seem to work for me.\n",
                 MCC, MMFLAGS);
         ierr = GetIntRange(1, 0, 1, "   ",
                            "1 to re-enter, 0 to continue anyway");
      }
   }
   while(ierr);
   if (!GOGO)
   {
      GetString(stdin, CLINKER, "   ", "C Linker ", 127, CLINKER);
      GetString(stdin, CLINKFLAGS, "   ", "C Link Flags ", 511, CLINKFLAGS);
   }
   ATL_mprintf(2, fplog, stdout, "MCC & FLAGS: %s %s\n", MCC, MMFLAGS);
   ATL_mprintf(2, fplog, stdout, "CLINKER & FLAGS: %s %s\n\n",
               CLINKER, CLINKFLAGS);
   if (XCOMP)
   {
      ierr = !GOGO;
      do
      {
         if (ierr != 0)
         {
            GetString(stdin, XCC, "   ",
                      "ANSI C compiler for cross-compiling machine (XCC)",
                      127, XCC);
            GetString(stdin, XCCFLAGS, "   ",
                      "C Flags for cross-compiling machine (XCCFLAGS)",
                      511, XCCFLAGS);
         }
         Wstrfndsub("$(CC)", CC, XCC, comp);
         Wstrfndsub("$(CCFLAGS)", CCFLAGS, XCCFLAGS, ln);
         Wstrfndsub("$(CDEFS)", "", ln, cflg);
         sprintf(ln, "%s %s -c CONFIG/tst.c %s\n", comp, cflg, redir);
         ierr = system(ln);
         if (ierr)
         {
            fprintf(stderr,
                    "Compiling with \'%s %s\' doesn't seem to work for me.\n",
                    XCC, XCCFLAGS);
            ierr = GetIntRange(1, 0, 1, "   ",
                               "1 to re-enter, 0 to continue anyway");
         }
      }
      while(ierr);
      ATL_mprintf(2, fplog, stdout, "XCC & FLAGS: %s %s\n\n", XCC, XCCFLAGS);
   }

   if (!GOGO)
   {
      GetString(stdin, ARCHIVER, "   ", "Archiver ", 127, ARCHIVER);
      GetString(stdin, ARFLAGS, "   ", "Archiver flags ", 255, ARFLAGS);
      GetString(stdin, RANLIB, "   ", "Ranlib ", 127, RANLIB);
      GetString(stdin, BLASlib, "   ", "BLAS library", 255, BLASlib);
      GetString(stdin, LIBS, "   ", "General and system libs", 255, LIBS);
   }

   fprintf(stdout,
           "\nFinding F77 to C calling conventions (this may take a while):\n");
   findF2C(redir, targ, TOPdir, F77, F77FLAGS, FLINKER, FLINKFLAGS, CC, CCFLAGS,
           &f2cnam, &f2cint, &f2cstr);
   fprintf(stdout, "\nCalculated F77/C interoperation conventions:\n");
   sp = F2CDEFS;
   if (f2cnam == f2c_Add_)
   {
      ATL_mprintf(2, fplog, stdout, "   Suffix F77 names with _\n");
      strcpy(sp, "-DAdd_ ");
      sp += 7;
   }
   else if (f2cnam == f2c_NoChange)
   {
      ATL_mprintf(2, fplog, stdout, "   C & F77 share the same namespace\n");
      strcpy(sp, "-DNoChange ");
      sp += 11;
   }
   else if (f2cnam == f2c_UpCase)
   {
      strcpy(sp, "-DUpCase ");
      sp += 9;
      ATL_mprintf(2, fplog, stdout, "   Make F77 names upper case\n");
   }
   else if (f2cnam == f2c_Add__)
   {
      strcpy(sp, "-DAdd__ ");
      sp += 8;
      ATL_mprintf(2, fplog, stdout,
                  "   Suffix F77 names with underscores with __\n");
   }
   else ATL_mprintf(2, fplog, stdout,
                    "   Unable to determine naming conventions\n");

   if (f2cint == FintCint)
      ATL_mprintf(2, fplog, stdout, "   F77 INTEGER -> C int\n");
   else if (f2cint == FintClong)
   {
      ATL_mprintf(2, fplog, stdout, "   F77 INTEGER -> C long\n");
      strcpy(sp, "-DF77_INTEGER=long ");
      sp += 19;
   }
   else if (f2cint == FintCshort)
   {
      ATL_mprintf(2, fplog, stdout,"   F77 INTEGER -> C short\n");
      strcpy(sp, "-DF77_INTEGER=short ");
      sp += 20;
   }
   else ATL_mprintf(2, fplog, stdout,
                    "   Unable to determine F77/C integer correspondence\n");

   if (f2cstr == fstrSun)
   {
      strcpy(sp, "-DStringSunStyle");
      ATL_mprintf(2, fplog, stdout,
                  "   F77 strings handled via standard sun style\n");
   }
   else if (f2cstr == fstrCray)
   {
      strcpy(sp, "-DCRAY");
      ATL_mprintf(2, fplog, stdout, "   F77 strings handled using CRAY fcd\n");
   }
   else if (f2cstr == fstrStructVal)
   {
      strcpy(sp, "-DStringStructVal");
      ATL_mprintf(2, fplog, stdout,
                  "   F77 strings handled by passing a struct\n");
   }
   else if (f2cstr == fstrStructPtr)
   {
      strcpy(sp, "-DStringStructPtr");
      ATL_mprintf(2, fplog, stdout,
                  "   F77 strings handled by passing a pointer to struct\n");
   }
   else ATL_mprintf(2, fplog, stdout,
                    "   Unable to determine F77/C string interoperation\n");

   mmdef[0] = archdef[0] = '\0';
   j = 1;
   if (mach != MACHOther && !BADMCC && (flag & CF_ARCHDEF))
   {
      if (mach == SunUS4) sp = "SunUS2";
      else sp = ARCH0;
      sprintf(ln, "CONFIG/ARCHS/%s.tgz", sp);
      if (FileIsThere(ln))
      {
         DisplayFile("CONFIG/NoSearch.txt", stdout, NLINES);
         if ( IsYes('y', "", "Use supplied default values for install?") )
         {
            ATL_mprintf(2, fplog, stdout,
                        "\n\nUnpacking Architectural defaults . . . ");
            sprintf(ln, "cd CONFIG/ARCHS ; %s -c %s.tgz | %s xf - %s\n",
                    GUNZIP, sp, TAR, redir);
            system(ln);
            ATL_mprintf(2, fplog, stdout, "done.\n");
            GetArchDef(OS, mach, sp, CC, CCFLAGS, MCC, MMFLAGS, mmdef, archdef);
            sprintf(ln, "%s%s/dAXPBY_SUMM", TOPdir, archdef+9);
            j = !FileIsThere(ln);
        }
      }
   }
   if (j && (flag & CF_ARCHDEF))
   {
      DisplayFile("CONFIG/l1def.txt", stdout, NLINES);
      USEDEFL1 = !IsYes('y', "", "Tune the Level 1 BLAS?");
   }

   ATL_mprintf(2, fplog, stdout,"\nCreating make include file Make.%s\n", ARCH);
   sprintf(ln, "Make.%s", ARCH);
   fpout = fopen(ln, "w");
   assert(fpout);
   fprintf(fpout, "#  -----------------------------\n");
   fprintf(fpout, "#  Make.ARCH for ATLAS3.6.0\n");
   fprintf(fpout, "#  -----------------------------\n\n");

   fprintf(fpout, "#  ----------------------------------\n");
   fprintf(fpout, "#  Make sure we get the correct shell\n");
   fprintf(fpout, "#  ----------------------------------\n");
   fprintf(fpout, "   SHELL = /bin/sh\n\n");

   fprintf(fpout, "#  -------------------------------------------------\n");
   fprintf(fpout, "#  Name indicating the platform to configure BLAS to\n");
   fprintf(fpout, "#  -------------------------------------------------\n");
   fprintf(fpout, "   ARCH = %s\n\n", ARCH);

   fprintf(fpout, "#  -------------------\n");
   fprintf(fpout, "#  Various directories\n");
   fprintf(fpout, "#  -------------------\n");
   fprintf(fpout, "   TOPdir = %s\n", TOPdir);
   fprintf(fpout, "   INCdir = $(TOPdir)/include/$(ARCH)\n");
   fprintf(fpout, "   SYSdir = $(TOPdir)/tune/sysinfo/$(ARCH)\n");
   fprintf(fpout, "   GMMdir = $(TOPdir)/src/blas/gemm/$(ARCH)\n");
   fprintf(fpout, "   UMMdir = %s\n", UMMdir);
   fprintf(fpout, "   GMVdir = $(TOPdir)/src/blas/gemv/$(ARCH)\n");
   fprintf(fpout, "   GR1dir = $(TOPdir)/src/blas/ger/$(ARCH)\n");
   fprintf(fpout, "   L1Bdir = $(TOPdir)/src/blas/level1/$(ARCH)\n");
   fprintf(fpout, "   L2Bdir = $(TOPdir)/src/blas/level2/$(ARCH)\n");
   fprintf(fpout, "   L3Bdir = $(TOPdir)/src/blas/level3/$(ARCH)\n");
   fprintf(fpout, "   TSTdir = $(TOPdir)/src/testing/$(ARCH)\n");
   fprintf(fpout, "   AUXdir = $(TOPdir)/src/auxil/$(ARCH)\n");
   fprintf(fpout, "   CBLdir = $(TOPdir)/interfaces/blas/C/src/$(ARCH)\n");
   fprintf(fpout, "   FBLdir = $(TOPdir)/interfaces/blas/F77/src/$(ARCH)\n");
   fprintf(fpout, "   BINdir = $(TOPdir)/bin/$(ARCH)\n");
   fprintf(fpout, "   LIBdir = %s\n", LIBdir);
   fprintf(fpout, "   PTSdir = $(TOPdir)/src/pthreads\n");
   fprintf(fpout, "   MMTdir = $(TOPdir)/tune/blas/gemm/$(ARCH)\n");
   fprintf(fpout, "   MVTdir = $(TOPdir)/tune/blas/gemv/$(ARCH)\n");
   fprintf(fpout, "   R1Tdir = $(TOPdir)/tune/blas/ger/$(ARCH)\n");
   fprintf(fpout, "   L1Tdir = $(TOPdir)/tune/blas/level1/$(ARCH)\n");
   fprintf(fpout, "   L3Tdir = $(TOPdir)/tune/blas/level3/$(ARCH)\n\n");

   fprintf(fpout,
"#  ---------------------------------------------------------------------\n");
   fprintf(fpout,
"#  Name and location of scripts for running executables during tuning\n");
   fprintf(fpout,
"#  ---------------------------------------------------------------------\n");
   fprintf(fpout, "   ATLRUN = $(BINdir)/ATLrun.sh\n");
   fprintf(fpout, "   ATLFWAIT = $(BINdir)/xatlas_waitfile\n\n");

   fprintf(fpout, "#  ---------------------\n");
   fprintf(fpout, "#  Libraries to be built\n");
   fprintf(fpout, "#  ---------------------\n");
   fprintf(fpout, "   ATLASlib = $(LIBdir)/%s\n", ATLASlib);
   fprintf(fpout, "   CBLASlib = $(LIBdir)/%s\n", CBLASlib);
   fprintf(fpout, "   F77BLASlib = $(LIBdir)/%s\n", F77BLASlib);
   if (THREADS)
   {
      fprintf(fpout, "   PTCBLASlib = $(LIBdir)/%s\n", PTCBLASlib);
      fprintf(fpout, "   PTF77BLASlib = $(LIBdir)/%s\n", PTF77BLASlib);
   }
   fprintf(fpout, "   LAPACKlib = $(LIBdir)/liblapack.a\n\n");
   fprintf(fpout, "   TESTlib = $(LIBdir)/libtstatlas.a\n");

   fprintf(fpout, "#  -------------------------------------------\n");
   fprintf(fpout, "#  Upper bound on largest cache size, in bytes\n");
   fprintf(fpout, "#  -------------------------------------------\n");
   fprintf(fpout, "   L2SIZE = -DL2SIZE=%d\n\n", L2SIZE);

   fprintf(fpout, "#  ---------------------------------------\n");
   fprintf(fpout, "#  Command setting up correct include path\n");
   fprintf(fpout, "#  ---------------------------------------\n");
   fprintf(fpout,
      "   INCLUDES = -I$(TOPdir)/include -I$(TOPdir)/include/$(ARCH) \\\n");
   fprintf(fpout,
      "              -I$(TOPdir)/include/contrib \n\n");

   fprintf(fpout, "#  -------------------------------------------\n");
   fprintf(fpout, "#  Defines for setting up F77/C interoperation\n");
   fprintf(fpout, "#  -------------------------------------------\n");
   fprintf(fpout, "   F2CDEFS = %s\n\n", F2CDEFS);

   fprintf(fpout, "#  --------------------------------------\n");
   fprintf(fpout, "#  Special defines for user-supplied GEMM\n");
   fprintf(fpout, "#  --------------------------------------\n");
   fprintf(fpout, "   UMMDEFS = %s\n\n", UMMdef);

   fprintf(fpout, "#  ------------------------------\n");
   fprintf(fpout, "#  Architecture identifying flags\n");
   fprintf(fpout, "#  ------------------------------\n");
   fprintf(fpout, "   ARCHDEFS =");
   if (OS != OSOther) fprintf(fpout, " -DATL_OS_%s", osnam[OS]);
   if (mach != MACHOther) fprintf(fpout, " -DATL_ARCH_%s", machnam[mach]);
   if (USERMM) fprintf(fpout, " -DUSERGEMM");
   if (OSIsWin(OS))
      fprintf(fpout, " -DGCCWIN -DUseClock");
   if (ISAX == ISA_AV && strstr(CC, "gcc") && strstr(MCC, "gcc"))
      fprintf(fpout, " -DATL_%s -DATL_AVgcc", ISAXNAM[ISAX]);

   else if (ISAX != ISA_None) fprintf(fpout, " -DATL_%s", ISAXNAM[ISAX]);
   if (ISAX == ISA_SSE2) fprintf(fpout, " -DATL_SSE1");
   if (Use3DNow) fprintf(fpout, " -DATL_3DNowFLOPS");
   if (mach == IA64Itan || mach == IA64Itan2 )
      fprintf(fpout, " -DATL_MAXNREG=128");
   if (ASMD != ASM_None) fprintf(fpout, " -DATL_%s", ASMNAM[ASMD]);
   if (mach == AmdHammer32 && (OS != OSWinNT && OS != OSWin9x))
      fprintf(fpout, " -m32");
   else if (mach == AmdHammer64) fprintf(fpout, " -m64");
   if (mach == IA64Itan2 && strstr(CC, "icc"))
      fprintf(fpout, " -DATL_IntelIccBugs");
   fprintf(fpout, "\n\n");

   fprintf(fpout,
   "#  -------------------------------------------------------------------\n");
   fprintf(fpout,
   "#  NM is the flag required to name a compiled object/executable\n");
   fprintf(fpout,
   "#  OJ is the flag required to compile to object rather than executable\n");
   fprintf(fpout, "#  These flags are used by all compilers.\n");
   fprintf(fpout,
   "#  -------------------------------------------------------------------\n");
   fprintf(fpout, "   NM = -o\n");
   fprintf(fpout, "   OJ = -c\n\n");

   DisplayFile("CONFIG/f77make.txt", fpout, 0);
   fprintf(fpout, "   F77 = %s\n", F77);
   fprintf(fpout, "   F77FLAGS = %s\n", F77FLAGS);
   fprintf(fpout, "   FLINKER = %s\n", FLINKER);
   fprintf(fpout, "   FLINKFLAGS = %s\n", FLINKFLAGS);
   fprintf(fpout, "   FCLINKFLAGS = %s\n\n", FCLINKFLAGS);

   DisplayFile("CONFIG/CCmake.txt", fpout, 0);
   fprintf(fpout, "   CDEFS = $(L2SIZE) $(INCLUDES) $(F2CDEFS) $(ARCHDEFS)");
   if (UCDEF[0])
      fprintf(fpout, " %s", UCDEF);
   if (THREADS)
   {
      fprintf(fpout, " -DATL_NCPU=%d", ncpu);
      if (OS == OSFreeBSD) fprintf(fpout, " -D_THREAD_SAFE -D_REENTRANT");
      if (OS == OSAIX) fprintf(fpout, " -DIBM_PT_ERROR");
      if (OS == OSIRIX) fprintf(fpout, " -D_POSIX_C_SOURCE=199506L");
   }
   if (delay) fprintf(fpout, " -DATL_FOPENDELAY");
   fprintf(fpout, "\n\n");

   if (*GOODGCC)
   {
      i = strlen(GOODGCC);
      for (i--; i >= 0 && isspace(GOODGCC[i]); i--) GOODGCC[i] = '\0';
      if (i > 3 && GOODGCC[i] == 'c' && GOODGCC[i-1] == 'c' &&
          GOODGCC[i-2] == 'g' && GOODGCC[i-3] == '/')
      {
         GOODGCC[i-3] = '\0';
         fprintf(fpout, "   GCCdir = %s\n", GOODGCC);
         GOODGCC[i-3] = '/';

      }
      fprintf(fpout, "   GOODGCC = %s\n", GOODGCC);
   }
   else fprintf(fpout, "   GOODGCC = gcc\n");
   fprintf(fpout, "   CC = %s\n", CC);
   fprintf(fpout, "   CCFLAG0 = %s\n", CCFLAGS);
   fprintf(fpout, "   CCFLAGS = $(CDEFS) $(CCFLAG0)\n");
   fprintf(fpout, "   MCC = %s\n", MCC);
   fprintf(fpout, "   MMFLAGS = %s\n", MMFLAGS);
   fprintf(fpout, "   XCC = %s\n", XCC);
   if (strstr(XCCFLAGS, "CCFLAGS"))
      fprintf(fpout, "   XCCFLAGS = %s\n", XCCFLAGS);
   else fprintf(fpout, "   XCCFLAGS = $(CDEFS) %s\n", XCCFLAGS);
   fprintf(fpout, "   CLINKER = %s\n", CLINKER);
   fprintf(fpout, "   CLINKFLAGS = %s\n", CLINKFLAGS);
   if (SOLERR)
   {
      fprintf(fpout, "   BC = %s\n", BC);
      fprintf(fpout, "   BCFLAGS = $(CDEFS) %s\n", BCFLAGS);
   }
   else if (strstr(CC, "ATLwin_") || strstr(CCFLAGS, "-mno-cygwin"))
   {
      fprintf(fpout, "   BC = $(XCC)\n", CC);
      fprintf(fpout, "   BCFLAGS = $(XCCFLAGS)\n");
   }
   else
   {
      fprintf(fpout, "   BC = $(CC)\n");
      fprintf(fpout, "   BCFLAGS = $(CCFLAGS)\n");
   }
   fprintf(fpout, "   ARCHIVER = %s\n", ARCHIVER);
   fprintf(fpout, "   ARFLAGS  = %s\n", ARFLAGS);
   fprintf(fpout, "   RANLIB   = %s\n\n", RANLIB);

   fprintf(fpout, "#  -------------------------------------\n");
   fprintf(fpout, "#  tar, gzip, gunzip, and parallel make\n");
   fprintf(fpout, "#  -------------------------------------\n");
   fprintf(fpout, "   TAR    = %s\n", TAR);
   fprintf(fpout, "   GZIP   = %s\n", GZIP);
   fprintf(fpout, "   GUNZIP = %s\n", GUNZIP);
   fprintf(fpout, "   PMAKE  = %s\n\n", PMAKE);

   fprintf(fpout, "#  ------------------------------------\n");
   fprintf(fpout, "#  Reference and system libraries\n");
   fprintf(fpout, "#  ------------------------------------\n");
   fprintf(fpout, "   BLASlib = %s\n", BLASlib);
   fprintf(fpout, "   FBLASlib = \n");
   fprintf(fpout, "   FLAPACKlib = \n");
   fprintf(fpout, "   LIBS = %s\n\n", LIBS);

   fprintf(fpout,
      "#  ----------------------------------------------------------\n");
   fprintf(fpout,
      "#  ATLAS install resources (include arch default directories)\n");
   fprintf(fpout,
      "#  ----------------------------------------------------------\n");
   fprintf(fpout, "   ARCHDEF = %s\n", archdef);
   fprintf(fpout, "   MMDEF = %s\n", mmdef);
   if (USEDEFL1) fprintf(fpout, "   INSTFLAGS = -1 1\n\n");
   else fprintf(fpout, "   INSTFLAGS = \n\n");
   fprintf(fpout, "#  ---------------------------------------\n");
   fprintf(fpout, "#  Generic targets needed by all makefiles\n");
   fprintf(fpout, "#  ---------------------------------------\n");
   if (delay)
   {
      fprintf(fpout, "   waitfile = wfdefault\n");
      fprintf(fpout, "waitfile:\n\tcd $(BINdir) ; make xatlas_waitfile\n");
      fprintf(fpout, "\t$(ATLFWAIT) -s %d -f $(waitfile)\n", delay);
   }
   else fprintf(fpout, "waitfile:\n");
   fclose(fpout);
   ATL_mprintf(2, fplog, stdout, "Make.%s successfully created.\n\n", ARCH);

   if (flag & CF_CREATEATLRUN)
   {
      ATL_mprintf(2, fplog, stdout, "\n\nCreating ATLrun.sh\n\n", ARCH);
      sprintf(ln, "CONFIG/ATLrun.%s", ARCH);
      fpout = fopen(ln, "w");
      assert(fpout);
      fprintf(fpout, "#!/bin/sh\n");
      fprintf(fpout, "atldir=$1\nshift\n");
      if (XCOMP)
         fprintf(fpout, "exe=$1\nshift\nssh %s \"cd $atldir ; ./$exe $*\"",
                 TARGNAM);
      else fprintf(fpout, "$atldir/$*\n");
      fclose(fpout);
      sprintf(ln, "chmod a+rx CONFIG/ATLrun.%s\n", ARCH);
      assert(system(ln) == 0);
   }

   if (flag & CF_CREATEDIRS)
   {
      fprintf(stdout, "Creating subdirectories:\n");
      fprintf(stdout, "   Checking for already existing subdirectories  .....");
      fflush(stdout);
      sprintf(ln, "cd bin/%s %s", ARCH, redir);
      i = 1;
      if (system(ln) == 0)
      {
         fprintf(stdout, "...... found!\n");
         i = IsYes('y', "", "kill old subdirectories?");
      }
      else fprintf(stdout, "... no\n");
      if (i)
      {
         sprintf(ln, "make killall arch=%s %s\n", ARCH, redir);
         system(ln);
         sprintf(ln, "make startup arch=%s %s\n", ARCH, redir);
         if (system(ln))
         {
            fps[0] = stderr;
            fps[1] = fplog;
            for (i=0; i < 2; i++)
            {
               fprintf(fps[i], "UNABLE TO CREATE NECESSARY SUBDIRECTORIES:\n");
               fprintf(fps[i],
       "  review Make.%s for accurracy, paying special attention to TOPdir.\n",
                       ARCH);
               fprintf(fps[i], "Examine %s and %s for specifics.\n",
                       lognam, dmpnam);
               fprintf(fps[i],
                  "   for further help, see ATLAS/README/TroubleShoot.txt.\n");
               fprintf(fps[i],
                       "   when you have corrected the problem, finish the\n");
               fprintf(fps[i], "   configuration by typing :\n");
               fprintf(fps[i], "      make startup arch=%s\n", ARCH);
            }
            sprintf(ln, "make killall arch=%s %s\n", ARCH, redir);
            system(ln);
            fclose(fplog);
            exit(-1);
         }
      }
      ATL_mprintf(2, fplog, stdout, "Subdirectories successfully created.\n\n");
   }
   if (L1SIZE != -1 && (flag & CF_STOREL1))
   {
      ATL_mprintf(2, fplog, stdout, "\nStoring L1 cache size of %dKB.\n\n",
                  L1SIZE);
      sprintf(ln, "make -f Make.top ISetL1 arch=%s L1Size=%d %s\n",
              ARCH, L1SIZE, redir);
      if (system(ln))
      {
         ATL_mprintf(2, fplog, stderr,
                     "Unable to store L1CacheSize, aborting.\n");
         fclose(fplog);
         exit(-1);
      }
   }
   fprintf(fplog, "\n\nConfiguration completed successfully.\n\n");
   fclose(fplog);
   if (flag & CF_MOVELOG)
   {
      fprintf(stdout,
              "\nMoving config logfiles %s and %s to bin/%s/INSTALL_LOG/\n",
              lognam, dmpnam, ARCH);
      sprintf(ln, "mv -f %s bin/%s/INSTALL_LOG/.\n", lognam, ARCH);
      system(ln);
      sprintf(ln, "mv -f %s bin/%s/INSTALL_LOG/.\n", dmpnam, ARCH);
      system(ln);
   }
   fprintf(stdout, "\n\nConfiguration completed successfully.  You may want to examine the make include \n");
   fprintf(stdout,
"file (Make.%s) for accuracy before starting the install with the command:\n",
           ARCH);
   fprintf(stdout, "   make install arch=%s\n\n", ARCH);
}

void PrintUsage(char *name)
{
   fprintf(stderr, "USAGE: %s [flags]\n", name);
   DisplayFile("CONFIG/config_flags.txt", stderr, 0);
   exit(-1);
}

int GetFlags(int nargs, char **args, enum MACHTYPE *mach, int *ncpu,
             char *ucdef, char *TOPdir, char *ARCH, char *blaslib,
             char *cc, char *ccflags, char *f77, char *f77flags,
             char *mcc, char *mmflags, char *gcc, char *xcc, char *xccflags)
{
    int i;
    int flag;

    *ncpu = 0;
    *mach = MACHOther;
    flag = CF_ARCHDEF | CF_CREATEDIRS | CF_CREATEATLRUN | CF_CREATEWINF77 |
           CF_STOREL1 | CF_MOVELOG;
   *ARCH = *TOPdir = *cc = *ccflags = *f77 = *f77flags =
           *mcc = *mmflags = *gcc = *xcc = *xccflags = *ucdef = '\0';
   *blaslib = '0'; blaslib[1] = '\0';

   for (i=1; i < nargs; i++)
   {
      if (args[i][0] != '-')
         PrintUsage(args[0]);
      switch(args[i][1])
      {
      case 'C':
         strcpy(ucdef, args[++i]);
         break;
      case 'A':
         *mach = atoi(args[++i]);
         break;
      case 'N':
         *ncpu = atoi(args[++i]);
         break;
      case 'D':
         flag = atoi(args[++i]);
         break;
      case 'a': /* user sets ARCH from command line */
         strcpy(ARCH, args[++i]);
         break;
      case 't':
         strcpy(TOPdir, args[++i]);
         break;
      case 'c':
         strcpy(cc, args[++i]);
         break;
      case 'f':
         strcpy(f77, args[++i]);
         break;
      case 'm':
         strcpy(mcc, args[++i]);
         break;
      case 'g':
         strcpy(gcc, args[++i]);
         break;
      case 'x':
         strcpy(xcc, args[++i]);
         break;
      case 'F':
         i++;
         if (args[i][0] == 'c')
            strcpy(ccflags, args[++i]);
         else if (args[i][0] == 'm')
            strcpy(mmflags, args[++i]);
         else if (args[i][0] == 'f')
            strcpy(f77flags, args[++i]);
         else if (args[i][0] == 'x')
            strcpy(xccflags, args[++i]);
         else
            PrintUsage(args[0]);
         break;
      case 'b':
         i++;
         if (args[i][0] == '1' && args[i][1] == '\0')
            *blaslib = '\0';
         else
            strcpy(blaslib, args[i]);
         break;
      default:
         PrintUsage(args[0]);
      }
   }
   return(flag);
}

main(int nargs, char *args[])
{
   char TOPdir[512], blaslib[512], ARCH[512];
   char F77[512], CC[512], MCC[512], XCC[512], GOODGCC[512];
   char F77FLAGS[512], CCFLAGS[512], MMFLAGS[512], XCCFLAGS[512];
   int flag, ncpu;
   char UCDEF[256];
   enum MACHTYPE mach;

   flag = GetFlags(nargs, args, &mach, &ncpu, UCDEF, TOPdir, ARCH, blaslib,
                   CC, CCFLAGS, F77, F77FLAGS, MCC, MMFLAGS, GOODGCC,
                   XCC, XCCFLAGS);
   GoToTown(flag, mach, ncpu, TOPdir, ARCH, blaslib, F77, F77FLAGS, CC, CCFLAGS,
            MCC, MMFLAGS, GOODGCC, XCC, XCCFLAGS, UCDEF);
   exit(0);
}
