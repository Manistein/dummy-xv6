#include "memlayout.h"
#include "riscv.h"

.section trampsec

.global trampoline
.global usertrap
trampoline:
.align 4 

.global uservec
uservec:
    sret

.global userret
userret:
    sret