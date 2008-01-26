
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
#ifdef ATL_AVgcc
   #include <altivec.h>
#endif
main(int nargs, char **args)
{
   char ln[128];
   float *tp;
#ifdef ATL_AVgcc
   vector float v0, v1 = (vector float) {2.0f, 3.0f, 4.0f, 5.0f};
#else
   vector float v0, v1 = (vector float) (2.0f, 3.0f, 4.0f, 5.0f);
#endif

   tp = (void*) (16+ ((((size_t)ln)>>4)<<4));
   tp[0] = 0.0; tp[1] = 1.0; tp[2] = 2.0; tp[3] = 3.0;
   v0 = vec_ld(0, tp);
   v0 = vec_add(v0, v1);
   vec_st(v0, 0, tp);
   vec_st(v1, 0, tp+4);
   if (tp[0] != 2.0f || tp[1] != 4.0f || tp[2] != 6.0f || tp[3] != 8.0f)
   {
      printf("FAILURE\n");
      exit(1);
   }
   else if (tp[4] != 2.0f || tp[5] != 3.0f || tp[6] != 4.0f || tp[7] != 5.0f)
   {
      printf("FAILURE\n");
      exit(2);
   }
   printf("SUCCESS\n");
   exit(0);
}
