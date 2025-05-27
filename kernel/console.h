#ifndef _console_h_
#define _console_h_

#define BACKSPACE 0x08

void uartinit(void);
void uartputc_sync(int c);
void uartintr(void);

void consoleinit(void);
void consoleputc(int c);
void consoleintr(int c);

#endif 