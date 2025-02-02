#include "common.h"
#include "proc.h"
#include "param.h"
#include "riscv.h"
#include "vm.h"

static struct cpu cpus[NCPU];
static struct proc procs[NPROC];

struct cpu* mycpu() {
    int id = cpuid();
    struct cpu* c = &cpus[id];
    return c;
}

void initproc() {
    for (int i = 0; i < NCPU; i++) {
        cpus[i].noff = 0;
        cpus[i].intena = 0;
    }
}

void proc_mapstack(pagetable_t pagetable) {
    for (int i = 0; i < NPROC; i++) {
        struct proc* p = &procs[i];

        uint64_t pa = (uint64_t)kalloc();
        if (pa == 0) {
            panic("proc_mapstack:fail to kalloc");
        }

        memset((void*)pa, 0, PGSIZE);

        uint64_t va = KSTACK(p - procs);
        kvmmap(pagetable, va, pa, PGSIZE, PTE_R | PTE_W);
    }
}