#include "sysfile.h"
#include "proc.h"
#include "printf.h"
#include "trap.h"

uint64_t sys_write() {
    struct proc* p = myproc();
    int fd = p->trapframe->a0;
    char* s = (char*)p->trapframe->a1;
    // int n = p->trapframe->a2;

    if (fd == 1) {
        printf("%s", s);
    }

    return 0;
}