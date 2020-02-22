#include <am.h>
#include <x86.h>
#include <nemu.h>

#define PG_ALIGN __attribute((aligned(PGSIZE)))

static PDE kpdirs[NR_PDE] PG_ALIGN = {};
static PTE kptabs[(PMEM_SIZE + MMIO_SIZE) / PGSIZE] PG_ALIGN = {};
static void* (*pgalloc_usr)(size_t) = NULL;
static void (*pgfree_usr)(void*) = NULL;
static int vme_enable = 0;

static _Area segments[] = {      // Kernel memory mappings
  {.start = (void*)0,          .end = (void*)PMEM_SIZE},
  {.start = (void*)MMIO_BASE,  .end = (void*)(MMIO_BASE + MMIO_SIZE)}
};

#define NR_KSEG_MAP (sizeof(segments) / sizeof(segments[0]))

int _vme_init(void* (*pgalloc_f)(size_t), void (*pgfree_f)(void*)) {
  pgalloc_usr = pgalloc_f;
  pgfree_usr = pgfree_f;

  int i;

  // make all PDEs invalid
  for (i = 0; i < NR_PDE; i ++) {
    kpdirs[i] = 0;
  }

  PTE *ptab = kptabs;
  for (i = 0; i < NR_KSEG_MAP; i ++) {
    uint32_t pdir_idx = (uintptr_t)segments[i].start / (PGSIZE * NR_PTE);
    uint32_t pdir_idx_end = (uintptr_t)segments[i].end / (PGSIZE * NR_PTE);
    for (; pdir_idx < pdir_idx_end; pdir_idx ++) {
      // fill PDE
      kpdirs[pdir_idx] = (uintptr_t)ptab | PTE_P;

      // fill PTE
      PTE pte = PGADDR(pdir_idx, 0, 0) | PTE_P;
      PTE pte_end = PGADDR(pdir_idx + 1, 0, 0) | PTE_P;
      for (; pte < pte_end; pte += PGSIZE) {
        *ptab = pte;
        ptab ++;
      }
    }
  }

  set_cr3(kpdirs);
  set_cr0(get_cr0() | CR0_PG);
  vme_enable = 1;

  return 0;
}

int _protect(_AddressSpace *as) {
  PDE *updir = (PDE*)(pgalloc_usr(1));
  as->ptr = updir;
  // map kernel space
  for (int i = 0; i < NR_PDE; i ++) {
    updir[i] = kpdirs[i];
  }

  return 0;
}

void _unprotect(_AddressSpace *as) {
}

static _AddressSpace *cur_as = NULL;
void __am_get_cur_as(_Context *c) {
  c->as = cur_as;
}

void __am_switch(_Context *c) {
  if (vme_enable) {
    set_cr3(c->as->ptr);
    cur_as = c->as;
  }
}

int _map(_AddressSpace *as, void *va, void *pa, int prot) {
  // printf("hjx-debug: _map(): as->ptr=%p\n", as->ptr);
  uintptr_t ppage = (uintptr_t)pa | PTE_P;
  uintptr_t vdir = PDX(va);
  uintptr_t vpage = PTX(va);
  uintptr_t vp_addr = ((uintptr_t*)as->ptr)[vdir];
  if ((vp_addr & PTE_P) == 1) {
    vp_addr = vp_addr & 0xfffff000;
    ((uintptr_t*)vp_addr)[vpage] = ppage;
    // printf("hjx-debug: remap() %#x[%#x] = %#x, \n", vp_addr, vpage, ppage);
  }
  else {
    PDE *npt = (PDE*)(pgalloc_usr(1));
    ((uintptr_t*)as->ptr)[vdir] = (uintptr_t)npt | PTE_P;
    npt[vpage] = ppage;
    // printf("hjx-debug: newmap() %p[%#x] = %#x, \n", npt, vpage, ppage);
  }
  return 0;
}

_Context *_ucontext(_AddressSpace *as, _Area ustack, _Area kstack, void *entry, void *args) {
  ustack.end = (uintptr_t *)ustack.end - 3; // stack frame of void _start(int argc, char *argv[], char *envp[])
  _Context *c = (_Context *)ustack.end - 1;
  c->as = as;
  c->eflags |= 0x200; // IF is 10th bit(start from 1)
  c->eip = (uintptr_t)entry;
  return c;
}
