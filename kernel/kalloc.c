#include "kalloc.h"
#include "spinlock.h"
#include "string.h"

extern char end[]; // first address after kernel loaded from ELF file

struct run {
  struct run *next;
};

struct {
  struct spinlock lock;
  struct run *freelist;
} kmem;

void freerange(void *vstart, void *vend) {
  char *p;
  p = (char *)PROUNDUP((uint64_t)vstart);
  for (; p + PGSIZE <= (char *)vend; p += PGSIZE)
    kfree(p);
}

void kinit() {
  initlock(&kmem.lock, "kmem");
  kmem.freelist = 0;

  freerange(end, (void *)PHYSTOP);
}

void *kalloc() {
  acquire(&kmem.lock);
  struct run *r = kmem.freelist;
  if (r)
    kmem.freelist = r->next;
  release(&kmem.lock);

  memset((char *)r, 5, PGSIZE); // fill with junk

  return (void *)r;
}

void kfree(void *v) {
  memset(v, 1, PGSIZE); // fill with junk

  struct run *r = (struct run *)v;

  acquire(&kmem.lock);
  r->next = kmem.freelist;
  kmem.freelist = r;
  release(&kmem.lock);
}

