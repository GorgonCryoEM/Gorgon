
# x86-32 assembler for:
# int asm_probe(int i)
# RETURNS: i*3
#
.text
.global asm_probe
        .type asm_probe,@function
asm_probe:
        movl    4(%esp), %eax
        movl    %eax, %ecx
        shl     $1, %eax
        addl    %ecx, %eax
        ret
