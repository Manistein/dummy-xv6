#include "param.h"
#include "riscv.h"

// The stack space for each CPU to run start function in machine mode.
__attribute__((aligned(16))) char stack0[4096 * NCPU];

void timerinit();

void start() {
    // Switch to supervisor mode after calling mret.
    uint64_t mstatus_v = r_mstatus();
    mstatus_v = ((mstatus_v & (~MSTATUS_MPP_MASK)) | MSTATUS_MPP_S);    
    w_mstatus(mstatus_v);

    // Jump to main function in supervisor mode after calling mret.
    w_mepc((uint64_t)main);

    // Disable page table now,
    w_satp(0);

    // Delegate all interrupts and exceptions to supervisor mode.
    w_mideleg(0xffff);
    w_medeleg(0xffff);
    w_sie(r_sie() | SIE_SEIE | SIE_STIE | SIE_SSIE);
    
    // Save the hartid to tp register.
    uint64_t mhartid = r_mhartid();
    w_tp(mhartid);

    w_pmpaddr0(0x3fffffffffffffull);
    w_pmpcfg0(0xf);

    timerinit();

    asm volatile("mret");
}

void timerinit() {
    // Enable supervisor timer interrupt.
    w_mie(r_mie() | MIE_STIE);

    // Set STCE bit to 1 to enable the timer in supervisor mode.
    w_menvcfg(1ul << 63);

    // Set TM bit to 1 to enable the timer in supervisor mode.
    w_mcounteren(r_mcounteren() | 2);

    // A timer interrupt will be triggered after 1s.
    w_stimecmp(r_time() + 1000000);
}