#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint64_t);

/* We use the `readline' library to provide more flexibility to read from stdin. */
static char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  cpu_exec(-1); // hjx: -1(long)=0xffffffff=MAX(unsign long) !!!
  return 0;
}

static int cmd_q(char *args) {
  return -1;
}

static int cmd_help(char *args);
static int cmd_si_n(char *args);
static int cmd_info_subcmd(char *args);
static int cmd_p_expr(char *args);
static int cmd_x_n_expr(char *args);
static int cmd_w_expr(char *args);
static int cmd_d_n(char *args);

static struct {
  char *name;
  char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display informations about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
  { "si", "si [n] - Single step N instructions, N default 1", cmd_si_n },
  { "info", "info SUBCMD - Print program states, SUBCMD: r(reg) or w(watchpoint) or f(eflags)", cmd_info_subcmd },
  { "p", "p EXPR - Calculate value of EXPR", cmd_p_expr },
  { "x", "x N EXPR - Scand memory, take EXPR value as the starting memory address, and output N consecutive 4 bytes in hexadecimal form", cmd_x_n_expr },
  { "w", "w EXPR - Set watchpoint, when value of EXPR has changed, stop the program", cmd_w_expr },
  { "d", "d N - Delete N_st watchpoint", cmd_d_n },

  /* TODO: Add more commands */

};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

static int cmd_si_n(char *args) {
  uint64_t n = 1;
  if (strtok(NULL, " ") != NULL) {
    sscanf(args, "%lu", &n);
    n = n == 0 ? 1 : n;
  }
  cpu_exec(n);
  return 0;
}

static char cmd_info_subcmd_usage[] = "Usage: info SUBCMD, SUBCMD: r(reg) or w(watchpoint) or f(eflags)";
static int cmd_info_subcmd(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");

  if (arg == NULL) {
    /* no argument given */
    printf("%s\n", cmd_info_subcmd_usage);
    return 1;
  }
  else {
    if (strcmp(arg, "r") == 0 || strcmp(arg, "reg") == 0) {
      extern void isa_reg_display(void);
      isa_reg_display();
    }
    else if (strcmp(arg, "w") == 0 || strcmp(arg, "watchpoint") == 0) {
      display_wp();
    }
    else if (strcmp(arg, "f") == 0 || strcmp(arg, "eflags") == 0) {
      extern void isa_eflags_display(void);
      isa_eflags_display();
    }
    else {
      printf("Unknown command info '%s'\n%s\n", arg, cmd_info_subcmd_usage);
      return 1;
    }
  }
  return 0;
}

static char cmd_x_n_expr_usage[] = "Usage: x N EXPR, 0 < N < 0xffffffff";
static int cmd_x_n_expr(char *args) {
  uint32_t i, j, cnt, N;
  char expression[MAX_EXPR_LEN];
  int ret = sscanf(args, "%u %*[\"]%[^\"\n]", &N, expression);
  // printf("hjx-debug: %d %s\n", N, expression);
  if (ret != 2 || N < 1) {
    printf("%s\n", cmd_x_n_expr_usage);
    return 1;
  }
  bool expr_res = false;
  vaddr_t start_vaddr = expr(expression, &expr_res);
  Assert(expr_res, "Fail in Calculate value of EXPR");
  for (i = 0, cnt = 0; i < (N>>3)+1; i++) { // N/8
    printf("  0x%07x:   ", start_vaddr+cnt); // PMEM_SIZE: 7 hex num
    for (j = 0; j < 8 && cnt < N; j++, cnt++) {
      printf("0x%02x    ", vaddr_read(start_vaddr+cnt, 1));
    }
    printf("\n");
  } 
  return 0;
}

static int cmd_p_expr(char *args) {
  char expression[MAX_EXPR_LEN];
  sscanf(args, "%*[\"]%[^\"\n]", expression);
  // sscanf(args, "%[^\n]", expression);
  // printf("hjx-debug: %s\n", expression);
  bool expr_res = false;
  uint32_t expr_val = expr(expression, &expr_res);
  Assert(expr_res, "Fail in Calculate value of EXPR");
  printf("  Expression Value: ");
  int i, exprlen = strlen(expression);
  for (i = 0; i < exprlen; i++) {
    if (expression[i] != ' ') {
      printf("%c", expression[i]);
    }
  }
  printf("=0x%x(%u)\n", expr_val, expr_val);
  return 0;
}

static int cmd_w_expr(char *args) {
  char expression[MAX_EXPR_LEN];
  sscanf(args, "%*[\"]%[^\"\n]", expression);
  WP *newwp = new_wp();
  strncpy(newwp->wp_expr, expression, MAX_EXPR_LEN);
  bool expr_res = false;
  newwp->orig_expr_val = expr(expression, &expr_res);
  Assert(expr_res, "Fail in Calculate value of EXPR");
  printf("  Set watchpoint %-3d: %s(original val=%u)\n", newwp->NO, newwp->wp_expr, newwp->orig_expr_val);
  return 0;
}

static int cmd_d_n(char *args) {
  int n = -1;
  sscanf(args, "%d", &n);
  Assert(n >= 0 && n < NR_WP, "Error, Watchpoint num out of range [0, %d)", NR_WP);
  WP dwp = { .NO=n };
  free_wp(&dwp);
  return 0;
}


// If test reg, ptr-deref... should modify code.
void expr_test() {
  // extern void isa_reg_display(void);
  // isa_reg_display();
  char line_buf[16+65536] = {0};
  FILE *fp = fopen("/home/hjxjh/Desktop/ics2019/nemu/tools/gen-expr/input", "r");
  assert(fp != NULL);

  while (fgets(line_buf, 16+65536, fp) != NULL) {
    char *result1 = strtok(line_buf, " ");
    if (result1 == NULL) { continue; }
    char *expression = result1 + strlen(result1) + 1;
    expression[strlen(expression)-1] = '\0'; // line_buf end with \n
    // printf("hjx-debug: now process: %s\n", expression);
    bool succ;
    uint32_t result2 = expr(expression, &succ);
    if (succ) {
      Assert(strtoul(result1, NULL, 10) == result2, "expr test error: %s = %s != %u", expression, result1, result2);      
      // printf("hjx-debug: %s = %u\n", expression, result2);
    }
    
  }
  Log("Axpr_test pass.");
  fclose(fp);  
}

void ui_mainloop(int is_batch_mode) {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  expr_test();

  for (char *str; (str = rl_gets()) != NULL; ) {
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef HAS_IOE
    extern void sdl_clear_event_queue(void);
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}
