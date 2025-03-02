#ifndef _trap_h_
#define _trap_h_

#include "common.h"

struct trapframe {
    uint64_t kernel_satp; // kernel page table
    uint64_t kernel_sp;   // top of process's kernel stack
    uint64_t kernel_trap; // user trap number
    uint64_t kernel_hartid; // core number

    uint64_t epc;         // exception program counter

    uint64_t ra; // Return address
    uint64_t sp; // Stack pointer

    uint64_t gp; // Global pointer
    uint64_t tp; // Thread pointer

    uint64_t t0; // Temporaries
    uint64_t t1;
    uint64_t t2;

    uint64_t s0; // frame pointer
    uint64_t s1;
    
    uint64_t a0; // Arguments
    uint64_t a1;
    uint64_t a2;
    uint64_t a3;
    uint64_t a4;
    uint64_t a5;
    uint64_t a6;
    uint64_t a7;

    uint64_t s2; // Saved registers
    uint64_t s3;
    uint64_t s4;
    uint64_t s5;
    uint64_t s6;
    uint64_t s7;
    uint64_t s8;
    uint64_t s9;
    uint64_t s10;
    uint64_t s11;

    uint64_t t3; // Temporaries
    uint64_t t4;
    uint64_t t5;
    uint64_t t6;
};

void usertrapret();

#endif 