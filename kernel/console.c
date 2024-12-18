#include "common.h"
#include "console.h"

void consoleinit(void) {
    uartinit();
}

void consoleputc(char c) {
    uartputc_sync(c);
}