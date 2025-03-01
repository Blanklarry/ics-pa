#include "proc.h"

#define MAX_NR_PROC 4

static PCB pcb[MAX_NR_PROC] __attribute__((used)) = {};
static PCB pcb_boot = {};
PCB *current = NULL;
int fg_pcb = 1; // 0 is hello, 1/2/3 is for scheduling.

void switch_boot_pcb() {
  current = &pcb_boot;
}

void hello_fun(void *arg) {
  int j = 1;
  while (1) {
    Log("Hello World from Nanos-lite for the %dth time!", j);
    j ++;
    _yield();
  }
}

void init_proc() {
  // context_kload(&pcb[0], (void *)hello_fun);
  context_uload(&pcb[0], "/bin/hello");
  context_uload(&pcb[1], "/bin/pal");
  context_uload(&pcb[2], "/bin/pal");
  context_uload(&pcb[3], "/bin/pal");

  switch_boot_pcb();

  Log("Initializing processes...");

  // // load program here
  // naive_uload(NULL, "/bin/init");

}

_Context* schedule(_Context *prev) {
  // save the context pointer
  current->cp = prev;

  // always select pcb[0] as the new process
  assert(fg_pcb > 0 && fg_pcb < MAX_NR_PROC);
  static int timesche = 0;
  current = (timesche < 1000 ? &pcb[fg_pcb] : &pcb[0]);
  timesche = timesche < 1000 ? timesche + 1 : 0; 
  // current = &pcb[0];

  // then return the new context
  return current->cp;
}
