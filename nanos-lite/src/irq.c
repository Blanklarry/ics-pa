#include "common.h"

extern _Context* do_syscall(_Context *c);

static _Context* do_event(_Event e, _Context* c) {
  _Context *ret = NULL;
  switch (e.event) {
    case _EVENT_YIELD: Log("Trap in event yield"); break;
    case _EVENT_SYSCALL: Log("Trap in event syscall"); ret = do_syscall(c); break;
    default: panic("Unhandled event ID = %d", e.event);
  }
  return ret;
}

void init_irq(void) {
  Log("Initializing interrupt/exception handler...");
  _cte_init(do_event);
}
