#include "usys.h"
#include <stdarg.h>
#include "../kernel/string.h"

static void putc(int fd, char c) {
    write(fd, &c, 1);
}

static char digits[] = "0123456789abcdef";
static void printint(int fd, int xx, int base, int sign) {
    char buf[16];
    int i;
    unsigned int x;

    if (sign && (sign = xx < 0))
        x = -xx;
    else
        x = xx;

    i = 0;
    do {
        buf[i++] = digits[x % base];
    } while ((x /= base) != 0);

    if (sign)
        buf[i++] = '-';

    while (--i >= 0)
        putc(fd, buf[i]);
}

static void printptr(int fd, uint64_t x) {
    int i;
    char buf[16];
    for (i = 0; i < 16; i++) {
        buf[i] = digits[x % 16];
        x /= 16;
    }
    for (i = 16 - 1; i >= 0; i--) {
        putc(fd, buf[i]);
    }
}

// only understands %d, %x, %p, %s
static vprintf(int fd, char* fmt, va_list ap) {
    int c0, c1, c2, i, state;
    c0 = c1 = c2 = i = state = 0;

    int slen = strlen(fmt);

    for (i = 0; i < slen; i++) {
        c0 = fmt[i] & 0xff;
        if (state == 0) {
            if (c0 == '%') {
                state = '%';
            } else {
                putc(fd, c0);
            }
        } else if (state == '%') {
            if (c0) c1 = fmt[i + 1] & 0xff;
            if (c1) c2 = fmt[i + 2] & 0xff;

            if (c0 == 'd') { // print "%d"
                printint(fd, va_arg(ap, int), 10, 1);
            }
            else if (c0 == 'l' && c1 == 'd' ) { // print "%ld"
                printfint(fd, va_arg(ap, int64_t), 10, 1);
                i = i + 1;
            } 
            else if (c0 == 'l' && c1 == 'l' && c2 == 'd') { // print "%lld"
                printint(fd, va_arg(ap, int64_t), 10, 1);
                i = i + 2;
            }
            else if (c0 == 'x') { // print "%x"
                printint(fd, va_arg(ap, int), 16, 0);
            }
            else if (c0 == 'l' && c1 == 'x') { // print "%lx"
                printint(fd, va_arg(ap, uint64_t), 16, 0);
                i = i + 1;
            }
            else if (c0 == 'l' && c1 == 'l' && c2 == 'x') { // print "%llx"
                printint(fd, va_arg(ap, uint64_t), 16, 0);
                i = i + 2;
            }
            else if (c0 == 'p') {
                printptr(fd, va_arg(ap, uint64_t));
            }
            else if (c0 == 's') {
                char* s = va_arg(ap, char*);
                while (*s) {
                    putc(fd, *s);
                    s++;
                }
            }
            else if (c0 == 'c') {
                putc(fd, va_arg(ap, char));
            }
            else if (c0 == '%') {
                putc(fd, '%');
            }

            state = 0;
        }
    }
}

void printf(char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vprintf(1, fmt, ap);
    va_end(ap);
}