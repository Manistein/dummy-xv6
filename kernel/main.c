#include "common.h"
#include "printf.h"
#include "param.h"
#include "riscv.h"

void main() {
    if (cpuid() == 0) {
        uartinit();
        printf("dummy-xv6:booting!\n");
        printf("cpu0:hello world!\n");

        printf("----test printf begin----\n");
        printf("char: %c\n", 'a');
        printf("string: %s\n", "hello world");

        printf("decimal: %d\n", 123);
        printf("long: %ld\n", 1234567890);
        printf("long long: %lld\n", 1234567890123456789);

        printf("hex: %x\n", 0x123);
        printf("long hex: %lx\n", 0x1234567890);
        printf("long long hex: %llx\n", 0x1234567890123456);

        printf("unsigned: %u\n", 123);
        printf("long unsigned: %lu\n", 1234567890);
        printf("long long unsigned: %llu\n", 1234567890123456789);

        printf("pointer: %p\n", (void*)0x12345678);

        printf("%%\n");
        printf("\taaa\n");
        printf("\abbb\n");
        printf("----test printf end----\n");

        printf("test lld: %lld\n test x:%x\n", 1234, 0x1234);
    } 
    else {

    }
}