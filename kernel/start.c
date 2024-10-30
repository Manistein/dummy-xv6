#include "param.h"

// The stack space for each CPU to run start function in machine mode.
__attribute__((aligned(16))) char stack0[4096 * NCPU];

void start() {

}