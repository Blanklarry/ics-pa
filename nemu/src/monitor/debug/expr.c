#include "nemu.h"
#include "memory/memory.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>
#include <string.h>
#include <stdlib.h>

enum {
  TK_NOTYPE = 256, 
  TK_EQ, // ==
  TK_NEQ, // !=
  TK_DEC_NUM, TK_DEC_NUM_OF /* buffer overflow, extern TKNUM */,
  TK_HEX_NUM, // 0x
  TK_REG, // $
  TK_LOGIC_AND,  // &&
  TK_PTR_DEREF // *ptr

  /* TODO: Add more token types */

};

static struct rule {
  char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},    // spaces
  {"\\+", '+'},         // plus
  {"-", '-'},           // sub
  {"\\*", '*'},         // mul
  {"/", '/'},           // div
  {"\\(", '('},         // left parenthesis
  {"\\)", ')'},         // right parenthesis
  {"0x[0-9a-fA-F]+", TK_HEX_NUM},  // hexadecimal number
  {"\\$[0-9a-zA-Z]+", TK_REG},  // register // todo: need to be improving
  {"[0-9]+", TK_DEC_NUM},   // decimal number
  {"==", TK_EQ},        // equal
  {"!=", TK_NEQ},       // not equal
  {"&&", TK_LOGIC_AND},       // logic AND
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

#define MAX_TOKEN_STR_LEN 32
typedef struct token {
  int type;
  char str[MAX_TOKEN_STR_LEN];
} Token;

static Token tokens[64] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;
#define TOKEN_ARRAY_LEN (sizeof(tokens)/sizeof(Token))

static bool make_token(char *e) {
  memset(tokens, 0, sizeof(tokens));
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        // Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
        //     i, rules[i].regex, position, substr_len, substr_len, substr_start);
        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

        switch (rules[i].token_type) {
          case '+':
          case '-':
          case '*':
          case '/':
          case '(':
          case ')':
          case TK_REG:
          case TK_HEX_NUM:         
          case TK_DEC_NUM:
          case TK_EQ:
          case TK_NEQ:
          case TK_LOGIC_AND:
            tokens[nr_token].type = rules[i].token_type;
            if (substr_len > MAX_TOKEN_STR_LEN-1) {
              TODO(); // silde: tokens.str[32], will overflow?
                      // but the assumption is all number is unint32 which max_num's chr len must < 32!
            }
            strncpy(tokens[nr_token].str, substr_start, substr_len);
            nr_token++;
#if 0
            do {
              strncpy(tokens[nr_token++].str, substr_start, 31);
              if (nr_token > TOKEN_ARRAY_LEN) {
                printf("out of range: tokens[%d]\n", TOKEN_ARRAY_LEN);
                return false;
              }
              substr_start += MAX_TOKEN_STR_LEN-1;
              substr_len -= MAX_TOKEN_STR_LEN-1;
              if (nr_token < sizeof(tokens)/sizeof(Token)) {
                tokens[nr_token].type = TK_DEC_NUM_OF;
              }
            }while (substr_len > MAX_TOKEN_STR_LEN-1);
#endif
            break;
          case TK_NOTYPE:break;
          default: /*TODO();*/break;
        }

        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }

    if (nr_token > TOKEN_ARRAY_LEN) {
      printf("out of range: tokens[%u]", TOKEN_ARRAY_LEN);
      return false;
    }
  }

  return true;
}

typedef unsigned int tok_idx_t;

bool check_parentheses(tok_idx_t p, tok_idx_t q) {
  if (tokens[p].type == '(' && tokens[q].type == ')') {
    int i, stack_top = -1;
    for (i = p + 1; i < q; i++) {
      if (tokens[i].type == '(') {
        stack_top++;
      } else if (tokens[i].type == ')') {
        if (stack_top >= 0) {
          stack_top--;
        }
        else {
          // panic("don't match parenthesiss:()");
          // just return false is ok
          // consider: (4 + 3) * (2 - 1)
          // This func(check_parentheses) will return false,
          // but it is a legal expr, should panic!
          // Temporary, get_main_op() will check parentheses simply again.
          return false;
        }
      }
    }
    if (stack_top == -1) {
      return true;
    }
  }
  return false;
}

void log_token(tok_idx_t p, tok_idx_t q) {
  printf("hjx-log_token(): ");
  tok_idx_t i;
  for (i = p; i <= q; i++) {
    printf("%s ", tokens[i].str);
  }
  printf("\n");
}

static struct op_priority {
  int token_type;
  char pri;
  char assoc; // 'l' or 'r'
} op_prioritys[] = {
  { '+', 16-5, 'l' },
  { '-', 16-5, 'l' },
  { '*', 16-4, 'l' }, // * as ptr deref, pri is 16-2(which >16-4), so eval() it before last else{} is ok;
  { '/', 16-4, 'l' },
  { TK_EQ, 16-8, 'l'},
  { TK_NEQ, 16-8, 'l'},
  { TK_LOGIC_AND, 16-12, 'l'},
};
#define NR_OP_PRIO (sizeof(op_prioritys) / sizeof(op_prioritys[0]) )

// p "(2 -5+7)  +  4194304/4"
tok_idx_t get_main_op(tok_idx_t p, tok_idx_t q) {
  // log_token(p, q);
  int i, j, cur_main_op = -1;
  char cur_min_pri = 127; // max char
  int in_parenthesis = 0; // use to simple check parentheses
  for (i = p; i <= q; i++) {
    if (tokens[i].type == '(') {
      in_parenthesis++;
      continue;
    } else if (tokens[i].type == ')') {
      in_parenthesis--;
      continue;
    } else if (in_parenthesis > 0) {
      continue;
    } else if (in_parenthesis < 0 ) {
      // Even num_( == num_), can panic here!  eg. (4+3))*((2-1)
      // Because num_( >= num_) always true.
      panic("don't match parenthesiss:()");
    }
    for (j = 0; j < NR_OP_PRIO; j++) {
      if (tokens[i].type == op_prioritys[j].token_type) {
        if (op_prioritys[j].pri < cur_min_pri) {
          cur_main_op = i;
          cur_min_pri = op_prioritys[j].pri;
        }
        else if (op_prioritys[j].pri == cur_min_pri) {
          if (op_prioritys[j].assoc == 'l') {
            cur_main_op = i;
          }
          else { // (op_prioritys[j].assoc == 'r') 
            // not need to change
          }
        } 
        break;
      }
    }
  }
  if (in_parenthesis < 0 ) {
    // redundant ) behind the expr
    panic("don't match parenthesiss:()");
  }
  Assert(cur_main_op >= 0, "get_main_op() error");
  return cur_main_op;
}

uint32_t eval(tok_idx_t p, tok_idx_t q) {
  // log_token(p, q);
  if (p > q) {
    panic("Bad expression");
  }
  else if (p == q) {
    /* Single token.
     * For now this token should be a number.
     * Return the value of the number.
     */
    uint32_t ret;
    bool succ;
    switch (tokens[p].type) {
      // TODO: if tokens.str overflow?
      case TK_DEC_NUM: ret = strtoul(tokens[p].str, NULL, 10); break;
      case TK_HEX_NUM: ret = strtoul(tokens[p].str, NULL, 16); break;
      case TK_REG: 
        ret = isa_reg_str2val(tokens[p].str, &succ);
        if (succ) {
          break;
        }
      default: panic("Not a num or reg: %s", tokens[p].str); break;
    }
    return ret;
  }
  // ()'s pri is 16-1
  else if (check_parentheses(p, q)) {
    /* The expression is surrounded by a matched pair of parentheses.
     * If that is the case, just throw away the parentheses.
     */
    return eval(p+1, q-1);
  }
  // * as ptr deref, pri is 16-2(which > +-*/)
  else if (tokens[p].type == TK_PTR_DEREF) {
    uint32_t valptr = eval(p+1, q);
    return vaddr_read(valptr, 4);
  }
  else {
    tok_idx_t op = get_main_op(p, q);
    // printf("hjx-debug: now main op=%u\n", op);
    uint32_t val1 = eval(p, op-1);
    uint32_t val2 = eval(op+1, q);
    // printf("hjx-debug: val1=%u, val2=%u\n", val1, val2);
    switch (tokens[op].type) {
      case '+': return val1 + val2;
      case '-': return val1 - val2;
      case '*': return val1 * val2;
      case '/': 
        Assert(val2 != 0, "panic: div 0");
        return val1 / val2;
      case TK_EQ: return val1 == val2;
      case TK_NEQ: return val1 != val2;
      case TK_LOGIC_AND: return val1 && val2;  
      default: Assert(0, "Unknown op");
    }
  }
}

uint32_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }

  int i;
  for (i = 0; i < nr_token; i++) {
    if (tokens[i].type == '*' && 
      (i == 0 || tokens[i-1].type == '+' || tokens[i-1].type == '-' 
              || tokens[i-1].type == '*' || tokens[i-1].type == '/'
              || tokens[i-1].type == TK_LOGIC_AND
              || tokens[i-1].type == TK_EQ
              || tokens[i-1].type == TK_NEQ)) {
      tokens[i].type = TK_PTR_DEREF;
    }
  }
  /* TODO: Insert codes to evaluate the expression. */
  *success = true;
  return eval(0, nr_token-1);
}
