#include "common.h"
#include "printf.h"
#include "param.h"
#include "riscv.h"
#include "kalloc.h"
#include <string.h>
#include "vm.h"

static volatile int is_initialized = 0;

void main() {
    if (cpuid() == 0) {
        uartinit();
        printfinit();

        printf("dummyxv6 booting!\n");
        printf("start to initialize the kernel.\n");

        kinit();

        kvminit();
        kvmhartinit();
        
        printf("CPU%d initialize success.\n", cpuid());
        is_initialized = 1;

        __sync_synchronize();
    } 
    else {
        while (is_initialized == 0);

        printf("CPU%d starts initializing.\n", cpuid());

        // test kalloc
        void *p = kalloc();
        strcpy(p, "hello, world!\n");
        printf("CPU%d: %s", cpuid(), p);
        kfree(p); 

        printf("Initialization of CPU%d complete.\n", cpuid());
    }
}