#include "trap.h"
#include "riscv.h"
#include "types.h"
#include "printf.h"
#include "memlayout.h"
#include "proc.h"

extern char trampoline[], uservec[], userret[], kernelvec[];

void kerneltrap() {
    uint64_t sstatus = r_sstatus();
    if ((sstatus & SSTATUS_SPP_MASK) != 1) {
        panic("kerneltrap: not from supervisor mode");
    }

    if (intr_get() != 0) {
        panic("kerneltrap: interrupts enabled");
    }

    uint64_t sepc = r_sepc();
    uint64_t scause = r_scause();

    // TODO: handle interrupt
    int where_dev = 0;
    if ((where_dev = devintr(scause)) == 0) {
        printf("kerneltrap(): unexpected scause %p pid=%d\n", scause, p->pid);
        panic("kerneltrap: unexpected scause");
    }

    if (where_dev == 2) {
        yield();
    }
    
    w_sepc(sepc);
    w_sstatus(sstatus);
}

void usertrap() {
    struct proc* p = myproc();

    uint64_t sstatus = r_sstatus();
    if ((sstatus & SSTATUS_SPP_MASK) != 0) {
        panic("usertrap: not from user mode");
    }

    if (intr_get() != 0) {
        panic("usertrap: interrupts enabled");
    }

    uint64_t sepc = r_sepc();
    p->trapframe->epc = sepc;

    w_stvec(kernelvec);

    int which_dev = 0;
    uint64_t scause = r_scause();
    if (scause == 8) {
        // system call
        intr_on();
        syscall();
    }
    else if (which_dev = devintr(scause) == 0) {
        printf("usertrap(): unexpected scause %p pid=%d\n", scause, p->pid);
        panic("usertrap: unexpected scause");
    }
    else {
        panic("usertrap: not handled");
    }

    if (which_dev == 2) {
        yield();
    }
        
    usertrapret();
}

void usertrapret() {
    // Disable interrupt while running usertrapret
    intr_off();

    w_stvec((uint64_t)(TRAMPOLINE + (uservec - trampoline)));
    uint64_t trampoline_userret = TRAMPOLINE + (userret - trampoline);

    struct proc* p = myproc();
    p->trapframe->kernel_satp = r_satp();
    p->trapframe->kernel_sp = p->kstack + PGSIZE;
    p->trapframe->kernel_trap = (uint64_t)usertrap;
    p->trapframe->kernel_hartid = r_tp();
    
    uint64_t sstatus = r_sstatus();
    sstatus = (sstatus & ~SSTATUS_SPP_MASK)  | 0;   // return to user mode
    sstatus = sstatus | SSTATUS_SPIE_MASK;          // enable interrupts in user mode
    w_sstatus(sstatus);

    w_sepc(p->trapframe->epc);

    uint64_t satp = MAKE_SATP((uint64_t)p->pagetable);
    ((void (*)(uint64_t))trampoline_userret)(satp);
}

static void clockintr() {
    // ask for a timer interrupt in 100ms later
    uint64_t timecmp = r_time() + 1000000;
    w_stimecmp(timecmp);
}

int devintr(uint64_t scause) {
    // timer interrupt
    if (scause == 0x8000000000000009L) {
        // TODO
        return 1;
    }
    // timer interrupt
    else if (scause == 0x8000000000000005L) {
        clockintr();
        return 2;
    }
    else {
        printf("unexpected scause %p\n", scause);
    }

    return 0;
}