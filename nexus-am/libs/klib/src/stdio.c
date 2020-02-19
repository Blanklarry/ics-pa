#include "klib.h"
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

#define LEN_num_str 16
char num_str[LEN_num_str];
char out_str[81]; // why 81? because word wrap column always 80, so I'm assuming that there's no case where the width is more than 80.
char pad_ch = ' ';
char pad_dir = 'r';
int width = 0;
char prefix_str[4] = {0}; // store "0x"/"0X"...

// return offset of num_str, so the caller can access:
//   num_str+offset       -- the real num_str
//   LEN_num_str-offset-1 -- the len of real num_str
int int2str(int d) {
  int i = LEN_num_str-1;
  num_str[i--] = '\0';
  if (d < 0) {
    num_str[0] = '-';
  }
  do {
    num_str[i--] = '0' + (d % 10);
    d = d / 10;
  } while (d != 0);
  if (num_str[0] == '-') {
    num_str[i--] = '-';
  }
  return i+1;
}

int uint2str(unsigned int u, int base, char is_upper) {
  assert(base == 8 || base == 10 || base == 16);
  int i = LEN_num_str-1;
  num_str[i--] = '\0';
  unsigned int tmp;
  is_upper = is_upper ? 'A' : 'a';
  do {
    tmp = u % base;
    if (tmp < 10) {
      num_str[i--] = '0' + tmp;
    }
    else{
      num_str[i--] = is_upper + (tmp - 10);
    }
    u = u / base;
  } while (u != 0);
  return i+1;
}

void getfmtnumstr(const int offset_numstr) {
  // memset() ??
  int prefix_len = strlen(prefix_str);
  if (prefix_len != 0) {
    strcpy(out_str, prefix_str);
  }
  int numstr_len = LEN_num_str - offset_numstr - 1, i;
  if (width <= numstr_len) {
    strcpy(out_str+prefix_len, num_str+offset_numstr);
    return;
  }
  if (pad_dir == 'r') {
    for (i = 0; i < width-numstr_len; i++) {
      out_str[i+prefix_len] = pad_ch;
    }
    strcpy(out_str+i+prefix_len, num_str+offset_numstr);
  }
  else {
    strcpy(out_str+prefix_len, num_str+offset_numstr);
    for (i = numstr_len; i < width+numstr_len; i++) {
      out_str[i+prefix_len] = pad_ch;
    }
  }
  out_str[width+prefix_len] = '\0';
}

int parsewidth(const char *fmt) {
  int offset = 0;
  int n = fmt[offset++] - '0';
  for (; fmt[offset] >= '0' && fmt[offset] <= '9'; offset++) {
    n = n * 10 + (fmt[offset] - '0');
  }
  width = n;
  return offset;
}

int parsefmt(const char *fmt, va_list *ap) {
  int offset = 0;
  int num_offset = 0;
  char c, *s;
  int d;
  unsigned int u;
  // do not use switch-casa, these flags have order.
  if (fmt[offset] == '-') {
    pad_dir = 'l';
    offset++;
  }
  if (fmt[offset] == '0') {
    pad_ch = '0';
    offset++;
  }
  if (fmt[offset] > '0' && fmt[offset] <= '9') {
    offset += parsewidth(fmt+offset);
  }

  switch (fmt[offset]) {
    case '%': 
      out_str[0] = '%';
      out_str[1] = '\0';
      break;
    case 'c':
      c = va_arg(*ap, int);
      out_str[0] = c;
      out_str[1] = '\0';
      break;
    case 'd': 
      d = va_arg(*ap, int);
      num_offset = int2str(d);
      break;
    case 'o':
      u = va_arg(*ap, unsigned int);
      num_offset = uint2str(u, 8, 0);
      break;
    case 'u':
      u = va_arg(*ap, unsigned int);
      num_offset = uint2str(u, 10, 0);
      break;
    case 'p':
      u = va_arg(*ap, unsigned int);
      num_offset = uint2str(u, 16, 0);
      strcpy(prefix_str, "0x");
      break;
    case '#':
      if(fmt[offset+1] == 'x') {
        strcpy(prefix_str, "0x");
        u = va_arg(*ap, unsigned int);
        num_offset = uint2str(u, 16, 0);
      } 
      else if (fmt[offset+1] == 'X') {
        strcpy(prefix_str, "0X");
        u = va_arg(*ap, unsigned int);
        num_offset = uint2str(u, 16, 1);
      }
      else {
        assert(0);
      }
      offset++;
      break;
    case 'x': 
      u = va_arg(*ap, unsigned int);
      num_offset = uint2str(u, 16, 0);
      break;
    case 'X':
      u = va_arg(*ap, unsigned int);
      num_offset = uint2str(u, 16, 1);
      break;
    case 's':
      s = va_arg(*ap, char*);
      strcpy(out_str, s);
      break;
    // case '#':
    default: assert(0);
  }
  offset++;
  if (num_offset != 0) {
    getfmtnumstr(num_offset);
  }

  pad_ch = ' ';
  pad_dir = 'r';
  width = 0;
  prefix_str[0] = '\0';
  return offset;
}

int _puts(const char *s) {
  int len = strlen(s), i;
  for(i = 0; i < len; i++) {
    _putc(s[i]);
  }
  return len;
}

int printf(const char *fmt, ...) {
  size_t n = strlen(fmt), i = 0, j = 0;
  va_list ap;
  va_start(ap, fmt);
  for (i = 0; i < n; i++) {
    if (fmt[i] != '%') {
      _putc(fmt[i]);
      j++;
    }
    else {
      if (i+1 >= n) {
        break;
      }
      i += parsefmt(fmt+i+1, &ap);
      _puts(out_str);
    }
  }
  va_end(ap);
  return j;
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  return 0;
}

int sprintf(char *out, const char *fmt, ...) {
  size_t n = strlen(fmt), i = 0, j = 0;
  va_list ap;
  va_start(ap, fmt);
  for (i = 0; i < n; i++) {
    if (fmt[i] != '%') {
      out[j++] = fmt[i];
    }
    else {
      if (i+1 >= n) {
        break;
      }
      i += parsefmt(fmt+i+1, &ap);
      strcpy(out+j, out_str);
      j += strlen(out_str);
    }
  }
  va_end(ap);
  out[j] = '\0';
  return j;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  size_t nfmt = strlen(fmt), i = 0, j = 0;
  va_list ap;
  va_start(ap, fmt);
  for (i = 0; i < nfmt; i++) {
    if (fmt[i] != '%') {
      out[j++] = fmt[i];
    }
    else {
      if (i+1 >= nfmt) {
        break;
      }
      i += parsefmt(fmt+i+1, &ap);
      strcpy(out+j, out_str);
      j += strlen(out_str);
    }
    if (j >= n-1) {
      break;
    }
  }
  va_end(ap);
  out[j] = '\0';
  return j;
}

#endif
