#ifndef _proc_h_
#define _proc_h_

#include "types.h"

struct cpu {
    int intena; // Saved interrupt enable bit
    int noff;   // Depth of push_off() nesting
};

struct proc {
    struct cpu* cpu;
    uint64_t sz; // Size of process memory (bytes)
    pagetable_t pagetable; // Page table
};

struct cpu* mycpu();
void initproc();
void proc_mapstack(pagetable_t pagetable);

#endif