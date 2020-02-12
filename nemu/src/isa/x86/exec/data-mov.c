#include "cpu/exec.h"

make_EHelper(mov) {
  operand_write(id_dest, &id_src->val);
  print_asm_template2(mov);
}

make_EHelper(push) {
  // TODO();
  // if the source operand is an immediate and its size is less than the operand size, 
  // a sign-extended value is pushed on the stack.
  rtl_mv(&s0, &id_dest->val);
  if (id_dest->type == OP_TYPE_IMM && id_dest->width == 1) {
    rtl_sext(&s0, &id_dest->val, id_dest->width);
  }
  rtl_push(&s0);
  print_asm_template1(push);
}

make_EHelper(pop) {
  // TODO();
  rtl_pop(&s0);
  operand_write(id_dest, &s0);
  print_asm_template1(pop);
}

make_EHelper(pusha) {
  // TODO();
  rtl_lr(&s0, R_ESP, 4);
  rtl_push(&reg_l(R_EAX));
  rtl_push(&reg_l(R_ECX));
  rtl_push(&reg_l(R_EDX));
  rtl_push(&reg_l(R_EBX));
  rtl_push(&s0);
  rtl_push(&reg_l(R_EBP));
  rtl_push(&reg_l(R_ESI));
  rtl_push(&reg_l(R_EDI));
  print_asm("pusha");
}

make_EHelper(popa) {
  // TODO();
  rtl_pop(&reg_l(R_EDI));
  rtl_pop(&reg_l(R_ESI));
  rtl_pop(&reg_l(R_EBP));
  rtl_pop(&s0);
  rtl_pop(&reg_l(R_EBX));
  rtl_pop(&reg_l(R_EDX));
  rtl_pop(&reg_l(R_ECX));
  rtl_pop(&reg_l(R_EAX));
  print_asm("popa");
}

make_EHelper(leave) {
  // TODO();
  rtl_lr(&s0, R_BP, 4);
  rtl_sr(R_SP, &s0, 4);
  rtl_pop(&reg_l(R_EBP));
  print_asm("leave");
}

make_EHelper(cltd) {
  if (decinfo.isa.is_operand_size_16) {
    // TODO();
    rtl_lr(&s0, R_AX, 2);
    rtl_msb(&s1, &s0, 2);
    if (s1) {
      rtl_li(&ir, 0xffff);
      rtl_sr(R_DX, &ir, 2);
    }
    else {
      rtl_li(&ir, 0);
      rtl_sr(R_DX, &ir, 2);
    }
  }
  else {
    // TODO();
    rtl_lr(&s0, R_EAX, 4);
    rtl_msb(&s1, &s0, 4);
    if (s1) {
      rtl_li(&ir, 0xffffffff);
      rtl_sr(R_EDX, &ir, 4);
    }
    else {
      rtl_li(&ir, 0);
      rtl_sr(R_EDX, &ir, 4);
    }
  }

  print_asm(decinfo.isa.is_operand_size_16 ? "cwtl" : "cltd");
}

make_EHelper(cwtl) {
  if (decinfo.isa.is_operand_size_16) {
    TODO();
  }
  else {
    TODO();
  }

  print_asm(decinfo.isa.is_operand_size_16 ? "cbtw" : "cwtl");
}

make_EHelper(movsx) {
  id_dest->width = decinfo.isa.is_operand_size_16 ? 2 : 4;
  rtl_sext(&s0, &id_src->val, id_src->width);
  operand_write(id_dest, &s0);
  print_asm_template2(movsx);
}

make_EHelper(movzx) {
  id_dest->width = decinfo.isa.is_operand_size_16 ? 2 : 4;
  operand_write(id_dest, &id_src->val);
  print_asm_template2(movzx);
}

make_EHelper(lea) {
  operand_write(id_dest, &id_src->addr);
  print_asm_template2(lea);
}
