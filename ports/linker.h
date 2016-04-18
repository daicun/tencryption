#ifndef _TE_LINK_H
#define _TE_LINK_H
#include <stdio.h>
#include <fcntl.h>

#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/types.h>

/****************************** common I/O interface ******************************/
int (*linker_dup)(int oldfd);

int (*linker_stat)(const char *path, struct stat *buf);
int (*linker_lstat)(const char *path, struct stat *buf);
int (*linker_fstat)(int fd, struct stat *buf);

int (*linker_open)(const char *path, int flags, ...);
ssize_t (*linker_read)(int fd, void *buf, size_t count);
ssize_t (*linker_write)(int fd, const void *buf, size_t count);
int (*linker_close)(int fd);

off_t (*linker_lseek)(int fd, off_t offset, int whence);
off64_t (*linker_lseek64)(int fd, off64_t offset, int whence);

ssize_t (*linker_pread)(int fd, void *buf, size_t count, off_t offset);
ssize_t (*linker_pwrite)(int fd, const void *buf, size_t count, off_t offset);

void *(*linker_mmap)(void *addr, size_t length, int prot, int flags, int fd, off_t offset);
/****************************** common I/O interface ******************************/

size_t (*linker_fread)(void *ptr, size_t size, size_t nmemb, FILE *stream);

#endif
