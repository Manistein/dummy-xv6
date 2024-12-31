#ifndef _proc_h_
#define _proc_h_

struct cpu {
    int intena; // Saved interrupt enable bit
    int noff;   // Depth of push_off() nesting
};

struct cpu* mycpu();
void initproc();

#endif