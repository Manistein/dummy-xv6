#ifndef _memlayout_h_
#define _memlayout_h_

// base address of UART
#define UART0 0x10000000

// physical address of hardware
#define PGSIZE 4096
#define KERNEL_BASE 0x80000000L
#define PHYSTOP (KERNEL_BASE + 128 * 1024 * 1024)
#define PROUNDUP(sz) (((sz) + PGSIZE - 1) & ~((PGSIZE) - 1))

// The max virtual address of risc-v sv39
#define MAXVA (1L << (9 + 9 + 9 + 12 - 1))
#define TRAMPOLINE (MAXVA - PGSIZE)
#define TRAPFRAME (TRAMPOLINE - PGSIZE)
#define KSTACK(p) (TRAMPOLINE - ((p) + 1) * 2 * PGSIZE)

#define PGSHIFT 12
#define PGMASK 0x1FF
#define PTX(va, level) (((va) >> (PGSHIFT + (9 * (level)))) & PGMASK)
#define PA2PTE(pa) (((pa) >> PGSHIFT) << 10)
#define PTE2PA(pte) (((pte) >> 10) << PGSHIFT)

// page table entry flags
#define PTE_V 0x01
#define PTE_R 0x02
#define PTE_W 0x04
#define PTE_X 0x08
#define PTE_U 0x10

#endif 