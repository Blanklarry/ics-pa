#include "common.h"
#include <amdev.h>
#include "device.h"
#include <klib.h>

// hjx-comment:
// all ReadFn and WriteFn can't return 0 if success!
size_t serial_write(const void *buf, size_t offset, size_t len) {
  size_t cnt = 0;
  for (cnt = 0; cnt < len; cnt++) {
    _putc(((char*)buf)[cnt]);
  }
  return cnt;
}

#define NAME(key) \
  [_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
  [_KEY_NONE] = "NONE",
  _KEYS(NAME)
};

size_t events_read(void *buf, size_t offset, size_t len) {
  int key = read_key();
  int down = 0;
  size_t ret = 0;
  if (key & 0x8000) {
    key ^= 0x8000;
    down = 1;
  }
  if (key != _KEY_NONE) {
    ret = snprintf(buf, len, "k%c %s\n", (down ? 'd' : 'u'), keyname[key]);
  }
  else {
    ret = snprintf(buf, len, "t %u\n", uptime());
  }
  return ret;
}

static char dispinfo[128] __attribute__((used)) = {};

size_t dispinfo_read(void *buf, size_t offset, size_t len) {
  size_t real_len = len;
  if (offset >= 128) {
    return 0;
  }
  if (offset + len >= 128) {
    real_len = 128 - offset;
  }
  strncpy(buf, dispinfo+offset, real_len);
  return real_len;
}

// hjx-comment:
// Notice! take care of measuring unit!
// in nanos-lite, measuring unit is byte, but in am-ioe(like draw_rect()) is pixel(4 bytes).
// One pixel store in 32 bits, R(red), G(green), B(blue), and A(alpha) each store in 8 bits,
size_t fb_write(const void *buf, size_t offset, size_t len) {
  int w = screen_width();
  int h = screen_height();
  int fb_size = w * h * 4; // *32bit
  size_t real_len = len;
  if (offset >= fb_size) {
    return 0;
  }
  if (offset + len >= fb_size) {
    real_len = fb_size - offset;
  }
  int y = offset / 4 / w;
  int x = offset / 4 - y * w;
  draw_rect((uint32_t*)buf, x, y, real_len/4, 1);
  return len;
}

size_t fbsync_write(const void *buf, size_t offset, size_t len) {
  draw_sync();
  return len;
}

void init_device() {
  Log("Initializing devices...");
  _ioe_init();

  // TODO: print the string to array `dispinfo` with the format
  // described in the Navy-apps convention
  sprintf(dispinfo, "WIDTH:%d\nHEIGHT:%d\n", screen_width(), screen_height());
}
