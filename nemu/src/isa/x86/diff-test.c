#include "nemu.h"
#include "monitor/diff-test.h"

bool isa_difftest_checkregs(CPU_state *ref_r, vaddr_t pc) {
  // uint32_t eax, ecx, edx, ebx, esp, ebp, esi, edi;
  bool res = true;
  static int cnt = 0;
  if (cpu.eax != ref_r->eax) {
    printf("hjx-debug%d: eax %x != %x\n", cnt, cpu.eax, ref_r->eax);
    res = false;
  }
  if (cpu.ecx != ref_r->ecx) {
    printf("hjx-debug%d: ecx %x != %x\n", cnt, cpu.ecx, ref_r->ecx);
    res = false;
  }
  if (cpu.edx != ref_r->edx) {
    printf("hjx-debug%d: edx %x != %x\n", cnt, cpu.edx, ref_r->edx);
    res = false;
  }
  if (cpu.ebx != ref_r->ebx) {
    printf("hjx-debug%d: ebx %x != %x\n", cnt, cpu.ebx, ref_r->ebx);
    res = false;
  }
  if (cpu.esp != ref_r->esp) {
    printf("hjx-debug%d: esp %x != %x\n", cnt, cpu.esp, ref_r->esp);
    res = false;
  }
  if (cpu.ebp != ref_r->ebp) {
    printf("hjx-debug%d: ebp %x != %x\n", cnt, cpu.ebp, ref_r->ebp);
    res = false;
  }
  if (cpu.esi != ref_r->esi) {
    printf("hjx-debug%d: esi %x != %x\n", cnt, cpu.esi, ref_r->esi);
    res = false;
  }
  if (cpu.edi != ref_r->edi) {
    printf("hjx-debug%d: edi %x != %x\n", cnt, cpu.edi, ref_r->edi);
    res = false;
  }
  if (!res) {
    printf("hjx-debug%d: eip %x ?= %x\n", cnt, cpu.eip, ref_r->eip);
  }
  cnt++;
  return res;
}

void isa_difftest_attach(void) {
}
