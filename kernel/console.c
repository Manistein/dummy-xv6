#include "common.h"
#include "console.h"

void consoleinit(void) {
    uartinit();
}

void consoleputc(int c) {
    if (c == BACKSPACE) {
        uartputc_sync('\b');
        uartputc_sync(' ');
        uartputc_sync('\b');
    }
    else {
        uartputc_sync(c);
    }
}

void consoleintr(int c) {
    // TODO: Implement console interrupt handling

    consoleputc(c); // For now, just echo the character
}