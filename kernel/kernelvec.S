# kernelvec
#include "trap.h"

.global kernelvec
.global kerneltrap 
.align 4

kernelvec:
    addi sp, sp, -256

    # save caller-saved registers
    sd ra, 0(sp)
    sd sp, 8(sp)
    sd gp, 16(sp)
    sd tp, 24(sp)
    sd t0, 32(sp)
    sd t1, 40(sp)
    sd t2, 48(sp)
    sd a0, 56(sp)
    sd a1, 64(sp)
    sd a2, 72(sp)
    sd a3, 80(sp)
    sd a4, 88(sp)
    sd a5, 96(sp)
    sd a6, 104(sp)
    sd a7, 112(sp)
    sd t3, 120(sp) 
    sd t4, 128(sp)
    sd t5, 136(sp)
    sd t6, 144(sp)

    call kerneltrap

    ld ra, 0(sp)
    ld sp, 8(sp)
    ld gp, 16(sp)
    ld tp, 24(sp)
    ld t0, 32(sp)
    ld t1, 40(sp)
    ld t2, 48(sp)
    ld a0, 56(sp)
    ld a1, 64(sp)
    ld a2, 72(sp)
    ld a3, 80(sp)
    ld a4, 88(sp)
    ld a5, 96(sp)
    ld a6, 104(sp)
    ld a7, 112(sp)
    ld t3, 120(sp) 
    ld t4, 128(sp)
    ld t5, 136(sp)
    ld t6, 144(sp)

    addi sp, sp, 256

    sret