#include "string.h"

void *memset(void *dst, int c, uint64_t n) {
    char *d = dst;
    while (n-- > 0) {
        *d++ = c;
    }
    return dst;
}

void* memcpy(void *dst, const void* src, uint64_t n) {
    char *d = dst;
    const char *s = src;
    while (n-- > 0) {
        *d++ = *s++;
    }
    return dst;
}

char* strcpy(char *dst, const char* src) {
    char* temp = dst;
    while ((*dst++ = *src++) != '\0');
    return temp;
}