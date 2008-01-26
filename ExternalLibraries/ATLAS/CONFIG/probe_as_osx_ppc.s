
; Linux PPC assembler for:
; int asm_probe(int i)
; RETURNS: i*3
.globl _asm_probe
_asm_probe:
	add	r4, r3, r3
	add	r3, r3, r4
	blr
