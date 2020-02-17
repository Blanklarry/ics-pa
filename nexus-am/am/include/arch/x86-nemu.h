#ifndef __ARCH_H__
#define __ARCH_H__

/* 
 * hjx-comment: 
 * Base on ctx which constructed by trap.S and ics-pa-gitbook,
 * we can know the construction order of the ctx is:
 * 1. push eflags, cs, eip when exec `int 0x81` inst;
 * 2. push 0x81(irq_num) in trap.S, eg. __am_vectrap
 * 3. pusha(eax, ecx, edx, ebx, esp, ebp, esi, edi) in __am_asm_trap
 * 4. push $0 in __am_asm_trap
 * After that, it will pushl %esp then call __am_irq_handle,
 * so this %esp is the parameter of _Context* __am_irq_handle(_Context *c) in nexus-am/am/src/x86/nemu/cte.c
 * Therefore, the organization order of struct _Context is as follows,
 * The reverse of the push order!!!
 * 
 * In summary, the trap ctx came from trap.S by pushing in the Stack
 */
struct _Context {
  // uintptr_t eflags, cs, eip; // push in int 0x80 -> void raise_intr(uint32_t NO, vaddr_t ret_addr)
  // int irq; // pushl $0x81, entry in trap.S
  // uintptr_t eax, ecx, edx, ebx, esp, ebp, esi, edi; // pushal, jmp in __am_asm_trap
  // struct _AddressSpace *as; // pushl $0
  // --*--*--*-- reverse order --*--*--*--
  struct _AddressSpace *as;
  uintptr_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
  int irq;
  uintptr_t eip, cs, eflags;
};

#define GPR1 eax
#define GPR2 eip
#define GPR3 eip
#define GPR4 eip
#define GPRx eip

#endif
