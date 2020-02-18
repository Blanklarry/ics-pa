#include "common.h"
#include "syscall.h"
#include <stdlib.h>

uintptr_t sys_yield() {
  _yield();
  return 0;
}

uintptr_t sys_exit(_Context *c) {
  _halt(c->GPR2);
  return 0;
}

#define STDOUT 1
#define STDERR 2
/*
 * hjx-comment:
 * in ics-pa-gitbook: printf() will try to malloc a buf to store the output str, 
 * if malloc error, then printf() will output char by char.
 * So, when you don't complete the heap management in pa3.2,
 * you will see hello test(if you Log sth) will output one char in one times.
 */ 
uintptr_t sys_write(_Context *c) {
  int fd = c->GPR2;
  uintptr_t cnt = 0;
  char *buf = (char*)c->GPR3;
  switch (fd) {
    case STDOUT:
    case STDERR: 
      for (cnt = 0; cnt < c->GPR4; cnt++) {
        _putc(buf[cnt]);
      }
      break;
    default: panic("Unhandle fd = %d\n", fd);
  }
  return cnt;
}

extern char _end;
uintptr_t pro_brk = (uintptr_t)&_end;
uintptr_t sys_brk(_Context *c) {
  pro_brk = c->GPR2;
  return 0;
}

_Context* do_syscall(_Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;

  switch (a[0]) {
    case SYS_yield: Log("do_syscall: SYS_yield"); c->GPRx = sys_yield(); break;
    case SYS_exit: Log("do_syscall: SYS_exit"); c->GPRx = sys_exit(c); break;
    case SYS_write: Log("do_syscall: SYS_write"); c->GPRx = sys_write(c); break;
    case SYS_brk: Log("do_syscall: SYS_brk"); c->GPRx = sys_brk(c); break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }

  return c;
}
