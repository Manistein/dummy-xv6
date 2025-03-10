# system call entries for user processes
#include "../kernel/syscall.h"

.global write
write:
    li a7 SYS_write
    ecall
    ret