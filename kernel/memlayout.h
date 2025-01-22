#ifndef _memlayout_h_
#define _memlayout_h_

// base address of UART
#define UART0 0x10000000

// physical address of hardware
#define PGSIZE 4096
#define KERNEL_BASE 0x80000000L
#define MAX_MEM_LIMIT (KERNEL_BASE + 128 * 1024 * 1024)
#define PROUNDUP(sz) (((sz) + PGSIZE - 1) & ~((PGSIZE) - 1))

#endif 