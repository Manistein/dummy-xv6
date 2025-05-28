#include "common.h"
#include "console.h"
#include "spinlock.h"

static struct spinlock cons_lock;

void consoleinit(void) {
    uartinit();

    initlock(&cons_lock, "console");
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
    acquire(&cons_lock);

    consoleputc(c); // For now, just echo the character

    release(&cons_lock);
}