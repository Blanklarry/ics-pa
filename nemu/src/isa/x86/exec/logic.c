#include "cpu/exec.h"
#include "cc.h"

make_EHelper(test) {
  // TODO();
  // printf("hjx-debug: type: dest:%d, src:%d\n", id_dest->type, id_src->type);
  rtl_and(&s0, &id_dest->val, &id_src->val);
  rtl_seti_CF(0);
  rtl_seti_OF(0);
  rtl_update_ZFSF(&s0, id_dest->width);
  print_asm_template2(test);
}

make_EHelper(and) {
  // TODO();
  rtl_mv(&s0, &id_src->val);
  if (id_dest->width > id_src->width) {
    rtl_sext(&s0, &id_src->val, id_src->width);
  }
  rtl_and(&s1, &id_dest->val, &s0);
  operand_write(id_dest, &s1);
  rtl_update_ZFSF(&s1, id_dest->width);
  rtl_seti_CF(0);
  rtl_seti_OF(0);
  print_asm_template2(and);
}

make_EHelper(xor) {
  // TODO();
  rtl_mv(&s0, &id_src->val);
  if (id_dest->width > id_src->width) {
    rtl_sext(&s0, &id_src->val, id_src->width);
  }
  rtl_xor(&s1, &id_dest->val, &s0);
  operand_write(id_dest, &s1);
  rtl_update_ZFSF(&s1, id_dest->width);
  rtl_seti_CF(0);
  rtl_seti_OF(0);
  print_asm_template2(xor);
}

make_EHelper(or) {
  // TODO();
  rtl_mv(&s0, &id_src->val);
  if (id_dest->width > id_src->width) {
    rtl_sext(&s0, &id_src->val, id_src->width);
  }
  rtl_or(&s1, &id_dest->val, &s0);
  operand_write(id_dest, &s1);
  rtl_update_ZFSF(&s1, id_dest->width);
  rtl_seti_CF(0);
  rtl_seti_OF(0);
  print_asm_template2(or);
}

make_EHelper(sar) {
  // TODO();
  // unnecessary to update CF and OF in NEMU
  rtl_andi(&s0, &id_dest->val, 1);
  rtl_set_CF(&s0);
  rtl_sar(&s0, &id_dest->val, &id_src->val);
  operand_write(id_dest, &s0);
  rtl_update_ZFSF(&s0, id_dest->width);
  if (id_src->val == 1) {
    rtl_seti_OF(0);
  }
  print_asm_template2(sar);
}

make_EHelper(shl) {
  // TODO();
  // unnecessary to update CF and OF in NEMU
  rtl_msb(&s0, &id_dest->val, id_dest->width);
  rtl_set_CF(&s0);
  rtl_shl(&s1, &id_dest->val, &id_src->val);
  operand_write(id_dest, &s1);
  rtl_update_ZFSF(&s1, id_dest->width);
  if (id_src->val == 1) {
    rtl_msb(&s1, &id_dest->val, id_dest->width);
    rtl_xor(&s1, &s0, &s1); // if != then 1 else 0
    rtl_set_OF(&s1);
  }
  print_asm_template2(shl);
}

make_EHelper(shr) {
  // TODO();
  // unnecessary to update CF and OF in NEMU
  rtl_andi(&s0, &id_dest->val, 1);
  rtl_set_CF(&s0);
  rtl_shr(&s0, &id_dest->val, &id_src->val);
  operand_write(id_dest, &s0);
  rtl_update_ZFSF(&s0, id_dest->width);
  if (id_src->val == 1) {
    rtl_msb(&s0, &id_dest->val, id_dest->width);
    rtl_set_OF(&s0);
  }
  print_asm_template2(shr);
}

make_EHelper(setcc) {
  uint32_t cc = decinfo.opcode & 0xf;

  rtl_setcc(&s0, cc);
  operand_write(id_dest, &s0);

  print_asm("set%s %s", get_cc_name(cc), id_dest->str);
}

make_EHelper(not) {
  // TODO();
  rtl_not(&s0, &id_dest->val);
  // unnecessary to consider the width, because operand_write() will do this.
  operand_write(id_dest, &s0);
  print_asm_template1(not);
}
