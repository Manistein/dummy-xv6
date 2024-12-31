#include "common.h"
#include "printf.h"
#include "param.h"
#include "riscv.h"

static volatile int is_initialized = 0;

void main() {
    if (cpuid() == 0) {
        uartinit();
        printfinit();

        printf("dummyxv6 booting!\n");
        printf("start to initialize the kernel.\n");

        printf("CPU%d initialize success.\n", cpuid());
        is_initialized = 1;

        __sync_synchronize();
    } 
    else {
        while (is_initialized == 0);

        printf("CPU%d starts initializing.\n", cpuid());

        // todo

        printf("Initialization of CPU%d complete.\n", cpuid());
    }
}