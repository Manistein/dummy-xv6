# qemu -kernel loads the kernel at 0x80000000
# and cause every core to jump here.
# The start address of _entry is also at 0x80000000 in corresponding elf file.

.section .text
.global _entry
_entry:
    # load address of the stack0 array which is defined in 
    # kernel/start.c into the sp(stack pointer) register
    la sp, stack0

    # since our stack expands from higher address to lower address
    # so the sp must points to the highest address of current stack space
    li a0, 4096
    csrr a1, mhartid
    addi a1, a1, 1
    mul a0, a0, a1
    addi sp, sp, a0

    # jump to start funcion in kernel/start.c
    call start
spin: 
    j spin