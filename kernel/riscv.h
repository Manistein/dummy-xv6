#ifndef _riscv_h_
#define _riscv_h_

#include "common.h" 

// mask for the mstatus and sstatus registers
#define MSTATUS_SIE_MASK 0x0000000000000002L
#define MSTATUS_MIE_MASK 0x0000000000000008L
#define MSTATUS_SPP_MASK 0x0000000000000100L
#define MSTATUS_MPP_MASK 0x0000000000001800L

#define MSTATUS_MPP_M (3 << 11L)
#define MSTATUS_MPP_S (1 << 11L)
#define MSTATUS_MPP_U (0 << 11L)

#define MIE_STIE (1 << 5L)

#define SSTATUS_SIE_MASK MSTATUS_SIE_MASK
#define SSTATUS_SPP_MASK MSTATUS_SPP_MASK
#define SSTATUS_SPIE_MASK (1 << 5L)

#define SIE_SSIE (1 << 1L)
#define SIE_STIE (1 << 5L)
#define SIE_SEIE (1 << 9L)

// use riscv sv39 scheme
#define SATP_SV39 (8L << 60)
// sv39 scheme
#define MAKE_SATP(pagetable) (SATP_SV39 | ((uint64_t)pagetable >> 12))

int cpuid();

// for machine mode
uint64_t r_mhartid();
uint64_t r_tp();
void w_tp(uint64_t val);

uint64_t r_mstatus();
void w_mstatus(uint64_t val); 

uint64_t r_mideleg();
void w_mideleg(uint64_t val);

uint64_t r_medeleg();
void w_medeleg(uint64_t val);

uint64_t r_mepc();
void w_mepc(uint64_t val);

void w_pmpaddr0(uint64_t val);
void w_pmpcfg0(uint8_t val);

uint64_t r_mie();
void w_mie(uint64_t val);

void w_menvcfg(uint64_t val);
uint64_t r_menvcfg();

uint32_t r_mcounteren();
void w_mcounteren(uint64_t val);


// for supervisor mode
uint64_t r_sstatus();
void w_sstatus(uint64_t val);

uint64_t r_sepc();
void w_sepc(uint64_t val);

uint64_t r_satp();
void w_satp(uint64_t val);

uint64_t r_stvec();
void w_stvec(uint64_t val);

uint64_t r_sie();
void w_sie(uint64_t val);

uint64_t r_time();
void w_stimecmp(uint64_t val);

uint64_t r_scause();

int intr_get();
void intr_off();
void intr_on();

void sfence_vma();

#endif