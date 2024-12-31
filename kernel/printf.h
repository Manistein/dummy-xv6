#ifndef _printf_h_
#define _printf_h_

#include "common.h"
#include <stdarg.h>
#include "console.h"

void printfinit(void);
void printf(const char *fmt, ...);
void panic(const char *s);

#endif