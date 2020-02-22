#include "nemu.h"
#include "isa/mmu.h"

paddr_t page_translate(vaddr_t addr) {
  paddr_t ptr2_page_directory = cpu.cr3.page_directory_base << 12;
  paddr_t ptr2_real_pd = (ptr2_page_directory & 0xfffff000) + PDX(addr) * 4;
  PDE page_table;
  page_table.val = paddr_read(ptr2_real_pd, 4); // val in PAGE DIRECTORY, it ptr to page table
  Assert(page_table.present, "addr=%#x, PGDB=%#x, ptr2_real_pd=%#x", addr, ptr2_page_directory, ptr2_real_pd); // nemu no protection mode
  paddr_t ptr2_real_pt = (page_table.page_frame << 12) + PTX(addr) * 4;
  PTE page_frame;
  page_frame.val = paddr_read(ptr2_real_pt, 4); // val in PAGE TABLE, it ptr to page
  assert(page_frame.present); // nemu no protection mode
  paddr_t translate_addr = (page_frame.page_frame << 12) + OFF(addr); // here, do not *4
  return translate_addr;
}

uint32_t isa_vaddr_read(vaddr_t addr, int len) {
  if (cpu.cr0.paging) {
    if (OFF(addr) + len > 0x1000) { //data cross the page boundary, 0xfff+1
      int len1 = 0x1000 - OFF(addr);
      int len2 = len - len1;
      vaddr_t addr1 = addr;
      vaddr_t addr2 = addr1 + len1;
      paddr_t paddr1 = page_translate(addr1);
      paddr_t paddr2 = page_translate(addr2);
      uint32_t data1 = paddr_read(paddr1, len1);
      uint32_t data2 = paddr_read(paddr2, len2) << (len1 * 8);
      return data2 | data1;
    }
    else {
      paddr_t paddr = page_translate(addr);
      return paddr_read(paddr, len);
    }
  }
  else {
    return paddr_read(addr, len);
  }
}

void isa_vaddr_write(vaddr_t addr, uint32_t data, int len) {
  if (cpu.cr0.paging) {
    if (OFF(addr) + len > 0x1000) {
      int len1 = 0x1000 - OFF(addr);
      int len2 = len - len1;
      vaddr_t addr1 = addr;
      vaddr_t addr2 = addr1 + len1;
      paddr_t paddr1 = page_translate(addr1);
      paddr_t paddr2 = page_translate(addr2);
      uint32_t data1 = data & (~0U >> ((4 - len1) * 8));
      uint32_t data2 = data >> (len1 * 8);
      paddr_write(paddr1, data1, len1);
      paddr_write(paddr2, data2, len2);
    }
    else {
      paddr_t paddr = page_translate(addr);
      paddr_write(paddr, data, len);
    }
  }
  else {
    paddr_write(addr, data, len);
  }
}
