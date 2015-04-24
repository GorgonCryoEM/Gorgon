
/*
 *             Automatically Tuned Linear Algebra Software v3.6.0
 *                   (C) Copyright 2000 Peter Soendergaard
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

#define VECLEN 2

#define gen_vec_rr(op,reg1,reg2) \
        __asm__ __volatile__ (#op " %%" #reg1 ", %%" #reg2 \
                              :  /* nothing */ \
                              : /* nothing */)


#define gen_vec_mr(op,mem,reg) \
        __asm__ __volatile__ (#op " %0, %%" #reg \
                              :  /* nothing */ \
                              : "m" (((mem)[0])), "m" (((mem)[1])))

#define gen_vec_rm(op,reg,mem) \
        __asm__ __volatile__ (#op " %%" #reg ", %0" \
                              : "=m" (((mem)[0])), "=m" (((mem)[1])) \
			      :  /* nothing */ )


#define vec_mov_mr(mem,reg)     gen_vec_mr(movq,mem,reg)
#define vec_mov_rm(reg,mem)     gen_vec_rm(movq,reg,mem)
#define vec_add_rr(reg1,reg2)   gen_vec_rr(pfadd,reg1,reg2)

#define reg0 mm0
#define reg1 mm1

main()
{

  int i;
  float testv1[VECLEN],testv2[VECLEN],testv3[VECLEN];

  for (i=0;i<VECLEN;i++)
    {
      testv1[i]=i;
      testv2[i]=i+2;
      testv3[i]=0;
    }

  __asm__ __volatile__ ("femms");

  vec_mov_mr(testv1,reg0);
  vec_mov_mr(testv2,reg1);
  vec_add_rr(reg1,reg0);
  vec_mov_rm(reg0,testv3);

  __asm__ __volatile__ ("femms");

  for (i=0;i<VECLEN;i++)
    {
      if (testv3[i]!=(2*i+2))
	{
	  printf("FAILURE\n");
	  exit(1);
	}
    }

  printf("SUCCESS\n");
  exit(0);

}
