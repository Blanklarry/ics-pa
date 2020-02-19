#include "proc.h"
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
  uintptr_t i = 0;
  for (i = 0; i < elf_header.e_phnum; i++) {
    fs_lseek(fd, phoff, SEEK_SET);
    fs_read(fd, &pro_header, sizeof(Elf_Phdr));
    if (pro_header.p_type == PT_LOAD) {
      fs_lseek(fd, pro_header.p_offset, SEEK_SET);
      fs_read(fd, (uint8_t*)pro_header.p_vaddr, pro_header.p_memsz);
      // hjx-comment:
      // in general, FileSiz <= MemSize, else the (FileSiz - MemSize) is .bss
      if (pro_header.p_vaddr < pro_header.p_filesz) {
        memset((uint8_t*)(pro_header.p_vaddr+pro_header.p_filesz), 0, pro_header.p_filesz-pro_header.p_memsz);
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
  uintptr_t entry = loader(pcb, filename);

  _Area stack;
  stack.start = pcb->stack;
  stack.end = stack.start + sizeof(pcb->stack);

  pcb->cp = _ucontext(&pcb->as, stack, stack, (void *)entry, NULL);
}
