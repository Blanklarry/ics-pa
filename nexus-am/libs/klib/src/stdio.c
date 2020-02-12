#include "klib.h"
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

char intstr[12];
char *int2str(int d) {
  int i = 12-1;
  intstr[i--] = '\0';
  if (d < 0) {
    intstr[0] = '-';
  }
  while (d != 0) {
    intstr[i--] = '0' + (d % 10);
    d = d / 10;
  }
  if (intstr[0] == '-') {
    intstr[i--] = '-';
  }
  return intstr+i+1;
}

int printf(const char *fmt, ...) {
  return 0;
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  return 0;
}

int sprintf(char *out, const char *fmt, ...) {
  size_t n = strlen(fmt), i = 0, j = 0;
  va_list ap;
  va_start(ap, fmt);
  char c, *s;
  int d;
  for (i = 0; i < n; i++) {
    if (fmt[i] != '%') {
      out[j++] = fmt[i];
    }
    else {
      if (i+1 >= n) {
        break;
      }
      switch(fmt[i+1]) {
        case '%': out[j++] = '%'; break;
        case 'd':
          d = va_arg(ap, int);
          strcpy(out+j, int2str(d));
          j += strlen(int2str(d));
          break;
        case 's':
          s = va_arg(ap, char*); 
          strncpy(out+j, s, strlen(s));
          j += strlen(s);
          break;
        case 'c':
          c = va_arg(ap, int);
          out[j++] = (char)c;
          break;
        default: assert(0);
      }
      i++;
    }
  }
  va_end(ap);
  out[j] = '\0';
  return j;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  return 0;
}

#endif
