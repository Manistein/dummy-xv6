#ifndef _string_h_
#define _string_h_

#include "common.h"

void *memset(void *dst, int c, uint64_t n);
void* memcpy(void *dst, const void *src, uint64_t n);
char* strcpy(char *dst, const char *src);

#endif