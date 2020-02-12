#include "nemu.h"
#include <stdlib.h>
#include <time.h>
#include <string.h>

const char *regsl[] = {"eax", "ecx", "edx", "ebx", "esp", "ebp", "esi", "edi"};
const char *regsw[] = {"ax", "cx", "dx", "bx", "sp", "bp", "si", "di"};
const char *regsb[] = {"al", "cl", "dl", "bl", "ah", "ch", "dh", "bh"};

void reg_test() {
  srand(time(0));
  uint32_t sample[8];
  uint32_t pc_sample = rand();
  cpu.pc = pc_sample;

  int i;
  for (i = R_EAX; i <= R_EDI; i ++) {
    sample[i] = rand();
    reg_l(i) = sample[i];
    assert(reg_w(i) == (sample[i] & 0xffff));
  }

  assert(reg_b(R_AL) == (sample[R_EAX] & 0xff));
  assert(reg_b(R_AH) == ((sample[R_EAX] >> 8) & 0xff));
  assert(reg_b(R_BL) == (sample[R_EBX] & 0xff));
  assert(reg_b(R_BH) == ((sample[R_EBX] >> 8) & 0xff));
  assert(reg_b(R_CL) == (sample[R_ECX] & 0xff));
  assert(reg_b(R_CH) == ((sample[R_ECX] >> 8) & 0xff));
  assert(reg_b(R_DL) == (sample[R_EDX] & 0xff));
  assert(reg_b(R_DH) == ((sample[R_EDX] >> 8) & 0xff));

  assert(sample[R_EAX] == cpu.eax);
  assert(sample[R_ECX] == cpu.ecx);
  assert(sample[R_EDX] == cpu.edx);
  assert(sample[R_EBX] == cpu.ebx);
  assert(sample[R_ESP] == cpu.esp);
  assert(sample[R_EBP] == cpu.ebp);
  assert(sample[R_ESI] == cpu.esi);
  assert(sample[R_EDI] == cpu.edi);

  assert(pc_sample == cpu.pc);
}

void isa_reg_display() {
  int i = 0;
  for (; i < sizeof(regsl)/sizeof(char*); i++) {
    printf("%-6s  0x%08x  %u\n", regsl[i], reg_l(i), reg_l(i));
  }
}

void isa_eflags_display() {
  printf(" +--------+----+----+----+----+\n");
  printf(" | eflags | CF | ZF | SF | OF |\n");
  printf(" +--------+----+----+----+----+\n");
  printf(" |        |  %d |  %d |  %d |  %d |\n", 
      reg_flag(CF), reg_flag(ZF), reg_flag(SF), reg_flag(OF));
  printf(" +--------+----+----+----+----+\n");
}

uint32_t isa_reg_str2val(const char *s, bool *success) {
  const char *reg_str = s[0] == '$' ? s+1 : s;
  *success = false;
  if (strcmp(reg_str, "pc") == 0) {
    *success = true;
    return cpu.pc;
  }
  int i = 0;
  for (i = 0; i < sizeof(regsl)/sizeof(char*); i++) {
    if (strcmp(reg_str, regsl[i]) == 0) {
      *success = true;
      return reg_l(i);
    }
  }
  for (i = 0; i < sizeof(regsw)/sizeof(char*); i++) {
    if (strcmp(reg_str, regsw[i]) == 0) {
      *success = true;
      return reg_w(i);
    }
  }
  for (i = 0; i < sizeof(regsb)/sizeof(char*); i++) {
    if (strcmp(reg_str, regsb[i]) == 0) {
      *success = true;
      return reg_b(i);
    }
  }
  return 0;
}
