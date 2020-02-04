#include "monitor/watchpoint.h"
#include "monitor/expr.h"
#include "nemu.h"

// static variable, other file can't access them directly,
// and access them by interface new_wp() and free_wp() etc.
// meet requirement of pool data structure.
static WP wp_pool[NR_WP] = {};
// keep NO ascending order when new and free
static WP *head = NULL, *free_ = NULL;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = &wp_pool[i + 1];
    wp_pool[i].is_hit = false;
  }
  wp_pool[NR_WP - 1].next = NULL;

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */

WP* new_wp() {
  Assert(free_ != NULL, "new_wp() error: no free in wp_pool");
  WP *ret = free_;
  free_ = free_->next;

  WP *p = head, *q = p;
  if (head == NULL || head->NO > ret->NO) {
    ret->next = head;
    head = ret;
    return ret;
  }
  // head has at least 1 node, and insert as least 2nd pos
  while (p != NULL && p->NO < ret->NO) {
    q = p;
    p = p->next;
  }
  q->next = ret;
  ret->next = p;
  return ret;
}

void free_wp(WP *wp) {
  WP *p = head, *q = p;
  while (p != NULL) {
    if (p->NO == wp->NO) {
      memset(p->wp_expr, 0, MAX_EXPR_LEN);
      p->orig_expr_val = 0;
      p->is_hit = false;

      // remove from head
      if (p == head) {
        head = p->next;
      }
      else {
        q->next = p->next;
      }

      // insert in free_
      WP *r = free_, *s = r;
      if (free_ == NULL || free_->NO > p->NO) {
        p->next = free_;
        free_ = p;
      }
      // head has at least 1 node, and insert as least 2nd pos
      while (r != NULL && r->NO < p->NO) {
        s = r;
        r = r->next;
      }
      s->next = p;
      p->next = r;
      return;
    }
    q = p;
    p = p->next;
  }
  panic("free_wp(*wp) error: can't find *wp");
}


int check_wp() {
  // Log("checking wp...");
  WP *p = head;
  int hit_times = 0;
  for (p = head; p != NULL; p = p->next) {
    bool expr_res = false;
    uint32_t now_val = expr(p->wp_expr, &expr_res);
    Assert(expr_res, "Fail in Calculate value of EXPR: %s", p->wp_expr);
    if (!p->is_hit && now_val != p->orig_expr_val) {
      printf("  Hit watchpoint %-3d: %s(orig=%u,now=%u)\n", 
                p->NO, p->wp_expr, p->orig_expr_val, now_val);
      p->is_hit = true;
      hit_times++;
    }
  }
  return hit_times;
}


void display_wp() {
  WP *p = head;
  for (p = head; p != NULL; p = p->next) {
    printf("  Watchpoint %-3d: %s, is_hit=%s, orig=%u\n", p->NO, p->wp_expr, p->is_hit ? "true" : "false", p->orig_expr_val);
  }
}