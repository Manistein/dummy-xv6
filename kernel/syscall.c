#include "syscall.h"
#include "proc.h"
#include "printf.h"
#include "trap.h"

extern void sys_write();

static uint64_t (*syscalls[])(void) = {
    [SYS_write] = sys_write,
};

void syscall() {
    struct proc* p = myproc(); 
    uint64_t num = p->trapframe->a7;
    if (num > 0 && num < (sizeof(syscalls)/sizeof(syscalls[0])) && syscalls[num]) {
        p->trapframe->a0 = syscalls[num]();
    } else {
        printf("unknown sys call num %d\n", num);
    }
}