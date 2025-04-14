#ifndef _proc_h_
#define _proc_h_

#include "types.h"
#include "spinlock.h"

// States of processes
enum ProcessStates {
    UNUSED,
    USED,
    SLEEPING,
    RUNNABLE,
    RUNNING,
    ZOMBIE
};

// context switch 
struct context {
    uint64_t ra; // Return address
    uint64_t sp; // Stack pointer

    // Callee-saved registers
    uint64_t s0; // frame pointer
    uint64_t s1;
    uint64_t s2;
    uint64_t s3;
    uint64_t s4;
    uint64_t s5;
    uint64_t s6;
    uint64_t s7;
    uint64_t s8;
    uint64_t s9;
    uint64_t s10;
    uint64_t s11;
};
    
struct cpu {
    int intena; // Saved interrupt enable bit
    int noff;   // Depth of push_off() nesting
    struct context context;
    struct proc* proc; // The process running on this cpu
};

struct proc {
    uint64_t pid;                // Process ID
    struct cpu* cpu;
    uint64_t sz;                 // Size of process memory (bytes)
    pagetable_t pagetable;       // Page table

    uint64_t kstack;
    int state;                   // process state

    struct context context;
    struct trapframe* trapframe; // Trap frame for current interrupt

    struct spinlock lock;
};

struct cpu* mycpu();
struct proc* myproc();

void proc_mapstack(pagetable_t pagetable);
void procinit();
void prochartinit(); // set kernelvec to stvec
void test_userinit();
struct proc* userinit();
void scheduler();

struct proc* allocproc();
void freeproc(struct proc* p);

void forkret();

void yield();

#endif