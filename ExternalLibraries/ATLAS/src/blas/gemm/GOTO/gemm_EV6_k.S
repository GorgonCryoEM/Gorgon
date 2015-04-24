    /*        Fast GEMM routine for Alpha 21264            */
    /*         on  Linux, Digital UNIX                     */
    /*        by Kazushige Goto <goto@statabo.rim.or.jp>   */

/*
     99.07.30  First optimizing for 21264
     99.07.30  integer register optimizing
     99.07.31  2*4*1 unrolling
     99.07.31  4*4*1 unrolling
     99.08.01  4*4*1 optimized for data accessing
     99.08.03  works fine. but not optimized well.
     99.08.04  loop register optimize
     99.08.04  optimizing of C level has finished
     99.08.08  non-optimized assembler routine
     99.08.12  most opimization has finished
     99.08.15  Assembler optimization has finished
     99.08.20  Blocking sizes are adjusted
*/

/* 
                        Register Map 
Integer:
 $0 : temp	 $1 : temp	 $2 : a_offset	 $3 : a_offset1
 $4 : b_offset	 $5 : b_offset1	 $6 : i		 $7 : j
 $8 : l		 $ 9: m		 $10: n		 $11: k
 $12: b		 $13: min_i	 $14: lda(x8)	  $15: c_offset
 $16: ls	 $17: is	 $18: js	 $19: min_j
 $20: a   	 $21: min_l	 $22: ldb(x8)	 $23: c
 $24: ldc(x8)	 $25: c_offset1	 $26: c_offset2	 $27: c_offset3
 $at: c_offset4	 $29: ---	 $30: SP	 $31: Zero

Floating Point:
$f0 : ctemp16	$f1 : ctemp9	$f2 : ctemp10	$f3 : ctemp7
$f4 : ctemp11	$f5 : ctemp8	$f6 : ctemp12	$f7 : alpha
$f8 : prefech	$f9 : prefetch	$f10: temp1	$f11: temp3
$f12: temp2	$f13: temp4	$f14: btemp1	$f15: atemp1
$f16: ctemp5	$f17: ctemp15	$f18: ctemp14	$f19: ctemp6
$f20: ctemp13	$f21: ctemp4	$f22: btemp2	$f23: btemp3
$f24: btemp4	$f25: ctemp1	$f26: atemp2	$f27: atemp3
$f28: atemp4	$f29: ctemp2	$f30: ctemp3	$f31: Zero

*/

#define WHSIZE 2

#ifdef DGEMM
#define	COPY_PREFETCH_SIZE 12
#else
#define	COPY_PREFETCH_SIZE 12
#endif

#ifndef P
#ifdef DGEMM
#if 1
#define P	112
#define Q	504
#define R	500
#else
#define P	128
#define Q	400
#define R	404
#endif
#else
#define P	120
#define Q	504
#define R	504
#endif
#endif

#define LDA  (Q<<2)

#define MATRIX_A   (((P>>2)*LDA*SIZE))   /* size = P * Q * SIZE */
#define MATRIX_B   (((R>>2)*LDA*SIZE))   /* size = R * Q * SIZE */

#define SA_ADDRESS 0
#define SB_ADDRESS (MATRIX_A+0)

#ifdef NN
#define ROUTINE GEMM_NN
#endif
#ifdef NT
#define ROUTINE GEMM_NT
#endif
#ifdef TN
#define ROUTINE GEMM_TN
#endif
#ifdef TT
#define ROUTINE GEMM_TT
#endif

	.set	noat
	.set	noreorder
	.arch ev6

.text
	.align	5
	.globl	ROUTINE
	.ent	ROUTINE

#define STACKSIZE 20*8

ROUTINE:
	ldq	$19,  0($30)	# b
	ble	$16, $quickend
	ldl	$22,  8($30)
	ble	$17, $quickend

	ldq	$23, 16($30)	# c
	ble	$18, $quickend
	ldl	$24, 24($30)
	lda	$30, -STACKSIZE($30)

	stq	$26,  0($30)
	stq	$9,   8($30)
	stq	$10, 16($30)
	stq	$11, 24($30)

	stq	$12, 32($30)
	stq	$13, 40($30)
	stq	$14, 48($30)
	stq	$15, 56($30)

	stt	$f2, 64($30)
	stt	$f3, 72($30)
	stt	$f4, 80($30)
	stt	$f5, 88($30)

	stt	$f6, 96($30)
	SXADDQ	$21, 0, $21
	stt	$f7,104($30)
	SXADDQ	$22, 0, $22			# ldb(x8)

	stt	$f8,112($30)
	SXADDQ	$24, 0, $24			# ldc  :24904
	stt	$f9,120($30)
	clr	$9				# ls = 0

	stq	$29,128($30)
	.prologue 0
	ldq	$29, 32 + STACKSIZE($30)	# buffer
	.align	4

$L5:
	lda	$0,   Q
	subl	$18, $9, $14		# min_l = k - ls
	cmple	$14, $0, $1		# $1 = (min_l > Q)
	cmoveq	$1,  $0, $14		# if ($1) min_l = Q

	clr	$11
	.align	4

$L10:
#if defined(NN) || defined(TN)
/* Outer Copying */

	lda	$0,    R
	subl	$17, $11, $12		# min_j = n - js
	cmple	$12, $0,  $1		# $1 = (min_J > R)
	ldah	$2,  Address_H(SB_ADDRESS)($29)	# a_offset = &sb[0][0]

	cmoveq	$1,  $0,  $12		# if ($1) min_j = R
	mull	$22, $11,  $1		# ldb * js
	lda	$2,  Address_L(SB_ADDRESS)($2)	# a_offset = &sb[0][0]
	sra	$12, 2, $7		# j = (min_j >> 2)

	addq	$1, $19, $4		# b_offset = b + ldb * js
	ble	$7, $L12		# if (j <= 0) 
	.align	4

$L13:
	mov	$2, $3			# a_offset1 = a_offset
	lda	$2, LDA*SIZE($2)	# a_offset += LDA

	SXADDQ	$9, $4, $25		# c_offset1 = b_offset  + ls

	addq	$25, $22, $26		# c_offset2 = c_offset1 + ldb
	addq	$26, $22, $27		# c_offset3 = c_offset2 + ldb
	addq	$27, $22, $at		# c_offset4 = c_offset3 + ldb
	s4addq	$22, $4, $4		# b_offset += 4*ldb

	mov	$14, $8			# l = min_l
	lda	$7,        -1($7)		# j --
	.align	4

$L16:
	LD	$f25,  0*SIZE($25)	# ctemp1  = *(c_offset1 + 0)
	LD	$f29,  1*SIZE($25)	# ctemp2  = *(c_offset1 + 1)
	LD	$f30,  2*SIZE($25)	# ctemp3  = *(c_offset1 + 2)
	LD	$f21,  3*SIZE($25)	# ctemp4  = *(c_offset1 + 3)

	LD	$f16,  0*SIZE($26)	# ctemp5  = *(c_offset2 + 0)
	LD	$f7,   1*SIZE($26)	# ctemp6  = *(c_offset2 + 1)
	LD	$f3,   2*SIZE($26)	# ctemp7  = *(c_offset2 + 2)
	LD	$f5,   3*SIZE($26)	# ctemp8  = *(c_offset2 + 3)

	LD	$f1,   0*SIZE($27)	# ctemp9  = *(c_offset3 + 0)
	LD	$f2,   1*SIZE($27)	# ctemp10 = *(c_offset3 + 1)
	LD	$f4,   2*SIZE($27)	# ctemp11 = *(c_offset3 + 2)
	LD	$f6,   3*SIZE($27)	# ctemp12 = *(c_offset3 + 3)

	LD	$f20,  0*SIZE($at)	# ctemp13 = *(c_offset4 + 0)
	LD	$f18,  1*SIZE($at)	# ctemp14 = *(c_offset4 + 1)
	LD	$f17,  2*SIZE($at)	# ctemp15 = *(c_offset4 + 2)
	LD	$f0,   3*SIZE($at)	# ctemp16 = *(c_offset4 + 3)

	ldl	$31,  COPY_PREFETCH_SIZE*SIZE($25)
	lda	$8,        -4($8)	# l -= 4
	ldl	$31,  COPY_PREFETCH_SIZE*SIZE($26)
	lda	$25,   4*SIZE($25)	# c_offset1 += 4

	ldl	$31,  COPY_PREFETCH_SIZE*SIZE($27)
	lda	$26,   4*SIZE($26)	# c_offset2 += 4
	ldl	$31,  COPY_PREFETCH_SIZE*SIZE($at)
	lda	$27,   4*SIZE($27)	# c_offset3 += 4

	ST	$f25,  0*SIZE($3)	# *(a_offset1 +  0) = ctemp1
	ST	$f16,  1*SIZE($3)	# *(a_offset1 +  1) = ctemp5
	ST	$f1,   2*SIZE($3)	# *(a_offset1 +  2) = ctemp9
	ST	$f20,  3*SIZE($3)	# *(a_offset1 +  3) = ctemp13

	ST	$f29,  4*SIZE($3)	# *(a_offset1 +  4) = ctemp2
	ST	$f7,   5*SIZE($3)	# *(a_offset1 +  5) = ctemp6
	ST	$f2,   6*SIZE($3)	# *(a_offset1 +  6) = ctemp10
	ST	$f18,  7*SIZE($3)	# *(a_offset1 +  7) = ctemp14

	ST	$f30,  8*SIZE($3)	# *(a_offset1 +  8) = ctemp3
	ST	$f3,   9*SIZE($3)	# *(a_offset1 +  9) = ctemp7
	ST	$f4,  10*SIZE($3)	# *(a_offset1 + 10) = ctemp11
	ST	$f17, 11*SIZE($3)	# *(a_offset1 + 11) = ctemp15

	ST	$f21, 12*SIZE($3)	# *(a_offset1 + 12) = ctemp4
	ST	$f5,  13*SIZE($3)	# *(a_offset1 + 13) = ctemp8
	ST	$f6,  14*SIZE($3)	# *(a_offset1 + 14) = ctemp12
	lda	$at,   4*SIZE($at)	# c_offset4 += 4

	ST	$f0,  15*SIZE($3)	# *(a_offset1 + 15) = ctemp16
	lda	$3,   16*SIZE($3)	# a_offset += 16
	bgt	$8, $L16
	bgt	$7, $L13
	.align 4

$L12:
	and	$12, 3, $7		# j = (min_j & 3)
	clr	$10			# is = 0
	ble	$7,  $L37

	.align	4

$L24:
	mov	$2,  $3			# a_offset1 = a_offset
	lda	$2,    1*SIZE($2)	# a_offset ++
	SXADDQ	$9,  $4, $25		# c_offset1 = b_offset + ls
	addq	$22, $4, $4		# b_offset += ldb
	mov	$14, $8			# l = min_l
	.align	4

$L27:
	LD	$f14,  0*SIZE($25)	# btemp1  = *(c_offset1 + 0)
	LD	$f22,  1*SIZE($25)	# btemp2  = *(c_offset2 + 0)
	LD	$f23,  2*SIZE($25)	# btemp3  = *(c_offset3 + 0)
	LD	$f24,  3*SIZE($25)	# btemp4  = *(c_offset4 + 0)

	ST	$f14,  0*SIZE($3)	# *(a_offset1 +  0) = btemp1
	ST	$f22,  4*SIZE($3)	# *(a_offset1 +  4) = btemp2
	ST	$f23,  8*SIZE($3)	# *(a_offset1 +  8) = btemp3
	ST	$f24, 12*SIZE($3)	# *(a_offset1 + 12) = btemp4

	lda	$3,   16*SIZE($3)	# a_offset1 += 16
	lda	$25,   4*SIZE($25)	# c_offset1 += 4

	lda	$8,        -4($8)	# l -= 4
	bgt	$8, $L27

	lda	$7,        -1($7)	# j --
	bgt	$7, $L24
	.align 4

#else /* defined(NT) || (TT) */

	lda	$0,    R
	subl	$17, $11, $12		# min_j = n - js
	cmple	$12, $0,  $1		# $1 = (min_J > R)
	ldah	$2,  Address_H(SB_ADDRESS)($29)	# a_offset = &sb[0][0]

	cmoveq	$1,  $0,  $12		# if ($1) min_j = R
	mull	$22, $9,  $1		# ldb * ls
	lda	$2,  Address_L(SB_ADDRESS)($2)
	sra	$14, 2, $8		# l = (min_l >> 2)

	addq	$1, $19, $4		# b_offset = b + ldb * ls
	ble	$8, $L12		# if (l <= 0) 
	.align	4

$L13:
	SXADDQ	$11, $4, $25		# c_offset1 = b_offset  + js
	mov	$2, $3			# a_offset1 = a_offset
	lda	$2,   16*SIZE($2)	# a_offset += 16
	addq	$22, $22, $0

	addq	$25, $22, $26		# c_offset2 = c_offset1 + ldb
	addq	$25, $0,  $27		# c_offset3 = c_offset2 + ldb
	mov	$12, $7			# j = min_j
	lda	$8,        -1($8)	# l --

	addq	$26, $0,  $at		# c_offset4 = c_offset3 + ldb
	unop
	unop
	s4addq	$22, $4, $4		# b_offset += 4*ldb
	.align	4

$L16:
	LD	$f25,  0*SIZE($25)	# ctemp1  = *(c_offset1 + 0)
	LD	$f29,  1*SIZE($25)	# ctemp2  = *(c_offset1 + 1)
	LD	$f30,  2*SIZE($25)	# ctemp3  = *(c_offset1 + 2)
	LD	$f21,  3*SIZE($25)	# ctemp4  = *(c_offset1 + 3)

	LD	$f16,  0*SIZE($26)	# ctemp5  = *(c_offset2 + 0)
	LD	$f7,   1*SIZE($26)	# ctemp6  = *(c_offset2 + 1)
	LD	$f3,   2*SIZE($26)	# ctemp7  = *(c_offset2 + 2)
	LD	$f5,   3*SIZE($26)	# ctemp8  = *(c_offset2 + 3)

	LD	$f1,   0*SIZE($27)	# ctemp9  = *(c_offset3 + 0)
	LD	$f2,   1*SIZE($27)	# ctemp10 = *(c_offset3 + 1)
	LD	$f4,   2*SIZE($27)	# ctemp11 = *(c_offset3 + 2)
	LD	$f6,   3*SIZE($27)	# ctemp12 = *(c_offset3 + 3)

	LD	$f20,  0*SIZE($at)	# ctemp13 = *(c_offset4 + 0)
	LD	$f18,  1*SIZE($at)	# ctemp14 = *(c_offset4 + 1)
	LD	$f17,  2*SIZE($at)	# ctemp15 = *(c_offset4 + 2)
	LD	$f0,   3*SIZE($at)	# ctemp16 = *(c_offset4 + 3)

	ldl	$31,  COPY_PREFETCH_SIZE*SIZE($25)
	lda	$7,        -4($7)	# j -= 4
	ldl	$31,  COPY_PREFETCH_SIZE*SIZE($26)
	lda	$25,   4*SIZE($25)	# c_offset1 += 4

	ldl	$31,  COPY_PREFETCH_SIZE*SIZE($27)
	lda	$26,   4*SIZE($26)	# c_offset2 += 4
	ldl	$31,  COPY_PREFETCH_SIZE*SIZE($at)
	lda	$27,   4*SIZE($27)	# c_offset3 += 4

	ST	$f25,  0*SIZE($3)	# *(a_offset1 +  0) = ctemp1
	ST	$f29,  1*SIZE($3)	# *(a_offset1 +  1) = ctemp5
	ST	$f30,  2*SIZE($3)	# *(a_offset1 +  2) = ctemp9
	ST	$f21,  3*SIZE($3)	# *(a_offset1 +  3) = ctemp13

	ST	$f16,  4*SIZE($3)	# *(a_offset1 +  4) = ctemp2
	ST	$f7,   5*SIZE($3)	# *(a_offset1 +  5) = ctemp6
	ST	$f3,   6*SIZE($3)	# *(a_offset1 +  6) = ctemp10
	ST	$f5,   7*SIZE($3)	# *(a_offset1 +  7) = ctemp14

	ST	$f1,   8*SIZE($3)	# *(a_offset1 +  8) = ctemp3
	ST	$f2,   9*SIZE($3)	# *(a_offset1 +  9) = ctemp7
	ST	$f4,  10*SIZE($3)	# *(a_offset1 + 10) = ctemp11
	ST	$f6,  11*SIZE($3)	# *(a_offset1 + 11) = ctemp15

	ST	$f20, 12*SIZE($3)	# *(a_offset1 + 12) = ctemp4
	ST	$f18, 13*SIZE($3)	# *(a_offset1 + 13) = ctemp8
	ST	$f17, 14*SIZE($3)	# *(a_offset1 + 14) = ctemp12
	lda	$at,   4*SIZE($at)	# c_offset4 += 4

	ST	$f0,  15*SIZE($3)	# *(a_offset1 + 15) = ctemp16
	lda	$3,  LDA*SIZE($3)	# a_offset += LDB
	bgt	$7, $L16
	bgt	$8, $L13
	.align 4

$L12:
	and	$14, 3, $8		# l = (min_l & 3)
	clr	$10			# is = 0
	unop
	ble	$8, $L37
	.align	4

$L24:
	mov	$2, $3			# a_offset1 = a_offset
	lda	$2,    4*SIZE($2)	# a_offset += 4
	SXADDQ	$11, $4, $25		# c_offset1 = b_offset + js
	addq	$22, $4, $4		# b_offset += ldb

	mov	$12, $7			# j = min_j
	unop
	unop
	lda	$8,        -1($8)	# l --
	.align	4

$L27:
	LD	$f14,  0*SIZE($25)	# btemp1  = *(c_offset1 + 0)
	LD	$f22,  1*SIZE($25)	# btemp2  = *(c_offset2 + 0)
	LD	$f23,  2*SIZE($25)	# btemp3  = *(c_offset3 + 0)
	LD	$f24,  3*SIZE($25)	# btemp4  = *(c_offset4 + 0)

	lda	$25,   4*SIZE($25)	# c_offset1 += 4
	lda	$7,        -4($7)	# j -= 4
	lda	$3,  LDA*SIZE($3)	# a_offset1 += LDB
	nop

	ST	$f14,  (0-LDA)*SIZE($3)	# *(a_offset1 +  0) = btemp1
	ST	$f22,  (1-LDA)*SIZE($3)	# *(a_offset1 +  4) = btemp2
	ST	$f23,  (2-LDA)*SIZE($3)	# *(a_offset1 +  8) = btemp3
	ST	$f24,  (3-LDA)*SIZE($3)	# *(a_offset1 + 16) = btemp4

	bgt	$7, $L27
	unop
	unop
	bgt	$8, $L24
	.align 4
#endif  /* TN or TT of Outer copying */

$L37:
#if defined(NN) || defined(NT)
	mull	$21,  $9, $6		# ls * lda
	lda	$0,   P
	subl	$16, $10, $13		# min_i = m - is
	mov	$29, $2			# a_offset = &sa[0][0]

	cmple	$13, $0,  $1		# $1 = (min_i > P)
	cmoveq	$1,  $0,  $13		# if (!$1) min_i = P
	sra	$14, 2, $8		# l = (min_l >> 2)
	addq	$20, $6, $15		# c_offst = a + ls*lda

	unop
	unop
	unop
	ble	$8,$L39
	.align	4

$L40:
	SXADDQ	$10, $15, $25		# c_offset1 = c_offset  + is
	mov	$2, $3			# a_offset1 = a_offset
	addq	$21, $21, $0
	mov	$13, $6			# i = min_i

	addq	$25, $21, $26		# c_offset2 = c_offset1 + lda
	s4addq	$21, $15, $15		# c_offset += 4*lda
	addq	$25, $0 , $27		# c_offset3 = c_offset2 + lda
	lda	$2,   16*SIZE($2)	# a_offset += 16

	addq	$27, $21, $at		# c_offset4 = c_offset3 + lda
	WH64(3)
	unop
	lda	$8,        -1($8)	# l --
	.align	4

	LD	$f25,  0*SIZE($25)	# ctemp1  = *(c_offset1 + 0)
	LD	$f16,  0*SIZE($26)	# ctemp5  = *(c_offset2 + 0)
	LD	$f1,   0*SIZE($27)	# ctemp9  = *(c_offset3 + 0)
	LD	$f20,  0*SIZE($at)	# ctemp13 = *(c_offset4 + 0)

	LD	$f29,  1*SIZE($25)	# ctemp2  = *(c_offset1 + 1)
	LD	$f30,  2*SIZE($25)	# ctemp3  = *(c_offset1 + 2)
	LD	$f21,  3*SIZE($25)	# ctemp4  = *(c_offset1 + 3)
	LD	$f7,   1*SIZE($26)	# ctemp6  = *(c_offset2 + 1)

	LD	$f3,   2*SIZE($26)	# ctemp7  = *(c_offset2 + 2)
	LD	$f5,   3*SIZE($26)	# ctemp8  = *(c_offset2 + 3)
	LD	$f2,   1*SIZE($27)	# ctemp10 = *(c_offset3 + 1)
	LD	$f4,   2*SIZE($27)	# ctemp11 = *(c_offset3 + 2)

	LD	$f6,   3*SIZE($27)	# ctemp12 = *(c_offset3 + 3)
	lda	$6,        -4($6)	# i -= 4
	LD	$f18,  1*SIZE($at)	# ctemp14 = *(c_offset4 + 1)
	lda	$25,   4*SIZE($25)	# c_offset1 += 4

	LD	$f17,  2*SIZE($at)	# ctemp15 = *(c_offset4 + 2)
	lda	$26,   4*SIZE($26)	# c_offset2 += 4
	LD	$f0,   3*SIZE($at)	# ctemp16 = *(c_offset4 + 3)
	lda	$27,   4*SIZE($27)	# c_offset3 += 4

	lda	$at,   4*SIZE($at)	# c_offset4 += 4
	lda	$4,     LDA*WHSIZE   *SIZE($3)
	lda	$5,    (LDA*WHSIZE+8)*SIZE($3)
	ble	$6, $L45
	.align 4

$L43:
	ST	$f25,  0*SIZE($3)	# *(a_offset1 +  0) = ctemp1
	LD	$f25,  0*SIZE($25)	# ctemp1  = *(c_offset1 + 0)
	ST	$f29,  1*SIZE($3)	# *(a_offset1 +  1) = ctemp2
	LD	$f29,  1*SIZE($25)	# ctemp2  = *(c_offset1 + 1)

	ST	$f30,  2*SIZE($3)	# *(a_offset1 +  2) = ctemp3
	LD	$f30,  2*SIZE($25)	# ctemp3  = *(c_offset1 + 2)
	ST	$f21,  3*SIZE($3)	# *(a_offset1 +  3) = ctemp4
	LD	$f21,  3*SIZE($25)	# ctemp4  = *(c_offset1 + 3)

	ST	$f16,  4*SIZE($3)	# *(a_offset1 +  4) = ctemp5
	LD	$f16,  0*SIZE($26)	# ctemp5  = *(c_offset2 + 0)
	ST	$f7,   5*SIZE($3)	# *(a_offset1 +  5) = ctemp6
	LD	$f7,   1*SIZE($26)	# ctemp6  = *(c_offset2 + 1)

	ST	$f3,   6*SIZE($3)	# *(a_offset1 +  6) = ctemp7
	LD	$f3,   2*SIZE($26)	# ctemp7  = *(c_offset2 + 2)
	ST	$f5,   7*SIZE($3)	# *(a_offset1 +  7) = ctemp8
	LD	$f5,   3*SIZE($26)	# ctemp8  = *(c_offset2 + 3)

	ST	$f1,   8*SIZE($3)	# *(a_offset1 +  8) = ctemp9
	LD	$f1,   0*SIZE($27)	# ctemp9  = *(c_offset3 + 0)
	ST	$f2,   9*SIZE($3)	# *(a_offset1 +  9) = ctemp10
	LD	$f2,   1*SIZE($27)	# ctemp10 = *(c_offset3 + 1)

	ST	$f4,  10*SIZE($3)	# *(a_offset1 + 10) = ctemp11
	LD	$f4,   2*SIZE($27)	# ctemp11 = *(c_offset3 + 2)
	ST	$f6,  11*SIZE($3)	# *(a_offset1 + 11) = ctemp12
	LD	$f6,   3*SIZE($27)	# ctemp12 = *(c_offset3 + 3)

	ST	$f20, 12*SIZE($3)	# *(a_offset1 + 12) = ctemp13
	LD	$f20,  0*SIZE($at)	# ctemp13 = *(c_offset4 + 0)
	ST	$f18, 13*SIZE($3)	# *(a_offset1 + 13) = ctemp14
	LD	$f18,  1*SIZE($at)	# ctemp14 = *(c_offset4 + 1)

	ST	$f17, 14*SIZE($3)	# *(a_offset1 + 14) = ctemp15
	LD	$f17,  2*SIZE($at)	# ctemp15 = *(c_offset4 + 2)
	ST	$f0,  15*SIZE($3)	# *(a_offset1 + 15) = ctemp16
	LD	$f0,   3*SIZE($at)	# ctemp16 = *(c_offset4 + 3)

	WH64(4)
	lda	$6,        -4($6)	# i -= 4
	ldl	$31,  COPY_PREFETCH_SIZE*SIZE($25)
	lda	$25,   4*SIZE($25)	# c_offset1 += 4

#ifdef DGEMM
	WH64(5)
#else
	unop
#endif
	lda	$3,  LDA*SIZE($3)	# a_offset1 += LDA
	ldl	$31,  COPY_PREFETCH_SIZE*SIZE($26)
	lda	$26,   4*SIZE($26)	# c_offset2 += 4

	ldl	$31,  COPY_PREFETCH_SIZE*SIZE($27)
	lda	$27,   4*SIZE($27)	# c_offset3 += 4
	ldl	$31,  COPY_PREFETCH_SIZE*SIZE($at)
	lda	$at,   4*SIZE($at)	# c_offset4 += 4

	lda	$4,     LDA * SIZE($4)
	lda	$5,     LDA * SIZE($5)
	unop
	bgt	$6, $L43
	.align 4

$L45:
	ST	$f25,  0*SIZE($3)	# *(a_offset1 +  0) = ctemp1
	ST	$f29,  1*SIZE($3)	# *(a_offset1 +  1) = ctemp2
	ST	$f30,  2*SIZE($3)	# *(a_offset1 +  2) = ctemp3
	ST	$f21,  3*SIZE($3)	# *(a_offset1 +  3) = ctemp4

	ST	$f16,  4*SIZE($3)	# *(a_offset1 +  4) = ctemp5
	ST	$f7,   5*SIZE($3)	# *(a_offset1 +  5) = ctemp6
	ST	$f3,   6*SIZE($3)	# *(a_offset1 +  6) = ctemp7
	ST	$f5,   7*SIZE($3)	# *(a_offset1 +  7) = ctemp8

	ST	$f1,   8*SIZE($3)	# *(a_offset1 +  8) = ctemp9
	ST	$f2,   9*SIZE($3)	# *(a_offset1 +  9) = ctemp10
	ST	$f4,  10*SIZE($3)	# *(a_offset1 + 10) = ctemp11
	ST	$f6,  11*SIZE($3)	# *(a_offset1 + 11) = ctemp12

	ST	$f20, 12*SIZE($3)	# *(a_offset1 + 12) = ctemp13
	ST	$f18, 13*SIZE($3)	# *(a_offset1 + 13) = ctemp14
	ST	$f17, 14*SIZE($3)	# *(a_offset1 + 14) = ctemp15
	ST	$f0,  15*SIZE($3)	# *(a_offset1 + 15) = ctemp16

	bgt	$8, $L40
	.align 4

$L39:
	and	$14, 3, $8		# l = (min_l & 3)
	mull	$11, $24, $1		# js * ldc
	fclr	$f4			# ctemp11 = ZERO
	ble	$8,$L50
	.align	4

$L51:
	mov	$2, $3			# a_offset1 = a_offset
	lda	$2,    4*SIZE($2)	# a_offset += 4
	SXADDQ	$10, $15, $25		# c_offset1 = c_offset + ls
	addq	$15, $21, $15		# c_offset += lda

	mov	$13, $6			# i = min_i
	unop
	unop
	lda	$8,        -1($8)	# l --
	.align	4

$L54:
	LD	$f25,  0*SIZE($25)	# ctemp1  = *(c_offset1 + 0)
	LD	$f29,  1*SIZE($25)	# ctemp2  = *(c_offset1 + 1)
	LD	$f30,  2*SIZE($25)	# ctemp3  = *(c_offset1 + 2)
	LD	$f21,  3*SIZE($25)	# ctemp4  = *(c_offset1 + 3)

	lda	$25,   4*SIZE($25)	# c_offset1 += 4
	lda	$6,        -4($6)	# i -= 4
	lda	$3,  LDA*SIZE($3)	# a_offset1 += LDA
	nop

	ST	$f25,  (0-LDA)*SIZE($3)	# *(a_offset1 +  0) = ctemp1
	ST	$f29,  (1-LDA)*SIZE($3)	# *(a_offset1 +  1) = ctemp2
	ST	$f30,  (2-LDA)*SIZE($3)	# *(a_offset1 +  2) = ctemp3
	ST	$f21,  (3-LDA)*SIZE($3)	# *(a_offset1 +  3) = ctemp4

	bgt	$6, $L54
	unop
	fclr	$f4			# ctemp11 = ZERO
	bgt	$8,  $L51
	.align 4

#else /* defined(TN) || defined(TT) */
	mull	$21, $10, $8		# is * lda
	lda	$0,   P
	subl	$16, $10, $13		# min_i = m - is
	mov	$29, $2			# a_offset = &sa[0][0]

	cmple	$13, $0,  $1		# $1 = (min_i > P)
	cmoveq	$1,  $0,  $13		# if (!$1) min_i = P
	sra	$13, 2, $6		# i = (min_i >> 2)
	addq	$20, $8,  $15		# c_offst = a + is*lda

	unop
	unop	
	unop
	ble	$6, $L39
	.align	4

$L40:
	SXADDQ	$9,  $15, $25		# c_offset1 = c_offset  + ls
	mov	$2, $3			# a_offset1 = a_offset
	lda	$2,  LDA*SIZE($2)	# a_offset += LDA
	addq	$21, $21, $0

	addq	$25, $21, $26		# c_offset2 = c_offset1 + lda
	addq	$25, $0,  $27		# c_offset3 = c_offset2 + lda
	mov	$14, $8			# l = min_l
	lda	$6,        -1($6)	# i --

	addq	$26, $0,  $at		# c_offset4 = c_offset3 + lda
	lda	$8,        -4($8)	# l -= 4
	lda	$4,      8*WHSIZE*SIZE($3)
	lda	$5,      8*(WHSIZE+1)*SIZE($3)

	LD	$f25,  0*SIZE($25)	# ctemp1  = *(c_offset1 + 0)
	LD	$f29,  1*SIZE($25)	# ctemp2  = *(c_offset1 + 1)
	LD	$f30,  2*SIZE($25)	# ctemp3  = *(c_offset1 + 2)
	LD	$f21,  3*SIZE($25)	# ctemp4  = *(c_offset1 + 3)

	LD	$f16,  0*SIZE($26)	# ctemp5  = *(c_offset2 + 0)
	LD	$f7,   1*SIZE($26)	# ctemp6  = *(c_offset2 + 1)
	LD	$f3,   2*SIZE($26)	# ctemp7  = *(c_offset2 + 2)
	LD	$f5,   3*SIZE($26)	# ctemp8  = *(c_offset2 + 3)

	LD	$f1,   0*SIZE($27)	# ctemp9  = *(c_offset3 + 0)
	LD	$f2,   1*SIZE($27)	# ctemp10 = *(c_offset3 + 1)
	LD	$f4,   2*SIZE($27)	# ctemp11 = *(c_offset3 + 2)
	LD	$f6,   3*SIZE($27)	# ctemp12 = *(c_offset3 + 3)

	LD	$f20,  0*SIZE($at)	# ctemp13 = *(c_offset4 + 0)
	LD	$f18,  1*SIZE($at)	# ctemp14 = *(c_offset4 + 1)
	LD	$f17,  2*SIZE($at)	# ctemp15 = *(c_offset4 + 2)
	LD	$f0,   3*SIZE($at)	# ctemp16 = *(c_offset4 + 3)

	WH64(4)
	lda	$25,   4*SIZE($25)	# c_offset1 += 4
	lda	$26,   4*SIZE($26)	# c_offset2 += 4
	lda	$27,   4*SIZE($27)	# c_offset3 += 4

#ifdef DGEMM
	WH64(5)
#else
	unop
#endif
	lda	$at,   4*SIZE($at)	# c_offset4 += 4
	s4addq	$21, $15, $15		# c_offset += 4*lda
	ble	$8, $L46
	.align 4

$L43:
	ST	$f25,  0*SIZE($3)	# *(a_offset1 +  0) = ctemp1
	LD	$f25,  0*SIZE($25)	# ctemp1  = *(c_offset1 + 0)
	ST	$f16,  1*SIZE($3)	# *(a_offset1 +  1) = ctemp2
	LD	$f16,  0*SIZE($26)	# ctemp5  = *(c_offset2 + 0)

	ST	$f1,   2*SIZE($3)	# *(a_offset1 +  2) = ctemp3
	LD	$f1,   0*SIZE($27)	# ctemp9  = *(c_offset3 + 0)
	ST	$f20,  3*SIZE($3)	# *(a_offset1 +  3) = ctemp4
	LD	$f20,  0*SIZE($at)	# ctemp13 = *(c_offset4 + 0)

	ST	$f29,  4*SIZE($3)	# *(a_offset1 +  4) = ctemp5
	LD	$f29,  1*SIZE($25)	# ctemp2  = *(c_offset1 + 1)
	ST	$f7,   5*SIZE($3)	# *(a_offset1 +  5) = ctemp6
	LD	$f7,   1*SIZE($26)	# ctemp6  = *(c_offset2 + 1)

	ST	$f2,   6*SIZE($3)	# *(a_offset1 +  6) = ctemp7
	LD	$f2,   1*SIZE($27)	# ctemp10 = *(c_offset3 + 1)
	ST	$f18,  7*SIZE($3)	# *(a_offset1 +  7) = ctemp8
	LD	$f18,  1*SIZE($at)	# ctemp14 = *(c_offset4 + 1)

	ST	$f30,  8*SIZE($3)	# *(a_offset1 +  8) = ctemp9
	LD	$f30,  2*SIZE($25)	# ctemp3  = *(c_offset1 + 2)
	ST	$f3,   9*SIZE($3)	# *(a_offset1 +  9) = ctemp10
	LD	$f3,   2*SIZE($26)	# ctemp7  = *(c_offset2 + 2)

	ST	$f4,  10*SIZE($3)	# *(a_offset1 + 10) = ctemp11
	LD	$f4,   2*SIZE($27)	# ctemp11 = *(c_offset3 + 2)
	ST	$f17, 11*SIZE($3)	# *(a_offset1 + 11) = ctemp12
	LD	$f17,  2*SIZE($at)	# ctemp15 = *(c_offset4 + 2)

	ST	$f21, 12*SIZE($3)	# *(a_offset1 + 12) = ctemp13
	LD	$f21,  3*SIZE($25)	# ctemp4  = *(c_offset1 + 3)
	ST	$f5,  13*SIZE($3)	# *(a_offset1 + 13) = ctemp14
	LD	$f5,   3*SIZE($26)	# ctemp8  = *(c_offset2 + 3)

	ST	$f6,  14*SIZE($3)	# *(a_offset1 + 14) = ctemp15
	LD	$f6,   3*SIZE($27)	# ctemp12 = *(c_offset3 + 3)
	ST	$f0,  15*SIZE($3)	# *(a_offset1 + 15) = ctemp16
	LD	$f0,   3*SIZE($at)	# ctemp16 = *(c_offset4 + 3)

	WH64(4)
	lda	$8,        -4($8)	# l -= 4
	ldl	$31,  COPY_PREFETCH_SIZE*SIZE($25)
	lda	$25,   4*SIZE($25)	# c_offset1 += 4

#ifdef DGEMM
	WH64(5)
#else
	unop
#endif
	lda	$3,   16*SIZE($3)	# a_offset1 += 16
	ldl	$31,  COPY_PREFETCH_SIZE*SIZE($26)
	lda	$26,   4*SIZE($26)	# c_offset2 += 4

	ldl	$31,  COPY_PREFETCH_SIZE*SIZE($27)
	lda	$4,    16*SIZE($4)
	lda	$27,   4*SIZE($27)	# c_offset3 += 4
	lda	$5,    16*SIZE($5)

	ldl	$31,  COPY_PREFETCH_SIZE*SIZE($at)
	lda	$at,   4*SIZE($at)	# c_offset4 += 4
	unop
	bgt	$8, $L43
	.align 4

$L46:
	ST	$f25,  0*SIZE($3)	# *(a_offset1 +  0) = ctemp1
	ST	$f16,  1*SIZE($3)	# *(a_offset1 +  1) = ctemp2
	ST	$f1,   2*SIZE($3)	# *(a_offset1 +  2) = ctemp3
	ST	$f20,  3*SIZE($3)	# *(a_offset1 +  3) = ctemp4

	ST	$f29,  4*SIZE($3)	# *(a_offset1 +  4) = ctemp5
	ST	$f7,   5*SIZE($3)	# *(a_offset1 +  5) = ctemp6
	ST	$f2,   6*SIZE($3)	# *(a_offset1 +  6) = ctemp7
	ST	$f18,  7*SIZE($3)	# *(a_offset1 +  7) = ctemp8

	ST	$f30,  8*SIZE($3)	# *(a_offset1 +  8) = ctemp9
	ST	$f3,   9*SIZE($3)	# *(a_offset1 +  9) = ctemp10
	ST	$f4,  10*SIZE($3)	# *(a_offset1 + 10) = ctemp11
	ST	$f17, 11*SIZE($3)	# *(a_offset1 + 11) = ctemp12

	ST	$f21, 12*SIZE($3)	# *(a_offset1 + 12) = ctemp13
	ST	$f5,  13*SIZE($3)	# *(a_offset1 + 13) = ctemp14
	ST	$f6,  14*SIZE($3)	# *(a_offset1 + 14) = ctemp15
	ST	$f0,  15*SIZE($3)	# *(a_offset1 + 15) = ctemp16
	lda	$3,   16*SIZE($3)	# a_offset1 += 16
	bgt	$6,$L40
	.align 4

$L39:
	and	$13, 3, $6		# i = (min_i & 3)
	mull	$11, $24, $1		# js * ldc
	fclr	$f4			# ctemp11 = ZERO
	ble	$6,$L50
	.align	4

$L51:
	mov	$2, $3			# a_offset1 = a_offset
	lda	$2,    1*SIZE($2)	# a_offset ++
	SXADDQ	$9,  $15, $25		# c_offset1 = c_offset + ls
	addq	$15, $21, $15		# c_offset += lda

	mov	$14, $8			# l = min_l
	unop
	unop
	lda	$6,        -1($6)	# i --
	.align	4

$L54:
	LD	$f25,  0*SIZE($25)	# ctemp1  = *(c_offset1 + 0)
	LD	$f29,  1*SIZE($25)	# ctemp2  = *(c_offset1 + 1)
	LD	$f30,  2*SIZE($25)	# ctemp3  = *(c_offset1 + 2)
	LD	$f21,  3*SIZE($25)	# ctemp4  = *(c_offset1 + 3)

	lda	$25,   4*SIZE($25)	# c_offset1 += 4
	lda	$8,        -4($8)	# l -= 4
	lda	$3,   16*SIZE($3)	# a_offset1 += 16
	nop

	ST	$f25, -16*SIZE($3)	# *(a_offset1 +  0) = ctemp1
	ST	$f29, -12*SIZE($3)	# *(a_offset1 +  4) = ctemp2
	ST	$f30,  -8*SIZE($3)	# *(a_offset1 +  8) = ctemp3
	ST	$f21,  -4*SIZE($3)	# *(a_offset1 + 12) = ctemp4

	bgt	$8, $L54
	unop
	fclr	$f4			# ctemp11 = ZERO
	bgt	$6,  $L51
	.align 4

#endif

/* Main Routine */
$L50:
	SXADDQ	$10, $23, $0		# $0 = c + is
	fclr	$f20			# ctemp13 = ZERO
	ldah	$4,  Address_H(SB_ADDRESS)($29)	# a_offset = &sb[0][0]
	fclr	$f10			# temp1   = ZERO

	sra	$12, 2, $7		# j = (min_j >> 2)
	fclr	$f1			# ctemp9  = ZERO
	lda	$4,  Address_L(SB_ADDRESS)($4)
	fclr	$f2			# ctemp10 = ZERO

	addq	$1, $0, $15		# c_offset = c + is + js*ldc
#ifndef DEBUG
	ftoit	$f19, $0
#else
	stt	$f19, 136($30)
#endif
	fclr	$f6			# ctemp12 = ZERO
	ble	$7,$L61			# if (j <=0) goto $L61
	.align 4
	
$L62:
	sra	$13,   2, $6		# i = (min_i >> 2)
	fclr	$f25			# ctemp1  = ZERO
	addq	$15, $24, $26		# c_offset2 = c_offset1 + ldc
	fclr	$f29			# ctemp2  = ZERO

	mov	$29, $2			# a_offset = &sa[0][0]
	fclr	$f30			# ctemp3  = ZERO
	addq	$26, $24, $27		# c_offset3 = c_offset2 + ldc
	fclr	$f21			# ctemp4  = ZERO

	mov	$15, $25		# c_offset1 = c_offset
	fclr	$f16			# ctemp5  = ZERO
	addq	$27, $24, $at		# c_offset4 = c_offset3 + ldc
	fclr	$f7			# ctemp6  = ZERO

	s4addq	$24, $15, $15		# c_offset  = c_offset4 + ldc
	fclr	$f3			# ctemp7  = ZERO
	fclr	$f5			# ctemp8  = ZERO
	ble	$6, $L65
	.align 4

$L66:
	LD	$f15,  0*SIZE($2)	# atemp1 = *(a_offset1 + 0)
	lda	$8,        -2($14)	# l = min_l - 2
	LD	$f14,  0*SIZE($4)	# btemp1 = *(b_offset1 + 0)
	lda	$5,    4*SIZE($4)	# b_offset1 += 4

	LD	$f26,  1*SIZE($2)	# atemp2 = *(a_offset1 + 1)
	fclr	$f18			# ctemp14 = ZERO
	LD	$f27,  2*SIZE($2)	# atemp3 = *(a_offset1 + 2)
	fclr	$f12			# temp2   = ZERO

	LD	$f28,  3*SIZE($2)	# atemp4 = *(a_offset1 + 3)
	fclr	$f17			# ctemp15 = ZERO
	LD	$f22,  1*SIZE($4)	# btemp2 = *(b_offset1 + 1)
	fclr	$f11			# temp3   = ZERO

	LD	$f23,  2*SIZE($4)	# btemp3 = *(b_offset1 + 2)
	fclr	$f0			# ctemp16 = ZERO
	LD	$f24,  3*SIZE($4)	# btemp4 = *(b_offset1 + 3)
	fclr	$f13			# temp4   = ZERO

	lda	$3,    4*SIZE($2)	# a_offset1 = a_offset + 4
 	ldl	$31,   8*SIZE($25)
	ldl	$31,   8*SIZE($26)
	ble	$8, $L69
	.align	4

$L70:
/*  1 */
	ADD	$f20, $f10, $f20	# ctemp13 = ctemp13 + temp1
#ifdef DGEMM
	ldq	$31, 16*SIZE($3)
#else
	ldq	$31, 40*SIZE($3)
#endif
	MUL	$f14, $f15, $f10	# temp1   = btemp1 * atemp1	
#ifdef DGEMM
	ldt	$f31, 28*SIZE($5)
#else
	ldt	$f31, 44*SIZE($5)
#endif

	ADD	$f18, $f12, $f18	# ctemp14 = ctemp14 + temp2
	unop
	MUL	$f14, $f26, $f12	# temp2   = btemp1 * atemp2
	LD	$f8,   0*SIZE($3)	# atemp1 = *(a_offset1 + 0)

	ADD	$f17, $f11, $f17	# ctemp15 = ctemp15 + temp3
	unop
	MUL	$f14, $f27, $f11	# temp3   = btemp1 * atemp3
	LD	$f19,  0*SIZE($5)	# btemp1 = *(b_offset1 + 0)

	ADD	$f0,  $f13, $f0 	# ctemp16 = ctemp16 + temp4
	unop
	MUL	$f14, $f28, $f13	# temp4   = btemp1 * atemp4
	unop

/*  2 */
	ADD	$f25, $f10, $f25	# ctemp1  = ctemp1  + temp1
	MUL	$f22, $f15, $f10	# temp1   = btemp2 * atemp1
	ADD	$f29, $f12, $f29	# ctemp2  = ctemp2  + temp2
	MUL	$f22, $f26, $f12	# temp2   = btemp2 * atemp2

	ADD	$f30, $f11, $f30	# ctemp3  = ctemp3  + temp3
	unop
	MUL	$f22, $f27, $f11	# temp3   = btemp2 * atemp3
	unop

	ADD	$f21, $f13, $f21	# ctemp4  = ctemp4  + temp4
	unop
	MUL	$f22, $f28, $f13	# temp4   = btemp2 * atemp4
	LD	$f22,  1*SIZE($5)	# btemp2 = *(b_offset1 + 1)

/*  3 */
	ADD	$f16, $f10, $f16	# ctemp5  = ctemp5  + temp1
	MUL	$f23, $f15, $f10	# temp1   = btemp3 * atemp1
	ADD	$f7,  $f12, $f7		# ctemp6  = ctemp6  + temp2
	MUL	$f23, $f26, $f12	# temp2   = btemp3 * atemp2

	ADD	$f3,  $f11, $f3 	# ctemp7  = ctemp7  + temp3
	unop
	MUL	$f23, $f27, $f11	# temp3   = btemp3 * atemp3
	unop

	ADD	$f5,  $f13, $f5 	# ctemp8  = ctemp8  + temp4
	unop
	MUL	$f23, $f28, $f13	# temp4   = btemp3 * atemp4
	LD	$f23,  2*SIZE($5)	# btemp3 = *(b_offset1 + 2)

/*  4 */
	ADD	$f1,  $f10, $f1 	# ctemp9  = ctemp5  + temp1
	unop
	MUL	$f24, $f15, $f10	# temp1   = btemp4 * atemp1
	unop

	ADD	$f2,  $f12, $f2 	# ctemp10 = ctemp6  + temp2
	unop
	MUL	$f24, $f26, $f12	# temp2   = btemp4 * atemp2
	LD	$f26,  1*SIZE($3)	# atemp2 = *(a_offset1 + 1)

	ADD	$f4,  $f11, $f4 	# ctemp11 = ctemp7  + temp3
	unop
	MUL	$f24, $f27, $f11	# temp3   = btemp4 * atemp3
	LD	$f27,  2*SIZE($3)	# atemp3 = *(a_offset1 + 2)

	ADD	$f6,  $f13, $f6 	# ctemp12 = ctemp8  + temp4
	unop
	MUL	$f24, $f28, $f13	# temp4   = btemp4 * atemp4
	LD	$f28,  3*SIZE($3)	# atemp4 = *(a_offset1 + 3)

/*  5 */
	ADD	$f20, $f10, $f20	# ctemp13 = ctemp13 + temp1
	unop
	MUL	$f19, $f8,  $f10	# temp1   = btemp1 * atemp1
	LD	$f9,   3*SIZE($5)	# btemp4 = *(b_offset1 + 3)

	ADD	$f18, $f12, $f18	# ctemp14 = ctemp14 + temp2
	MUL	$f19, $f26, $f12	# temp2   = btemp1 * atemp2
	ADD	$f17, $f11, $f17	# ctemp15 = ctemp15 + temp3
	MUL	$f19, $f27, $f11	# temp3   = btemp1 * atemp3

	ADD	$f0,  $f13, $f0 	# ctemp16 = ctemp16 + temp4
	LD	$f15,  4*SIZE($3)	# atemp1 = *(a_offset1 + 4)
	MUL	$f19, $f28, $f13	# temp4   = btemp1 * atemp4
	LD	$f14,  4*SIZE($5)	# btemp1 = *(b_offset1 + 4)

/*  6 */
	ADD	$f25, $f10, $f25	# ctemp1  = ctemp1  + temp1
	MUL	$f22, $f8,  $f10	# temp1   = btemp2 * atemp1
	ADD	$f29, $f12, $f29	# ctemp2  = ctemp2  + temp2
	MUL	$f22, $f26, $f12	# temp2   = btemp2 * atemp2

	ADD	$f30, $f11, $f30	# ctemp3  = ctemp3  + temp3
	unop
	MUL	$f22, $f27, $f11	# temp3   = btemp2 * atemp3
	lda	$8,        -2($8)

	ADD	$f21, $f13, $f21	# ctemp4  = ctemp4  + temp4
	unop
	MUL	$f22, $f28, $f13	# temp4   = btemp2 * atemp4
	LD	$f22,  5*SIZE($5)	# btemp2 = *(b_offset1 + 5)

/*  7 */
	ADD	$f16, $f10, $f16	# ctemp5  = ctemp5  + temp1
	MUL	$f23, $f8,  $f10	# temp1   = btemp3 * atemp1
	ADD	$f7,  $f12, $f7		# ctemp6  = ctemp6  + temp2
	MUL	$f23, $f26, $f12	# temp2   = btemp3 * atemp2

	ADD	$f3,  $f11, $f3 	# ctemp7  = ctemp7  + temp3
	unop
	MUL	$f23, $f27, $f11	# temp3   = btemp3 * atemp3
	lda	$3,    8*SIZE($3)

	ADD	$f5,  $f13, $f5 	# ctemp8  = ctemp8  + temp4
	unop
	MUL	$f23, $f28, $f13	# temp4   = btemp3 * atemp4
	LD	$f23,  6*SIZE($5)	# btemp3 = *(b_offset1 + 6)

/*  8 */
	ADD	$f1,  $f10, $f1 	# ctemp9  = ctemp5  + temp1
	unop
	MUL	$f9,  $f8,  $f10	# temp1   = btemp4 * atemp1
	LD	$f24,  7*SIZE($5)	# btemp4 = *(b_offset1 + 7)

	ADD	$f2,  $f12, $f2 	# ctemp10 = ctemp6  + temp2
	unop
	MUL	$f9,  $f26, $f12	# temp2   = btemp4 * atemp2
	LD	$f26, -3*SIZE($3)	# atemp2 = *(a_offset1 + 5)

	ADD	$f4,  $f11, $f4 	# ctemp11 = ctemp7  + temp3
	lda	$5,    8*SIZE($5)
	MUL	$f9,  $f27, $f11	# temp3   = btemp4 * atemp3
	LD	$f27, -2*SIZE($3)	# atemp3 = *(a_offset1 + 6)

	ADD	$f6,  $f13, $f6 	# ctemp12 = ctemp8  + temp4
	MUL	$f9,  $f28, $f13	# temp4   = btemp4 * atemp4
	LD	$f28, -1*SIZE($3)	# atemp4 = *(a_offset1 + 7)
	bgt	$8,$L70
	.align 4

$L69:
	ADD	$f20, $f10, $f20	# ctemp13 = ctemp13 + temp1
	unop
	MUL	$f14, $f15, $f10	# temp1   = btemp1 * atemp1
	blbs	$14, $L75
	.align 4

/*  1 */
	ADD	$f18, $f12, $f18	# ctemp14 = ctemp14 + temp2
	lds	$f31,  8*SIZE($27)
	MUL	$f14, $f26, $f12	# temp2   = btemp1 * atemp2
	lds	$f31,  8*SIZE($at)

	ADD	$f17, $f11, $f17	# ctemp15 = ctemp15 + temp3
	unop
	MUL	$f14, $f27, $f11	# temp3   = btemp1 * atemp3
	unop

	ADD	$f0,  $f13, $f0 	# ctemp16 = ctemp16 + temp4
	unop
	MUL	$f14, $f28, $f13	# temp4   = btemp1 * atemp4
	LD	$f14,  0*SIZE($5)	# btemp1 = *(b_offset1 + 0)

/*  2 */
	ADD	$f25, $f10, $f25	# ctemp1  = ctemp1  + temp1
	MUL	$f22, $f15, $f10	# temp1   = btemp2 * atemp1
	ADD	$f29, $f12, $f29	# ctemp2  = ctemp2  + temp2
	MUL	$f22, $f26, $f12	# temp2   = btemp2 * atemp2

	ADD	$f30, $f11, $f30	# ctemp3  = ctemp3  + temp3
	unop
	MUL	$f22, $f27, $f11	# temp3   = btemp2 * atemp3
	unop

	ADD	$f21, $f13, $f21	# ctemp4  = ctemp4  + temp4
	unop
	MUL	$f22, $f28, $f13	# temp4   = btemp2 * atemp4
	LD	$f22,  1*SIZE($5)	# btemp2 = *(b_offset1 + 1)

/*  3 */
	ADD	$f16, $f10, $f16	# ctemp5  = ctemp5  + temp1
	unop
	MUL	$f23, $f15, $f10	# temp1   = btemp3 * atemp1
	unop

	ADD	$f7,  $f12, $f7		# ctemp6  = ctemp6  + temp2
	MUL	$f23, $f26, $f12	# temp2   = btemp3 * atemp2
	ADD	$f3,  $f11, $f3 	# ctemp7  = ctemp7  + temp3
	MUL	$f23, $f27, $f11	# temp3   = btemp3 * atemp3

	ADD	$f5,  $f13, $f5 	# ctemp8  = ctemp8  + temp4
	unop
	MUL	$f23, $f28, $f13	# temp4   = btemp3 * atemp4
	LD	$f23,  2*SIZE($5)	# btemp3 = *(b_offset1 + 2)

/*  4 */
	ADD	$f1,  $f10, $f1 	# ctemp9  = ctemp5  + temp1
	unop
	MUL	$f24, $f15, $f10	# temp1   = btemp4 * atemp1
	LD	$f15,  0*SIZE($3)	# atemp1 = *(a_offset1 + 0)

	ADD	$f2,  $f12, $f2 	# ctemp10 = ctemp6  + temp2
	unop
	MUL	$f24, $f26, $f12	# temp2   = btemp4 * atemp2
	LD	$f26,  1*SIZE($3)	# atemp2 = *(a_offset1 + 1)

	ADD	$f4,  $f11, $f4 	# ctemp11 = ctemp7  + temp3
	unop
	MUL	$f24, $f27, $f11	# temp3   = btemp4 * atemp3
	LD	$f27,  2*SIZE($3)	# atemp3 = *(a_offset1 + 2)

	ADD	$f6,  $f13, $f6 	# ctemp12 = ctemp8  + temp4
	unop
	MUL	$f24, $f28, $f13	# temp4   = btemp4 * atemp4
	LD	$f28,  3*SIZE($3)	# atemp4 = *(a_offset1 + 3)

	ADD	$f20, $f10, $f20	# ctemp13 = ctemp13 + temp1
	unop
	MUL	$f14, $f15, $f10	# temp1   = btemp1 * atemp1
	LD	$f24,  3*SIZE($5)	# btemp4 = *(b_offset1 + 3)
	.align 4

$L75:
	ADD	$f18, $f12, $f18	# ctemp14 = ctemp14 + temp2
	unop
	MUL	$f14, $f26, $f12	# temp2   = btemp1 * atemp2
#ifndef DEBUG
	itoft	$0, $f19
#else
	ldt	$f19, 136($30)
#endif

	ADD	$f17, $f11, $f17	# ctemp15 = ctemp15 + temp3
	MUL	$f14, $f27, $f11	# temp3   = btemp1 * atemp3
	ADD	$f0,  $f13, $f0 	# ctemp16 = ctemp16 + temp4
	MUL	$f14, $f28, $f13	# temp4   = btemp1 * atemp4

	ADD	$f25, $f10, $f25	# ctemp1  = ctemp1  + temp1
	unop
	MUL	$f22, $f15, $f10	# temp1   = btemp2 * atemp1
	LD	$f14,  0*SIZE($25)	# ctemp1  = *(c_offset1 + 0)

	ADD	$f29, $f12, $f29	# ctemp2  = ctemp2  + temp2
	MUL	$f22, $f26, $f12	# temp2   = btemp2 * atemp2
	ADD	$f30, $f11, $f30	# ctemp3  = ctemp3  + temp3
	MUL	$f22, $f27, $f11	# temp3   = btemp2 * atemp3

	ADD	$f21, $f13, $f21	# ctemp4  = ctemp4  + temp4
	unop
	MUL	$f22, $f28, $f13	# temp4   = btemp2 * atemp4
	LD	$f22,  0*SIZE($26)	# ctemp5  = *(c_offset2 + 0)

	ADD	$f16, $f10, $f16	# ctemp5  = ctemp5  + temp1
	unop
	MUL	$f23, $f15, $f10	# temp1   = btemp3 * atemp1
	unop

	ADD	$f7,  $f12, $f7		# ctemp6  = ctemp6  + temp2
	MUL	$f23, $f26, $f12	# temp2   = btemp3 * atemp2
	ADD	$f3,  $f11, $f3 	# ctemp7  = ctemp7  + temp3
	MUL	$f23, $f27, $f11	# temp3   = btemp3 * atemp3

	ADD	$f5,  $f13, $f5 	# ctemp8  = ctemp8  + temp4
	unop
	MUL	$f23, $f28, $f13	# temp4   = btemp3 * atemp4
	LD	$f23,  0*SIZE($27)	# ctemp9  = *(c_offset3 + 0)

	ADD	$f1,  $f10, $f1 	# ctemp9  = ctemp5  + temp1
	unop
	MUL	$f24, $f15, $f10	# temp1   = btemp4 * atemp1
	unop

	ADD	$f2,  $f12, $f2 	# ctemp10 = ctemp6  + temp2
	MUL	$f24, $f26, $f12	# temp2   = btemp4 * atemp2
	ADD	$f4,  $f11, $f4 	# ctemp11 = ctemp7  + temp3
	MUL	$f24, $f27, $f11	# temp3   = btemp4 * atemp3

	ADD	$f6,  $f13, $f6 	# ctemp12 = ctemp8  + temp4
	unop
	MUL	$f24, $f28, $f13	# temp4   = btemp4 * atemp4
	LD	$f24,  0*SIZE($at)	# ctemp13 = *(c_offset4 + 0)

	ADD	$f20, $f10, $f20	# ctemp13 = ctemp13 + temp1
	unop
	MUL	$f19, $f25, $f10	# temp1   = alpha   * ctemp1
	unop
	
	ADD	$f18, $f12, $f18	# ctemp14 = ctemp14 + temp2
	unop
	MUL	$f19, $f29, $f12	# temp2   = alpha   * ctemp2
	LD	$f29,  1*SIZE($25)	# ctemp2  = *(c_offset1 + 1)

	ADD	$f17, $f11, $f17	# ctemp15 = ctemp15 + temp3
	unop
	MUL	$f19, $f30, $f11	# temp3   = alpha   * ctemp3
	LD	$f30,  2*SIZE($25)	# ctemp3  = *(c_offset1 + 2)

	ADD	$f0,  $f13, $f0 	# ctemp16 = ctemp16 + temp4
	unop
	MUL	$f19, $f21, $f13	# temp4   = alpha   * ctemp4
	LD	$f21,  3*SIZE($25)	# ctemp4  = *(c_offset1 + 3)

	ADD	$f14, $f10, $f25	# ctemp1  = ctemp1  + temp1
	unop
	MUL	$f19, $f16, $f10	# temp1   = alpha   * ctemp5
	unop

	ADD	$f29, $f12, $f29	# ctemp2  = ctemp2  + temp2
	unop
	MUL	$f19, $f7,  $f12	# temp2   = alpha   * ctemp6
	LD	$f7,   1*SIZE($26)	# ctemp6  = *(c_offset2 + 1)

	ADD	$f30, $f11, $f30	# ctemp3  = ctemp3  + temp3
	unop
	MUL	$f19, $f3,  $f11	# temp3   = alpha   * ctemp7
	LD	$f3,   2*SIZE($26)	# ctemp7  = *(c_offset2 + 2)

	ADD	$f21, $f13, $f21	# ctemp4  = ctemp4  + temp4
	unop
	MUL	$f19, $f5,  $f13	# temp4   = alpha   * ctemp8
	LD	$f5,   3*SIZE($26)	# ctemp8  = *(c_offset2 + 3)

	ADD	$f22, $f10, $f16	# ctemp5  = ctemp5  + temp1
	unop
	MUL	$f19, $f1,  $f10	# temp1   = alpha   * ctemp9
	unop

	ADD	$f7,  $f12, $f7		# ctemp6  = ctemp6  + temp2
	unop
	MUL	$f19, $f2,  $f12	# temp2   = alpha   * ctemp10
	LD	$f2,   1*SIZE($27)	# ctemp10 = *(c_offset3 + 1)

	ADD	$f3,  $f11, $f3 	# ctemp7  = ctemp7  + temp3
	ST	$f25,  0*SIZE($25)	# *(c_offset1 + 0) = ctemp1
	MUL	$f19, $f4,  $f11	# temp3   = alpha   * ctemp11
	LD	$f4,   2*SIZE($27)	# ctemp11 = *(c_offset3 + 2)

	ADD	$f5,  $f13, $f5 	# ctemp8  = ctemp8  + temp4
	ST	$f29,  1*SIZE($25)	# *(c_offset1 + 1) = ctemp2
	MUL	$f19, $f6,  $f13	# temp4   = alpha   * ctemp12
	LD	$f6,   3*SIZE($27)	# ctemp12 = *(c_offset3 + 3)

	ADD	$f23, $f10, $f1 	# ctemp9  = ctemp9  + temp1
	ST	$f30,  2*SIZE($25)	# *(c_offset1 + 2) = ctemp3
	MUL	$f19, $f20, $f10	# temp1   = alpha   * ctemp13
	lda	$27,   4*SIZE($27)	# c_offset += 4

	ADD	$f2,  $f12, $f2 	# ctemp10 = ctemp10 + temp2
	ST	$f21,  3*SIZE($25)	# *(c_offset1 + 3) = ctemp4
	MUL	$f19, $f18, $f12	# temp2   = alpha   * ctemp14
	LD	$f18,  1*SIZE($at)	# ctemp14 = *(c_offset4 + 1)

	ADD	$f4,  $f11, $f4 	# ctemp11 = ctemp11 + temp3
	ST	$f16,  0*SIZE($26)	# *(c_offset2 + 0) = ctemp5
	MUL	$f19, $f17, $f11	# temp3   = alpha   * ctemp15
	LD	$f17,  2*SIZE($at)	# ctemp15 = *(c_offset4 + 2)

	ADD	$f6,  $f13, $f6 	# ctemp12 = ctemp12 + temp4
	ST	$f7,   1*SIZE($26)	# *(c_offset2 + 1) = ctemp6
	MUL	$f19, $f0,  $f13	# temp4   = alpha   * ctemp16
	LD	$f0,   3*SIZE($at)	# ctemp16 = *(c_offset4 + 3)

	ADD	$f24, $f10, $f20 	# ctemp13 = ctemp13 + temp1
	ST	$f3,   2*SIZE($26)	# *(c_offset2 + 2) = ctemp7
	fclr	$f3			# ctemp7  = ZERO
	lda	$2,  LDA*SIZE($2)	# a_offset += LDA

	ADD	$f18, $f12, $f18 	# ctemp14 = ctemp14 + temp2
	ST	$f5,   3*SIZE($26)	# *(c_offset2 + 3) = ctemp8
	fclr	$f5			# ctemp8  = ZERO
	lda	$6,        -1($6)	# i--;

	ADD	$f17, $f11, $f17 	# ctemp15 = ctemp15 + temp3
	ST	$f1,  -4*SIZE($27)	# *(c_offset3 + 0) = ctemp9
	fclr	$f1			# ctemp9  = ZERO
	lda	$25,   4*SIZE($25)	# c_offset += 4

	ADD	$f0,  $f13, $f0 	# ctemp16 = ctemp16 + temp4
	ST	$f2,  -3*SIZE($27)	# *(c_offset3 + 1) = ctemp10
	fclr	$f2			# ctemp10 = ZERO
	lda	$at,   4*SIZE($at)	# c_offset += 4

	ST	$f4,  -2*SIZE($27)	# *(c_offset3 + 2) = ctemp11
	fclr	$f4			# ctemp11 = ZERO
	fclr	$f10			# temp1   = ZERO
	unop

	ST	$f6,  -1*SIZE($27)	# *(c_offset3 + 3) = ctemp12
	fclr	$f6			# ctemp12 = ZERO
	fclr	$f25			# ctemp1  = ZERO
	unop

	ST	$f20, -4*SIZE($at)	# *(c_offset4 + 0) = ctemp13
	fclr	$f20			# ctemp13 = ZERO
	fclr	$f29			# ctemp2  = ZERO
	unop

	ST	$f18, -3*SIZE($at)	# *(c_offset4 + 1) = ctemp14
	fclr	$f30			# ctemp3  = ZERO
	fclr	$f21			# ctemp4  = ZERO
	unop

	ST	$f17, -2*SIZE($at)	# *(c_offset4 + 2) = ctemp15
	fclr	$f16			# ctemp5  = ZERO
	fclr	$f7			# ctemp6  = ZERO
	lda	$26,   4*SIZE($26)	# c_offset += 4

	ST	$f0,  -1*SIZE($at)	# *(c_offset4 + 3) = ctemp16
	unop
	unop
	bgt	$6, $L66
	.align 4

$L65:
	and	$13, 3, $6		# i = (min_i & 3)
	unop
	unop
	ble	$6, $L78		# i = min_i & 3
	.align	4

$L79:
	LD	$f15,  0*SIZE($2)	# atemp1 = *(a_offset  + 0)
	fclr	$f10			# temp1   = 0
	LD	$f14,  0*SIZE($4)	# btemp1 = *(b_offset1 + 0)
	fclr	$f12			# temp2   = 0

	LD	$f22,  1*SIZE($4)	# btemp2 = *(b_offset1 + 1)
	fclr	$f11			# temp3   = 0
	LD	$f23,  2*SIZE($4)	# btemp3 = *(b_offset1 + 2)
	fclr	$f25			# ctemp1  = 0

	LD	$f24,  3*SIZE($4)	# btemp4 = *(b_offset1 + 3)
	fclr	$f13			# temp4   = 0
	lda	$3,    4*SIZE($2)	# a_offset1 = a_offset + 4
	fclr	$f29			# ctemp2  = 0

	lda	$2,    1*SIZE($2)	# a_offset  = a_offset + 1
	fclr	$f30			# ctemp3  = 0
	lda	$8,        -1($14)	# l = min_l - 1
	fclr	$f21			# ctemp4  = 0

	lda	$5,    4*SIZE($4)	# b_offset1 += 4
	unop
	unop
	ble	$8, $L82
	.align	4

$L83:
	ADD	$f25, $f10, $f25	# ctemp1  = ctemp1  + temp1
	lda	$8,        -1($8)
	MUL	$f14, $f15, $f10	# temp1   = btemp1 * atemp1
	LD	$f14,  0*SIZE($5)

	ADD	$f29, $f12, $f29	# ctemp2  = ctemp2  + temp2
	lda	$3,    4*SIZE($3)	# a_offset1 = a_offset + 4
	MUL	$f22, $f15, $f12	# temp2   = btemp2 * atemp1
	LD	$f22,  1*SIZE($5)

	ADD	$f30, $f11, $f30	# ctemp3  = ctemp3  + temp3
	unop
	MUL	$f23, $f15, $f11	# temp3   = btemp3 * atemp1
	LD	$f23,  2*SIZE($5)

	ADD	$f21, $f13, $f21	# ctemp4  = ctemp4  + temp4
	MUL	$f24, $f15, $f13	# temp4   = btemp4 * atemp1
	LD	$f15, -4*SIZE($3)	# atemp1 = *(a_offset  + 0)
	LD	$f24,  3*SIZE($5)

	lda	$5,    4*SIZE($5)	# b_offset1 += 4
	unop
	unop
	bgt	$8, $L83
	.align 4

$L82:
	ADD	$f25, $f10, $f25	# ctemp1  = ctemp1  + temp1
	MUL	$f14, $f15, $f10	# temp1   = btemp1 * atemp1
	ADD	$f29, $f12, $f29	# ctemp2  = ctemp2  + temp2
	MUL	$f22, $f15, $f12	# temp2   = btemp2 * atemp1

	ADD	$f30, $f11, $f30	# ctemp3  = ctemp3  + temp3
	LD	$f26,  0*SIZE($26)	# atemp2 = *(c_offset2 + 0)
	MUL	$f23, $f15, $f11	# temp3   = btemp3 * atemp1
	LD	$f27,  0*SIZE($27)	# atemp3 = *(c_offset3 + 0)

	ADD	$f21, $f13, $f21	# ctemp4  = ctemp4  + temp4
	LD	$f28,  0*SIZE($at)	# atemp4 = *(c_offset4 + 0)
	MUL	$f24, $f15, $f13	# temp4   = btemp4 * atemp1
	LD	$f15,  0*SIZE($25)	# atemp1 = *(c_offset1 + 0)

	ADD	$f25, $f10, $f25	# ctemp1  = ctemp1  + temp1
	ADD	$f29, $f12, $f29	# ctemp2  = ctemp2  + temp2
	ADD	$f30, $f11, $f30	# ctemp3  = ctemp3  + temp3
	ADD	$f21, $f13, $f21	# ctemp4  = ctemp4  + temp4

	MUL	$f19, $f25, $f10	# temp1   = alpha   * ctemp1
	MUL	$f19, $f29, $f12	# temp2   = alpha   * ctemp2
	MUL	$f19, $f30, $f11	# temp3   = alpha   * ctemp3
	MUL	$f19, $f21, $f13	# temp4   = alpha   * ctemp4

	ADD	$f10, $f15, $f10	# temp1   = atemp1  * temp1
	ADD	$f12, $f26, $f12	# temp2   = atemp2  * temp2
	ADD	$f11, $f27, $f11	# temp3   = atemp3  * temp3
	ADD	$f13, $f28, $f13	# temp4   = atemp4  * temp4

	ST	$f10,  0*SIZE($25)	# *(c_offset1 + 0) = temp1
	lda	$25,   1*SIZE($25)	# c_offset1 ++
	ST	$f12,  0*SIZE($26)	# *(c_offset2 + 0) = temp2
	lda	$26,   1*SIZE($26)	# c_offset2 ++

	ST	$f11,  0*SIZE($27)	# *(c_offset3 + 0) = temp3
	lda	$27,   1*SIZE($27)	# c_offset3 ++
	ST	$f13,  0*SIZE($at)	# *(c_offset4 + 0) = temp4
	lda	$6,        -1($6)	# i --

	lda	$at,   1*SIZE($at)	# c_offset4 ++
	unop
	unop
	bgt	$6, $L79
	.align 4

$L78:
	lda	$7,        -1($7)	# j --
	fclr	$f20			# ctemp13 = ZERO
	lda	$4,  LDA*SIZE($4)	# b_offset += LDA
	fclr	$f10			# temp1   = ZERO

	fclr	$f1			# ctemp9  = ZERO
	nop
	fclr	$f2			# ctemp10 = ZERO
	nop

	fclr	$f4			# ctemp11 = ZERO
	nop
	fclr	$f6			# ctemp12 = ZERO
	bgt	$7, $L62
	.align 4

$L61:
	and	$12, 3, $7		# j = (min_j & 3)
	unop
	unop
	ble	$7, $L36		# if (j>0)
	.align	4

$L93:
	sra	$13, 2, $6		# i = (min_i >> 2)
	mov	$15, $25
	mov	$29, $2			# a_offset = &sa[0][0]
	ble	$6, $L96
	.align	4

$L97:
	LD	$f15,  0*SIZE($2)	# atemp1 = *(a_offset1 + 0)
	fclr	$f25			# ctemp1 = ZERO
	LD	$f14,  0*SIZE($4)	# btemp1 = *(b_offset1 + 0)
	fclr	$f10			# temp1  = ZERO

	LD	$f26,  1*SIZE($2)	# atemp2 = *(a_offset1 + 1)
	fclr	$f29			# ctemp2 = ZERO
	LD	$f27,  2*SIZE($2)	# atemp3 = *(a_offset1 + 2)
	fclr	$f12			# temp2  = ZERO

	LD	$f28,  3*SIZE($2)	# atemp4 = *(a_offset1 + 3)
	fclr	$f30			# ctemp3 = ZERO
	lda	$8,        -1($14)	# l = min_l - 1
	fclr	$f21			# ctemp4 = ZERO

	lda	$3,    4*SIZE($2)	# a_offset1 += a_offset + 4
	fclr	$f11			# temp3  = ZERO
	lda	$2,  LDA*SIZE($2)	# a_offset += LDA
	fclr	$f13			# temp4  = ZERO

	lda	$5,    4*SIZE($4)	# b_offset1 += 4
	unop
	unop
	ble	$8, $L100
	.align	4

$L101:
	ADD	$f25, $f10, $f25	# ctemp1  = ctemp1  + temp1
	lda	$8,        -1($8)
	MUL	$f14, $f15, $f10	# temp1   = btemp1  * atemp1
	LD	$f15,  0*SIZE($3)	# atemp1 = *(a_offset1 + 0)

	ADD	$f29, $f12, $f29	# ctemp2  = ctemp2  + temp2
	lda	$5,    4*SIZE($5)	# b_offset1 += 4
	MUL	$f14, $f26, $f12	# temp2   = btemp1  * atemp2
	LD	$f26,  1*SIZE($3)	# atemp2 = *(a_offset1 + 1)

	ADD	$f30, $f11, $f30	# ctemp3  = ctemp3  + temp3
	unop
	MUL	$f14, $f27, $f11	# temp3   = btemp1  * atemp3
	LD	$f27,  2*SIZE($3)	# atemp3 = *(a_offset1 + 2)

	ADD	$f21, $f13, $f21	# ctemp4  = ctemp4  + temp4
	MUL	$f14, $f28, $f13	# temp4   = btemp1  * atemp4
	LD	$f28,  3*SIZE($3)	# atemp4 = *(a_offset1 + 3)
	LD	$f14, -4*SIZE($5)	# btemp1 = *(b_offset1 + 0)

	lda	$3,    4*SIZE($3)	# a_offset1 += 4
	unop
	unop
	bgt	$8, $L101
	.align 4

$L100:
	ADD	$f25, $f10, $f25	# ctemp1  = ctemp1  + temp1
	lda	$6,        -1($6)	# i --
	MUL	$f14, $f15, $f10	# temp1   = btemp1  * atemp1
	LD	$f15,  0*SIZE($25)	# atemp1 = *(a_offset1 + 0)

	ADD	$f29, $f12, $f29	# ctemp2  = ctemp2  + temp2
	unop
	MUL	$f14, $f26, $f12	# temp2   = btemp1  * atemp2
	LD	$f26,  1*SIZE($25)	# atemp2 = *(a_offset1 + 1)

	ADD	$f30, $f11, $f30	# ctemp3  = ctemp3  + temp3
	unop
	MUL	$f14, $f27, $f11	# temp3   = btemp1  * atemp3
	LD	$f27,  2*SIZE($25)	# atemp3 = *(a_offset1 + 2)

	ADD	$f21, $f13, $f21	# ctemp4  = ctemp4  + temp4
	unop
	MUL	$f14, $f28, $f13	# temp4   = btemp1  * atemp4
	LD	$f28,  3*SIZE($25)	# atemp4 = *(a_offset1 + 3)

	ADD	$f25, $f10, $f25	# ctemp1  = ctemp1  + temp1
	MUL	$f19, $f25, $f10	# temp1   = alpha   * ctemp1
	ADD	$f29, $f12, $f29	# ctemp2  = ctemp2  + temp2
	MUL	$f19, $f29, $f12	# temp2   = alpha   * ctemp2

	ADD	$f30, $f11, $f30	# ctemp3  = ctemp3  + temp3
	MUL	$f19, $f30, $f11	# temp3   = alpha   * ctemp3
	ADD	$f21, $f13, $f21	# ctemp4  = ctemp4  + temp4
	MUL	$f19, $f21, $f13	# temp4   = alpha   * ctemp4

	ADD	$f10, $f15, $f10	# temp1   = atemp1  + temp1
	ADD	$f12, $f26, $f12	# temp2   = atemp2  + temp2
	ADD	$f11, $f27, $f11	# temp3   = atemp3  + temp3
	ADD	$f13, $f28, $f13	# temp4   = atemp4  + temp4

	ST	$f10,  0*SIZE($25)	# *(a_offset1 + 0) = atemp1
	ST	$f12,  1*SIZE($25)	# *(a_offset1 + 1) = atemp2
	ST	$f11,  2*SIZE($25)	# *(a_offset1 + 2) = atemp3
	ST	$f13,  3*SIZE($25)	# *(a_offset1 + 3) = atemp4

	lda	$25,   4*SIZE($25)	# c_offset += 4
	unop
	unop
	bgt	$6, $L97
	.align 4

$L96:
	and	$13, 3, $6		# i = (min_i & 3)
	unop
	unop
	ble	$6, $L108		# if (i>0)
	.align	4

$L109:
	LD	$f15,  0*SIZE($2)	# atemp1 = *(a_offset1 + 0)
	fclr	$f25			# ctemp1 = ZERO
	LD	$f14,  0*SIZE($4)	# btemp1 = *(b_offset1 + 0)
	fclr	$f10			# temp1  = ZERO

	lda	$3,    4*SIZE($2)	# a_offset1 = a_offset + 4
	lda	$8,        -1($14)	# l = min_l - 1
	lda	$5,    4*SIZE($4)	# b_offset1 += 4
	ble	$8, $L112
	.align	4

$L113:
	ADD	$f25, $f10, $f25	# ctemp1  = ctemp1  + temp1
	MUL	$f14, $f15, $f10	# temp1   = btemp1  * atemp1
	LD	$f15,  0*SIZE($3)	# atemp1 = *(a_offset1 + 0)
	LD	$f14,  0*SIZE($5)	# btemp1 = *(b_offset1 + 0)

	lda	$8,        -1($8)
	lda	$3,    4*SIZE($3)	# a_offset1 += 4
	lda	$5,    4*SIZE($5)	# b_offset1 += 4
	bgt	$8, $L113
	.align 4

$L112:
	ADD	$f25, $f10, $f25	# ctemp1  = ctemp1  + temp1
	unop
	MUL	$f14, $f15, $f10	# temp1   = btemp1  * atemp1
	LD	$f15,  0*SIZE($25)	# atemp1 = *(c_offset1 + 0)

	ADD	$f25, $f10, $f25	# ctemp1  = ctemp1  + temp1
	lda	$25,   1*SIZE($25)	# c_offset1 ++
	MUL	$f19, $f25, $f10	# temp1   = alpha   * ctemp1
	lda	$6,        -1($6)	# i--

	ADD	$f10, $f15, $f10	# temp1   = atemp1  + temp1
	lda	$2,    1*SIZE($2)	# a_offset ++
	ST	$f10, -1*SIZE($25)	# *(c_offset1 + 0) = temp1
	bgt	$6, $L109
	.align 4

$L108:
	lda	$4,    1*SIZE($4)	# b_offset ++
	lda	$7,        -1($7)	# j --
	addq	$15, $24, $15
	bgt	$7, $L93
	.align 4

$L36:
	lda	$10,    P($10)		# is += P
	nop
	cmplt	$10, $16, $1
	bne	$1, $L37
	.align 4

$L9:
	lda	$11,    R($11)		# js += R
	nop
	cmplt	$11, $17, $1		# $1 = (js < n)
	bne	$1, $L10
	.align 4

$L4:
	lda	$9,     Q($9)		# ls += Q
	nop
	cmplt	$9, $18, $1		# $1 = (ls < k)
	bne	$1, $L5
	.align 4

$end:
	ldq	$26,  0($30)
	ldq	$9,   8($30)
	ldq	$10, 16($30)
	ldq	$11, 24($30)
	ldq	$12, 32($30)
	ldq	$13, 40($30)
	ldq	$14, 48($30)
	ldq	$15, 56($30)
	ldt	$f2, 64($30)
	ldt	$f3, 72($30)
	ldt	$f4, 80($30)
	ldt	$f5, 88($30)
	ldt	$f6, 96($30)
	ldt	$f7,104($30)
	ldt	$f8,112($30)
	ldt	$f9,120($30)
	ldq	$29,128($30)
	clr	$0
	lda	$30, STACKSIZE($30)
	ret
	.align 4

$quickend:
	clr	$0
	ret
	.ident	VERSION
	.end	ROUTINE
