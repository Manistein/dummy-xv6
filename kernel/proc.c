#include "common.h"
#include "proc.h"
#include "param.h"
#include "riscv.h"

static struct cpu cpus[NCPU];

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