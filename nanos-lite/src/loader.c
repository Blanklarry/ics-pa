#include "proc.h"
#include "memory.h"
#include <elf.h>
#include <fs.h>

#ifdef __ISA_AM_NATIVE__
# define Elf_Ehdr Elf64_Ehdr
# define Elf_Phdr Elf64_Phdr
#else
# define Elf_Ehdr Elf32_Ehdr
# define Elf_Phdr Elf32_Phdr
#endif


static uintptr_t loader(PCB *pcb, const char *filename) {
  // TODO();
  Elf_Ehdr elf_header;
  Elf_Phdr pro_header;
  int fd = fs_open(filename, 0, 0);
  fs_read(fd, &elf_header, sizeof(Elf_Ehdr));
  uintptr_t phoff = elf_header.e_phoff;
  uintptr_t i = 0, j = 0;
  pcb->max_brk = 0;
  for (i = 0; i < elf_header.e_phnum; i++) {
    fs_lseek(fd, phoff, SEEK_SET);
    fs_read(fd, &pro_header, sizeof(Elf_Phdr));
    if (pro_header.p_type == PT_LOAD) {
      fs_lseek(fd, pro_header.p_offset, SEEK_SET);
      intptr_t lastmemsz = pro_header.p_memsz;
      uint8_t* buf1st = new_page(1);
      _map(&pcb->as, (void*)pro_header.p_paddr, buf1st, 0);
      fs_read(fd, buf1st, lastmemsz < PGSIZE ? lastmemsz : PGSIZE);
      lastmemsz -= PGSIZE;
      for (j = 1; j <= pro_header.p_memsz/PGSIZE; j++) {
        uint8_t *buf = new_page(1);
        _map(&pcb->as, (uint8_t*)pro_header.p_paddr+PGSIZE*j, buf, 0);
        fs_read(fd, buf, lastmemsz < PGSIZE ? lastmemsz : PGSIZE);
        lastmemsz -= PGSIZE;
      }
      if (pcb->max_brk < pro_header.p_vaddr + PGSIZE * j) { // j >= 1
        pcb->max_brk = pro_header.p_vaddr + PGSIZE * j;
      }
      // hjx-comment:
      // [VirtAddr + FileSiz, VirtAddr + MemSiz) memset(0), .bss
      // [fix me]this implememt is assume that the physical addr is new consecutively!!
      if (pro_header.p_memsz > pro_header.p_filesz) {
        memset(buf1st+pro_header.p_filesz, 0, pro_header.p_memsz-pro_header.p_filesz);
      }
    }
    phoff += sizeof(Elf_Phdr);
  }
  fs_close(fd);
  return elf_header.e_entry;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %x", entry);
  ((void(*)())entry) ();
}

void context_kload(PCB *pcb, void *entry) {
  _Area stack;
  stack.start = pcb->stack;
  stack.end = stack.start + sizeof(pcb->stack);

  pcb->cp = _kcontext(stack, entry, NULL);
}

void context_uload(PCB *pcb, const char *filename) {
  _protect(&pcb->as);
  uintptr_t entry = loader(pcb, filename);

  _Area stack;
  stack.start = pcb->stack;
  stack.end = stack.start + sizeof(pcb->stack);

  pcb->cp = _ucontext(&pcb->as, stack, stack, (void *)entry, NULL);
}
