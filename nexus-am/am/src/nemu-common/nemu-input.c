#include <am.h>
#include <amdev.h>
#include <nemu.h>

#define KEYDOWN_MASK 0x8000

size_t __am_input_read(uintptr_t reg, void *buf, size_t size) {
  switch (reg) {
    case _DEVREG_INPUT_KBD: {
      _DEV_INPUT_KBD_t *kbd = (_DEV_INPUT_KBD_t *)buf;
      uint32_t k = inl(KBD_ADDR);
      // hjx-comment:
      // down: sent make code(=break code | KEYDOWN_MASK)
      // up:   sent break code
      // when we get code from KBD_ADDR:
      // 1st: determine whether the code is make(down) or break(up)
      //   => 16th-bit of code indicate that whether the code is make(1) or break(0)
      //   => other bits indicate real keycode
      kbd->keydown = (k & KEYDOWN_MASK ? 1 : 0);
      // 2nd: let 16th-bit code = 0
      kbd->keycode = k & ~KEYDOWN_MASK;
      return sizeof(_DEV_INPUT_KBD_t);
    }
  }
  return 0;
}
