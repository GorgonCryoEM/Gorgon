
# x86-64 assembler for:
# int asm_probe(int i)
# RETURNS: i*3
#
.text
.global asm_probe
        .type asm_probe,@function
asm_probe:
        movl    %edi, %eax
        shl     $1, %eax
        addl    %edi, %eax
        ret
