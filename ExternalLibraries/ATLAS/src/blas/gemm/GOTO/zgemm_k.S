    /*        Fast GEMM routine for Alpha 21164/21264      */
    /*         on  Linux, Digital UNIX                     */
    /*        by Kazushige Goto <goto@statabo.rim.or.jp>   */

#define ASSEMBLER
#include "common.h"
#include "version.h"

#ifndef P
#ifdef EV6
#ifdef DGEMM
#define P	112
#define Q	264
#define R	500
#else
#define P	112
#define Q	252
#define R	504
#endif
#else
#ifdef DGEMM
#define P	 32
#define Q	132
#define R	132
#else
#define P	 32
#define Q	168
#define R	168
#endif
#endif
#endif

#define LDA ((Q<<2))

#ifdef NN
#define ROUTINE ZGEMM_NN
#endif
#ifdef CN
#define ROUTINE ZGEMM_CN
#endif
#ifdef TN
#define ROUTINE ZGEMM_TN
#endif
#ifdef NC
#define ROUTINE ZGEMM_NC
#endif
#ifdef NT
#define ROUTINE ZGEMM_NT
#endif
#ifdef CC
#define ROUTINE ZGEMM_CC
#endif
#ifdef CT
#define ROUTINE ZGEMM_CT
#endif
#ifdef TC
#define ROUTINE ZGEMM_TC
#endif
#ifdef TT
#define ROUTINE ZGEMM_TT
#endif
#ifdef NR
#define ROUTINE ZGEMM_NR
#endif
#ifdef TR
#define ROUTINE ZGEMM_TR
#endif
#ifdef CR
#define ROUTINE ZGEMM_CR
#endif
#ifdef RN
#define ROUTINE ZGEMM_RN
#endif
#ifdef RT
#define ROUTINE ZGEMM_RT
#endif
#ifdef RC
#define ROUTINE ZGEMM_RC
#endif
#ifdef RR
#define ROUTINE ZGEMM_RR
#endif

#define MATRIX_A   (((P>>1)*LDA*SIZE))   /* size = P * Q * SIZE * 2 */
#define MATRIX_B   (((R>>1)*LDA*SIZE))   /* size = R * Q * SIZE * 2 */

#define SA_ADDRESS 0
#define SB_ADDRESS (MATRIX_A+0)

#define STACKSIZE 20*8

	.set noat
	.set noreorder
#ifdef EV6
	.arch ev6
#endif

.text
	.align	5
	.globl	ROUTINE
	.ent	ROUTINE

/*             Register Map 

$0 : ---	$1 : temp	$2 : a_offset	$3 : a_offset1
$4 : b_offset	$5 : b_offset1	$6 : c_offset	$7 : c_offset1
$8 : c_offset2	$9 : m		$10: n		$11: k
$12: a		$13: lda	$14: ldb	$15: ldc
$16: min_l	$17: min_i	$18: min_j	$19: i
$20: j		$21: l		$22: is		$23: js
$24: ls		$25: b		$26: c		$27: sa
$at: sb 	$29: GP		$30: Stack	$31: Zero
*/

#ifdef DGEMM
#define	COPY_PREFETCH_SIZE 16
#else
#define	COPY_PREFETCH_SIZE 12
#endif

ROUTINE:
	lda	$30, -STACKSIZE($30)

	stq	$26,    0($30)
	stq	$9,     8($30)
	stq	$10,   16($30)
	stq	$11,   24($30)
	stq	$12,   32($30)
	stq	$13,   40($30)
	stq	$14,   48($30)
	stq	$15,   56($30)

	stt	$f2,   64($30)
	stt	$f3,   72($30)
	stt	$f4,   80($30)
	stt	$f5,   88($30)
	stt	$f6,   96($30)
	stt	$f7,  104($30)
	stt	$f8,  112($30)
	stt	$f9,  120($30)
	.prologue	0

	mov	$16, $9			# m
	mov	$17, $10		# n
	mov	$18, $11		# k
	mov	$20, $12		# a
	mov	$21, $13		# lda

	LD	$f7,   0*SIZE($19)	# alpha_r
	LD	$f8,   1*SIZE($19)	# alpha_i

	ldq	$25, 0 + STACKSIZE($30)	# b
	ldl	$14, 8 + STACKSIZE($30)	# ldb
	ldq	$26,16 + STACKSIZE($30)	# c
	ldl	$15,24 + STACKSIZE($30)	# ldc
	ldq	$27,32 + STACKSIZE($30)	# buffer

	clr	$24			# ls = 0
	ble	$9,  $END

#if defined(EV6) && !defined(DEBUG)
	ftoit	$f7, $at
#else
	stt	$f7, 144($30)
#endif
	ble	$10, $END
#if defined(EV6) && !defined(DEBUG)
	ftoit	$f8, $29
#else
	stt	$f8, 152($30)
#endif
	ble	$11, $END
	.align 4

$L5:
	clr	$23			# js = 0

	subl	$11, $24, $16		# min_l = k - ls
	lda	$0,   Q
	cmple	$16, $0,  $1		# $1 = (min_l > Q)
	cmoveq	$1,  $0,  $16		# if (min_l>Q) min_l = Q
	.align 4

$L10:
	subl	$10, $23, $18		# min_j = n - js
	lda	$0,  R
	cmple	$18, $0,  $1		# $1 = (min_j > R)
	cmoveq	$1,  $0,  $18		# if (min_j>R) min_j = R

/* Outer Copying */

#if defined(NN) || defined(TN) || defined(CN) || defined(RN) || defined(NR) || defined(TR) || defined(CR) || defined(RR)

	mull	$14, $23, $3		# js * ldb
	ldah	$4,  Address_H(SB_ADDRESS)($27)	# b_offset = &sb[0][0]
	addq	$24, $24, $1		# $1  = (ls*2)
	lda	$4, Address_L(SB_ADDRESS)($4)

	SXADDQ	$1,  $25, $1		# $1 += b
	sra	$18,  1, $20		# j = (min_j >> 1)
	SXADDQ	$3,  $1, $6		# c_offset
	ble	$20, $L12
	.align 4

$L13:
	mov	$4,  $5			# b_offset1 = b_offset
	lda	$4,   LDA*SIZE($4)	# b_offset += LDA

	mov	$6, $7			# c_offset1 = c_offset
	SXADDQ	$14, $6,  $8		# c_offset2 = c_offset  + ldb
	SXADDQ	$14, $8,  $6		# c_offset  = c_offset2 + ldb
	mov	$16, $21		# l = min_l
	.align 4

$L16:
#ifdef DGEMM
	LD	$f28,   0*SIZE($7)
	LD	$f20,   1*SIZE($7)
	LD	$f29,   0*SIZE($8)
	LD	$f17,   1*SIZE($8)

	LD	$f3,    2*SIZE($7)
	LD	$f1,    3*SIZE($7)
	LD	$f4,    2*SIZE($8)
	LD	$f2,    3*SIZE($8)

	LD	$f18,   4*SIZE($7)
	LD	$f21,   5*SIZE($7)
	LD	$f19,   4*SIZE($8)
	LD	$f30,   5*SIZE($8)

	LD	$f5,    6*SIZE($7)
	LD	$f0,    7*SIZE($7)
	LD	$f6,    6*SIZE($8)
	LD	$f16,   7*SIZE($8)

	ST	$f28,   0*SIZE($5)
	ST	$f20,   1*SIZE($5)
	ST	$f29,   2*SIZE($5)
	ST	$f17,   3*SIZE($5)

	ST	$f3,    4*SIZE($5)
	ST	$f1,    5*SIZE($5)
	ST	$f4,    6*SIZE($5)
	ST	$f2,    7*SIZE($5)

	ST	$f18,   8*SIZE($5)
	ST	$f21,   9*SIZE($5)
	ST	$f19,  10*SIZE($5)
	ST	$f30,  11*SIZE($5)

	ST	$f5,   12*SIZE($5)
	ST	$f0,   13*SIZE($5)
	ST	$f6,   14*SIZE($5)
	ST	$f16,  15*SIZE($5)
#else
	ldt	$f28,   0*SIZE($7)
	ldt	$f29,   0*SIZE($8)
	ldt	$f3,    2*SIZE($7)
	ldt	$f4,    2*SIZE($8)

	ldt	$f18,   4*SIZE($7)
	ldt	$f19,   4*SIZE($8)
	ldt	$f5,    6*SIZE($7)
	ldt	$f6,    6*SIZE($8)

	stt	$f28,   0*SIZE($5)
	stt	$f29,   2*SIZE($5)
	stt	$f3,    4*SIZE($5)
	stt	$f4,    6*SIZE($5)

	stt	$f18,   8*SIZE($5)
	stt	$f19,  10*SIZE($5)
	stt	$f5,   12*SIZE($5)
	stt	$f6,   14*SIZE($5)
#endif
	lda	$7,     8*SIZE($7)		# c_offset1 += 8
	lda	$8,     8*SIZE($8)		# c_offset2 += 8
	lda	$5,    16*SIZE($5)		# b_offset1 += 16

	subl	$21,  4, $21		# l -= 4
	bgt	$21,$L16
	subl	$20, 1, $20		# j --
	bgt	$20,$L13
	.align 4

$L12:
	blbc	$18,$L23

	mov	$4,  $5			# b_offset1 = b_offset
	mov	$6,  $7			# c_offset1 = c_offset
	mov	$16, $21		# l = min_l
	.align 4

$L24:
#ifdef DGEMM
	LD	$f28,   0*SIZE($7)
	LD	$f20,   1*SIZE($7)
	LD	$f3,    2*SIZE($7)
	LD	$f1,    3*SIZE($7)

	LD	$f18,   4*SIZE($7)
	LD	$f21,   5*SIZE($7)
	LD	$f5,    6*SIZE($7)
	LD	$f0,    7*SIZE($7)

	ST	$f28,   0*SIZE($5)
	ST	$f20,   1*SIZE($5)
	ST	$f3,    4*SIZE($5)
	ST	$f1,    5*SIZE($5)

	ST	$f18,   8*SIZE($5)
	ST	$f21,   9*SIZE($5)
	ST	$f5,   12*SIZE($5)
	ST	$f0,   13*SIZE($5)
	.align 4

#else
	ldt	$f28,   0*SIZE($7)
	ldt	$f3,    2*SIZE($7)
	ldt	$f18,   4*SIZE($7)
	ldt	$f5,    6*SIZE($7)

	stt	$f28,   0*SIZE($5)
	stt	$f3,    4*SIZE($5)
	stt	$f18,   8*SIZE($5)
	stt	$f5,   12*SIZE($5)
	.align 4
#endif
	lda	$7,     8*SIZE($7)	# c_offset1 += 8
	lda	$5,    16*SIZE($5)	# b_offset1 += 16

	subl	$21,  4, $21		# l -= 4
	bgt	$21,$L24
	.align 4

#else /* NC || NT || CC || CT || TC || TT || RT || RC */

	mull	$14, $24, $3		# ls * ldb
	ldah	$4,  Address_H(SB_ADDRESS)($27)	# b_offset = &sb[0][0]
	addq	$23, $23, $1		# $1  = (js*2)
	lda	$4,  Address_L(SB_ADDRESS)($4)

	SXADDQ	$1,  $25, $1		# $1 += b
	mov	$16, $21		# l = min_l
	unop
	SXADDQ	$3,  $1, $6		# c_offset
	.align 4

$L13:
	mov	$6, $7			# c_offset1  = c_offset
	SXADDQ	$14, $6,  $6		# c_offset  += ldb

	mov	$4,  $5			# b_offset1 = b_offset
	lda	$4,   4*SIZE($4)	# b_offset += 4

	mov	$18, $20		# j = min_j
	.align 4

$L16:
#ifdef DGEMM
	LD	$f28,   0*SIZE($7)
	LD	$f20,   1*SIZE($7)
	LD	$f29,   2*SIZE($7)
	LD	$f17,   3*SIZE($7)

	LD	$f3,    4*SIZE($7)
	LD	$f1,    5*SIZE($7)
	LD	$f4,    6*SIZE($7)
	LD	$f2,    7*SIZE($7)

	LD	$f18,   8*SIZE($7)
	LD	$f21,   9*SIZE($7)
	LD	$f19,  10*SIZE($7)
	LD	$f30,  11*SIZE($7)

	LD	$f5,   12*SIZE($7)
	LD	$f0,   13*SIZE($7)
	LD	$f6,   14*SIZE($7)
	LD	$f16,  15*SIZE($7)

	ST	$f28, (0 + 0 * LDA)*SIZE($5)
	ST	$f20, (1 + 0 * LDA)*SIZE($5)
	ST	$f29, (2 + 0 * LDA)*SIZE($5)
	ST	$f17, (3 + 0 * LDA)*SIZE($5)

	ST	$f3,  (0 + 1 * LDA)*SIZE($5)
	ST	$f1,  (1 + 1 * LDA)*SIZE($5)
	ST	$f4,  (2 + 1 * LDA)*SIZE($5)
	ST	$f2,  (3 + 1 * LDA)*SIZE($5)

	ST	$f18, (0 + 2 * LDA)*SIZE($5)
	ST	$f21, (1 + 2 * LDA)*SIZE($5)
	ST	$f19, (2 + 2 * LDA)*SIZE($5)
	ST	$f30, (3 + 2 * LDA)*SIZE($5)

	ST	$f5,  (0 + 3 * LDA)*SIZE($5)
	ST	$f0,  (1 + 3 * LDA)*SIZE($5)
	ST	$f6,  (2 + 3 * LDA)*SIZE($5)
	ST	$f16, (3 + 3 * LDA)*SIZE($5)
#else
	ldt	$f28,   0*SIZE($7)
	ldt	$f29,   2*SIZE($7)
	ldt	$f3,    4*SIZE($7)
	ldt	$f4,    6*SIZE($7)

	ldt	$f18,   8*SIZE($7)
	ldt	$f19,  10*SIZE($7)
	ldt	$f5,   12*SIZE($7)
	ldt	$f6,   14*SIZE($7)

	stt	$f28, (0 + 0 * LDA)*SIZE($5)
	stt	$f29, (2 + 0 * LDA)*SIZE($5)
	stt	$f3,  (0 + 1 * LDA)*SIZE($5)
	stt	$f4,  (2 + 1 * LDA)*SIZE($5)

	stt	$f18, (0 + 2 * LDA)*SIZE($5)
	stt	$f19, (2 + 2 * LDA)*SIZE($5)
	stt	$f5,  (0 + 3 * LDA)*SIZE($5)
	stt	$f6,  (2 + 3 * LDA)*SIZE($5)
#endif

	lda	$7,    16*SIZE($7)		# c_offset1 += 16
	lda	$5, 4*LDA*SIZE($5)		# b_offset1 += 4*LDB

	subl	$20,  8, $20		# j -= 8
	bgt	$20,$L16
	subl	$21,  1, $21		# l --
	bgt	$21,$L13
	.align 4
#endif

$L23:
	clr	$22			# is = 0
	.align 4

$L32:
	subl	$9,  $22, $17		# min_i = m - is
	lda	$0,  P
	cmple	$17, $0,  $1		# (min_i > P)
	cmoveq	$1,  $0,  $17		# if min_i > P min_i = P

/* Innter Copying */
#if defined(NN) || defined(NT) || defined(NC) || defined(NR) || defined(RN) || defined(RT) || defined(RC) || defined(RR)

	mull	$13, $24, $3		# lda * ls
	addq	$22,$22,$4		# $4 = is * 2

	mov	$27, $6			# c_offset = &sa[0][0]
	mov	$16, $21		# l = min_l
	SXADDQ	$4,  $12, $1
	SXADDQ	$3,  $1,  $2		# a_offset = a + ls*lda + is*2
	.align 4

$L34:
	mov	$2, $3			# = a_offset
	mov	$6, $7			# c_offset1 = c_offset
	lda	$6,     4*SIZE($6)	# c_offset += 4
	mov	$17,  $19		# i = min_i
	SXADDQ	$13, $2, $2		# a_offset += lda
	subl	$21,  1, $21		# l --
	.align 4

$L37:
#ifdef DGEMM
	LD	$f28,   0*SIZE($3)
	LD	$f20,   1*SIZE($3)
	LD	$f29,   2*SIZE($3)
	LD	$f17,   3*SIZE($3)

	LD	$f3,    4*SIZE($3)
	LD	$f1,    5*SIZE($3)
	LD	$f4,    6*SIZE($3)
	LD	$f2,    7*SIZE($3)

	LD	$f18,   8*SIZE($3)
	LD	$f21,   9*SIZE($3)
	LD	$f19,  10*SIZE($3)
	LD	$f30,  11*SIZE($3)

	LD	$f5,   12*SIZE($3)
	LD	$f0,   13*SIZE($3)
	LD	$f6,   14*SIZE($3)
	LD	$f16,  15*SIZE($3)

	lda	$3,    16*SIZE($3)	# c_offset1 += 16
	unop
	subl	$19, 8, $19		# i -= 8
	unop

	ST	$f28, (0 + 0 * LDA)*SIZE($7)
	ST	$f20, (1 + 0 * LDA)*SIZE($7)
	ST	$f29, (2 + 0 * LDA)*SIZE($7)
	ST	$f17, (3 + 0 * LDA)*SIZE($7)

	ST	$f3,  (0 + 1 * LDA)*SIZE($7)
	ST	$f1,  (1 + 1 * LDA)*SIZE($7)
	ST	$f4,  (2 + 1 * LDA)*SIZE($7)
	ST	$f2,  (3 + 1 * LDA)*SIZE($7)

	ST	$f18, (0 + 2 * LDA)*SIZE($7)
	ST	$f21, (1 + 2 * LDA)*SIZE($7)
	ST	$f19, (2 + 2 * LDA)*SIZE($7)
	ST	$f30, (3 + 2 * LDA)*SIZE($7)

	ST	$f5,  (0 + 3 * LDA)*SIZE($7)
	ST	$f0,  (1 + 3 * LDA)*SIZE($7)
	ST	$f6,  (2 + 3 * LDA)*SIZE($7)
	ST	$f16, (3 + 3 * LDA)*SIZE($7)
#else
	ldt	$f28,   0*SIZE($3)
	ldt	$f29,   2*SIZE($3)
	ldt	$f3,    4*SIZE($3)
	ldt	$f4,    6*SIZE($3)

	ldt	$f18,   8*SIZE($3)
	ldt	$f19,  10*SIZE($3)
	ldt	$f5,   12*SIZE($3)
	ldt	$f6,   14*SIZE($3)

	lda	$3,    16*SIZE($3)	# c_offset1 += 16
	unop
	subl	$19, 8, $19		# i -= 8
	unop

	stt	$f28, (0 + 0 * LDA)*SIZE($7)
	stt	$f29, (2 + 0 * LDA)*SIZE($7)
	stt	$f3,  (0 + 1 * LDA)*SIZE($7)
	stt	$f4,  (2 + 1 * LDA)*SIZE($7)

	stt	$f18, (0 + 2 * LDA)*SIZE($7)
	stt	$f19, (2 + 2 * LDA)*SIZE($7)
	stt	$f5,  (0 + 3 * LDA)*SIZE($7)
	stt	$f6,  (2 + 3 * LDA)*SIZE($7)
#endif

	lda	$7, 4*LDA*SIZE($7)	# b_offset1 += 4*LDA
	bgt	$19,$L37
	unop
	bgt	$21,$L34
	.align 4

#else /* TN || TT || CN || CC || CT || TC || TR || CR */

	mull	$13, $22, $3		# lda * is
	addq	$24, $24, $4		# $4 = ls * 2

	sra	$17,  1,  $19		# i  = (min_i >> 1)
	SXADDQ	$4,  $12, $1
	SXADDQ	$3,  $1,  $6		# c_offset = a + is*lda + ls*2

	mov	$27, $4			# b_offset = &sa[0][0]
	ble	$19, $L101
	.align 4

$L34:
	mov	$4, $5			# b_offset1 = b_offset
	mov	$6, $7			# c_offset1 = c_offset
	lda	$4,   LDA*SIZE($4)	# b_offset += LDA
	SXADDQ	$13, $6, $8		# c_offset2 = c_offset  + lda

	mov	$16,  $21		# l = min_l
	SXADDQ	$13, $8, $6		# c_offset  = c_offset2 + lda
	subl	$19,  1, $19		# i --
	unop
	.align 4

$L37:
#ifdef DGEMM
	LD	$f28,   0*SIZE($7)
	LD	$f20,   1*SIZE($7)
	LD	$f29,   0*SIZE($8)
	LD	$f17,   1*SIZE($8)

	LD	$f3,    2*SIZE($7)
	LD	$f1,    3*SIZE($7)
	LD	$f4,    2*SIZE($8)
	LD	$f2,    3*SIZE($8)

	LD	$f18,   4*SIZE($7)
	LD	$f21,   5*SIZE($7)
	LD	$f19,   4*SIZE($8)
	LD	$f30,   5*SIZE($8)

	LD	$f5,    6*SIZE($7)
	LD	$f0,    7*SIZE($7)
	LD	$f6,    6*SIZE($8)
	LD	$f16,   7*SIZE($8)

	lda	$7,     8*SIZE($7)	# c_offset1 += 8
	lda	$8,     8*SIZE($8)	# c_offset2 += 8
	subl	$21,  4, $21		# l -= 4
	unop

	ST	$f28,   0*SIZE($5)
	ST	$f20,   1*SIZE($5)
	ST	$f29,   2*SIZE($5)
	ST	$f17,   3*SIZE($5)

	ST	$f3,    4*SIZE($5)
	ST	$f1,    5*SIZE($5)
	ST	$f4,    6*SIZE($5)
	ST	$f2,    7*SIZE($5)

	ST	$f18,   8*SIZE($5)
	ST	$f21,   9*SIZE($5)
	ST	$f19,  10*SIZE($5)
	ST	$f30,  11*SIZE($5)

	ST	$f5,   12*SIZE($5)
	ST	$f0,   13*SIZE($5)
	ST	$f6,   14*SIZE($5)
	ST	$f16,  15*SIZE($5)
#else
	ldt	$f28,   0*SIZE($7)
	ldt	$f29,   0*SIZE($8)
	ldt	$f3,    2*SIZE($7)
	ldt	$f4,    2*SIZE($8)

	ldt	$f18,   4*SIZE($7)
	ldt	$f19,   4*SIZE($8)
	ldt	$f5,    6*SIZE($7)
	ldt	$f6,    6*SIZE($8)

	lda	$7,     8*SIZE($7)	# c_offset1 += 8
	lda	$8,     8*SIZE($8)	# c_offset2 += 8
	subl	$21,  4, $21		# l -= 4
	unop

	stt	$f28,   0*SIZE($5)
	stt	$f29,   2*SIZE($5)
	stt	$f3,    4*SIZE($5)
	stt	$f4,    6*SIZE($5)

	stt	$f18,   8*SIZE($5)
	stt	$f19,  10*SIZE($5)
	stt	$f5,   12*SIZE($5)
	stt	$f6,   14*SIZE($5)
#endif

	lda	$5,    16*SIZE($5)	# b_offset1 += 16
	bgt	$21,$L37
	unop
	bgt	$19,$L34
	.align 4

$L101:
	blbc	$17, $MAIN
	.align 4

$L102:
	mov	$4, $5			# b_offset1 = b_offset
	mov	$6, $7			# c_offset1 = c_offset
	mov	$16,  $21		# l = min_l
	subl	$19,  1, $19		# i --
	.align 4

$L103:
#ifdef DGEMM
	LD	$f28,   0*SIZE($7)
	LD	$f20,   1*SIZE($7)
	LD	$f3,    2*SIZE($7)
	LD	$f1,    3*SIZE($7)

	LD	$f18,   4*SIZE($7)
	LD	$f21,   5*SIZE($7)
	LD	$f5,    6*SIZE($7)
	LD	$f0,    7*SIZE($7)

	lda	$7,     8*SIZE($7)	# c_offset1 += 8
	unop
	subl	$21,  4, $21		#  l -= 4
	unop

	ST	$f28,   0*SIZE($5)
	ST	$f20,   1*SIZE($5)
	ST	$f3,    4*SIZE($5)
	ST	$f1,    5*SIZE($5)

	ST	$f18,   8*SIZE($5)
	ST	$f21,   9*SIZE($5)
	ST	$f5,   12*SIZE($5)
	ST	$f0,   13*SIZE($5)
	.align 4

#else
	ldt	$f28,   0*SIZE($7)
	ldt	$f3,    2*SIZE($7)
	ldt	$f18,   4*SIZE($7)
	ldt	$f5,    6*SIZE($7)

	lda	$7,     8*SIZE($7)	# c_offset1 += 8
	unop
	subl	$21,  4, $21		#  l -= 4
	unop

	stt	$f28,   0*SIZE($5)
	stt	$f3,    4*SIZE($5)
	stt	$f18,   8*SIZE($5)
	stt	$f5,   12*SIZE($5)
	.align 4
#endif

	lda	$5,    16*SIZE($5)	# b_offset1 += 16
	bgt	$21,$L103
	lda	$4,   LDA*SIZE($4)	# b_offset += LDA
	bgt	$19,$L102
	.align 4
#endif

$MAIN:
	mull	$15, $23, $3		# ldc * js
	addq	$22, $22, $6		# $4 = is * 2
	sra	$18,  1, $20		# j = (min_j >> 1)
	ldah	$4,  Address_H(SB_ADDRESS)($27)	# b_offset = &sb[0][0]

	SXADDQ	$6,  $26, $6		# c_offset = c + is*2
	lda	$4,  Address_L(SB_ADDRESS)($4)
	SXADDQ	$3,  $6,  $6		# c_offset = c + is*2 + js*ldc
	ble	$20,$L44
	.align 4

$L45:
	sra	$17,  1, $19		# i = (min_i >> 1)
	fclr	$f4
	SXADDQ	$15, $6, $8		# c_offset2 = c_offset  + ldc
	fclr	$f2

	mov	$27, $2			# c_offset = &sa[0][0]
	fclr	$f18
	mov	$6, $7			# c_offset1 = c_offset
	fclr	$f21

	SXADDQ	$15, $8, $6		# c_offset  = c_offset2 + ldc
	fclr	$f19
	fclr	$f30
	ble	$19,$L48		# if (i>0)
	.align 4

$L49:
	LD	$f24,   0*SIZE($2)	# atemp1  = *(a_offset1 + 0)
	fclr	$f5
	LD	$f15,   0*SIZE($4)	# btemp1  = *(b_offset1 + 0)
	fclr	$f12

	LD	$f25,   1*SIZE($2)	# atemp2  = *(a_offset1 + 1)
	fclr	$f0
	LD	$f14,   1*SIZE($4)	# btemp2  = *(b_offset1 + 1)
	fclr	$f10

	LD	$f26,   2*SIZE($2)	# atemp3  = *(a_offset1 + 2)
	fclr	$f6
	LD	$f23,   2*SIZE($4)	# btemp3  = *(b_offset1 + 2)
	fclr	$f11

	LD	$f27,   3*SIZE($2)	# atemp4  = *(a_offset1 + 3)
	fclr	$f16
	LD	$f22,   3*SIZE($4)	# btemp4  = *(b_offset1 + 3)
	fclr	$f13

	lda	$3,     4*SIZE($2)	# a_offset1 = a_offset + 4
	fclr	$f28
	lda	$2,   LDA*SIZE($2)	# a_offset += LDA
	fclr	$f20

	subl	$16, 2, $21		# l = min_l - 2
	fclr	$f29
	mov	$4,  $5			# b_offset1 = b_offset
	fclr	$f17

	lda	$5,     4*SIZE($4)	# b_offset1 += 4
	fclr	$f3
	fclr	$f1
	ble	$21,$L52
	.align 4

$L53:
	ADD	$f5,  $f12, $f5		# ctemp13 = ctemp13 + temp1
#ifdef EV6
#ifdef DGEMM
	ldq	$31,  24*SIZE($3)
#else
	ldq	$31,  24*SIZE($3)
#endif
#else
#ifdef DGEMM
	ldq	$31,  48*SIZE($3)
#else
	ldq	$31,  48*SIZE($3)
#endif
#endif
	MUL	$f15, $f24, $f12	# temp1   = btemp1  * atemp1
#ifdef EV6
#ifdef DGEMM
	ldt	$f31, 20*SIZE($5)
#else
	ldt	$f31, 20*SIZE($5)
#endif
#else
#ifdef DGEMM
	ldt	$f31, 40*SIZE($5)
#else
	ldt	$f31, 40*SIZE($5)
#endif
#endif

	ADD	$f6,  $f11, $f6		# ctemp15 = ctemp15 + temp3
	unop
	MUL	$f15, $f25, $f11	# temp3   = btemp1  * atemp2
	LD	$f8,   0*SIZE($3)	# atemp1  = *(a_offset1 + 0)

	ADD	$f0,  $f10, $f0		# ctemp14 = ctemp14 + temp2
	unop
	MUL	$f14, $f25, $f10	# temp2   = btemp2  * atemp2
	LD	$f7,   0*SIZE($5)	# btemp1  = *(b_offset1 + 0)

	ADD	$f16, $f13, $f16	# ctemp16 = ctemp16 + temp4
	unop
	MUL	$f14, $f24, $f13	# temp4   = btemp2  * atemp1
	unop

	ADD	$f28, $f12, $f28	# ctemp1  = ctemp1  + temp1
	MUL	$f15, $f26, $f12	# temp1   = btemp1  * atemp3
	ADD	$f29, $f11, $f29	# ctemp3  = ctemp3  + temp3
	MUL	$f15, $f27, $f11	# temp3   = btemp1  * atemp4

	ADD	$f20, $f10, $f20	# ctemp2  = ctemp2  + temp2
	MUL	$f14, $f27, $f10	# temp2   = btemp2  * atemp4
	ADD	$f17, $f13, $f17	# ctemp4  = ctemp4  + temp4
	MUL	$f22, $f24, $f13	# temp4   = btemp4  * atemp1

	ADD	$f3,  $f12, $f3		# ctemp5  = ctemp5  + temp1
	MUL	$f23, $f24, $f12	# temp1   = btemp3  * atemp1
	ADD	$f4,  $f11, $f4		# ctemp7  = ctemp7  + temp3
	MUL	$f23, $f25, $f11	# temp3   = btemp3  * atemp2

	ADD	$f1,  $f10, $f1		# ctemp6  = ctemp6  + temp2
	unop
	MUL	$f22, $f25, $f10	# temp2   = btemp4  * atemp2
	LD	$f25,   1*SIZE($3)	# atemp2  = *(a_offset1 + 1)

	ADD	$f30, $f13, $f30	# ctemp12 = ctemp12 + temp4
	unop
	MUL	$f14, $f26, $f13	# temp4   = btemp2  * atemp3
	LD	$f14,   1*SIZE($5)	# btemp2  = *(b_offset1 + 1)

	ADD	$f18, $f12, $f18	# ctemp9  = ctemp9  + temp1
	unop
	MUL	$f23, $f26, $f12	# temp1   = btemp3  * atemp3
	LD	$f9,    2*SIZE($3)	# atemp3  = *(a_offset1 + 2)

	ADD	$f19, $f11, $f19	# ctemp11 = ctemp11 + temp3
	unop
	MUL	$f23, $f27, $f11	# temp3   = btemp3  * atemp4
	LD	$f23,   2*SIZE($5)	# btemp3  = *(b_offset1 + 2)

	ADD	$f21, $f10, $f21	# ctemp10 = ctemp10 + temp2
	unop
	MUL	$f22, $f27, $f10	# temp2   = btemp4  * atemp4
	LD	$f27,   3*SIZE($3)	# atemp4  = *(a_offset1 + 3)

	ADD	$f2,  $f13, $f2		# ctemp8  = ctemp8  + temp4
	unop
	MUL	$f22, $f26, $f13	# temp4   = btemp4  * atemp3
	LD	$f22,   3*SIZE($5)	# btemp4  = *(b_offset1 + 3)

	ADD	$f5,  $f12, $f5		# ctemp13 = ctemp13 + temp1
	unop
	MUL	$f7,  $f8,  $f12	# temp1   = btemp1  * atemp1
	LD	$f24,   4*SIZE($3)	# atemp1  = *(a_offset1 + 4)

	ADD	$f6,  $f11, $f6		# ctemp15 = ctemp15 + temp3
	unop
	MUL	$f7,  $f25, $f11	# temp3   = btemp1  * atemp2
	LD	$f15,   4*SIZE($5)	# btemp1  = *(b_offset1 + 4)

	ADD	$f0,  $f10, $f0		# ctemp14 = ctemp14 + temp2
	MUL	$f14, $f25, $f10	# temp2   = btemp2  * atemp2
	ADD	$f16, $f13, $f16	# ctemp16 = ctemp16 + temp4
	MUL	$f14, $f8,  $f13	# temp4   = btemp2  * atemp1

	ADD	$f28, $f12, $f28	# ctemp1  = ctemp1  + temp1
	MUL	$f7,  $f9,  $f12	# temp1   = btemp1  * atemp3
	ADD	$f29, $f11, $f29	# ctemp3  = ctemp3  + temp3
	MUL	$f7,  $f27, $f11	# temp3   = btemp1  * atemp4

	ADD	$f20, $f10, $f20	# ctemp2  = ctemp2  + temp2
	MUL	$f14, $f27, $f10	# temp2   = btemp2  * atemp4
	ADD	$f17, $f13, $f17	# ctemp4  = ctemp4  + temp4
	MUL	$f22, $f8,  $f13	# temp4   = btemp4  * atemp1

	ADD	$f3,  $f12, $f3		# ctemp5  = ctemp5  + temp1
	lda	$3,     8*SIZE($3)	# a_offset1 += 16
	MUL	$f23, $f8,  $f12	# temp1   = btemp3  * atemp1
	subl	$21, 2, $21		# l -= 2

	ADD	$f4,  $f11, $f4		# ctemp7  = ctemp7  + temp3
	lda	$5,     8*SIZE($5)	# b_offset1 += 16
	MUL	$f23, $f25, $f11	# temp3   = btemp3  * atemp2
	sll	$31, $31, $31

	ADD	$f1,  $f10, $f1		# ctemp6  = ctemp6  + temp2
	unop
	MUL	$f22, $f25, $f10	# temp2   = btemp4  * atemp2
	LD	$f25,  -3*SIZE($3)	# atemp2  = *(a_offset1 + 5)

	ADD	$f30, $f13, $f30	# ctemp12 = ctemp12 + temp4
	unop
	MUL	$f14, $f9,  $f13	# temp4   = btemp2  * atemp3
	LD	$f14,  -3*SIZE($5)	# btemp2  = *(b_offset1 + 5)

	ADD	$f18, $f12, $f18	# ctemp9  = ctemp9  + temp1
	unop
	MUL	$f23, $f9, $f12		# temp1   = btemp3  * atemp3
	LD	$f26,  -2*SIZE($3)	# atemp3  = *(a_offset1 + 6)

	ADD	$f19, $f11, $f19	# ctemp11 = ctemp11 + temp3
	unop
	MUL	$f23, $f27, $f11	# temp3   = btemp3  * atemp4
	LD	$f23,  -2*SIZE($5)	# btemp3  = *(b_offset1 + 6)

	ADD	$f21, $f10, $f21	# ctemp10 = ctemp10 + temp2
	unop
	MUL	$f22, $f27, $f10	# temp2   = btemp4  * atemp4
	LD	$f27,  -1*SIZE($3)	# atemp4  = *(a_offset1 + 7)

	ADD	$f2,  $f13, $f2		# ctemp8  = ctemp8  + temp4
	MUL	$f22, $f9, $f13		# temp4   = btemp4  * atemp3
	LD	$f22,  -1*SIZE($5)	# btemp4  = *(b_offset1 + 7)
	bgt	$21,$L53
	.align 4

$L52:
	ADD	$f5,  $f12, $f5		# ctemp13 = ctemp13 + temp1
#if defined(EV6) && !defined(DEBUG)
	itoft	$at, $f7
#else
	ldt	$f7, 144($30)
#endif
	MUL	$f15, $f24, $f12	# temp1   = btemp1  * atemp1
	blbs	$16,  $L58
	.align 4

	ADD	$f6,  $f11, $f6		# ctemp15 = ctemp15 + temp3
#ifdef EV6
#ifdef DGEMM
	lds	$f31,  8*SIZE($7)
#else
	lds	$f31, 16*SIZE($7)
#endif
#else
#ifdef DGEMM
	lds	$f31, 16*SIZE($7)
#else
	lds	$f31,  8*SIZE($7)
#endif
#endif
	MUL	$f15, $f25, $f11	# temp3   = btemp1  * atemp2
#ifdef EV6
#ifdef DGEMM
	lds	$f31,  8*SIZE($8)
#else
	lds	$f31, 16*SIZE($8)
#endif
#else
#ifdef DGEMM
	lds	$f31, 24*SIZE($8)
#else
	lds	$f31,  8*SIZE($8)
#endif
#endif

	ADD	$f0,  $f10, $f0		# ctemp14 = ctemp14 + temp2
	MUL	$f14, $f25, $f10	# temp2   = btemp2  * atemp2
	ADD	$f16, $f13, $f16	# ctemp16 = ctemp16 + temp4
	MUL	$f14, $f24, $f13	# temp4   = btemp2  * atemp1

	ADD	$f28, $f12, $f28	# ctemp1  = ctemp1  + temp1
	unop
	MUL	$f15, $f26, $f12	# temp1   = btemp1  * atemp3
	unop

	ADD	$f29, $f11, $f29	# ctemp3  = ctemp3  + temp3
	unop
	MUL	$f15, $f27, $f11	# temp3   = btemp1  * atemp4
	LD	$f15,   0*SIZE($5)	# btemp1  = *(b_offset1 + 0)

	ADD	$f20, $f10, $f20	# ctemp2  = ctemp2  + temp2
	MUL	$f14, $f27, $f10	# temp2   = btemp2  * atemp4
	ADD	$f17, $f13, $f17	# ctemp4  = ctemp4  + temp4
	MUL	$f22, $f24, $f13	# temp4   = btemp4  * atemp1

	ADD	$f3,  $f12, $f3		# ctemp5  = ctemp5  + temp1
	unop
	MUL	$f23, $f24, $f12	# temp1   = btemp3  * atemp1
	LD	$f24,   0*SIZE($3)	# atemp1  = *(a_offset1 + 0)

	ADD	$f4,  $f11, $f4		# ctemp7  = ctemp7  + temp3
	unop
	MUL	$f23, $f25, $f11	# temp3   = btemp3  * atemp2
	unop

	ADD	$f1,  $f10, $f1		# ctemp6  = ctemp6  + temp2
	unop
	MUL	$f22, $f25, $f10	# temp2   = btemp4  * atemp2
	LD	$f25,   1*SIZE($3)	# atemp2  = *(a_offset1 + 1)

	ADD	$f30, $f13, $f30	# ctemp12 = ctemp12 + temp4
	unop
	MUL	$f14, $f26, $f13	# temp4   = btemp2  * atemp3
	LD	$f14,   1*SIZE($5)	# btemp2  = *(b_offset1 + 1)

	ADD	$f18, $f12, $f18	# ctemp9  = ctemp9  + temp1
	unop
	MUL	$f23, $f26, $f12	# temp1   = btemp3  * atemp3
	unop

	ADD	$f19, $f11, $f19	# ctemp11 = ctemp11 + temp3
	unop
	MUL	$f23, $f27, $f11	# temp3   = btemp3  * atemp4
	LD	$f23,   2*SIZE($5)	# btemp3  = *(b_offset1 + 2)

	ADD	$f21, $f10, $f21	# ctemp10 = ctemp10 + temp2
	unop
	MUL	$f22, $f27, $f10	# temp2   = btemp4  * atemp4
	LD	$f27,   3*SIZE($3)	# atemp4  = *(a_offset1 + 3)

	ADD	$f2,  $f13, $f2		# ctemp8  = ctemp8  + temp4
	unop
	MUL	$f22, $f26, $f13	# temp4   = btemp4  * atemp3
	LD	$f26,   2*SIZE($3)	# atemp3  = *(a_offset1 + 2)

	ADD	$f5,  $f12, $f5		# ctemp13 = ctemp13 + temp1
	unop
	MUL	$f15, $f24, $f12	# temp1   = btemp1  * atemp1
	LD	$f22,   3*SIZE($5)	# btemp4  = *(b_offset1 + 3)
	.align 4

$L58:
	ADD	$f6,  $f11, $f6		# ctemp15 = ctemp15 + temp3
	MUL	$f15, $f25, $f11	# temp3   = btemp1  * atemp2
	ADD	$f0,  $f10, $f0		# ctemp14 = ctemp14 + temp2
	MUL	$f14, $f25, $f10	# temp2   = btemp2  * atemp2

	ADD	$f16, $f13, $f16	# ctemp16 = ctemp16 + temp4
	MUL	$f14, $f24, $f13	# temp4   = btemp2  * atemp1
	ADD	$f28, $f12, $f28	# ctemp1  = ctemp1  + temp1
	MUL	$f15, $f26, $f12	# temp1   = btemp1  * atemp3

	ADD	$f29, $f11, $f29	# ctemp3  = ctemp3  + temp3
	unop
	MUL	$f15, $f27, $f11	# temp3   = btemp1  * atemp4
	LD	$f15,  0*SIZE($8)	# btemp1  = *(c_offset2 + 0)

	ADD	$f20, $f10, $f20	# ctemp2  = ctemp2  + temp2
	MUL	$f14, $f27, $f10	# temp2   = btemp2  * atemp4
	ADD	$f17, $f13, $f17	# ctemp4  = ctemp4  + temp4
	MUL	$f22, $f24, $f13	# temp4   = btemp4  * atemp1

	ADD	$f3,  $f12, $f3		# ctemp5  = ctemp5  + temp1
#if defined(EV6) && !defined(DEBUG)
	itoft	$29, $f8
#else
	unop
#endif
	MUL	$f23, $f24, $f12	# temp1   = btemp3  * atemp1
	LD	$f24,  0*SIZE($7)	# atemp1  = *(c_offset1 + 0)

	ADD	$f4,  $f11, $f4		# ctemp7  = ctemp7  + temp3
	unop
	MUL	$f23, $f25, $f11	# temp3   = btemp3  * atemp2
#if defined(EV6) && !defined(DEBUG)
	unop
#else
	ldt	$f8, 152($30)
#endif

	ADD	$f1,  $f10, $f1		# ctemp6  = ctemp6  + temp2
	unop
	MUL	$f22, $f25, $f10	# temp2   = btemp4  * atemp2
	LD	$f25,  1*SIZE($7)	# atemp2  = *(c_offset1 + 1)

	ADD	$f30, $f13, $f30	# ctemp12 = ctemp12 + temp4
	lda	$7,    4*SIZE($7)	# c_offset1 += 4
	MUL	$f14, $f26, $f13	# temp4   = btemp2  * atemp3
	LD	$f14,  1*SIZE($8)	# btemp2  = *(c_offset2 + 1)

	ADD	$f18, $f12, $f18	# ctemp9  = ctemp9  + temp1
	unop
	MUL	$f23, $f26, $f12	# temp1   = btemp3  * atemp3
	unop

	ADD	$f19, $f11, $f19	# ctemp11 = ctemp11 + temp3
	subl	$19, 1, $19		# i --
	MUL	$f23, $f27, $f11	# temp3   = btemp3  * atemp4
	LD	$f23,  2*SIZE($8)	# btemp3  = *(c_offset2 + 2)

	ADD	$f21, $f10, $f21	# ctemp10 = ctemp10 + temp2
	unop
	MUL	$f22, $f27, $f10	# temp2   = btemp4  * atemp4
	LD	$f27, -1*SIZE($7)	# atemp4  = *(c_offset1 + 3)

	ADD	$f2,  $f13, $f2		# ctemp8  = ctemp8  + temp4
	MUL	$f22, $f26, $f13	# temp4   = btemp4  * atemp3
	LD	$f26, -2*SIZE($7)	# atemp3  = *(c_offset1 + 2)
	LD	$f22,  3*SIZE($8)	# btemp4  = *(c_offset2 + 3)

#if   defined(NN) || defined(NT) || defined(TN) || defined(TT) || \
      defined(CC) || defined(CR) || defined(RC) || defined(RR)

	SUB	$f28, $f20, $f20	# ctemp2  = ctemp1  - ctemp2
	ADD	$f29, $f17, $f17	# ctemp4  = ctemp3  + ctemp4
	SUB	$f3,  $f1,  $f1		# ctemp6  = ctemp5  - ctemp6
	ADD	$f4,  $f2,  $f2		# ctemp8  = ctemp7  + ctemp8

	ADD	$f5,  $f12, $f5		# ctemp13 = ctemp13 + temp1
	MUL	$f7,  $f20, $f28	# ctemp1  = alpha_r * ctemp2
	ADD	$f6,  $f11, $f6		# ctemp15 = ctemp15 + temp3
	MUL	$f8,  $f20, $f20	# ctemp2  = alpha_i * ctemp2

	ADD	$f0,  $f10, $f0		# ctemp14 = ctemp14 + temp2
	MUL	$f8,  $f17, $f29	# ctemp3  = alpha_i * ctemp4
	ADD	$f16, $f13, $f16	# ctemp16 = ctemp16 + temp4
	MUL	$f7,  $f17, $f17	# ctemp4  = alpha_r * ctemp4

	SUB	$f18, $f21, $f21	# ctemp10 = ctemp9  - ctemp10
	MUL	$f7,  $f1,  $f3		# ctemp5  = alpha_r * ctemp6
	ADD	$f19, $f30, $f30	# ctemp12 = ctemp11 + ctemp12
	MUL	$f8,  $f1,  $f1		# ctemp6  = alpha_i * ctemp6

	SUB	$f5,  $f0,  $f0		# ctemp14 = ctemp13 - ctemp14
	MUL	$f8,  $f2,  $f4		# ctemp7  = alpha_i * ctemp8
	ADD	$f6,  $f16, $f16	# ctemp16 = ctemp15 + ctemp16
	MUL	$f7,  $f2,  $f2		# ctemp8  = alpha_r * ctemp8

#elif defined(CN) || defined(CT) || defined(RN) || defined(RT)

	ADD	$f28, $f20, $f20	# ctemp2  = ctemp1  + ctemp2
	SUB	$f29, $f17, $f17	# ctemp4  = ctemp3  - ctemp4
	ADD	$f3,  $f1,  $f1		# ctemp6  = ctemp5  + ctemp6
	SUB	$f4,  $f2,  $f2		# ctemp8  = ctemp7  - ctemp8

	ADD	$f5,  $f12, $f5		# ctemp13 = ctemp13 + temp1
	MUL	$f7,  $f20, $f28	# ctemp1  = alpha_r * ctemp2
	ADD	$f6,  $f11, $f6		# ctemp15 = ctemp15 + temp3
	MUL	$f8,  $f20, $f20	# ctemp2  = alpha_i * ctemp2

	ADD	$f0,  $f10, $f0		# ctemp14 = ctemp14 + temp2
	MUL	$f8,  $f17, $f29	# ctemp3  = alpha_i * ctemp4
	ADD	$f16, $f13, $f16	# ctemp16 = ctemp16 + temp4
	MUL	$f7,  $f17, $f17	# ctemp4  = alpha_r * ctemp4

	ADD	$f18, $f21, $f21	# ctemp10 = ctemp9  + ctemp10
	MUL	$f7,  $f1,  $f3		# ctemp5  = alpha_r * ctemp6
	SUB	$f19, $f30, $f30	# ctemp12 = ctemp11 - ctemp12
	MUL	$f8,  $f1,  $f1		# ctemp6  = alpha_i * ctemp6

	ADD	$f5,  $f0,  $f0		# ctemp14 = ctemp13 + ctemp14
	MUL	$f8,  $f2,  $f4		# ctemp7  = alpha_i * ctemp8
	SUB	$f6,  $f16, $f16	# ctemp16 = ctemp15 - ctemp16
	MUL	$f7,  $f2,  $f2		# ctemp8  = alpha_r * ctemp8

#else /* defined(NC) || defined(TC) || defined(NR) || defined(TR) */

	ADD	$f28, $f20, $f20	# ctemp2  =  ctemp1  + ctemp2
	SUB	$f17, $f29, $f17	# ctemp4  = -ctemp3  + ctemp4
	ADD	$f3,  $f1,  $f1		# ctemp6  =  ctemp5  + ctemp6
	SUB	$f2,  $f4,  $f2		# ctemp8  = -ctemp7  + ctemp8

	ADD	$f5,  $f12, $f5		# ctemp13 = ctemp13 + temp1
	MUL	$f7,  $f20, $f28	# ctemp1  = alpha_r * ctemp2
	ADD	$f6,  $f11, $f6		# ctemp15 = ctemp15 + temp3
	MUL	$f8,  $f20, $f20	# ctemp2  = alpha_i * ctemp2

	ADD	$f0,  $f10, $f0		# ctemp14 = ctemp14 + temp2
	MUL	$f8,  $f17, $f29	# ctemp3  = alpha_i * ctemp4
	ADD	$f16, $f13, $f16	# ctemp16 = ctemp16 + temp4
	MUL	$f7,  $f17, $f17	# ctemp4  = alpha_r * ctemp4

	ADD	$f18, $f21, $f21	# ctemp10 =  ctemp9  + ctemp10
	MUL	$f7,  $f1,  $f3		# ctemp5  = alpha_r * ctemp6
	SUB	$f30, $f19, $f30	# ctemp12 = -ctemp11 + ctemp12
	MUL	$f8,  $f1,  $f1		# ctemp6  = alpha_i * ctemp6

	ADD	$f5,  $f0,  $f0		# ctemp14 =  ctemp13 + ctemp14
	MUL	$f8,  $f2,  $f4		# ctemp7  = alpha_i * ctemp8
	SUB	$f16, $f6,  $f16	# ctemp16 = -ctemp15 + ctemp16
	MUL	$f7,  $f2,  $f2		# ctemp8  = alpha_r * ctemp8

#endif

#if   defined(NN) || defined(NT) || defined(TN) || defined(TT)

	MUL	$f7,  $f21, $f18	# ctemp9  = alpha_r * ctemp10
	SUB	$f28, $f29, $f28	# ctemp1  = ctemp1  - ctemp3
	MUL	$f8,  $f30, $f19	# ctemp11 = alpha_i * ctemp12
	ADD	$f17, $f20, $f20	# ctemp2  = ctemp4  + ctemp2

	MUL	$f8,  $f21, $f21	# ctemp10 = alpha_i * ctemp10
	SUB	$f3,  $f4,  $f29	# ctemp3  = ctemp5  - ctemp7
	MUL	$f7,  $f30, $f30	# ctemp12 = alpha_r * ctemp12
	ADD	$f2,  $f1,  $f17	# ctemp4  = ctemp8  + ctemp6

	MUL	$f7,  $f0,  $f5		# ctemp13 = alpha_r * ctemp14
	fnop
	MUL	$f8,  $f0,  $f0		# ctemp14 = alpha_i * ctemp14
	SUB	$f18, $f19, $f18	# ctemp9  = ctemp9  - ctemp11

	MUL	$f8,  $f16, $f6		# ctemp15 = alpha_i * ctemp16
	fnop
	ADD	$f30, $f21, $f21	# ctemp10 = ctemp12 + ctemp10
	MUL	$f7,  $f16, $f16	# ctemp16 = alpha_r * ctemp16

	SUB	$f5,  $f6,  $f19	# ctemp11 = ctemp13 - ctemp15
	ADD	$f16, $f0,  $f30	# ctemp12 = ctemp16 + ctemp14
	ADD	$f24, $f28, $f28	# ctemp1  = atemp1  + ctemp1
	ADD	$f20, $f25, $f20	# ctemp2  = atemp2  + ctemp2

#else /* defined(CN)||defined(CT)||defined(NC)||defined(TC)||defined(CC) */
      /* defined(RN)||defined(RT)||defined(NR)||defined(TR)||defined(CR) */
      /* defined(RC)|| defined(RR) */

	MUL	$f7,  $f21, $f18	# ctemp9  = alpha_r * ctemp10
	ADD	$f28, $f29, $f28	# ctemp1  =  ctemp1  + ctemp3
	MUL	$f8,  $f30, $f19	# ctemp11 = alpha_i * ctemp12
	SUB	$f20, $f17, $f20	# ctemp2  = -ctemp4  + ctemp2

	MUL	$f8,  $f21, $f21	# ctemp10 = alpha_i * ctemp10
	ADD	$f3,  $f4,  $f29	# ctemp3  =  ctemp5  + ctemp7
	MUL	$f7,  $f30, $f30	# ctemp12 = alpha_r * ctemp12
	SUB	$f1,  $f2,  $f17	# ctemp4  = -ctemp8  + ctemp6

	MUL	$f7,  $f0,  $f5		# ctemp13 = alpha_r * ctemp14
	fnop
	MUL	$f8,  $f0,  $f0		# ctemp14 = alpha_i * ctemp14
	ADD	$f18, $f19, $f18	# ctemp9  =  ctemp9  + ctemp11

	MUL	$f8,  $f16, $f6		# ctemp15 = alpha_i * ctemp16
	fnop
	MUL	$f7,  $f16, $f16	# ctemp16 = alpha_r * ctemp16
	SUB	$f21, $f30, $f21	# ctemp10 = -ctemp12 + ctemp10

	ADD	$f5,  $f6,  $f19	# ctemp11 =  ctemp13 + ctemp15
	SUB	$f0,  $f16, $f30	# ctemp12 = -ctemp16 + ctemp14
	ADD	$f24, $f28, $f28	# ctemp1  = atemp1  + ctemp1
	ADD	$f20, $f25, $f20	# ctemp2  = atemp2  + ctemp2

#endif

	ADD	$f29, $f26, $f29	# ctemp3  = atemp3  + ctemp3
	ADD	$f17, $f27, $f17	# ctemp4  = atemp4  + ctemp4
	ADD	$f18, $f15, $f18	# ctemp9  = btemp1  + ctemp9
	ST	$f28, -4*SIZE($7)	# *(c_offset1 + 0) = ctemp1

	ADD	$f21, $f14, $f21	# ctemp10 = btemp2  + ctemp10
	ST	$f20, -3*SIZE($7)	# *(c_offset1 + 1) = ctemp2
	ADD	$f19, $f23, $f19	# ctemp11 = btemp3  + ctemp11
	ST	$f29, -2*SIZE($7)	# *(c_offset1 + 2) = ctemp3

	ADD	$f30, $f22, $f30	# ctemp12 = btemp4  + ctemp12
	fclr	$f4
	ST	$f17, -1*SIZE($7)	# *(c_offset1 + 3) = ctemp4
	unop

	ST	$f18,  0*SIZE($8)	# *(c_offset2 + 0) = ctemp9
	fclr	$f18
	lda	$8,    4*SIZE($8)	# c_offset2 += 4
	fclr	$f2

	ST	$f21, -3*SIZE($8)	# *(c_offset2 + 1) = ctemp10
	fclr	$f21
	ST	$f19, -2*SIZE($8)	# *(c_offset2 + 2) = ctemp11
	fclr	$f19

	ST	$f30, -1*SIZE($8)	# *(c_offset2 + 3) = ctemp12
	unop
	fclr	$f30
	bgt	$19, $L49
	.align 4

$L48:
	mov	$2, $3			# a_offset1 = a_offset
	blbc	$17, $L61		# if (min_i & 1)

	mov	$4,  $5			# b_offset1 = b_offset
	fclr	$f28
	subl	$16, 1, $21		# l = min_l - 1
	fclr	$f20

	LD	$f24,   0*SIZE($3)	# atemp1  = *(a_offset1 + 0)
	fclr	$f29
	LD	$f25,   1*SIZE($3)	# atemp2  = *(a_offset1 + 1)
	fclr	$f17

	LD	$f15,   0*SIZE($5)	# btemp1  = *(b_offset1 + 0)
	fclr	$f18
	LD	$f14,   1*SIZE($5)	# btemp2  = *(b_offset1 + 1)
	fclr	$f21

	LD	$f23,   2*SIZE($5)	# btemp3  = *(b_offset1 + 2)
	fclr	$f19
	LD	$f22,   3*SIZE($5)	# btemp4  = *(b_offset1 + 3)
	fclr	$f30

	lda	$3,     4*SIZE($3)	# a_offset 1 += 4
	fclr	$f12
	lda	$5,     4*SIZE($5)	# b_offset1 += 4
	fclr	$f10

	fclr	$f11
	unop
	fclr	$f13
	ble	$21,$L62
	.align 4

$L63:
	ADD	$f18, $f12, $f18	# ctemp9  = ctemp9  + temp1
	MUL	$f15, $f24, $f12	# temp1   = btemp1  * atemp1
	ADD	$f21, $f10, $f21	# ctemp10 = ctemp10 + temp2
	MUL	$f14, $f25, $f10	# temp2   = btemp2  * atemp2

	ADD	$f19, $f11, $f19	# ctemp11 = ctemp11 + temp3
	subl	$21,1,$21
	MUL	$f15, $f25, $f11	# temp3   = btemp1  * atemp2
	LD	$f15,  0*SIZE($5)	# btemp1  = *(c_offset2 + 0)

	ADD	$f30, $f13, $f30	# ctemp12 = ctemp12 + temp4
	lda	$3,    4*SIZE($3)	# a_offset 1 += 4
	MUL	$f14, $f24, $f13	# temp4   = btemp2  * atemp1
	LD	$f14,  1*SIZE($5)	# btemp2  = *(c_offset2 + 1)

	ADD	$f28, $f12, $f28	# ctemp1  = ctemp1  + temp1
	MUL	$f23, $f24, $f12	# temp1   = btemp3  * atemp1
	ADD	$f20, $f10, $f20	# ctemp2  = ctemp2  + temp2
	MUL	$f22, $f25, $f10	# temp2   = btemp4  * atemp2

	ADD	$f29, $f11, $f29	# ctemp3  = ctemp3  + temp3
	MUL	$f23, $f25, $f11	# temp3   = btemp3  * atemp2
	LD	$f25, -3*SIZE($3)	# atemp2  = *(a_offset1 + 1)
	LD	$f23,  2*SIZE($5)	# btemp3  = *(b_offset1 + 2)

	ADD	$f17, $f13, $f17	# ctemp4  = ctemp4  + temp4
	MUL	$f22, $f24, $f13	# temp4   = btemp4  * atemp1
	LD	$f24, -4*SIZE($3)	# atemp1  = *(a_offset1 + 0)
	LD	$f22,  3*SIZE($5)	# btemp4  = *(b_offset1 + 3)

	lda	$5,    4*SIZE($5)	# b_offset1 += 4
	bgt	$21,$L63
	.align 4

$L62:
	ADD	$f18, $f12, $f18	# ctemp9  = ctemp9  + temp1
	MUL	$f15, $f24, $f12	# temp1   = btemp1  * atemp1
	ADD	$f21, $f10, $f21	# ctemp10 = ctemp10 + temp2
	MUL	$f14, $f25, $f10	# temp2   = btemp2  * atemp2

	ADD	$f19, $f11, $f19	# ctemp11 = ctemp11 + temp3
	unop
	MUL	$f15, $f25, $f11	# temp3   = btemp1  * atemp2
	LD	$f15,  0*SIZE($8)	# btemp1  = *(c_offset2 + 0)

	ADD	$f30, $f13, $f30	# ctemp12 = ctemp12 + temp4
	unop
	MUL	$f14, $f24, $f13	# temp4   = btemp2  * atemp1
	LD	$f14,  1*SIZE($8)	# btemp2  = *(c_offset2 + 1)

	ADD	$f28, $f12, $f28	# ctemp1  = ctemp1  + temp1
	MUL	$f23, $f24, $f12	# temp1   = btemp3  * atemp1
	ADD	$f20, $f10, $f20	# ctemp2  = ctemp2  + temp2
	MUL	$f22, $f25, $f10	# temp2   = btemp4  * atemp2

	ADD	$f29, $f11, $f29	# ctemp3  = ctemp3  + temp3
	unop
	MUL	$f23, $f25, $f11	# temp3   = btemp3  * atemp2
	LD	$f25,  1*SIZE($7)	# atemp2  = *(c_offset1 + 1)

	ADD	$f17, $f13, $f17	# ctemp4  = ctemp4  + temp4
	unop
	MUL	$f22, $f24, $f13	# temp4   = btemp4  * atemp1
	LD	$f24,  0*SIZE($7)	# atemp1  = *(c_offset1 + 0)

	ADD	$f18, $f12, $f18	# ctemp9  = ctemp9  + temp1
	ADD	$f21, $f10, $f21	# ctemp10 = ctemp10 + temp2
	ADD	$f19, $f11, $f19	# ctemp11 = ctemp11 + temp3
	ADD	$f30, $f13, $f30	# ctemp12 = ctemp12 + temp4

#if   defined(NN) || defined(NT) || defined(TN) || defined(TT) || \
      defined(CC) || defined(CR) || defined(RC) || defined(RR)

	SUB	$f28, $f20, $f28	# ctemp1  = ctemp1  - ctemp2
	ADD	$f29, $f17, $f29	# ctemp3  = ctemp3  + ctemp4
	SUB	$f18, $f21, $f18	# ctemp9  = ctemp9  - ctemp10
	ADD	$f19, $f30, $f19	# ctemp11 = ctemp11 + ctemp12

#elif defined(CN) || defined(CT) || defined(RN) || defined(RT)

	ADD	$f28, $f20, $f28	# ctemp1  = ctemp1  + ctemp2
	SUB	$f29, $f17, $f29	# ctemp3  = ctemp3  - ctemp4
	ADD	$f18, $f21, $f18	# ctemp9  = ctemp9  + ctemp10
	SUB	$f19, $f30, $f19	# ctemp11 = ctemp11 - ctemp12

#else /* defined(NC) || defined(TC) || defined(NR) || defined(TR) */

	ADD	$f28, $f20, $f28	# ctemp1  =  ctemp1  + ctemp2
	SUB	$f17, $f29, $f29	# ctemp3  = -ctemp3  + ctemp4
	ADD	$f18, $f21, $f18	# ctemp9  =  ctemp9  + ctemp10
	SUB	$f30, $f19, $f19	# ctemp11 = -ctemp11 + ctemp12

#endif

	MUL	$f7,  $f28, $f3		# ctemp5  = alpha_r * ctemp1
	MUL	$f8,  $f29, $f1		# ctemp6  = alpha_i * ctemp3
	MUL	$f7,  $f29, $f4		# ctemp7  = alpha_r * ctemp3
	MUL	$f8,  $f28, $f2		# ctemp8  = alpha_i * ctemp1

	MUL	$f7,  $f18, $f5		# ctemp13 = alpha_r * ctemp9
	MUL	$f8,  $f19, $f0		# ctemp14 = alpha_i * ctemp11
	MUL	$f7,  $f19, $f6		# ctemp15 = alpha_r * ctemp11
	MUL	$f8,  $f18, $f16	# ctemp16 = alpha_i * ctemp9

#if   defined(NN) || defined(NT) || defined(TN) || defined(TT)

	SUB	$f3,  $f1,  $f26	# atemp3  = ctemp5  - ctemp6
	ADD	$f4,  $f2,  $f27	# atemp4  = ctemp7  + ctemp8
	SUB	$f5,  $f0,  $f23	# btemp3  = ctemp13 - ctemp14
	ADD	$f6,  $f16, $f22	# btemp4  = ctemp15 + ctemp16

#else /* defined(CN)||defined(CT)||defined(NC)||defined(TC)||defined(CC) */
      /* defined(RN)||defined(RT)||defined(NR)||defined(TR)||defined(CR) */
      /* defined(RC)||defined(RR) */

	ADD	$f3,  $f1,  $f26	# atemp3  =  ctemp5  + ctemp6
	SUB	$f2,  $f4,  $f27	# atemp4  = -ctemp7  + ctemp8
	ADD	$f5,  $f0,  $f23	# btemp3  =  ctemp13 + ctemp14
	SUB	$f16, $f6,  $f22	# btemp4  = -ctemp15 + ctemp16

#endif

	ADD	$f26, $f24, $f26	# atemp3  = atemp1  + atemp3
	ADD	$f27, $f25, $f27	# atemp4  = atemp2  + atemp4
	ADD	$f23, $f15, $f23	# btemp3  = btemp1  + btemp3
	ADD	$f22, $f14, $f22	# btemp4  = btemp2  + btemp4

	ST	$f26,  0*SIZE($7)	# *(c_offset1 + 0) = atemp3
	ST	$f27,  1*SIZE($7)	# *(c_offset1 + 1) = atemp4
	ST	$f23,  0*SIZE($8)	# *(c_offset2 + 0) = btemp3
	ST	$f22,  1*SIZE($8)	# *(c_offset2 + 1) = btemp4
	.align 4

$L61:
	subl	$20, 1, $20		# j --
	lda	$4,  LDA*SIZE($4)	# b_offset += LDA
	unop
	bgt	$20,  $L45
	.align 4

$L44:
	mov	$27, $2			# c_offset = &sa[0][0]
	blbc	$18,$L31
	sra	$17,  1, $19		# i = (min_i >> 1)
	ble	$19,$L71
	.align 4

$L72:
	LD	$f24,   0*SIZE($2)	# atemp1  = *(a_offset1 + 0)
	fclr	$f28
	LD	$f25,   1*SIZE($2)	# atemp2  = *(a_offset1 + 1)
	fclr	$f20

	LD	$f26,   2*SIZE($2)	# atemp3  = *(a_offset1 + 2)
	fclr	$f29
	LD	$f27,   3*SIZE($2)	# atemp4  = *(a_offset1 + 3)
	fclr	$f17

	LD	$f15,   0*SIZE($4)	# btemp1  = *(b_offset1 + 0)
	fclr	$f12
	LD	$f14,   1*SIZE($4)	# btemp2  = *(b_offset1 + 1)
	fclr	$f10

	subl	$16, 1, $21		# l = min_l - 1
	fclr	$f3
	lda	$3,     4*SIZE($2)	# a_offset 1 += 4
	fclr	$f1

	lda	$2,  LDA*SIZE($2)	# a_offset += LDA
	fclr	$f4
	lda	$5,     4*SIZE($4)	# b_offset1 += 4
	fclr	$f2

	fclr	$f11
	unop
	fclr	$f13
	ble	$21,  $L75
	.align 4

$L76:
	ADD	$f3,  $f12, $f3		# ctemp5  = ctemp5  + temp1
	MUL	$f15, $f24, $f12	# temp1   = btemp1  * atemp1
	ADD	$f1,  $f10, $f1		# ctemp6  = ctemp6  + temp2
	MUL	$f14, $f25, $f10	# temp2   = btemp2  * atemp2

	ADD	$f4,  $f11, $f4		# ctemp7  = ctemp7  + temp3
	lda	$5,     4*SIZE($5)	# b_offset1 += 4
	MUL	$f15, $f25, $f11	# temp3   = btemp1  * atemp2
	LD	$f25,   1*SIZE($3)	# atemp2  = *(a_offset1 + 1)

	ADD	$f2,  $f13, $f2		# ctemp8  = ctemp8  + temp4
	subl	$21, 1, $21		# l --
	MUL	$f14, $f24, $f13	# temp4   = btemp2  * atemp1
	LD	$f24,   0*SIZE($3)	# atemp1  = *(a_offset1 + 0)

	ADD	$f28, $f12, $f28	# ctemp1  = ctemp1  + temp1
	MUL	$f15, $f26, $f12	# temp1   = btemp1  * atemp3
	ADD	$f20, $f10, $f20	# ctemp2  = ctemp2  + temp2
	MUL	$f14, $f27, $f10	# temp2   = btemp2  * atemp4

	ADD	$f29, $f11, $f29	# ctemp3  = ctemp3  + temp3
	MUL	$f15, $f27, $f11	# temp3   = btemp1  * atemp4
	LD	$f27,   3*SIZE($3)	# atemp4  = *(a_offset1 + 3)
	LD	$f15,  -4*SIZE($5)	# btemp1  = *(b_offset1 + 0)

	ADD	$f17, $f13, $f17	# ctemp4  = ctemp4  + temp4
	MUL	$f14, $f26, $f13	# temp4   = btemp2  * atemp3
	LD	$f26,   2*SIZE($3)	# atemp3  = *(a_offset1 + 2)
	LD	$f14,  -3*SIZE($5)	# btemp2  = *(b_offset1 + 1)

	lda	$3,     4*SIZE($3)	# a_offset 1 += 4
	bgt	$21, $L76
	.align 4

$L75:
	ADD	$f3,  $f12, $f3		# ctemp5  = ctemp5  + temp1
	MUL	$f15, $f24, $f12	# temp1   = btemp1  * atemp1
	ADD	$f1,  $f10, $f1		# ctemp6  = ctemp6  + temp2
	MUL	$f14, $f25, $f10	# temp2   = btemp2  * atemp2

	ADD	$f4,  $f11, $f4		# ctemp7  = ctemp7  + temp3
	unop
	MUL	$f15, $f25, $f11	# temp3   = btemp1  * atemp2
	LD	$f25,  1*SIZE($6)	# atemp2  = *(c_offset + 1)

	ADD	$f2,  $f13, $f2		# ctemp8  = ctemp8  + temp4
	unop
	MUL	$f14, $f24, $f13	# temp4   = btemp2  * atemp1
	LD	$f24,  0*SIZE($6)	# atemp1  = *(c_offset + 0)

	ADD	$f28, $f12, $f28	# ctemp1  = ctemp1  + temp1
	MUL	$f15, $f26, $f12	# temp1   = btemp1  * atemp3
	ADD	$f20, $f10, $f20	# ctemp2  = ctemp2  + temp2
	MUL	$f14, $f27, $f10	# temp2   = btemp2  * atemp4

	ADD	$f29, $f11, $f29	# ctemp3  = ctemp3  + temp3
	unop
	MUL	$f15, $f27, $f11	# temp3   = btemp1  * atemp4
	LD	$f15,  2*SIZE($6)	# btemp1  = *(c_offset + 2)

	ADD	$f17, $f13, $f17	# ctemp4  = ctemp4  + temp4
	subl	$19, 1, $19		# i --
	MUL	$f14, $f26, $f13	# temp4   = btemp2  * atemp3
	LD	$f14,  3*SIZE($6)	# btemp2  = *(c_offset + 3)

	ADD	$f3,  $f12, $f3		# ctemp5  = ctemp5  + temp1
	ADD	$f1,  $f10, $f1		# ctemp6  = ctemp6  + temp2
	ADD	$f4,  $f11, $f4		# ctemp7  = ctemp7  + temp3
	ADD	$f2,  $f13, $f2		# ctemp8  = ctemp8  + temp4

#if   defined(NN) || defined(NT) || defined(TN) || defined(TT) || \
      defined(CC) || defined(CR) || defined(RC) || defined(RR)

	SUB	$f28, $f20, $f28	# ctemp1  = ctemp1  - ctemp2
	ADD	$f29, $f17, $f29	# ctemp3  = ctemp3  + ctemp4
	SUB	$f3,  $f1,  $f3		# ctemp5  = ctemp5  - ctemp6
	ADD	$f4,  $f2,  $f4		# ctemp7  = ctemp7  + ctemp8

#elif defined(CN) || defined(CT) || defined(RN) || defined(RT)

	ADD	$f28, $f20, $f28	# ctemp1  = ctemp1  + ctemp2
	SUB	$f29, $f17, $f29	# ctemp3  = ctemp3  - ctemp4
	ADD	$f3,  $f1,  $f3		# ctemp5  = ctemp5  + ctemp6
	SUB	$f4,  $f2,  $f4		# ctemp7  = ctemp7  - ctemp8

#else /* defined(NC) || defined(TC) || defined(NR) || defined(TR) */

	ADD	$f28, $f20, $f28	# ctemp1  =  ctemp1  + ctemp2
	SUB	$f17, $f29, $f29	# ctemp3  = -ctemp3  + ctemp4
	ADD	$f3,  $f1,  $f3		# ctemp5  =  ctemp5  + ctemp6
	SUB	$f2,  $f4,  $f4		# ctemp7  = -ctemp7  + ctemp8

#endif

	MUL	$f7,  $f28, $f18	# ctemp9  = alpha_r * ctemp1
	MUL	$f8,  $f29, $f21	# ctemp10 = alpha_i * ctemp3
	MUL	$f7,  $f29, $f19	# ctemp11 = alpha_r * ctemp3
	MUL	$f8,  $f28, $f30	# ctemp12 = alpha_i * ctemp1

	MUL	$f7,  $f3,  $f5		# ctemp13 = alpha_r * ctemp5
	MUL	$f8,  $f4,  $f0		# ctemp14 = alpha_i * ctemp7
	MUL	$f7,  $f4,  $f6		# ctemp15 = alpha_r * ctemp7
	MUL	$f8,  $f3,  $f16	# ctemp16 = alpha_i * ctemp5

#if   defined(NN) || defined(NT) || defined(TN) || defined(TT)

	SUB	$f18, $f21, $f26	# atemp3  = ctemp9  - ctemp10
	ADD	$f19, $f30, $f27	# atemp4  = ctemp11 + ctemp12
	SUB	$f5,  $f0,  $f23	# btemp3  = ctemp13 - ctemp14
	ADD	$f6,  $f16, $f22	# btemp4  = ctemp15 + ctemp16

#else /* defined(CN)||defined(CT)||defined(NC)||defined(TC)||defined(CC) */
      /* defined(RN)||defined(RT)||defined(NR)||defined(TR)||defined(CR) */
      /* defined(RC) || defined(RR) */

	ADD	$f18, $f21, $f26	# atemp3  =  ctemp9  + ctemp10
	SUB	$f30, $f19, $f27	# atemp4  = -ctemp11 + ctemp12
	ADD	$f5,  $f0,  $f23	# btemp3  =  ctemp13 + ctemp14
	SUB	$f16, $f6,  $f22	# btemp4  = -ctemp15 + ctemp16

#endif

	ADD	$f26, $f24, $f26	# atemp3  = atemp1 + atemp3
	ADD	$f27, $f25, $f27	# atemp4  = atemp2 + atemp4
	ADD	$f23, $f15, $f23	# btemp3  = btemp1 + btemp3
	ADD	$f22, $f14, $f22	# btemp4  = btemp2 + btemp4

	ST	$f26,  0*SIZE($6)	# *(c_offset + 0) = atemp3
	ST	$f27,  1*SIZE($6)	# *(c_offset + 1) = atemp4
	ST	$f23,  2*SIZE($6)	# *(c_offset + 2) = btemp3
	ST	$f22,  3*SIZE($6)	# *(c_offset + 3) = btemp4

	lda	$6,    4*SIZE($6)	# c_offset += 4
	bgt	$19, $L72
	.align 4

$L71:
	blbc	$17, $L31
	.align 4


	LD	$f24,  0*SIZE($2)	# atemp1  = *(a_offset + 0)
	fclr	$f28
	LD	$f25,  1*SIZE($2)	# atemp2  = *(a_offset + 1)
	fclr	$f20

	LD	$f15,  0*SIZE($4)	# btemp1  = *(b_offset + 0)
	fclr	$f29
	LD	$f14,  1*SIZE($4)	# btemp2  = *(b_offset + 1)
	fclr	$f17

	subl	$16, 1, $21		# l = min_l - 1
	fclr	$f12
	lda	$2,    4*SIZE($2)	# a_offset += 4
	fclr	$f10

	lda	$4 ,   4*SIZE($4)	# b_offset += 4
	fclr	$f11
	fclr	$f13
	ble	$21,$L84			# if l<=0 goto $L84
	.align 4

$L85:
	ADD	$f28, $f12, $f28	# ctemp1  = ctemp1  + temp1
	subl	$21, 1, $21		# l --
	MUL	$f15, $f24, $f12	# temp1   = btemp1  * atemp1
	lda	$2,    4*SIZE($2)	# a_offset += 4

	ADD	$f20, $f10, $f20	# ctemp2  = ctemp2  + temp2
	unop
	MUL	$f14, $f25, $f10	# temp2   = btemp2  * atemp2
	unop

	ADD	$f29, $f11, $f29	# ctemp3  = ctemp3  + temp3
	MUL	$f15, $f25, $f11	# temp3   = btemp1  * atemp2
	LD	$f25, -3*SIZE($2)	# atemp2  = *(a_offset + 1)
	LD	$f15,  0*SIZE($4)	# btemp1  = *(b_offset + 0)

	ADD	$f17, $f13, $f17	# ctemp4  = ctemp4  + temp4
	MUL	$f14, $f24, $f13	# temp4   = btemp2  * atemp1
	LD	$f24, -4*SIZE($2)	# atemp1  = *(a_offset + 0)
	LD	$f14,  1*SIZE($4)	# btemp2  = *(b_offset + 1)

	lda	$4 ,   4*SIZE($4)	# b_offset += 4
	bgt	$21,$L85
	.align 4

$L84:
	ADD	$f28, $f12, $f28	# ctemp1  = ctemp1  + temp1
	MUL	$f15, $f24, $f12	# temp1   = btemp1  * atemp1
	ADD	$f20, $f10, $f20	# ctemp2  = ctemp2  + temp2
	MUL	$f14, $f25, $f10	# temp2   = btemp2  * atemp2

	ADD	$f29, $f11, $f29	# ctemp3  = ctemp3  + temp3
	unop
	MUL	$f15, $f25, $f11	# temp3   = btemp1  * atemp2
	LD	$f25,  1*SIZE($6)	# atemp2  = *(c_offset + 1)

	ADD	$f17, $f13, $f17	# ctemp4  = ctemp4  + temp4
	unop
	MUL	$f14, $f24, $f13	# temp4   = btemp2  * atemp1
	LD	$f24,  0*SIZE($6)	# atemp1  = *(c_offset + 0)

	ADD	$f28, $f12, $f28	# ctemp1  = ctemp1  + temp1
	ADD	$f20, $f10, $f20	# ctemp2  = ctemp2  + temp2
	ADD	$f29, $f11, $f29	# ctemp3  = ctemp3  + temp3
	ADD	$f17, $f13, $f17	# ctemp4  = ctemp4  + temp4

#if   defined(NN) || defined(NT) || defined(TN) || defined(TT) || \
      defined(CC) || defined(CR) || defined(RC) || defined(RR)

	SUB	$f28, $f20, $f28	# ctemp1  = ctemp1  - ctemp2
	ADD	$f29, $f17, $f29	# ctemp3  = ctemp3  + ctemp4

#elif defined(CN) || defined(CT) || defined(RN) || defined(RT)

	ADD	$f28, $f20, $f28	# ctemp1  = ctemp1  + ctemp2
	SUB	$f29, $f17, $f29	# ctemp3  = ctemp3  - ctemp4

#else /* defined(NC) || defined(TC) || defined(NR) || defined(TR) */

	ADD	$f28, $f20, $f28	# ctemp1  =  ctemp1  + ctemp2
	SUB	$f17, $f29, $f29	# ctemp3  = -ctemp3  + ctemp4

#endif

	MUL	$f7,  $f28, $f12	# temp1   = alpha_r * ctemp1
	MUL	$f8,  $f29, $f10	# temp2   = alpha_i * ctemp3
	MUL	$f7,  $f29, $f11	# temp3   = alpha_r * ctemp3
	MUL	$f8,  $f28, $f13	# temp4   = alpha_i * ctemp1

#if   defined(NN) || defined(NT) || defined(TN) || defined(TT)

	SUB	$f12, $f10, $f26	# atemp4  = temp3  + temp4
	ADD	$f11, $f13, $f27	# atemp3  = temp1  - temp2

#else /* defined(CN)||defined(CT)||defined(NC)||defined(TC)||defined(CC) */
      /* defined(RN)||defined(RT)||defined(NR)||defined(TR)||defined(CR) */
      /* defined(RC) || defined(RR) */

	ADD	$f12, $f10, $f26	# atemp4  = temp3  + temp4
	SUB	$f13, $f11, $f27	# atemp3  = temp1  - temp2

#endif

	ADD	$f25, $f27, $f25	# atemp1  = atemp1 + atemp3
	ADD	$f24, $f26, $f24	# atemp2  = atemp2 + atemp4

	ST	$f24,  0*SIZE($6)	# *(c_offset + 0) = atemp1
	ST	$f25,  1*SIZE($6)	# *(c_offset + 1) = atemp2
	.align 4

$L31: 
	lda	$22,   P($22)		# is += P
	nop
	cmplt	$22, $9, $1
	bne	$1,  $L32		# end of is
	.align 4

$L9:
	lda	$23,   R($23)		# js += R
	nop
	cmplt	$23, $10, $1
	bne	$1,$L10
	.align 4			# end of js

$L4:
	lda	$24, Q($24)		# ls += Q
	nop
	cmplt	$24, $11, $1
	bne	$1,$L5			# end of ls

$END:
	ldq	$26,    0($30)
	ldq	$9,     8($30)
	ldq	$10,   16($30)
	ldq	$11,   24($30)
	ldq	$12,   32($30)
	ldq	$13,   40($30)
	ldq	$14,   48($30)
	ldq	$15,   56($30)

	ldt	$f2,   64($30)
	ldt	$f3,   72($30)
	ldt	$f4,   80($30)
	ldt	$f5,   88($30)
	ldt	$f6,   96($30)
	ldt	$f7,  104($30)
	ldt	$f8,  112($30)
	ldt	$f9,  120($30)
	lda	$30, STACKSIZE($30)
	clr	$0
	ret
	.ident	VERSION
	.end ROUTINE
