#include "common.h"
#include "proc.h"
#include "device.h"

extern _Context* do_syscall(_Context *c);

static _Context* do_event(_Event e, _Context* c) {
  _Context *ret = NULL;
  switch (e.event) {
    case _EVENT_YIELD: Logdbg("Trap in event yield"); ret = schedule(c); break;
    case _EVENT_SYSCALL: Logdbg("Trap in event syscall"); ret = do_syscall(c); break;
    case _EVENT_IRQ_TIMER: Logdbg("Trap in event irq timmer"); ret = c; events_read(NULL, 0, 0); _yield(); break;
    default: panic("Unhandled event ID = %d", e.event);
  }
  return ret;
}

void init_irq(void) {
  Log("Initializing interrupt/exception handler...");
  _cte_init(do_event);
}
