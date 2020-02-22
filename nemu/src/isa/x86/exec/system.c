#include "cpu/exec.h"

make_EHelper(lidt) {
  // TODO();
  rtl_lm(&s0, &id_dest->addr, 2);
  cpu.idtr.limit = s0;
  rtl_addi(&s1, &id_dest->addr, 2);
  rtl_lm(&s0, &s1, 4);
  cpu.idtr.base = s0;

  print_asm_template1(lidt);
}

make_EHelper(mov_r2cr) {
  // TODO();
  switch (id_dest->reg) {
    case 0: cpu.cr0.val = id_src->val; break;
    case 3: cpu.cr3.val = id_src->val; break;
    default: panic("Do not support: movl %%%s,%%cr%d", reg_name(id_src->reg, 4), id_dest->reg);
  }

  print_asm("movl %%%s,%%cr%d", reg_name(id_src->reg, 4), id_dest->reg);
}

make_EHelper(mov_cr2r) {
  // TODO();
  switch (id_src->reg) {
    case 0: rtl_li(&id_src->val, cpu.cr0.val); break;
    case 3: rtl_li(&id_src->val, cpu.cr3.val); break;
    default: panic("Do not support: movl %%cr%d,%%%s", id_src->reg, reg_name(id_dest->reg, 4));
  }
  operand_write(id_dest, &id_src->val);

  print_asm("movl %%cr%d,%%%s", id_src->reg, reg_name(id_dest->reg, 4));

  difftest_skip_ref();
}

extern void raise_intr(uint32_t NO, vaddr_t ret_addr);
make_EHelper(int) {
  // TODO();
  raise_intr(id_dest->val, decinfo.seq_pc);

  print_asm("int %s", id_dest->str);

  difftest_skip_dut(1, 2);
}

make_EHelper(iret) {
  // TODO();
  rtl_pop(&decinfo.jmp_pc);
  rtl_pop(&cpu.cs);
  rtl_pop(&cpu.eflags.val);
  rtl_j(decinfo.jmp_pc);
  print_asm("iret");
}

uint32_t pio_read_l(ioaddr_t);
uint32_t pio_read_w(ioaddr_t);
uint32_t pio_read_b(ioaddr_t);
void pio_write_l(ioaddr_t, uint32_t);
void pio_write_w(ioaddr_t, uint32_t);
void pio_write_b(ioaddr_t, uint32_t);

make_EHelper(in) {
  // TODO();
  switch (id_dest->width) {
    case 1: s0 = pio_read_b(id_src->val); break;
    case 2: s0 = pio_read_w(id_src->val); break;
    case 4: s0 = pio_read_l(id_src->val); break;
    default: assert(0);
  }
  operand_write(id_dest, &s0);
  print_asm_template2(in);
}

// hjx-comment:
//   in i386 manual, whether byte, word, or dword, all read from AL,
//   so, you do not need to switch(id_src->width)
make_EHelper(out) {
  // TODO();
  // switch (id_src->width) {
  //   case 1: pio_write_b(id_dest->val, id_src->val); break;
  //   case 2: pio_write_w(id_dest->val, id_src->val); break;
  //   case 4: pio_write_l(id_dest->val, id_src->val); break;
  //   default: assert(0);
  // }
  pio_write_b(id_dest->val, id_src->val);
  print_asm_template2(out);
}
