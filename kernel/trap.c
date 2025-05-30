#include "trap.h"
#include "riscv.h"
#include "types.h"
#include "printf.h"
#include "memlayout.h"
#include "proc.h"
#include "syscall.h"
#include "plic.h"
#include "console.h"

extern char trampoline[], uservec[], userret[], kernelvec[];

/**
 * Handles traps (exceptions and interrupts) that occur while in kernel mode.
 * Verifies the trap occurred in supervisor mode with interrupts disabled.
 * Processes device interrupts and yields CPU on timer interrupts.
 * Panics if trap cause is unexpected or trap conditions are invalid.
 *
 * Note: This function assumes it is called with interrupts disabled and
 * in supervisor mode. It preserves and restores sepc and sstatus.
 */
void kerneltrap()
{
    uint64_t sstatus = r_sstatus();
    if (((sstatus & SSTATUS_SPP_MASK) >> 8) != 1)
    {
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
        panic("kerneltrap: unexpected scause");
    }

    struct proc *p = myproc();
    if (where_dev == 2 && (p != NULL) && p->state == RUNNING) {
        yield();
    }
    
    w_sepc(sepc);
    w_sstatus(sstatus);
}

/*
 * Handle trap from user space.
 * Checks if trap originated from user mode and handles different trap causes:
 * - System calls (scause = 8): Enables interrupts and calls syscall handler
 * - Device interrupts: Handles via devintr()
 * - Timer interrupts (which_dev = 2): Yields CPU
 * Panics on unexpected trap causes or if trap not from user mode.
 * Saves trap state and returns to user mode via usertrapret().
 */
void usertrap()
{
    struct proc *p = myproc();

    uint64_t sstatus = r_sstatus();
    if ((sstatus & SSTATUS_SPP_MASK) != 0) {
        panic("usertrap: not from user mode");
    }

    if (intr_get() != 0) {
        panic("usertrap: interrupts enabled");
    }

    uint64_t sepc = r_sepc();
    p->trapframe->epc = sepc;

    w_stvec((uint64_t)kernelvec);

    int which_dev = 0;
    uint64_t scause = r_scause();
    if (scause == 8) {
        // system call
        intr_on();
        syscall();
    }
    else if ((which_dev = devintr(scause)) == 0) {
        printf("usertrap(): unexpected scause %p pid=%d\n", scause, p->pid);
        panic("usertrap: unexpected scause");
    }

    if (which_dev == 2) {
        // test printf
        // printf("usertrap():: pid:%d before yield in core %d\n", p->pid, cpuid()); 
        yield();
        // printf("usertrap():: pid:%d after yield in core %d\n", p->pid, cpuid()); 
    }
        
    usertrapret();
}

/*
 * Return to user space after handling a trap.
 * Switches from kernel mode to user mode by:
 * 1. Setting up trap vector and userret trampoline
 * 2. Saving kernel context in trapframe
 * 3. Configuring status register for user mode
 * 4. Restoring user program counter
 * 5. Switching to user page table
 * Must be called with interrupts disabled
 */
void usertrapret()
{
    // Disable interrupt while running usertrapret
    intr_off();

    // Set up trap vector and userret trampoline
    w_stvec((uint64_t)(TRAMPOLINE + (uservec - trampoline)));
    uint64_t trampoline_userret = TRAMPOLINE + (userret - trampoline);

    struct proc* p = myproc();
    p->trapframe->kernel_satp = r_satp();           // save kernel page table
    p->trapframe->kernel_sp = p->kstack + PGSIZE;   // save kernel stack pointer
    p->trapframe->kernel_trap = (uint64_t)usertrap; // save usertrap function address
    p->trapframe->kernel_hartid = r_tp();           // save hart id 
    
    uint64_t sstatus = r_sstatus();
    sstatus = (sstatus & ~SSTATUS_SPP_MASK);   // return to user mode
    sstatus = sstatus | SSTATUS_SPIE_MASK;     // enable interrupts in user mode
    w_sstatus(sstatus);

    w_sepc(p->trapframe->epc); // restore user program counter

    uint64_t satp = MAKE_SATP((uint64_t)p->pagetable);
    ((void (*)(uint64_t))trampoline_userret)(satp); // jump to userret trampoline
}

static void clockintr() {
    // ask for a timer interrupt in 1000ms later
    w_stimecmp(r_time() + 10000000);
}

int devintr(uint64_t scause) { 
    // check if the trap is from a device interrupt
    if (scause == 0x8000000000000009L) {
        int irq = plicclaim();

        if (irq == UART0_IRQ) {
            uartintr();
        } else if (irq != 0) {
            printf("devintr(): unexpected IRQ %d\n", irq);
        }

        pliccomplete(irq);
        return 1;
    }
    // timer interrupt
    else if (scause == 0x8000000000000005L) {
        clockintr();
        return 2;
    }
    else {
        printf("devintr():unexpected scause %llu\n", scause);
    }

    return 0;
}