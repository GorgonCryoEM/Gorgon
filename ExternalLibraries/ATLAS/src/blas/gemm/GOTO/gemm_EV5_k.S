    /*        Fast GEMM routine for Alpha 21164(A)         */
    /*         on  Linux, Digital UNIX and NT              */
    /*        by Kazushige Goto <goto@statabo.rim.or.jp>   */

#ifndef P
#ifdef DGEMM
#define P 32
#define Q 208
#define R 200
#else
#define P 40
#define Q 208
#define R 208
#endif
#endif

/* Internal BLOCK Leading size.*/
#define LDA (Q<<2)
#define LDB (Q<<1)

                   /* Now starting Main program */

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

#define MATRIX_A   ((P>>2)*LDA*SIZE)
#define MATRIX_B   ((R>>1)*LDB*SIZE)

#define STACKSIZE  16*8

	.set noreorder
	.set noat

.text
	.align 5
	.globl ROUTINE
	.ent ROUTINE

/* Initial Routine */
ROUTINE:
	lda	$30, -STACKSIZE($30)		# prepare stack

/* save original register value */
	stq	$26,   0($30)
	stq	$9,    8($30)
	stq	$10,  16($30)
	stq	$11,  24($30)
	stq	$12,  32($30)
	stq	$13,  40($30)
	stq	$14,  48($30)
	stq	$15,  56($30)
	stt	$f2,  64($30)
	stt	$f3,  72($30)
	stt	$f4,  80($30)
	stt	$f5,  96($30)
	stt	$f6, 104($30)
	stt	$f7, 112($30)
	stt	$f8, 120($30)
	.prologue	0

/* restore argument value */
	ldq	$9,  0+STACKSIZE($30)	# B
	ldl	$19, 8+STACKSIZE($30)	# ldb
	ldq	$13,16+STACKSIZE($30)	# C
	ldl	$10,24+STACKSIZE($30)	# ldc
	ldq	$28,32+STACKSIZE($30)	# buffer

	clr	$3			# ls = 0
	fmov	$f19, $f3

        clr     $14                     # js = 0
        .align 5

$L5:
	lda	$0,  Q
	subl	$18,$3,$7		# min_l = k - ls
	cmple	$7, $0, $1		# (min_l>Q)?
	cmoveq	$1, $0, $7		# if $1 then min_l = Q
	.align 4

#if defined(NN) || (!defined(C_VERSION) && defined(TN)) \
	|| (defined(C_VERSION) && defined(NT))

$L10:
#ifndef C_VERSION
	mull	$19,$14,$2		# js*ldb
	subl	$17,$14,$12		# min_j = n - js
#else
$L10:
	mull	$21,$14,$2		# js*ldb
	subl	$16,$14,$12		# min_j = n - js
#endif

	lda	$0,  R
        ldah    $24, Address_H(MATRIX_A)($28) # a_offset = &sb[0][0]
	cmple	$12, $0, $1		# (min_j>R) ?
	cmoveq	$1,  $0, $12		# if $1 then min_j = R

        lda     $24, Address_L(MATRIX_A)($24)
	mull	$10,$14,$25		# jsldc = js * ldc

	mov	$12, $27		# j = min_j
	unop
	unop
#ifndef C_VERSION
	SXADDQ	$2,$9,$0		# b_offset = b + js*ldb
#else
	SXADDQ	$2,$20,$0		# b_offset = b + js*ldb
#endif
	.align 4

$L15:
	mov	$7,  $5			# l = min_l
	mov	$24, $2			# a1_offset = a_offset

	SXADDQ	$3,  $0, $4		# b1_offset = b_offset + ls
#ifndef C_VERSION
	SXADDQ	$19, $0, $0		# b_offset += ldb
#else
	SXADDQ	$21, $0, $0		# b_offset += ldb
#endif

	SXADDQ	$3,  $0, $22		# c1_offset = b_offset + ls
#ifndef C_VERSION
	SXADDQ	$19, $0, $0		# b_offset += ldb
#else
	SXADDQ	$21, $0, $0		# b_offset += ldb
#endif
 	lda	$24,LDB*SIZE($24)	# a_offset += LDB
	subl	$27, 2, $27		# j--
	.align 4

$L19:
	LD	$f10,  0*SIZE($4)
	LD	$f11,  1*SIZE($4)
	LD	$f12,  2*SIZE($4)
	LD	$f13,  3*SIZE($4)

	LD	$f23,  4*SIZE($4)
	LD	$f24,  5*SIZE($4)
	LD	$f25,  6*SIZE($4)
	LD	$f26,  7*SIZE($4)

	LD	$f14,  0*SIZE($22)
	LD	$f15,  1*SIZE($22)
	LD	$f21,  2*SIZE($22)
	LD	$f22,  3*SIZE($22)

	LD	$f27,  4*SIZE($22)
	LD	$f28,  5*SIZE($22)
	LD	$f29,  6*SIZE($22)
	LD	$f30,  7*SIZE($22)

	lda	$2,   16*SIZE($2)	# a1_offset += 8
	subl	$5,  8, $5		# l -= 4
	lda	$4,    8*SIZE($4)	# b1_offset += 4
	lda	$22,   8*SIZE($22)	# c1_offset += 4

	ST	$f10, -16*SIZE($2)
	ST	$f14, -15*SIZE($2)
	ST	$f11, -14*SIZE($2)
	ST	$f15, -13*SIZE($2)

	ST	$f12, -12*SIZE($2)
	ST	$f21, -11*SIZE($2)
	ST	$f13, -10*SIZE($2)
	ST	$f22,  -9*SIZE($2)

	ST	$f23,  -8*SIZE($2)
	ST	$f27,  -7*SIZE($2)
	ST	$f24,  -6*SIZE($2)
	ST	$f28,  -5*SIZE($2)

	ST	$f25,  -4*SIZE($2)
	ST	$f29,  -3*SIZE($2)
	ST	$f26,  -2*SIZE($2)
	ST	$f30,  -1*SIZE($2)

	bgt	$5,  $L19		# if l>0 goto $L19
	bgt	$27, $L15		# if j>0 goto $L15

#else			/* NT or TT */

$L10:
#ifndef C_VERSION
	mull	$19, $3, $2		# ls*ldb
	subl	$17,$14,$12		# min_j = n - js
#else
	mull	$21, $3, $2		# ls*ldb
	subl	$16,$14,$12		# min_j = n - js
#endif

	lda	$0,  R
        ldah    $24, Address_H(MATRIX_A)($28) # a_offset = &sb[0][0]
	cmple	$12, $0, $1		# (min_j>R) ?
	cmoveq	$1,  $0, $12		# if $1 then min_j = R

        lda     $24, Address_L(MATRIX_A)($24)
	mull	$10,$14,$25		# jsldc = js * ldc

	mov	$12, $27		# j = min_j
	fnop
	fnop
#ifndef C_VERSION
	SXADDQ	$2,$9,$0		# b_offset = b + ls*ldb
#else
	SXADDQ	$2,$20,$0		# b_offset = b + ls*ldb
#endif
	.align 4

$L101:
	mov	$24, $2			# a1_offset = a_offset
	mov	$7,  $5			# l = min_l

	lda	$24, LDB*SIZE($24)	# a_offset += LDB
	SXADDQ	$14, $0, $4		# b1_offset = b_offset + js
	lda	$0,    2*SIZE($0)	# b_offset += 2
	.align 4

$L102:
#ifndef C_VERSION
	LD	$f10, 0*SIZE($4)	# atemp1 = *(b1_offset+0)
	LD	$f11, 1*SIZE($4)	# atemp2 = *(b1_offset+1)
	SXADDQ	$19, $4, $4		# b1_offset += ldb

	LD	$f12, 0*SIZE($4)	# atemp3 = *(b1_offset+0)
	LD	$f13, 1*SIZE($4)	# atemp4 = *(b1_offset+1)
	SXADDQ	$19, $4, $4		# b1_offset += ldb
#else
	LD	$f10, 0*SIZE($4)	# atemp1 = *(b1_offset+0)
	LD	$f11, 1*SIZE($4)	# atemp2 = *(b1_offset+1)
	SXADDQ	$21, $4, $4		# b1_offset += ldb

	LD	$f12, 0*SIZE($4)	# atemp3 = *(b1_offset+0)
	LD	$f13, 1*SIZE($4)	# atemp4 = *(b1_offset+1)
	SXADDQ	$21, $4, $4		# b1_offset += ldb
#endif

	subl	$5, 2, $5		# l--
	ST	$f10, 0*SIZE($2)	# *(a1_offset+0) = atemp1
	ST	$f11, 1*SIZE($2)	# *(a1_offset+1) = atemp2
	ST	$f12, 2*SIZE($2)	# *(a1_offset+3) = atemp3
	ST	$f13, 3*SIZE($2)	# *(a1_offset+4) = atemp4

	lda	$2,   4*SIZE($2)	# a1_offset += 4

	bgt	$5, $L102		# if l>0 goto $102

	subl	$27, 2, $27		# l--
	bgt	$27, $L101
#endif
	clr	$11			# is = 0
	.align 4

#if defined(NN) || (!defined(C_VERSION) && defined(NT)) \
	|| (defined(C_VERSION) && defined(TN))

$L30:
#ifndef C_VERSION
	mull	$21,$3,$2		# lslda = lda * ls
	subl	$16,$11,$15		# min_i = m - is
#else
	mull	$19,$3,$2		# lslda = lda * ls
	subl	$17,$11,$15		# min_i = m - is
#endif
	cmple	$15,P, $1		# (min_i>P)?
	cmoveq	$1, P, $15		# if $1 then min_i = P

	mov	$28, $0			# b_offset = &sa[0][0]
	mov	$7,  $5			# l = min_l

#ifndef C_VERSION
	SXADDQ	$2,$20,$24		# a_offset = a + lslda
#else
	SXADDQ	$2,$9, $24		# a_offset = a + lslda
#endif
	.align 4

$L35:
	SXADDQ	$11, $24, $2		# a1_offset = a_offset + is
#ifndef C_VERSION
	SXADDQ	$21, $24, $24		# a_offset += lda
#else
	SXADDQ	$19, $24, $24		# a_offset += lda
#endif

	SXADDQ	$11, $24, $22		# a1_offset = a_offset + is
#ifndef C_VERSION
	SXADDQ	$21, $24, $24		# a_offset += lda
#else
	SXADDQ	$19, $24, $24		# a_offset += lda
#endif

	mov	$0,  $4			# b1_offset = b_offset
	subl	$15,  8, $8		# i -= 8

	lda	$0,    8*SIZE($0)	# b_offset += 4
	subl	$5, 2, $5		# l--
	.align 4

	LD	$f10,  0*SIZE($2)
	LD	$f11,  1*SIZE($2)
	LD	$f23,  0*SIZE($22)
	LD	$f24,  1*SIZE($22)

	LD	$f12,  2*SIZE($2)
	LD	$f13,  3*SIZE($2)
	LD	$f25,  2*SIZE($22)
	LD	$f26,  3*SIZE($22)

	LD	$f14,  4*SIZE($2)
	LD	$f15,  5*SIZE($2)
	LD	$f27,  4*SIZE($22)
	LD	$f28,  5*SIZE($22)

	LD	$f21,  6*SIZE($2)
	LD	$f22,  7*SIZE($2)
	LD	$f29,  6*SIZE($22)
	LD	$f30,  7*SIZE($22)
	ble	$8,$L38			# if i>0 goto $L39
	.align 4

$L39:
	ST	$f10,  (0*LDA+0)*SIZE($4)
	ST	$f11,  (0*LDA+1)*SIZE($4)
	ST	$f12,  (0*LDA+2)*SIZE($4)
	ST	$f13,  (0*LDA+3)*SIZE($4)

	LD	$f10,  8*SIZE($2)
	LD	$f11,  9*SIZE($2)
	LD	$f12, 10*SIZE($2)
	LD	$f13, 11*SIZE($2)

	ST	$f23, (0*LDA+4)*SIZE($4)
	ST	$f24, (0*LDA+5)*SIZE($4)
	ST	$f25, (0*LDA+6)*SIZE($4)
	ST	$f26, (0*LDA+7)*SIZE($4)

	LD	$f23,  8*SIZE($22)
	LD	$f24,  9*SIZE($22)
	LD	$f25, 10*SIZE($22)
	LD	$f26, 11*SIZE($22)

	ST	$f14, (LDA+0)*SIZE($4)
	ST	$f15, (LDA+1)*SIZE($4)
	ST	$f21, (LDA+2)*SIZE($4)
	ST	$f22, (LDA+3)*SIZE($4)

	LD	$f14, 12*SIZE($2)
	LD	$f15, 13*SIZE($2)
	LD	$f21, 14*SIZE($2)
	LD	$f22, 15*SIZE($2)

	ST	$f27, (LDA+4)*SIZE($4)
	ST	$f28, (LDA+5)*SIZE($4)
	ST	$f29, (LDA+6)*SIZE($4)
	ST	$f30, (LDA+7)*SIZE($4)

	LD	$f27, 12*SIZE($22)
	LD	$f28, 13*SIZE($22)
	LD	$f29, 14*SIZE($22)
	LD	$f30, 15*SIZE($22)

	subl	$8,  8, $8		# i -= 8
	lda	$22,    8*SIZE($22)	# a1_offset += 4
	lda	$4,  2*LDA*SIZE($4)	# b1_offset += LDA
	lda	$2,     8*SIZE($2)	# a1_offset += 4

	bgt	$8,$L39			# if i>0 goto $L39
	.align 4
$L38:
	ST	$f10, (0*LDA+0)*SIZE($4)
	ST	$f11, (0*LDA+1)*SIZE($4)
	ST	$f12, (0*LDA+2)*SIZE($4)
	ST	$f13, (0*LDA+3)*SIZE($4)

	ST	$f23, (0*LDA+4)*SIZE($4)
	ST	$f24, (0*LDA+5)*SIZE($4)
	ST	$f25, (0*LDA+6)*SIZE($4)
	ST	$f26, (0*LDA+7)*SIZE($4)

	ST	$f14, (LDA+0)*SIZE($4)
	ST	$f15, (LDA+1)*SIZE($4)
	ST	$f21, (LDA+2)*SIZE($4)
	ST	$f22, (LDA+3)*SIZE($4)

	ST	$f27, (LDA+4)*SIZE($4)
	ST	$f28, (LDA+5)*SIZE($4)
	ST	$f29, (LDA+6)*SIZE($4)
	ST	$f30, (LDA+7)*SIZE($4)
	bgt	$5, $L35		# if l>0 goto $L35
	.align 4

#else       /* TN or TT */

$L30:

#ifndef C_VERSION
	mull	$11, $21, $2		# is*lda
	subl	$16, $11, $15		# min_i = m - is
#else
	mull	$11, $19, $2		# is*lda
	subl	$17, $11, $15		# min_i = m - is
#endif

	cmple	$15,P, $1		# (min_i>P)?
	cmoveq	$1, P, $15		# if $1 then min_i = P

#ifndef C_VERSION
	SXADDQ	$2, $20, $6		# c_offset = a + is*lda
	s4addq	$21, 0,  $1		# j = 4*lda
#else
	SXADDQ	$2, $9,  $6		# c_offset = a + is*lda
	s4addq	$19, 0,  $1		# j = 4*lda
#endif

	mov	$28, $0			# b_offset = &sa[0][0]
	mov	$15, $8			# i = min_i
	.align 4

$L201:
	mov	$0, $4			# b1_offset = b_offset
	lda	$0, LDA*SIZE($0)	# b_offset += LDA

	SXADDQ	$3, $6, $24		# a_offset = c_offset + ls
	mov	$7,  $5			# l = min_l
	.align 4

$L202:
	mov	$24, $2			# a1_offset = a_offset
	lda	$24,   4*SIZE($24)	# a_offset += 4

	LD	$f10,  0*SIZE($2)
	LD	$f11,  1*SIZE($2)
	LD	$f12,  2*SIZE($2)
	LD	$f13,  3*SIZE($2)

#ifndef C_VERSION
	SXADDQ	$21, $2, $2		# a1_offset += lda
#else
	SXADDQ	$19, $2, $2		# a1_offset += lda
#endif

	LD	$f14,  0*SIZE($2)
	LD	$f15,  1*SIZE($2)
	LD	$f21,  2*SIZE($2)
	LD	$f22,  3*SIZE($2)

#ifndef C_VERSION
	SXADDQ	$21, $2, $2		# a1_offset += lda
#else
	SXADDQ	$19, $2, $2		# a1_offset += lda
#endif

	LD	$f23,  0*SIZE($2)
	LD	$f24,  1*SIZE($2)
	LD	$f25,  2*SIZE($2)
	LD	$f26,  3*SIZE($2)

#ifndef C_VERSION
	SXADDQ	$21, $2, $2		# a1_offset += lda
#else
	SXADDQ	$19, $2, $2		# a1_offset += lda
#endif

	LD	$f27,  0*SIZE($2)
	LD	$f28,  1*SIZE($2)
	LD	$f29,  2*SIZE($2)
	LD	$f30,  3*SIZE($2)

	ST	$f10,  0*SIZE($4)	# atemp1
	ST	$f14,  1*SIZE($4)	# atemp5
	ST	$f23,  2*SIZE($4)	# ctemp1
	ST	$f27,  3*SIZE($4)	# ctemp5

	ST	$f11,  4*SIZE($4)	# atemp2
	ST	$f15,  5*SIZE($4)	# atemp6
	ST	$f24,  6*SIZE($4)	# ctemp2
	ST	$f28,  7*SIZE($4)	# ctemp6

	ST	$f12,  8*SIZE($4)	# atemp3
	ST	$f21,  9*SIZE($4)	# atemp7
	ST	$f25, 10*SIZE($4)	# ctemp3
	ST	$f29, 11*SIZE($4)	# ctemp7

	ST	$f13, 12*SIZE($4)	# atemp4
	ST	$f22, 13*SIZE($4)	# atemp8
	ST	$f26, 14*SIZE($4)	# ctemp4
	ST	$f30, 15*SIZE($4)	# ctemp8

	lda	$4,   16*SIZE($4)	# b1_offset += 16

	subl	$5, 4, $5		# l -= 4
	bgt	$5, $L202

	subl	$8, 4, $8		# i -= 4
	SXADDQ	$1, $6, $6		# c_offset += j
	bgt	$8, $L201

	.align 4
#endif

$L33:
	sra	$12, 1, $27		# j = (min_j>>1)
        ldah    $26, Address_H(MATRIX_A)($28) # a_offset = &sb[0][0]
        lda     $26, Address_L(MATRIX_A)($26)
	SXADDQ	$25,$13,$6		# c_offset = c + jsldc;

	unop
	unop
	unop
	ble	$27, $L48
	.align 4

$L50:
	mov	$26, $0			# b_offset = b_orig
	fclr	$f11
	lda	$26, SIZE*LDB($26)	# b_orig += LDB
	fclr	$f13

	mov	$28, $24		# a_offset = &sa[0][0]
	fclr	$f14
	sra	$15, 2, $8		# i = (min_i>>1)
 	fclr	$f15

	SXADDQ	$11, $6, $22		# c1_offset = c_offset + is
	fclr	$f12
	SXADDQ	$10, $6, $6		# c_offset += ldc(2 times)
	fclr	$f25

	SXADDQ	$10, $6, $6		# c_offset += ldc
	fclr	$f26
	fclr	$f27
	ble	$8,$L52			# if j<=0 goto $L52
	.align 4

$L54:
	mov	$24, $2			# a1_offset = a_offset
	fclr	$f28
	sra	$7, 2, $5		# l = (min_l>>2)
	fclr	$f29

	mov	$0, $4			# b1_offset = b_offset
	fclr	$f30
	SXADDQ	$10, $22, $23		# c2_offset = c1_offset + ldc
	fclr	$f21

	lda	$24, LDA*SIZE($24)	# a_offset += LDA
	LD	$f5, 0*SIZE($22)
	LD	$f6, 0*SIZE($23)
	ble	$5, $L55		# if l<=0 goto $L55

	LD	$f1,   0*SIZE($4)	# btemp1 = *(b1_offset+0)
	LD	$f10,  1*SIZE($4)	# btemp2 = *(b1_offset+1)
	LD	$f24,  2*SIZE($4)	# btemp3 = *(b1_offset+2)
	LD	$f23,  3*SIZE($4)	# btemp4 = *(b1_offset+3)

	LD	$f22,  0*SIZE($2)	# atemp1
	LD	$f20,  1*SIZE($2)	# atemp2
	LD	$f18,  2*SIZE($2)	# atemp3
	LD	$f16,  3*SIZE($2)	# atemp4

	LD	$f17,  4*SIZE($2)	# atemp5
	LD	$f0,   5*SIZE($2)	# atemp6
	LD	$f19,  6*SIZE($2)	# atemp7
	LD	$f2,   7*SIZE($2)	# atemp8

	lda	$2, 8*SIZE($2)		# a1_offset += 8
	subl	$5,  1, $5		# l--
	addq	$4, 4*SIZE, $4		# b1_offset += 2
	ble	$5, $L57		# if l<=0 goto $L57
	.align 4

/* 
   Main Loop.
   This loop is very important and affects calculating speed directry.
   Each load waits 12 clocks which is enough to load from 2nd cache.
   And one loop takes 33 clocks(multiply and add = 32 clock plus jump
   latency = 1 clock).  If you have 21164A with 600MHz machine,  it'll
   take 1163.6 MFlops in this loop.  This is theoretical value, but it's
   pretty fast, isn't it?
*/

$L58:
/*  1 */
	addt	$f28,$f11,$f28
#ifdef DGEMM
	ldt	$f31,      32*SIZE($4)	# prefetch
#else
	ldt	$f31,      36*SIZE($4)	# prefetch
#endif
	mult	$f1,$f22,$f11
#ifdef DGEMM
	ldt	$f31,      44*SIZE($2)	# prefetch
#else
	nop
#endif

/*  2 */
	addt	$f29,$f13,$f29
	mult	$f1,$f20,$f13
/*  3 */
	addt	$f30,$f14,$f30
	mult	$f1,$f18,$f14

/*  4 */
	addt	$f21,$f15,$f21
	subl	$5,1,$5
	mult	$f1,$f16,$f15
	LD	$f1,   0*SIZE($4)

/*  5 */
	addt	$f12,$f11,$f12
	nop
	mult	$f10,$f22,$f11
	LD	$f22,  0*SIZE($2)

/*  6 */
	addt	$f25,$f13,$f25
	nop
	mult	$f10,$f20,$f13
	LD	$f20,  1*SIZE($2)

/*  7 */
	addt	$f26,$f14,$f26
	nop
	mult	$f10,$f18,$f14
	LD	$f18,  2*SIZE($2)

/*  8 */
	addt	$f27,$f15,$f27
	nop
	mult	$f10,$f16,$f15
	LD	$f16,  3*SIZE($2)

/*  9 */
	addt	$f28,$f11,$f28
	nop
	mult	$f24,$f17,$f11
	LD	$f7,   1*SIZE($4)

/* 10 */
	addt	$f29,$f13,$f29
	mult	$f24,$f0,$f13
/* 11 */
	addt	$f30,$f14,$f30
	mult	$f24,$f19,$f14

/* 12 */
	addt	$f21,$f15,$f21
	nop
	mult	$f24,$f2,$f15
	LD	$f8,  2*SIZE($4)

/* 13 */
	addt	$f12,$f11,$f12
	nop
	mult	$f23,$f17,$f11
	LD	$f17,  4*SIZE($2)

/* 14 */
	addt	$f25,$f13,$f25
	nop
	mult	$f23,$f0,$f13
	LD	$f0,   5*SIZE($2)

/* 15 */
	addt	$f26,$f14,$f26
	nop
	mult	$f23,$f19,$f14
	LD	$f19,  6*SIZE($2)

/* 16 */
	addt	$f27,$f15,$f27
	LD	$f4,   3*SIZE($4)	# modified
	mult	$f23,$f2,$f15
	LD	$f2,   7*SIZE($2)

/* 17 */
	addt	$f28,$f11,$f28
	nop
	mult	$f1,$f22,$f11
	nop

/* 18 */
	addt	$f29,$f13,$f29
	nop
	mult	$f1,$f20,$f13
#ifdef DGEMM
	LD	$f31,   48*SIZE($2)
#else
	nop
#endif

/* 19 */
	addt	$f30,$f14,$f30
	nop
	mult	$f1,$f18,$f14
	lda	$2,   16*SIZE($2)		# a1_offset += 16

/* 20 */
	addt	$f21,$f15,$f21
	nop
	mult	$f1,$f16,$f15
	LD	$f1,   4*SIZE($4)

/* 21 */
	addt	$f12, $f11, $f12
	nop
	mult	$f7,  $f22, $f11
	LD	$f22, -8*SIZE($2)

/* 22 */
	addt	$f25,$f13,$f25
	nop
	mult	$f7,  $f20, $f13
	LD	$f20, -7*SIZE($2)

/* 23 */
	addt	$f26,$f14,$f26
	nop
	mult	$f7,  $f18, $f14
	LD	$f18, -6*SIZE($2)

/* 24 */
	addt	$f27,$f15,$f27
	nop
	mult	$f7,  $f16, $f15
	LD	$f16, -5*SIZE($2)

/* 25 */
	addt	$f28,$f11,$f28
	nop
	mult	$f8,$f17,$f11
	LD	$f10,  5*SIZE($4)	# modifiled

/* 26 */
	addt	$f29,$f13,$f29
	nop
	mult	$f8,$f0,$f13
	lda	$4,    8*SIZE($4)

/* 27 */
	addt	$f30,$f14,$f30
	nop
	mult	$f8,$f19,$f14
	LD	$f24, -2*SIZE($4)

/* 28 */
	addt	$f21,$f15,$f21
	nop
	mult	$f8,$f2,$f15
	LD	$f23, -1*SIZE($4)

/* 29 */
	addt	$f12,$f11,$f12
	nop
	mult	$f4,$f17,$f11
	LD	$f17, -4*SIZE($2)

/* 30 */
	addt	$f25,$f13,$f25
	nop
	mult	$f4,$f0,$f13
	LD	$f0,  -3*SIZE($2)

/* 31 */
	addt	$f26,$f14,$f26
	nop
	mult	$f4,$f19,$f14
	LD	$f19, -2*SIZE($2)

/* 32 */
	addt	$f27,$f15,$f27
	mult	$f4,$f2,$f15
	LD	$f2,  -1*SIZE($2)
	bgt	$5,$L58
	.align 4

$L57:
	addt	$f28,$f11,$f28
	lds	$f31,  8*SIZE($22)	# prefetch(modify intent)
	mult	$f1,$f22,$f11
#ifdef DGEMM
	lds	$f31, 12*SIZE($23)	# prefetch(modify intent)
#else
	unop
#endif

	addt	$f29,$f13,$f29
	mult	$f1,$f20,$f13
	addt	$f30,$f14,$f30
	mult	$f1,$f18,$f14

	addt	$f21,$f15,$f21
	unop
	mult	$f1,$f16,$f15
	LD	$f1,  0*SIZE($4)

	addt	$f12,$f11,$f12
	unop
	mult	$f10,$f22,$f11
	LD	$f22, 0*SIZE($2)

	addt	$f25,$f13,$f25
	unop
	mult	$f10,$f20,$f13
	LD	$f20, 1*SIZE($2)

	addt	$f26,$f14,$f26
	unop
	mult	$f10,$f18,$f14
	LD	$f18, 2*SIZE($2)

	addt	$f27,$f15,$f27
	unop
	mult	$f10,$f16,$f15
	LD	$f16, 3*SIZE($2)

	addt	$f28,$f11,$f28
	lda	$2,   4*SIZE($2)	# a1_offset += 4
	mult	$f24,$f17,$f11
	LD	$f7,  1*SIZE($4)

	addt	$f29,$f13,$f29
	mult	$f24,$f0,$f13
	addt	$f30,$f14,$f30
	mult	$f24,$f19,$f14

	addt	$f21,$f15,$f21
	unop
	mult	$f24,$f2,$f15
	LD	$f24, 2*SIZE($4)

	addt	$f12,$f11,$f12
	unop
	mult	$f23,$f17,$f11
	LD	$f17, 0*SIZE($2)

	addt	$f25,$f13,$f25
	unop
	mult	$f23,$f0,$f13
	LD	$f0,  1*SIZE($2)

	addt	$f26,$f14,$f26
	unop
	mult	$f23,$f19,$f14
	LD	$f19, 2*SIZE($2)

	addt	$f27,$f15,$f27
	unop
	mult	$f23,$f2,$f15
	LD	$f2,  3*SIZE($2)

	addt	$f28,$f11,$f28
	LD	$f4,  3*SIZE($4)
	mult	$f1,$f22,$f11
	lda	$2,  4*SIZE($2)		# a1_offset += 4

	addt	$f29,$f13,$f29
	addq	$4, 4*SIZE, $4
	mult	$f1,$f20,$f13
	unop

	addt	$f30,$f14,$f30
	mult	$f1,$f18,$f14
	addt	$f21,$f15,$f21
	mult	$f1,$f16,$f15

	addt	$f12, $f11, $f12
	mult	$f7,  $f22, $f11
	addt	$f25, $f13, $f25
	mult	$f7,  $f20, $f13

	addt	$f26,$f14,$f26
	mult	$f7, $f18,$f14
	addt	$f27,$f15,$f27
	mult	$f7, $f16,$f15

	addt	$f28,$f11,$f28
	mult	$f24,$f17,$f11
	addt	$f29,$f13,$f29
	mult	$f24,$f0,$f13

	addt	$f30,$f14,$f30
	mult	$f24,$f19,$f14
	addt	$f21,$f15,$f21
	mult	$f24,$f2,$f15

	addt	$f12, $f11, $f12
	mult	$f4,  $f17, $f11
	addt	$f25, $f13, $f25
	mult	$f4,  $f0,  $f13

	addt	$f26, $f14, $f26
	mult	$f4,  $f19, $f14
	addt	$f27, $f15, $f27
	mult	$f4,  $f2,  $f15
	.align 4

$L55:
	and	$7,3,$5			# l = (min_l&3)
	beq	$5,$L60			# if l<=0 goto $L60

	LD	$f22, 0*SIZE($2)	# atemp1
	LD	$f1,  0*SIZE($4)	# btemp1

	LD	$f20, 1*SIZE($2)	# atemp2
	LD	$f10, 1*SIZE($4)	# btemp2
	LD	$f18, 2*SIZE($2)	# atemp3
	LD	$f16, 3*SIZE($2)	# atemp4

	lda	$2,   4*SIZE($2)	# a1_offset += 4
	subl	$5, 1, $5		# l--
	lda	$4,   2*SIZE($4)	# b1_offset += 2
	ble	$5,$L62			# if l<=0 goto $L62
	.align 4

$L63:
	addt	$f28,$f11,$f28
#ifdef DGEMM
	LD	$f31, 8*8($4)
#else
	unop
#endif
	mult	$f1,$f22,$f11
	unop

	addt	$f29,$f13,$f29
	mult	$f1,$f20,$f13
	addt	$f30,$f14,$f30
	mult	$f1,$f18,$f14

	addt	$f21,$f15,$f21
	lda	$2,    4*SIZE($2)	# a1_offset += 4
	mult	$f1,$f16,$f15
	LD	$f1,   0*SIZE($4)

	addt	$f12,$f11,$f12
	lda	$4,    2*SIZE($4)	# b1_offset += 2
	mult	$f10,$f22,$f11
	LD	$f22, -4*SIZE($2)

	addt	$f25,$f13,$f25
	subl	$5,1,$5			# l--
	mult	$f10,$f20,$f13
	LD	$f20,  -3*SIZE($2)

	addt	$f26,$f14,$f26
	unop
	mult	$f10,$f18,$f14
	LD	$f18,  -2*SIZE($2)

	addt	$f27,$f15,$f27
	mult	$f10,$f16,$f15
	LD	$f10,  -1*SIZE($4)
	LD	$f16,  -1*SIZE($2)

	bgt	$5,$L63			# if l>0 goto $L63
	unop
	unop
	unop
	.align 4

$L62:
	addt	$f28,$f11,$f28
	mult	$f1,$f22,$f11
	addt	$f29,$f13,$f29
	mult	$f1,$f20,$f13

	addt	$f30,$f14,$f30
	mult	$f1,$f18,$f14
	addt	$f21,$f15,$f21
	mult	$f1,$f16,$f15

	addt	$f12,$f11,$f12
	mult	$f10,$f22,$f11
	addt	$f25,$f13,$f25
	mult	$f10,$f20,$f13

	addt	$f26,$f14,$f26
	mult	$f10,$f18,$f14
	addt	$f27,$f15,$f27
	mult	$f10,$f16,$f15
	.align 4

$L60:
	addt	$f28,$f11,$f28
	LD	$f20, 1*SIZE($22)	# atemp2 = *(c1_offset+1)
	mult	$f3,$f12,$f11
	LD	$f18, 2*SIZE($22)	# atemp3 = *(c1_offset+2)

	addt	$f29,$f13,$f29
	LD	$f16, 3*SIZE($22)	# atemp4 = *(c1_offset+3)
	mult	$f3,$f25,$f13
	LD	$f0,  1*SIZE($23)	# atemp6 = *(c2_offset+1)

	addt	$f30,$f14,$f30
	LD	$f19, 2*SIZE($23)	# atemp7 = *(c2_offset+2)
	mult	$f3,$f26,$f14
	LD	$f2,  3*SIZE($23)	# atemp8 = *(c2_offset+3)

	addt	$f21,$f15,$f21
	unop
	mult	$f3,$f27,$f15
	unop

	addt	$f5,$f11,$f12
	addq	$22, 4*SIZE, $22	# c1_offset += 4
	mult	$f3,$f28,$f11
	subl	$8,1,$8			# i--

	addt	$f20, $f13, $f25
	mult	$f3,  $f29, $f13
	addt	$f18, $f14, $f26
	mult	$f3,  $f30, $f14

	addt	$f16,$f15,$f27
	mult	$f3,$f21,$f15
	addt	$f6,$f11,$f28
	ST	$f12, -4*SIZE($22)	# *(c1_offset+0) = ctemp1

	addt	$f0,$f13,$f29
	unop
	ST	$f25, -3*SIZE($22)	# *(c1_offset+1) = ctemp2
	fclr	$f11
	
	addt	$f19,$f14,$f30
	unop
	ST	$f26, -2*SIZE($22)	# *(c1_offset+2) = ctemp3
	fclr	$f13

	addt	$f2,$f15,$f21
	unop
	ST	$f27,  -1*SIZE($22)	# *(c1_offset+3) = ctemp4
	fclr	$f14

	ST	$f28, 0*SIZE($23)	# *(c2_offset+0) = ctemp5
	fclr	$f15
	unop
	fclr	$f12

	ST	$f29, 1*SIZE($23)	# *(c2_offset+1) = ctemp6
	fclr	$f25
	unop
	fclr	$f26

	ST	$f30, 2*SIZE($23)	# *(c2_offset+2) = ctemp7
	fclr	$f27
	ST	$f21, 3*SIZE($23)	# *(c2_offset+3) = ctemp8
	bgt	$8,$L54			# if i>0 goto $L54
	.align 4

$L52:
	and	$15,3,$8		# i = (min_i&3)
	beq	$8,$L49			# if i<0 goto $L49
	unop
	unop
	.align 4

$L69:
	mov	$24, $2			# a1_offset = a_offset
	addq	$24, SIZE, $24		# a_offset ++
	mov	$0, $4			# b1_offset = b_offset
	SXADDQ	$10, $22, $23		# c2_offset = c1_offset + ldc

	fclr	$f12			# ctemp1 = ZERO
	mov	$7, $5			# l = min_l
	fclr	$f28			# ctemp5 = ZERO
	ble	$7,$L71			# if l<=0 goto $L71
	.align 4

$L73:
	LD	$f22,    0($2)		# atemp1
	lda	$2,    4*SIZE($2)	# a1_offset += 4

	LD	$f1,   0*SIZE($4)	# btemp1 = *(b1_offset+0)
	LD	$f10,  1*SIZE($4)	# btemp2 = *(b1_offset+2)

	mult	$f1, $f22,$f11		# temp1 = btemp1 * atemp1
	mult	$f10,$f22,$f13		# temp2 = btemp2 * atemp1
	lda	$4,    2*SIZE($4)	# b1_offset ++
	subl	$5,1,$5			# l--

	addt	$f12,$f11,$f12		# ctemp1 = ctemp1 + temp1
	addt	$f28,$f13,$f28		# ctemp5 = ctemp5 + temp2
	bgt	$5,$L73			# if l>0 goto $L73
	unop
	.align 4

$L71:
	mult	$f3,$f12,$f11		# temp1 = alpha*ctemp1
	mult	$f3,$f28,$f13		# temp2 = alpha*ctemp2
	LD	$f22, 0*SIZE($22)	# atemp1 = *c1_offset
	LD	$f17, 0*SIZE($23)	# atemp5 = *c2_offset

	addt	$f22,$f11,$f12		# ctemp1 = atemp1 + temp1
	addt	$f17,$f13,$f28		# ctemp2 = atemp5 + temp2
	subl	$8,1,$8			# i--
	ST	$f12, 0*SIZE($22)	# *c1_offset = ctemp1

	ST	$f28, 0*SIZE($23)	# *c2_offset = ctemp2
	addq	$22, SIZE, $22		# c1_offset ++
	bgt	$8,$L69			# if i>0 goto $L69
	unop
	.align 4

$L49:
	subl	$27,1,$27		# j --
	bgt	$27,$L50		# if j>0 goto $L50
	unop
	unop
	.align 4

$L48:
	blbc	$12,$L29		# if (!min_j&1) goto $L29
	fclr	$f11
	sra	$15,2,$8		# i = (min_i>>2)
	fclr	$f13

	mov	$26, $0			# b_offset = b_orig
	fclr	$f14
	mov	$28, $24		# a_offset = &sa[0][0]
	fclr	$f15

	SXADDQ	$11, $6, $22		# c1_offset = c_offset + is
	ble	$8,$L79			# if i<0 goto $L79
	unop
	unop
	.align 4

$L81:
	mov	$24, $2			# a1_offset = a_offset
	fclr	$f12
	sra	$7, 2, $5		# l = (min_l>>2)
	fclr	$f25

	mov	$0, $4			# b1_offset = b_offset
	fclr	$f26
	lda	$24, LDA*SIZE($24)	# a_offset += LDA
	fclr	$f27

	unop
	ble	$5,$L82

	LD	$f1,   0*SIZE($4)	# btemp1
	LD	$f10,  2*SIZE($4)	# btemp2

	LD	$f22,  0*SIZE($2)	# atemp1
	LD	$f20,  1*SIZE($2)	# atemp2
	LD	$f18,  2*SIZE($2)	# atemp3
	LD	$f16,  3*SIZE($2)	# atemp4
	LD	$f17,  4*SIZE($2)	# atemp5
	LD	$f0,   5*SIZE($2)	# atemp6
	LD	$f19,  6*SIZE($2)	# atemp7
	LD	$f2,   7*SIZE($2)	# atemp8

	lda	$2,    8*SIZE($2)	# a1_offset += 8

	subl	$5, 1, $5		# l--
	lda	$4,    4*SIZE($4)	# b1_offset += 4
	ble	$5, $L84		# if l<= 0 goto $L84
	.align 4
$L85:
	addt	$f12,$f11,$f12
	unop
	mult	$f1,$f22,$f11
	LD	$f22,  0*SIZE($2)	# atemp1

	addt	$f25,$f13,$f25
	unop
	mult	$f1,$f20,$f13
	LD	$f20,  1*SIZE($2)	# atemp2

	addt	$f26,$f14,$f26
	unop
	mult	$f1,$f18,$f14
	LD	$f18,  2*SIZE($2)	# atemp3

	addt	$f27,$f15,$f27
	unop
	mult	$f1,$f16,$f15
	LD	$f1,   0*SIZE($4)	# btemp1

	addt	$f12,$f11,$f12
	LD	$f16,  3*SIZE($2)	# atemp4
	mult	$f10,$f17,$f11
	LD	$f17,  4*SIZE($2)	# atemp5

	addt	$f25,$f13,$f25
	unop
	mult	$f10,$f0,$f13
	LD	$f0,   5*SIZE($2)	# atemp6

	addt	$f26,$f14,$f26
	unop
	mult	$f10,$f19,$f14
	LD	$f19,  6*SIZE($2)	# atemp7

	addt	$f27,$f15,$f27
	unop
	mult	$f10,$f2,$f15
	LD	$f10,  2*SIZE($4)	# btemp2

	addt	$f12,$f11,$f12
 	LD	$f2,   7*SIZE($2)	# atemp8
	mult	$f1,$f22,$f11
	LD	$f22,  8*SIZE($2)	# atemp1

	addt	$f25,$f13,$f25
	unop
	mult	$f1,$f20,$f13
	LD	$f20,  9*SIZE($2)	# atemp2

	addt	$f26,$f14,$f26
	unop
	mult	$f1,$f18,$f14
	LD	$f18, 10*SIZE($2)	# atemp3

	addt	$f27,$f15,$f27
	lda	$2,   16*SIZE($2)	# a1_offset += 16
	mult	$f1,$f16,$f15
	LD	$f1,   4*SIZE($4)	# btemp1

	addt	$f12,$f11,$f12
	LD	$f16, -5*SIZE($2)	# atemp4
	mult	$f10,$f17,$f11
	LD	$f17, -4*SIZE($2)	# atemp5

	addt	$f25,$f13,$f25
	unop
	mult	$f10,$f0,$f13
	LD	$f0,  -3*SIZE($2)	# atemp6

	addt	$f26,$f14,$f26
	unop
	mult	$f10,$f19,$f14
	LD	$f19, -2*SIZE($2)	# atemp7

	addt	$f27,$f15,$f27
	unop
	mult	$f10,$f2,$f15
	LD	$f10,  6*SIZE($4)	# btemp2

	LD	$f2,  -1*SIZE($2)	# atemp8
	subl	$5, 1, $5		# l--
	lda	$4,    8*SIZE($4)	# b1_offset += 8
	bgt	$5,$L85			# if l>0 goto $L85

$L84:
	addt	$f12,$f11,$f12
	unop
	mult	$f1,$f22,$f11
	LD	$f22,  0*SIZE($2)	# atemp1

	addt	$f25,$f13,$f25
	unop
	mult	$f1,$f20,$f13
	LD	$f20,  1*SIZE($2)	# atemp2

	addt	$f26,$f14,$f26
	unop
	mult	$f1,$f18,$f14
	LD	$f18,  2*SIZE($2)	# atemp3

	addt	$f27,$f15,$f27
	unop
	mult	$f1,$f16,$f15
	LD	$f1,   0*SIZE($4)	# btemp1

	addt	$f12,$f11,$f12
	LD	$f16,  3*SIZE($2)	# atemp4
	mult	$f10,$f17,$f11
	LD	$f17,  4*SIZE($2)	# atemp5

	addt	$f25,$f13,$f25
	unop
	mult	$f10,$f0,$f13
	LD	$f0,   5*SIZE($2)	# atemp6

	addt	$f26,$f14,$f26
	unop
	mult	$f10,$f19,$f14
	LD	$f19,  6*SIZE($2)	# atemp7

	addt	$f27,$f15,$f27
	lda	$2,    8*SIZE($2)	# a1_offset += 8
	mult	$f10, $f2, $f15
	LD	$f10,  2*SIZE($4)	# b2_offset

	addt	$f12,$f11,$f12
	LD	$f2,  -1*SIZE($2)	# atemp8
	mult	$f1,$f22,$f11
	lda	$4,    4*SIZE($4)	# b1_offset ++

	addt	$f25,$f13,$f25
	mult	$f1,$f20,$f13
	addt	$f26,$f14,$f26
	mult	$f1,$f18,$f14

	addt	$f27,$f15,$f27
	mult	$f1,$f16,$f15
	addt	$f12,$f11,$f12
	mult	$f10,$f17,$f11

	addt	$f25,$f13,$f25
	mult	$f10,$f0,$f13
	addt	$f26,$f14,$f26
	mult	$f10,$f19,$f14

	addt	$f27,$f15,$f27
	unop
	mult	$f10,$f2,$f15
	unop
	.align 4

$L82:
	and	$7, 3, $5		# l = (min_l&3)
	beq	$5, $L87		# if l<=0 goto $L87

	LD	$f22, 0*SIZE($2)	# atemp1
	LD	$f1,  0*SIZE($4)	# b1_offset

	LD	$f20, 1*SIZE($2)	# atemp2
	LD	$f18, 2*SIZE($2)	# atemp3
	LD	$f16, 3*SIZE($2)	# atemp4
	lda	$2,   4*SIZE($2)	# a1_offset += 4

	subl	$5, 1, $5		# l--
	lda	$4,   2*SIZE($4)	# b1_offset += 2

	ble	$5,$L89			# if l<=0 goto $L89
	unop
	.align 4

$L90:
	addt	$f12,$f11,$f12
	unop
	mult	$f1,$f22,$f11
	LD	$f22,  0*SIZE($2)	# atemp1

	addt	$f25,$f13,$f25
	unop
	mult	$f1,$f20,$f13
	LD	$f20,  1*SIZE($2)	# atemp2

	addt	$f26,$f14,$f26
	unop
	mult	$f1,$f18,$f14
	LD	$f18,  2*SIZE($2)	# atemp3

	addt	$f27,$f15,$f27
	lda	$2,    4*SIZE($2)	# a1_offset += 4
	mult	$f1,$f16,$f15
	LD	$f1,   0*SIZE($4)	# btemp1

	LD	$f16, -1*SIZE($2)	# atemp4
	subl	$5, 1, $5		# l--
	lda	$4,    2*SIZE($4)	# b1_offset += 2
	bgt	$5, $L90		# if l>0 goto $L90
	.align 4

$L89:
	addt	$f12,$f11,$f12
	mult	$f1,$f22,$f11
	addt	$f25,$f13,$f25
	mult	$f1,$f20,$f13

	addt	$f26,$f14,$f26
	mult	$f1,$f18,$f14
	addt	$f27,$f15,$f27
	mult	$f1,$f16,$f15
	.align 4

$L87:
	addt	$f12,$f11,$f12
	LD	$f22, 0*SIZE($22)
	addt	$f25,$f13,$f25
	LD	$f20, 1*SIZE($22)

	addt	$f26,$f14,$f26
	LD	$f18, 2*SIZE($22)
	addt	$f27,$f15,$f27
	LD	$f16, 3*SIZE($22)

	mult	$f3,$f12,$f11
	mult	$f3,$f25,$f13
	mult	$f3,$f26,$f14
	mult	$f3,$f27,$f15

	addt	$f22,$f11,$f12
	addt	$f20,$f13,$f25
	addt	$f18,$f14,$f26
	addt	$f16,$f15,$f27

	ST	$f12, 0*SIZE($22)
	fclr	$f11
	ST	$f25, 1*SIZE($22)
	fclr	$f13
	ST	$f26, 2*SIZE($22)
	fclr	$f14
	ST	$f27, 3*SIZE($22)
	fclr	$f15

	subl	$8, 1, $8		# i--
	addq	$22, 4*SIZE, $22	# c1_offset += 4
	bgt	$8, $L81
	unop
	.align 4

$L79:
	and	$15,3,$8		# i = (min_i&3)
	beq	$8,$L29			# if i<=0 goto $L29
	unop
	unop
	.align 4

$L96:
	mov	$24, $2			# a1_offset = a_offset
	fclr	$f12
	addq	$24, SIZE, $24		# a_offset ++
	fclr	$f11

	mov	$0, $4			# b1_offset = b_offset
	mov	$7, $5			# l = min_l
	ble	$7,$L98			# if l<=0 goto $L98
	unop
	.align 4

$L100:
	LD	$f22, 0*SIZE($2)	# atemp1
	LD	$f1,  0*SIZE($4)	# btemp1

	lda	$2,   4*SIZE($2)	# a1_offset += 4
	lda	$4,   2*SIZE($4)	# b1_offset += 2

	addt	$f12,$f11,$f12
	subl	$5,1,$5			# l--
	mult	$f1,$f22,$f11
	bgt	$5,$L100		# if l>0 goto $L100
	.align 4

$L98:
	addt	$f12,$f11,$f12		# ctemp1 += temp1
	mult	$f3,$f12,$f11		# temp1 = alpha*ctemp1
	LD	$f22, 0*SIZE($22)	# atemp1 = *c1_offset
	addt	$f22,$f11,$f12		# ctemp1 = atemp1 + temp1

	subl	$8,1,$8			# i--
	ST	$f12, 0*SIZE($22)	# *c1_offset = ctemp1
	addq	$22, SIZE, $22		# c1_offset ++
	bgt	$8,$L96			# if i>0 goto $L98
	.align 4

$L29:
	lda	$11,  P($11)		# is += P
	nop
#ifndef C_VERSION
	cmplt	$11,$16,$1		# is < m ?
#else
	cmplt	$11,$17,$1		# is < m ?
#endif
	bne	$1,$L30
	.align 4

$L9:
	lda	$14, R($14)		# js += R
	nop
#ifndef C_VERSION
	cmplt	$14,$17,$1		# js < n ?
#else
	cmplt	$14,$16,$1		# js < n ?
#endif
	bne	$1,$L10
	.align 4

$L4:
	lda	$3, Q($3)		# ls += Q
	clr	$14			# js = 0
	cmplt	$3,$18,$1		# ls < k ??
	bne	$1,$L5
	.align 4

$L3:
	ldq	$26, 0($30)
	ldq	$9,  8($30)
	ldq	$10,16($30)
	ldq	$11,24($30)
	ldq	$12,32($30)
	ldq	$13,40($30)
	ldq	$14,48($30)
	ldq	$15,56($30)
	ldt	$f2,64($30)
	ldt	$f3,72($30)
	ldt	$f4,80($30)
	ldt	$f5,96($30)
	ldt	$f6,104($30)
	ldt	$f7,112($30)
	ldt	$f8,120($30)

	lda	$30, STACKSIZE($30)
	clr	$0

	ret	$31,($26),1
	.ident	VERSION
	.end	ROUTINE

/* Finish !! */
