#ifndef __X86_REG_H__
#define __X86_REG_H__

#include "common.h"
#include "mmu.h"

#define PC_START IMAGE_START

enum { R_EAX, R_ECX, R_EDX, R_EBX, R_ESP, R_EBP, R_ESI, R_EDI };
enum { R_AX, R_CX, R_DX, R_BX, R_SP, R_BP, R_SI, R_DI };
enum { R_AL, R_CL, R_DL, R_BL, R_AH, R_CH, R_DH, R_BH };

/* TODO: Re-organize the `CPU_state' structure to match the register
 * encoding scheme in i386 instruction format. For example, if we
 * access cpu.gpr[3]._16, we will get the `bx' register; if we access
 * cpu.gpr[1]._8[1], we will get the 'ch' register. Hint: Use `union'.
 * For more details about the register encoding scheme, see i386 manual.
 */

typedef struct {
  union {
    union {
      uint32_t _32;
      uint16_t _16;
      uint8_t _8[2];
    } gpr[8];

    struct {
      rtlreg_t eax, ecx, edx, ebx, esp, ebp, esi, edi;
    };    
  };

  /* Do NOT change the order of the GPRs' definitions. */

  /* In NEMU, rtlreg_t is exactly uint32_t. This makes RTL instructions
   * in PA2 able to directly access these registers.
   */
  // rtlreg_t eax, ecx, edx, ebx, esp, ebp, esi, edi;
  
  // use in trap
  vaddr_t cs;  // PA don't use cs, just for DiffTest(PA3, in trap)
  struct {
    uint16_t limit;
    vaddr_t base;
  } idtr;

  union {
    vaddr_t pc;
    vaddr_t eip;
  };

	union {
		struct {
			rtlreg_t _CF :1;
			rtlreg_t _dummy0 :1;
			rtlreg_t _PF :1;
			rtlreg_t _dummy1 :1;
			rtlreg_t _AF :1;
			rtlreg_t _dummy2: 1;
			rtlreg_t _ZF :1;
			rtlreg_t _SF :1;
			rtlreg_t _TF :1;
			rtlreg_t _IF :1;
			rtlreg_t _DF :1;
			rtlreg_t _OF :1;
			rtlreg_t _OLIP :2;
			rtlreg_t _NT :1;
			rtlreg_t _dummy3 :1;
			rtlreg_t _RF :1;
			rtlreg_t _VM :1;
			rtlreg_t _dummy4 :14;
		};
		rtlreg_t val;	
	} eflags;
#if 0
  union {
		struct {
			rtlreg_t PE :1;
			rtlreg_t MP :1;
			rtlreg_t EM :1;
			rtlreg_t TS :1;
			rtlreg_t ET :1;
			rtlreg_t RESERVED: 26;
			rtlreg_t PG :1;
		};
		rtlreg_t val;	
	} cr0;

  union {
		struct {
			rtlreg_t RESERVED: 12;
			rtlreg_t PDBR :20;
		};
		rtlreg_t val;	
	} cr3;
#endif
  CR0 cr0;
  CR3 cr3;

} CPU_state;

static inline int check_reg_index(int index) {
  assert(index >= 0 && index < 8);
  return index;
}

#define reg_l(index) (cpu.gpr[check_reg_index(index)]._32)
#define reg_w(index) (cpu.gpr[check_reg_index(index)]._16)
#define reg_b(index) (cpu.gpr[check_reg_index(index) & 0x3]._8[index >> 2])

// tag: TODO() should check idx and v (Macros may not be available, should declare a func)
// idx: refer to enum
// v: 0 or 1
#define reg_flag(f) concat(cpu.eflags._, f)

static inline const char* reg_name(int index, int width) {
  extern const char* regsl[];
  extern const char* regsw[];
  extern const char* regsb[];
  assert(index >= 0 && index < 8);

  switch (width) {
    case 4: return regsl[index];
    case 1: return regsb[index];
    case 2: return regsw[index];
    default: assert(0);
  }
}

uint32_t isa_reg_str2val(const char *s, bool *success);

#endif
