#ifndef _kalloc_h_
#define _kalloc_h_

#include "common.h"
#include "memlayout.h"

void  kinit();
void* kalloc();
void  kfree(void *v);

#endif 