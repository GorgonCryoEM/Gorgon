
# Linux PPC assembler for:
# int asm_probe(int i)
# RETURNS: i*3
.globl asm_probe
asm_probe:
	add	4, 3, 3
	add	3, 3, 4
	blr
