#include "rtl/rtl.h"

#define GateDesc_SIZE 8
#define IRQ_TIMER     32     // for x86

// hjx-comment:
// about struct GateDesc(in x86)
//  31             23             15             7             0
// +-------------+-------------+---+------------------------+
// |   OFFSET 31..16 high-bit  | P |       Don't care       | 4 (here is 4>0)
// +---------------------------+---+------------------------+
// |          Don't care       |    OFFSET 15..0  low-bit   | 0 (notice!! here is 0)
// +-------------+-------------+---+-----------------+------+
void raise_intr(uint32_t NO, vaddr_t ret_addr) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */
  rtl_push(&cpu.eflags.val);
  reg_flag(IF) = 0;
  rtl_push(&cpu.cs);
  rtl_push(&ret_addr);
  uint32_t low_off_addr = cpu.idtr.base + GateDesc_SIZE * NO;
  rtl_lm(&t0, &low_off_addr, 4);
  uint32_t high_off_addr = low_off_addr + 4;
  rtl_lm(&t1, &high_off_addr, 4);
  t0 = (t1 & 0xffff0000) + (t0 & 0xffff);
  rtl_j(t0);
}

bool isa_query_intr(void) {
  if (reg_flag(IF) && cpu.INTR) {
    cpu.INTR = false;
    raise_intr(IRQ_TIMER, cpu.pc);
    return true;
  }
  return false;
}
