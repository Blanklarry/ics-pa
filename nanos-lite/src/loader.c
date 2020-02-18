#include "proc.h"
#include <elf.h>

#ifdef __ISA_AM_NATIVE__
# define Elf_Ehdr Elf64_Ehdr
# define Elf_Phdr Elf64_Phdr
#else
# define Elf_Ehdr Elf32_Ehdr
# define Elf_Phdr Elf32_Phdr
#endif

extern size_t ramdisk_read(void *buf, size_t offset, size_t len);

static uintptr_t loader(PCB *pcb, const char *filename) {
  // TODO();
  Elf_Ehdr elf_header;
  Elf_Phdr pro_header;
  ramdisk_read(&elf_header, 0, sizeof(Elf_Ehdr));
  uintptr_t phoff = elf_header.e_phoff;
  uintptr_t i = 0;
  for (i = 0; i < elf_header.e_phnum; i++) {
    ramdisk_read(&pro_header, phoff, sizeof(Elf_Phdr));
    if (pro_header.p_type == PT_LOAD) {
      ramdisk_read((uint8_t*)pro_header.p_vaddr, pro_header.p_offset, pro_header.p_memsz);
      // hjx-comment:
      // in general, FileSiz <= MemSize, else the (FileSiz - MemSize) is .bss
      if (pro_header.p_vaddr < pro_header.p_filesz) {
        memset((uint8_t*)(pro_header.p_vaddr+pro_header.p_filesz), 0, pro_header.p_filesz-pro_header.p_memsz);
      }
    }
    phoff += sizeof(Elf_Phdr);
  }
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
