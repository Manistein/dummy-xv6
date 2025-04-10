#include "common.h"
#include "vm.h"
#include "printf.h"
#include "proc.h"
#include "riscv.h"
#include "string.h"
#include "kalloc.h"

extern char etext[]; // kernel.ld sets this to the end of the kernel code.
extern char trampoline[]; // trampoline.S

static pagetable_t kpgtbl;

// kvminit initializes the kernel's page table.
void kvminit() {
    kvmmake();
}

void kvmhartinit() {
    sfence_vma();

    w_satp(MAKE_SATP((uint64_t)kpgtbl));

    sfence_vma();
}

void kvmmake() {
    kpgtbl = (pagetable_t)kalloc();
    if (kpgtbl == 0) {
        panic("kvmmake:fail to kalloc");
    }
    memset(kpgtbl, 0, PGSIZE);

    // uart registers
    kvmmap(kpgtbl, UART0, UART0, PGSIZE, PTE_R | PTE_W);

    // kernel text
    kvmmap(kpgtbl, KERNEL_BASE, KERNEL_BASE, (uint64_t)etext - KERNEL_BASE, PTE_R | PTE_X);

    // kernel data
    kvmmap(kpgtbl, (uint64_t)etext, (uint64_t)etext, PHYSTOP - (uint64_t)etext, PTE_R | PTE_W);

    // trampoline
    kvmmap(kpgtbl, TRAMPOLINE, (uint64_t)trampoline, PGSIZE, PTE_R | PTE_X);

    proc_mapstack(kpgtbl);
}

void kvmmap(pagetable_t kpgtbl, uint64_t va, uint64_t pa, uint64_t sz, int perm) {
    if (!mappages(kpgtbl, va, sz, pa, perm)) {
        panic("kvmmap:fail to map pages");
    }
}

int mappages(pagetable_t pagetable, uint64_t va, uint64_t size, uint64_t pa, int perm) {
    if (va % PGSIZE != 0)
        panic("mappages:va is not page align");
    
    if (pa % PGSIZE != 0)
        panic("mappages:pa is not page align");
    
    if (size % PGSIZE != 0)
        panic("mappages:size is not page align");

    uint64_t a, last;
    pte_t* pte;

    a = va;
    last = va + size;

    // printf("pagetable:0x%x -------------\n", (uint64_t)pagetable);
    for (; a < last; a += PGSIZE, pa += PGSIZE) {
        // printf("map pa:0x%x\n", pa);

        if ((pte = walk(pagetable, a, 1)) == 0)
            return 0;

        if (*pte & PTE_V)
            panic("mapages: remap");

        *pte = PA2PTE(pa) | perm | PTE_V;
    }

    return 1;
}

// walk returns a pointer to the PTE in the pagetable for the virtual address.
pte_t* walk(pagetable_t pagetable, uint64_t va, int alloc) {
    pte_t* pte;

    for (int level = 2; level > 0; level--) {
        pte = &pagetable[PTX(va, level)];
        // If the PTE is valid, just use it.
        if (*pte & PTE_V) {
            pagetable = (pagetable_t)PTE2PA(*pte);
        } else 
        // If the PTE is invalid, allocate a new page table.
        {
            if (!alloc || (pagetable = (pagetable_t)kalloc()) == 0)
                return 0;
            memset(pagetable, 0, PGSIZE);
            *pte = PA2PTE((uint64_t)pagetable) | PTE_V;
        }
    }

    return &pagetable[PTX(va, 0)];
}

int uvmunmap(pagetable_t pagetable, uint64_t va, uint64_t size, int do_free) {
    uint64_t a, last;
    pte_t* pte;

    if (va % PGSIZE != 0)
        panic("uvmunmap:va is not page align");
    
    if (size % PGSIZE != 0)
        panic("uvmunmap:size is not page align");
    
    a = va;
    last = va + size;

    for (; a < last; a += PGSIZE) {
        if ((pte = walk(pagetable, a, 0)) == 0)
            panic("uvmunmap: walk");
        if (*pte & PTE_V) {
            if (do_free) {
                uint64_t pa = PTE2PA(*pte);
                kfree((void*)pa);
            }
            *pte = 0;
        }
    }

    return 1;
}

// uvmcreate creates a new pagetable for a user process as its root pagetable.
pagetable_t uvmcreate() {
    pagetable_t pagetable;
    pagetable = (pagetable_t)kalloc();
    if (pagetable == 0)
        return 0;
    memset(pagetable, 0, PGSIZE);
    return pagetable;
}

void uvmfree(pagetable_t pagetable, uint64_t sz) {
    uvmdealloc(pagetable, sz, 0);
    freewalk(pagetable);
}

int uvmalloc(pagetable_t pagetable, uint64_t oldsz, uint64_t newsz, int perm) {
    if (newsz <= oldsz)
        return oldsz; 
    
    for (uint64_t a = oldsz; a < PROUNDUP(newsz); a += PGSIZE) {
        uint64_t pa = (uint64_t)kalloc();
        if (pa == 0) {
            uvmdealloc(pagetable, a, oldsz);
            return 0;
        }
        memset((void*)pa, 0, PGSIZE);
        if (!mappages(pagetable, a, PGSIZE, pa, perm)) {
            uvmdealloc(pagetable, a, oldsz);
            kfree((void*)pa);
            return 0;
        }
    }

    return newsz;
}

int uvmdealloc(pagetable_t pagetable, uint64_t oldsz, uint64_t newsz) {
    if (oldsz <= newsz)
        return oldsz;

    uvmunmap(pagetable, newsz, oldsz - newsz, 1);
    
    return newsz;
}

int freewalk(pagetable_t pagetable) {
    for (int i = 0; i < 512; i++) {
        pte_t* pte = &pagetable[i];
        if ((*pte & PTE_V) && ((*pte & (PTE_R | PTE_W | PTE_X | PTE_U)) == 0)) {
            pagetable_t child = (pagetable_t)PTE2PA(*pte);
            freewalk(child);
            pagetable[i] = 0;
        } else if (*pte & PTE_V) {
            panic("freewalk: not a leaf");
        }
    }
    kfree((void*)pagetable);
    return 1;
}