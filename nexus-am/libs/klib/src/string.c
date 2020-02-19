#include "klib.h"

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

// hjx-comment: when modify files in nexus-am/libs/klib/, you should re-compile in nexus-am, not in nemu.

size_t strlen(const char *s) {
  if (s == NULL) {
    return 0;
  }
  size_t ret = 0;
  while (*s++ != '\0') {
    ret++;
  }
  return ret;
}

char *strcpy(char* dst,const char* src) {
  if (src == NULL || dst == NULL) {
    return dst;
  }
  char *d = dst;
  while (*src != '\0') {
    *dst++ = *src++;
  }
  *dst = '\0';
  return d;
}

char* strncpy(char* dst, const char* src, size_t n) {  
  if (src == NULL || dst == NULL) {
    return dst;
  }
  size_t i = 0;
  char *d = dst;
  while (i < n && *src != '\0') {
    *dst++ = *src++;
    i++;
  }
  *dst = '\0';
  return d;
}

char* strcat(char* dst, const char* src) {
  if (src == NULL || dst == NULL) {
    return dst;
  }
  char *d = dst;
  while (*dst != '\0') { // if use *dst++, then should dst-- after this loop
    dst++;
  }
  while (*src != '\0') {
    *dst++ = *src++;
  }
  *dst = '\0';
  return d;
}

int strcmp(const char* s1, const char* s2) {
  if (s1 == NULL && s2 == NULL) {
    return 0;
  }
  if (s1 != NULL && s2 == NULL) {
    return 1;
  }
  if (s1 == NULL && s2 != NULL) {
    return -1;
  }
  while (*s1 != '\0' && *s2 != '\0' && *s1 == *s2) {
    s1++;
    s2++;
  }
  if (*s1 == *s2) {
    return 0;
  }
  else if (*s1 > *s2) {
    return 1;
  }
  else {
    return -1;
  }
}

int strncmp(const char* s1, const char* s2, size_t n) {
  assert(n > 0);
  if (s1 == NULL && s2 == NULL) {
    return 0;
  }
  if (s1 != NULL && s2 == NULL) {
    return 1;
  }
  if (s1 == NULL && s2 != NULL) {
    return -1;
  }
  size_t i = 0;
  while (*s1 != '\0' && *s2 != '\0' && *s1 == *s2 && i < n) {
    s1++;
    s2++;
    i++;
  }
  if (i == n) {
    s1--;
    s2--;
  }
  if (*s1 == *s2) {
    return 0;
  }
  else if (*s1 > *s2) {
    return 1;
  }
  else {
    return -1;
  }
  return 0;
}

void* memset(void* v,int c,size_t n) {
  if (v == NULL) {
    return v;
  }
  size_t i = 0;
  char *v1 = (char*)v;
  for (i = 0; i < n; i++) {
    v1[i] = c;
  }
  return v;
}

void* memcpy(void* out, const void* in, size_t n) {
  if (out == NULL || in == NULL) {
    return out;
  }
  size_t i = 0;
  char *out1 = (char*)out, *in1 = (char*)in;
  for (i = 0; i < n; i++) {
    out1[i] = in1[i];
  }
  return out;
}

int memcmp(const void* s1, const void* s2, size_t n){
  assert(n > 0);
  if (s1 == NULL && s2 == NULL) {
    return 0;
  }
  if (s1 != NULL && s2 == NULL) {
    return 1;
  }
  if (s1 == NULL && s2 != NULL) {
    return -1;
  }
  size_t i = 0;
  unsigned char *s11 = (unsigned char*)s1, *s21 = (unsigned char*)s2;
  for (i = 0; i < n; i++) {
    if (s11[i] != s21[i]) {
      break;
    }
  }
  if (i == n) {
    i--;
  }
  if (s11[i] > s21[i]) {
    return 1;
  }
  else if (s11[i] < s21[i]) {
    return -1;
  }
  else {
    return 0;
  }
}

#endif
