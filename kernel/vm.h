#ifndef _vm_h_
#define _vm_h_

#include "types.h"

void kvminit();
void kvmhartinit();
void kvmmake();
void kvmmap(pagetable_t kpgtbl, uint64_t va, uint64_t pa, uint64_t sz, int perm);
int mappages(pagetable_t pagetable, uint64_t va, uint64_t size, uint64_t pa, int perm);
pte_t* walk(pagetable_t pagetable, uint64_t va, int alloc);

int uvmunmap(pagetable_t pagetable, uint64_t va, uint64_t size, int do_free);
pagetable_t uvmcreate();
void uvmfree(pagetable_t pagetable, uint64_t sz);
int uvmalloc(pagetable_t pagetable, uint64_t oldsz, uint64_t newsz, int perm);
int uvmdealloc(pagetable_t pagetable, uint64_t oldsz, uint64_t newsz);
int freewalk(pagetable_t pagetable);

#endif