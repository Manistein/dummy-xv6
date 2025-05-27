#include "common.h"
#include "printf.h"
#include "param.h"
#include "riscv.h"
#include "kalloc.h"
#include <string.h>
#include "vm.h"
#include "proc.h"
#include "plic.h"
#include "console.h"

static volatile int is_initialized = 0;

void main() {
    if (cpuid() == 0) {
        consoleinit();
        printfinit();

        printf("dummyxv6 booting!\n");
        printf("start to initialize the kernel.\n");
        kinit();

        kvminit();
        kvmhartinit();

        procinit();
        prochartinit();

        plicinit();
        plicinithart();

        test_userinit();
        
        // printf("CPU%d initialize success.\n", cpuid());

        __sync_synchronize();

        is_initialized = 1;
    } 
    else {
        while (is_initialized == 0);

        __sync_synchronize();

        printf("CPU%d starts initializing.\n", cpuid());

        kvmhartinit();
        prochartinit();
        plicinithart();

        printf("Initialization of CPU%d complete.\n", cpuid());
    }

    scheduler();
}