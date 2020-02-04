#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

#define NR_WP 32
#define MAX_EXPR_LEN 128

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;

  /* TODO: Add more members if necessary */

  char wp_expr[MAX_EXPR_LEN];
  uint32_t orig_expr_val;
  bool is_hit;

} WP;

WP* new_wp();
void free_wp(WP *wp);

int check_wp();
void display_wp();

#endif
