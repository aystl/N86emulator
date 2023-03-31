#include "common.h"
#include "fs.h"
#define DEFAULT_ENTRY ((void *)0x4000000)

extern void ramdisk_read(void *buf, off_t offset,size_t len);
extern size_t get_ramdisk_size();

uintptr_t loader(_Protect *as, const char *filename) {
  //TODO();
  int index=fs_open(filename,0,0);
  int length=fs_filesz(index);
  fs_read(index,DEFAULT_ENTRY,length);
  fs_close(index);
  //ramdisk_read(DEFAULT_ENTRY,0,get_ramdisk_size());
  return (uintptr_t)DEFAULT_ENTRY;
}
