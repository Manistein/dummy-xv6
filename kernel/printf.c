#include "printf.h"
#include "spinlock.h"

struct {
    struct spinlock lock;
    int locking;
} pr;

static char digits[] = "0123456789abcdef";

void printfinit(void) {
    initlock(&pr.lock, "pr");
    pr.locking = 1;
}

static void printint(int xx, int base, int sign) {
    char buf[32];
    int i = 0;
    unsigned int x;

    if (sign && (sign = xx < 0)) {
        x = -xx;
    } else {
        x = xx;
    }

    do {
        if (i >= sizeof(buf)) {
            break;
        }

        buf[i++] = digits[x % base];
    } while ((x /= base) != 0);

    if (sign) {
        buf[i++] = '-';
    }

    while (--i >= 0) {
        consoleputc(buf[i]);
    }
}

void printf(const char* fmt, ...) {
    int locking = pr.locking;
    if (locking) {
        acquire(&pr.lock);
    }

    va_list args;
    va_start(args, fmt);

    for (const char* p = fmt; *p != 0; p++) {
        if (*p != '%') {
            consoleputc(*p);
            continue;
        }

        p++;

        switch(*p) {
            case 'c': {
                int c = va_arg(args, int);
                consoleputc((char)c);
                break;
            }
            case 'd': {
                int d = va_arg(args, int);
                printint(d, 10, 1);
                break;
            }
            case 'x': {
                int x = va_arg(args, int);
                consoleputc('0');
                consoleputc('x');
                printint(x, 16, 0);
                break;
            }
            case 's': {
                char* s = va_arg(args, char*);
                for (char* q = s; *q != 0; q++) {
                    consoleputc(*q);
                }
                break;
            }
            case 'u': {
                unsigned int u = va_arg(args, unsigned int);
                printint(u, 10, 0);
                break;
            }
            case 'p': {
                consoleputc('0');
                consoleputc('x');
                int x = va_arg(args, int);
                printint(x, 16, 0);
                break;
            }
            case '%': {
                consoleputc('%');
                break;
            }
            case 'l': {
                p++;
                if (*p == 'd') {
                    long d = va_arg(args, long);
                    printint(d, 10, 1);
                } else if (*p == 'x') {
                    long x = va_arg(args, long);
                    consoleputc('0');
                    consoleputc('x');
                    printint(x, 16, 0);
                } else if (*p == 'u') {
                    unsigned long u = va_arg(args, unsigned long);
                    printint(u, 10, 0);
                } else if (*p == 'l') {
                    p++;
                    if (*p == 'd') {
                        long long d = va_arg(args, long long);
                        printint(d, 10, 1);
                    } else if (*p == 'x') {
                        long long x = va_arg(args, long long);
                        consoleputc('0');
                        consoleputc('x');
                        printint(x, 16, 0);
                    } else if (*p == 'u') {
                        unsigned long long u = va_arg(args, unsigned long long);
                        printint(u, 10, 0);
                    }
                } else {
                    consoleputc('%');
                    consoleputc('l');
                    consoleputc(*p);
                }
                break;
            }
            case '\a': {
                consoleputc('\a');
                break;
            }
            case '\b': {
                consoleputc('\b');
                break;
            }
            case '\t': {
                consoleputc(' ');
                consoleputc(' ');
                consoleputc(' ');
                consoleputc(' ');
                break;
            }
            case '\r': {
                consoleputc('\r');
                break;
            }
            case '\n': {
                consoleputc('\r');
                consoleputc('\n');
                break;
            }
            case '\0': {
                break;
            }
            default: {
                consoleputc('%');
                consoleputc(*p);
                break;
            }
        }
    }

    va_end(args);

    if (locking) {
        release(&pr.lock);
    }
}

void panic(const char* s) {
    pr.locking = 0;
    printf("panic: ");
    printf(s);
    printf("\n");
    pr.locking = 1;
    for(;;);
}

void sprintf_unsafe(char* buf, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);

    char* p = buf;
    for (const char* f = fmt; *f != 0; f++) {
        if (*f != '%') {
            *p++ = *f;
            continue;
        }

        f++;

        switch(*f) {
            case 'c': {
                int c = va_arg(args, int);
                *p++ = (char)c;
                break;
            }
            case 'd': {
                int d = va_arg(args, int);
                printint(d, 10, 1);
                break;
            }
            case 'x': {
                int x = va_arg(args, int);
                *p++ = '0';
                *p++ = 'x';
                printint(x, 16, 0);
                break;
            }
            case 's': {
                char* s = va_arg(args, char*);
                for (char* q = s; *q != 0; q++) {
                    *p++ = *q;
                }
                break;
            }
            case 'u': {
                unsigned int u = va_arg(args, unsigned int);
                printint(u, 10, 0);
                break;
            }
            case '%': {
                *p++ = '%';
                break;
            }
            default: {
                *p++ = '%';
                *p++ = *f;
                break;
            }
        }
    }

    *p = '\0';
    va_end(args);
}