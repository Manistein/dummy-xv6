#include "common.h"
#include "spinlock.h"
#include "proc.h"
#include "printf.h"
#include "riscv.h"

void initlock(struct spinlock* lk, char* name) {
    lk->name = name;
    lk->locked = 0;
    lk->cpu = 0;
}

void acquire(struct spinlock* lk) {
    push_off();

    if (holding(lk)) {
        panic("acquire");
    }

    // atomic exchange
    while (__sync_lock_test_and_set(&lk->locked, 1));

    // Tell the C compiler and the processor to not move loads or stores
    // past this point, to ensure that the critical section's memory
    __sync_synchronize();
    
    lk->cpu = mycpu();
}

void release(struct spinlock* lk) {
    if (!holding(lk)) {
        panic("release");
    }

    lk->cpu = 0;

    // Tell the C compiler and the processor to not move loads or stores
    // past this point, to ensure that all the stores in the critical
    __sync_synchronize();

    __sync_lock_release(&lk->locked);

    pop_off();
}

int holding(struct spinlock* lk) {
    if (lk->locked && lk->cpu == mycpu()) {
        return 1;
    }
    else {
        return 0;
    }
}

void push_off() {
    int intr_ena = intr_get();
    if (mycpu()->noff == 0) {
        mycpu()->intena = intr_ena;
    }

    // disable interrupts to avoid deadlock.
    mycpu()->noff += 1;
    intr_off();
}

void pop_off() {
    mycpu()->noff -= 1;

    if (mycpu()->noff == 0 && mycpu()->intena) {
        intr_on();
    }
}