/*
 *             Automatically Tuned Linear Algebra Software v3.6.0
 *                    (C) Copyright 2001 R. Clint Whaley
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
#include <string.h>
#include <assert.h>
#define LNLEN 512
static char ARCHDEFS[LNLEN], ARCHDEF[LNLEN], ARCH[LNLEN], CCFLAGS[LNLEN],
            CC[LNLEN], F2CDEFS[LNLEN], F77FLAGS[LNLEN], F77[LNLEN],
            INSTFLAGS[LNLEN], MCC[LNLEN], MMDEF[LNLEN], MMFLAGS[LNLEN];
static char CCVERS[LNLEN], MCCVERS[LNLEN], F77VERS[LNLEN], SYS[LNLEN];
static char UNAM[64], DATE[128];
char *CmndResults(char *cmnd)
{
   static char tnam[128];
   static int FirstTime=1;
   char ln[512];

   if (FirstTime)
   {
      FirstTime = 0;
      assert(tmpnam(tnam));
   }
   sprintf(ln, "%s > %s\n", cmnd, tnam);
   fprintf(stderr, "system: %s", ln);
   if (!system(ln)) return(tnam);
   else return(NULL);
}

void FixString(char *ln)
/*
 * kills all trailing whitespace from string, and replaces [",`] with '
 */
{
   int i;

   for (i=0; ln[i]; i++) if (ln[i] == '"' || ln[i] == '`') ln[i] = '\'';

   for (i--; i >= 0 && (ln[i] == ' ' || ln[i] == '\n' || ln[i] == '\t'); i--)
      ln[i] = '\0';
}

int CmndOneLine(char *cmnd, char *ln)
/*
 * executes a system call with contents of cmnd, returns the output in ln;
 * Returns value returned by system call
 */
{
   int i;
   FILE *fp;
   char *tnam;

   ln[0] = '\0';
   tnam = CmndResults(cmnd);
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

void GetVers(char *comp, char *vers)
{
   char *vflag[4] = {"--version", "-V", "-v", "-version"};
   char ln[LNLEN];
   int i, iret;

   for (i=0; i < 4; i++)
   {
      sprintf(ln, "%s %s", comp, vflag[i]);
      iret = CmndOneLine(ln, vers);
      if (!iret) return;
   }
   strcpy(vers, "UNKNOWN");
}

void GetMakeMacro(char *str, char *val)
{
   char ln[128];
   sprintf(ln, "make print_%s | fgrep -v make", str);
   if (CmndOneLine(ln, val)) strcpy(val, "UNKNOWN");
}
void GetInstInfo()
{
   GetMakeMacro("F77", F77);
   FixString(F77);
   GetMakeMacro("F77FLAGS", F77FLAGS);
   FixString(F77FLAGS);
   GetMakeMacro("CC", CC);
   FixString(CC);
   GetMakeMacro("CCFLAGS", CCFLAGS);
   FixString(CCFLAGS);
   GetMakeMacro("MCC", MCC);
   FixString(MCC);
   GetMakeMacro("MMFLAGS", MMFLAGS);
   FixString(MMFLAGS);
   GetMakeMacro("ARCHDEFS", ARCHDEFS);
   FixString(ARCHDEFS);
   GetMakeMacro("F2CDEFS", F2CDEFS);
   FixString(F2CDEFS);
   GetMakeMacro("ARCHDEF", ARCHDEF);
   FixString(ARCHDEF);
   GetMakeMacro("MMDEF", MMDEF);
   FixString(MMDEF);
   GetMakeMacro("INSTFLAGS", INSTFLAGS);
   FixString(INSTFLAGS);
   GetMakeMacro("ARCH", ARCH);
   FixString(ARCH);
   GetVers(MCC, MCCVERS);
   FixString(MCCVERS);
   GetVers(CC, CCVERS);
   FixString(CCVERS);
   GetVers(F77, F77VERS);
   FixString(F77VERS);
   if (CmndOneLine("uname -a", SYS)) strcpy(UNAM, "UNKNOWN");
   else FixString(SYS);
   if (CmndOneLine("whoami", UNAM)) strcpy(UNAM, "UNKNOWN");
   FixString(UNAM);
   if (CmndOneLine("date", DATE)) strcpy(DATE, "UNKNOWN");
   FixString(DATE);
}

void PrintInstInfo(FILE *fpout)
{
   fprintf(fpout, "#define ATL_ARCH \"%s\"\n", ARCH);
   fprintf(fpout, "#define ATL_INSTFLAGS \"%s\"\n", INSTFLAGS);
   fprintf(fpout, "#define ATL_MMDEF \"%s\"\n", MMDEF);
   fprintf(fpout, "#define ATL_ARCHDEF \"%s\"\n", ARCHDEF);
   fprintf(fpout, "#define ATL_F2CDEFS \"%s\"\n", F2CDEFS);
   fprintf(fpout, "#define ATL_ARCHDEFS \"%s\"\n", ARCHDEFS);
   fprintf(fpout, "#define ATL_MMFLAGS \"%s\"\n", MMFLAGS);
   fprintf(fpout, "#define ATL_MCC \"%s\"\n", MCC);
   fprintf(fpout, "#define ATL_CCFLAGS \"%s\"\n", CCFLAGS);
   fprintf(fpout, "#define ATL_CC \"%s\"\n", CC);
   fprintf(fpout, "#define ATL_F77FLAGS \"%s\"\n", F77FLAGS);
   fprintf(fpout, "#define ATL_F77 \"%s\"\n", F77);
   fprintf(fpout, "#define ATL_MCCVERS \"%s\"\n", MCCVERS);
   fprintf(fpout, "#define ATL_CCVERS \"%s\"\n", CCVERS);
   fprintf(fpout, "#define ATL_F77VERS \"%s\"\n", F77VERS);
   fprintf(fpout, "#define ATL_SYSINFO \"%s\"\n", SYS);
   fprintf(fpout, "#define ATL_DATE    \"%s\"\n", DATE);
   fprintf(fpout, "#define ATL_UNAM    \"%s\"\n", UNAM);
   fprintf(fpout, "#define ATL_VERS    \"3.6.0\"\n");
}

void CreateFile(char *file)
{
   FILE *fpout;

   GetInstInfo();

   if (file) fpout = fopen(file, "w");
   else fpout = stdout;
   assert(fpout);

   fprintf(fpout, "#ifndef ATL_INSTINFO_H\n   #define ATL_INSTINFO_H\n\n");

   PrintInstInfo(fpout);

   fprintf(fpout, "\n#endif\n");
   if (fpout != stdout) fclose(fpout);
}

main(int nargs, char **args)
{
   FILE *fpout=stdout;
   char *file=NULL;
   if (nargs > 1) file = args[1];
   CreateFile(file);
}
