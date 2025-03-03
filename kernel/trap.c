#include "trap.h"
#include "riscv.h"
#include "types.h"
#include "printf.h"
#include "memlayout.h"
#include "proc.h"

extern char trampoline[], uservec[], userret[];

void usertrap() {

}

void usertrapret() {
    // Disable interrupt while running usertrapret
    intr_off();

    w_stvec((uint64_t)(TRAMPOLINE + (uservec - trampoline)));
    uint64_t trampoline_userret = TRAMPOLINE + (userret - trampoline);

    struct proc* p = myproc();
    p->trapframe->kernel_satp = r_satp();
    p->trapframe->kernel_sp = r_sp();
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

