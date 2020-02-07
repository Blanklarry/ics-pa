#ifndef __X86_RTL_H__
#define __X86_RTL_H__

#include "rtl/rtl.h"

/* RTL pseudo instructions */

static inline void rtl_lr(rtlreg_t* dest, int r, int width) {
  switch (width) {
    case 4: rtl_mv(dest, &reg_l(r)); return;
    case 1: rtl_host_lm(dest, &reg_b(r), 1); return;
    case 2: rtl_host_lm(dest, &reg_w(r), 2); return;
    default: assert(0);
  }
}

static inline void rtl_sr(int r, const rtlreg_t* src1, int width) {
  switch (width) {
    case 4: rtl_mv(&reg_l(r), src1); return;
    case 1: rtl_host_sm(&reg_b(r), src1, 1); return;
    case 2: rtl_host_sm(&reg_w(r), src1, 2); return;
    default: assert(0);
  }
}

// tag: TODO(), width can be 2, ignore this for the moment.
static inline void rtl_push(const rtlreg_t* src1) {
  // esp <- esp - 4
  rtl_subi(&reg_l(R_ESP), &reg_l(R_ESP),4);
  // M[esp] <- src1
  rtl_sm(&reg_l(R_ESP), src1, 4);
}

static inline void rtl_pop(rtlreg_t* dest) {
  // dest <- M[esp]
  rtl_lm(dest, &reg_l(R_ESP), 4);
  // esp <- esp + 4
  rtl_addi(&reg_l(R_ESP), &reg_l(R_ESP), 4);
}

static inline void rtl_is_sub_overflow(rtlreg_t* dest,
    const rtlreg_t* res, const rtlreg_t* src1, const rtlreg_t* src2, int width) {
  // dest <- is_overflow(src1 - src2)
  TODO();
}

static inline void rtl_is_sub_carry(rtlreg_t* dest,
    const rtlreg_t* res, const rtlreg_t* src1) {
  // dest <- is_carry(src1 - src2)
  TODO();
}

static inline void rtl_is_add_overflow(rtlreg_t* dest,
    const rtlreg_t* res, const rtlreg_t* src1, const rtlreg_t* src2, int width) {
  // dest <- is_overflow(src1 + src2)
  TODO();
}

static inline void rtl_is_add_carry(rtlreg_t* dest,
    const rtlreg_t* res, const rtlreg_t* src1) {
  // dest <- is_carry(src1 + src2)
  TODO();
}

#define make_rtl_setget_eflags(f) \
  static inline void concat(rtl_set_, f) (const rtlreg_t* src) { \
    set_reg_flag(concat(F_, f), *src); /* TODO(); */ \
  } \
  static inline void concat(rtl_get_, f) (rtlreg_t* dest) { \
    *dest = reg_flag(concat(F_, f)); /* TODO(); */ \
  }

make_rtl_setget_eflags(CF)
make_rtl_setget_eflags(OF)
make_rtl_setget_eflags(ZF)
make_rtl_setget_eflags(SF)

// Zero Flag -- Set if result is zero; cleared otherwise.
static inline void rtl_update_ZF(const rtlreg_t* result, int width) {
  // eflags.ZF <- is_zero(result[width * 8 - 1 .. 0])
  // TODO();
  if (*result & (~0u >> ((4 - width) << 3))) {
    set_reg_flag(F_ZF, 0);
  }
  else {
    set_reg_flag(F_ZF, 1);
  }
}

// Sign Flag -- Set equal to high-order bit of result (0 is positive, 1 if negative).
static inline void rtl_update_SF(const rtlreg_t* result, int width) {
  // eflags.SF <- is_sign(result[width * 8 - 1 .. 0])
  // TODO();
  set_reg_flag(F_SF, (*result >> ((width << 3) - 1)) & 1);
}

static inline void rtl_update_ZFSF(const rtlreg_t* result, int width) {
  rtl_update_ZF(result, width);
  rtl_update_SF(result, width);
}

#endif
