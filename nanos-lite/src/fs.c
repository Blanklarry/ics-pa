#include "fs.h"
#include "device.h"
#include <klib.h>

typedef size_t (*ReadFn) (void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn) (const void *buf, size_t offset, size_t len);

typedef struct {
  char *name;
  size_t size;
  size_t disk_offset;
  size_t open_offset;
  ReadFn read;
  WriteFn write;
} Finfo;

enum {FD_STDIN, FD_STDOUT, FD_STDERR, FD_EVENT, FD_FB, FD_FBSYNC, FD_DISPINFO, FD_TTY};

size_t invalid_read(void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

size_t invalid_write(const void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

/* This is the information about all files in disk. */
// hjx-comment:
// all ReadFn and WriteFn can't return 0 if success!
static Finfo file_table[] __attribute__((used)) = {
  {"stdin", 0, 0, 0, invalid_read, invalid_write},
  {"stdout", 0, 0, 0, invalid_read, serial_write},
  {"stderr", 0, 0, 0, invalid_read, serial_write},
  {"/dev/events", 0, 0, 0, events_read, invalid_write},
  {"/dev/fb", 0, 0, 0, invalid_read, fb_write}, // should support lseek, so the size will init in fs.c#init_fs()
  {"/dev/fbsync", 0, 0, 0, invalid_read, fbsync_write},
  {"/proc/dispinfo", 0, 0, 0, dispinfo_read, invalid_write},
  {"/dev/tty", 0, 0, 0, invalid_read, serial_write},
#include "files.h"
};

#define NR_FILES (sizeof(file_table) / sizeof(file_table[0]))

void init_fs() {
  // TODO: initialize the size of /dev/fb
  // hjx-comment: One pixel store in 32 bits, R(red), G(green), B(blue), and A(alpha) each store in 8 bits,
  file_table[FD_FB].size = screen_width() * screen_height() * 4; // *32bit 
}

int fs_open(const char *pathname, int flags, int mode) {
  int i = 0;
  for (i = 0; i < NR_FILES; i++) {
    if (strcmp(pathname, file_table[i].name) == 0) {
      file_table[i].open_offset = 0;
      return i;
    }
  }
  panic("cannot access %s: No such file or directory", pathname);
}

extern size_t ramdisk_read(void *buf, size_t offset, size_t len);
size_t fs_read(int fd, void *buf, size_t len) {
  assert(fd < NR_FILES);
  Finfo *f = &file_table[fd];
  size_t real_len = len, ret_len;

  switch (fd) {
    case FD_STDOUT:
    case FD_STDERR:
    case FD_STDIN:
    case FD_FB:
    case FD_FBSYNC:
    case FD_TTY:
      panic("Not support read in fd: %d", fd);
      break; 
    case FD_EVENT:
    case FD_DISPINFO:
      ret_len = f->read(buf, f->open_offset, len);
      break;
    default:
      if (f->size < len + f->open_offset) {
        real_len = f->size - f->open_offset;
      }
      ret_len = ramdisk_read(buf, f->disk_offset+f->open_offset, real_len);
      break;
  }
  f->open_offset += ret_len;
  return ret_len;
}

extern size_t ramdisk_write(const void *buf, size_t offset, size_t len);
size_t fs_write(int fd, const void *buf, size_t len) {
  assert(fd < NR_FILES);
  Finfo *f = &file_table[fd];
  size_t real_len = len, ret_len;
  switch (fd) {
    case FD_STDOUT:
    case FD_STDERR:
    case FD_FBSYNC:
    case FD_TTY:
      ret_len = f->write(buf, 0, len);
      break; 
    case FD_FB:
      ret_len = f->write(buf, f->open_offset, len);
      f->open_offset += ret_len;
      break; 
    case FD_STDIN:   
    case FD_EVENT:
    case FD_DISPINFO:
      panic("Not support write in fd: %d", fd);
      break;
    default:
      if (f->size < len + f->open_offset) {
        real_len = f->size - f->open_offset;
      }
      ret_len = ramdisk_write(buf, f->disk_offset+f->open_offset, real_len);
      f->open_offset += ret_len;
      break;
  }
  return ret_len;
}

size_t fs_lseek(int fd, size_t offset, int whence) {
  assert(fd < NR_FILES);
  Finfo *f = &file_table[fd];
  // base on manual, not necessary to check offset and f->size and f->open_offset
  switch (whence) {
    case SEEK_SET: 
      f->open_offset = offset;
      break;
    case SEEK_CUR:
      f->open_offset += offset;
      break;
    case SEEK_END: 
      f->open_offset = f->size + offset;
      break;
    default: 
      panic("Not support whence(should be one of (SEEK_SET, SEEK_CUR, SEEK_END))");
      break;
  }
  return f->open_offset;
}

int fs_close(int fd) {
  assert(fd < NR_FILES);
  file_table[fd].open_offset = 0;
  return 0;
}