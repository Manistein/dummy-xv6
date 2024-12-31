#ifndef _spinlock_h_
#define _spinlock_h_

struct spinlock {
    unsigned int locked;
    char* name;
    struct cpu* cpu;
};

void initlock(struct spinlock* lk, char* name);
void acquire(struct spinlock* lk);
void release(struct spinlock* lk);
int holding(struct spinlock* lk);

void push_off();
void pop_off();

#endif