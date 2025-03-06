#include "common.h"
#include "proc.h"
#include "param.h"
#include "riscv.h"
#include "vm.h"
#include "kalloc.h"
#include "string.h"
#include "printf.h"
#include "trap.h"

void kernelvec();
void swtch(struct context** old, struct context* new);

void sched();

extern char trampoline[];

// States of processes
enum ProcessStates {
    UNUSED,
    USED,
    SLEEPING,
    RUNNABLE,
    RUNNING,
    ZOMBIE
};

static struct cpu cpus[NCPU];
static struct proc procs[NPROC];

static struct proc* initproc;
static struct proc* initproc2; // for test;

struct spinlock alloclock;
static uint64_t nextpid = 0;

struct cpu* mycpu() {
    int id = cpuid();
    struct cpu* c = &cpus[id];
    return c;
}

struct proc* myproc() {
    struct proc* p = NULL;
    push_off();
    struct cpu* c = mycpu();
    p = c->proc;
    pop_off();
    return p;
}

static void initcpus() {
    for (int i = 0; i < NCPU; i++) {
        cpus[i].noff = 0;
        cpus[i].intena = 0;
        memset(&cpus[i].context, 0, sizeof(cpus[i].context));
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

void procinit() {
    initlock(&alloclock, "proc_alloc");

    initcpus();

    for (int i = 0; i < NPROC; i ++) {
        struct proc* p = &procs[i];
        p->state = UNUSED;
        p->kstack = (uint64_t)KSTACK((uint64_t)(p - procs));
        initlock(&p->lock, "proc");
        memset(&p->context, 0, sizeof(p->context));
    }
}

void prochartinit() {
    w_stvec(kernelvec); 
}

void test_userinit() {
    printf("test_userinit\n");

    initproc = userinit();
    initproc2 = userinit();
}

static char initcode[] = {
    // TODO
};

struct proc* userinit() {
    struct proc* p = allocproc();
    if (p == NULL) {
        panic("fail to allocproc");
    }

    void* mem = kalloc();
    if (mem == 0) {
        panic("fail to kalloc");
    }

    memcpy(mem, initcode, sizeof(initcode));

    if (!mappages(p->pagetable, 0, PGSIZE, (uint64_t)mem, PTE_R | PTE_X)) {
        panic("fail to mappages initcode");
    }

    p->sz = PGSIZE;
    p->context.ra = forkret;
    p->context.sp = p->kstack + PGSIZE;
    p->state = RUNNABLE;

    return p;
}

void scheduler() {
    struct proc* p = NULL;
    struct cpu* c = mycpu();
    c->proc = NULL;

    for (;;) {
        intr_on();

        int found = 0;
        for (int i = 0; i < NPROC; i++) {
            p = &procs[i];
            acquire(&p->lock);
            if (p->state == RUNNABLE) {
                found = 1;

                p->state = RUNNING;
                c->proc = p;
                swtch(&c->context, &p->context);

                c->proc = NULL;
            }
            release(&p->lock);
        }

        if (found == 0) {
            intr_on();
            asm volatile("wfi");
        }
    }
}

static uint64_t allocpid() {
    uint64_t pid = 0;

    acquire(&alloclock);
    pid = nextpid++;
    release(&alloclock);

    return pid;
}

struct proc* allocproc() {
    struct proc* p = NULL;
    for (int i = 0; i < NPROC; i++) {
        p = &procs[i];
        acquire(&p->lock);
        if (p->state == UNUSED) {
            p->state = USED;
            release(&p->lock);
            goto found;
        }
        else {
            release(&p->lock);
        }
    }

    return NULL;
found:
    p->pid = allocpid();

    p->pagetable = uvmcreate();
    if (p->pagetable == 0) {
        panic("fail to uvmcreate");
    }

    p->trapframe = (struct trapframe*)kalloc();
    if (p->trapframe == 0) {
        panic("fail to kalloc trapframe");
    }

    if (!mappages(p->pagetable, TRAPFRAME, PGSIZE, (uint64_t)p->trapframe, PTE_R | PTE_W)) {
        panic("fail to mappages trapframe");
    }

    if (!mappages(p->pagetable, TRAMPOLINE, PGSIZE, (uint64_t)trampoline, PTE_R | PTE_X)) {
        panic("fail to mappages trampoline");
    }
    
    return p;
}

void freeproc(struct proc* p) {
    acquire(&p->lock);

    p->state = UNUSED;

    uvmfree(p->pagetable, p->sz);
    p->pagetable = 0;
    kfree((void*)p->trapframe);
    p->trapframe = 0;

    release(&p->lock);
}

void forkret() {
    struct proc* p = myproc();
    release(&p->lock);

    p->trapframe->epc = 0;

    void* stack = (void*)kalloc();
    if (stack == 0) {
        panic("fail to kalloc stack");
    }

    p->trapframe->sp = p->sz + PGSIZE;
    p->sz += PGSIZE;

    if (!mappages(p->pagetable, p->sz - PGSIZE, PGSIZE, (uint64_t)stack, PTE_R | PTE_W)) {
        panic("fail to mappages stack");
    }

    usertrapret();
}

void yield() {
    struct proc* p = myproc();
    acquire(&p->lock);
    p->state = RUNNABLE;
    sched();
    release(&p->lock);
}

void sched() {
    struct cpu* c = mycpu();
    struct proc* p = c->proc;

    if (c->noff != 1) {
        panic("sched locks");
    }

    if (p->state == RUNNING) {
        panic("sched running");
    }

    if (intr_get()) {
        panic("sched interruptible");
    }

    int intena = c->intena;
    swtch(&p->context, &c->context);
    c->intena = intena;
}