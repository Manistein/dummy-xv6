#ifndef _plic_h_
#define _plic_h_

#include "memlayout.h"

void plicinit(void);
void plicinithart(void);
int plicclaim(void);
void pliccomplete(int irq);

#endif