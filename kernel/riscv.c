#include "riscv.h"

int cpuid() {
    int x = r_tp();
    return x;
}

// for machine mode
uint64_t r_mhartid() {
    uint64_t x;
    asm volatile("csrr %0, mhartid" : "=r" (x));
    return x;
}

uint64_t r_tp() {
    uint64_t x;
    asm volatile("mv %0, tp" : "=r" (x));
    return x;
}

void w_tp(uint64_t val) {
    asm volatile("mv tp, %0" : : "r" (val));
}

uint64_t r_mstatus() {
    uint64_t x;
    asm volatile("csrr %0, mstatus" : "=r" (x));
    return x;
}

void w_mstatus(uint64_t val) {
    asm volatile("csrw mstatus, %0" : : "r" (val));
}

uint64_t r_mepc() {
    uint64_t x;
    asm volatile("csrr %0, mepc" : "=r" (x));
    return x;
}

void w_mepc(uint64_t val) {
    asm volatile("csrw mepc, %0" : : "r" (val));
}

uint64_t r_mideleg() {
    uint64_t x;
    asm volatile("csrr %0, mideleg" : "=r" (x));
    return x;
}

void w_mideleg(uint64_t val) {
    asm volatile("csrw mideleg, %0" : : "r" (val));
}

uint64_t r_medeleg() {
    uint64_t x;
    asm volatile("csrr %0, medeleg" : "=r" (x));
    return x;
}

void w_medeleg(uint64_t val) {
    asm volatile("csrw medeleg, %0" : : "r" (val));
}

void w_pmpaddr0(uint64_t val) {
    asm volatile("csrw pmpaddr0, %0" : : "r" (val));
}

void w_pmpcfg0(uint8_t val) {
    asm volatile("csrw pmpcfg0, %0" : : "r" (val));
}

uint64_t r_mie() {
    uint64_t x;
    asm volatile("csrr %0, mie" : "=r" (x));
    return x;
}

void w_mie(uint64_t val) {
    asm volatile("csrw mie, %0" : : "r" (val));
}

void w_menvcfg(uint64_t val) {
    asm volatile("csrw 0x30a, %0" : : "r" (val));
}

uint64_t r_menvcfg() {
    uint64_t x;
    asm volatile("csrr %0, 0x30a" : "=r" (x));
    return x;
}

uint32_t r_mcounteren() {
    uint32_t x;
    asm volatile("csrr %0, mcounteren" : "=r" (x));
    return x;
}

void w_mcounteren(uint64_t val) {
    asm volatile("csrw mcounteren, %0" : : "r" (val));
}

// for supervisor mode
uint64_t r_sstatus() {
    uint64_t x;
    asm volatile("csrr %0, sstatus" : "=r" (x));
    return x;
}

void w_sstatus(uint64_t val) {
    asm volatile("csrw sstatus, %0" : : "r" (val));
}

uint64_t r_sepc() {
    uint64_t x;
    asm volatile("csrr %0, sepc" : "=r" (x));
    return x;
}

void w_sepc(uint64_t val) {
    asm volatile("csrw sepc, %0" : : "r" (val));
}

uint64_t r_satp() {
    uint64_t x;
    asm volatile("csrr %0, satp" : "=r" (x));
    return x;
}

void w_satp(uint64_t val) {
    asm volatile("csrw satp, %0" : : "r" (val));
}

uint64_t r_stvec() {
    uint64_t x;
    asm volatile("csrr %0, stvec" : "=r" (x));
    return x;
}

void w_stvec(uint64_t val) {
    asm volatile("csrw stvec, %0" : : "r" (val));
}

uint64_t r_sie() {
    uint64_t x;
    asm volatile("csrr %0, sie" : "=r" (x));
    return x;
}

void w_sie(uint64_t val) {
    asm volatile("csrw sie, %0" : : "r" (val));
}

uint64_t r_sip() {
    uint64_t x;
    asm volatile("csrr %0, sip" : "=r" (x));
    return x;
}

void w_sip(uint64_t val) {
    asm volatile("csrw sip, %0" : : "r" (val));
}

uint64_t r_time() {
    uint64_t x;
    asm volatile("csrr %0, time" : "=r" (x));
    return x;
}

uint64_t r_stimecmp() {
    uint64_t x;
    asm volatile("csrr %0, 0x14d" : "=r" (x)); // for 64-bit
    return x;
}

void w_stimecmp(uint64_t val) {
    asm volatile("csrw 0x14d, %0" : : "r" (val)); // for 64-bit
}

uint64_t r_scause() {
    uint64_t x;
    asm volatile("csrr %0, scause" : "=r" (x));
    return x;
}

int intr_get() {
    int x = r_sstatus();
    return (x & SSTATUS_SIE_MASK) != 0;
}

void intr_off() {
    w_sstatus(r_sstatus() & ~SSTATUS_SIE_MASK);
}

void intr_on() {
    w_sstatus(r_sstatus() | SSTATUS_SIE_MASK);
}

void sfence_vma() {
    asm volatile("sfence.vma zero, zero");
}