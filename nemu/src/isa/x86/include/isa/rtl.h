#ifndef __X86_RTL_H__
#define __X86_RTL_H__

#include "rtl/rtl.h"

/* RTL pseudo instructions */

/* 
 * hjx-comment: 
 * In the implementation of rtl, try not to use dest/src/src2 etc. parameters to store tmp results,
 * because some parameters may point to same variable.
 */

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
  // TODO();
  rtl_msb(&t0, src1, width);
  rtl_msb(&t1, src2, width);
  if (t0 == t1) {
    rtl_li(dest, 0);
  }
  else {
    rtl_msb(&t1, res, width);
    if (t1 == t0) {
      rtl_li(dest, 0);
    }
    else {
      rtl_li(dest, 1); // + - - = - OR - - + = +
    }
  }
}

static inline void rtl_is_sub_carry(rtlreg_t* dest,
    const rtlreg_t* res, const rtlreg_t* src1) {
  // dest <- is_carry(src1 - src2)
  // TODO();
  if (*src1 >= *res) {
    rtl_li(dest, 0);
  }
  else {
    rtl_li(dest, 1);
  }
}

static inline void rtl_is_add_overflow(rtlreg_t* dest,
    const rtlreg_t* res, const rtlreg_t* src1, const rtlreg_t* src2, int width) {
  // dest <- is_overflow(src1 + src2)
  // TODO();
  rtl_msb(&t0, src1, width);
  rtl_msb(&t1, src2, width);
  if (t0 == t1) {
    rtl_msb(&t1, res, width);
    if (t1 == t0) {
      rtl_li(dest, 0);
    }
    else {
      rtl_li(dest, 1); // + + + = - OR - + - = +
    }
  }
  else {
    rtl_li(dest, 0);
  }
}

static inline void rtl_is_add_carry(rtlreg_t* dest,
    const rtlreg_t* res, const rtlreg_t* src1) {
  // dest <- is_carry(src1 + src2)
  // TODO();
  if (*src1 > *res) {
    rtl_li(dest, 1);
  }
  else {
    rtl_li(dest, 0);
  }
}

#define make_rtl_setget_eflags(f) \
  static inline void concat(rtl_set_, f) (const rtlreg_t* src) { \
    reg_flag(f) = *src; /* TODO(); */ \
  } \
  static inline void concat(rtl_seti_, f) (const rtlreg_t imm) { \
    reg_flag(f) = imm; \
  } \
  static inline void concat(rtl_get_, f) (rtlreg_t* dest) { \
    *dest = reg_flag(f); /* TODO(); */ \
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
    rtl_seti_ZF(0);
  }
  else {
    rtl_seti_ZF(1);
  }
}

// Sign Flag -- Set equal to high-order bit of result (0 is positive, 1 if negative).
static inline void rtl_update_SF(const rtlreg_t* result, int width) {
  // eflags.SF <- is_sign(result[width * 8 - 1 .. 0])
  // TODO();
  rtl_msb(&t0, result, width);
  rtl_set_SF(&t0);
}

static inline void rtl_update_ZFSF(const rtlreg_t* result, int width) {
  rtl_update_ZF(result, width);
  rtl_update_SF(result, width);
}

#endif
