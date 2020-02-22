#include <am.h>
#include "memory.h"
#include "proc.h"

static void *pf = NULL;

void* new_page(size_t nr_page) {
  void *p = pf;
  pf += PGSIZE * nr_page;
  assert(pf < (void *)_heap.end);
  return p;
}

void free_page(void *p) {
  panic("not implement yet");
}

/* The brk() system call handler. */
/* 
 * hjx-comment:
 * In nemu, we do not think about free(),
 * and new() a Page each time, so here don't care about increment,
 * just keep align a Page.
 * Think about this situation: 
 * 1st malloc() just need to new 4bytes, but in nemu will new 4K bytes a time.
 * So next time malloc() if just need to new < 4K-4 bytes,
 * we don't need to new actually(not need to modify max_brk)
 */
int mm_brk(uintptr_t brk, intptr_t increment) {
  // printf("hjx-debug: mm_brk: max_brk=%#x, new_brk=%#x\n", current->max_brk, brk);
  if (brk > current->max_brk) {
    uintptr_t i = current->max_brk;
    for (i = current->max_brk; i < brk; i += PGSIZE) {
      uint8_t *buf = new_page(1);
      _map(&current->as, (void*)i, buf, 0);
      // printf("hjx-debug: mm_brk: new max_brk=%#x\n", current->max_brk);
    }
    current->max_brk = i;
  }
  return 0;
}

void init_mm() {
  pf = (void *)PGROUNDUP((uintptr_t)_heap.start);
  Log("free physical pages starting from %p", pf);

  _vme_init(new_page, free_page);
}
