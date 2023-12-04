#ifndef _RETARGET_H__
#define _RETARGET_H__

#include <stm32f334x8.h>
#include <sys/stat.h>
#include <errno.h>

#ifdef __cplusplus
extern "C" {
#endif

int _fstat(int fd, struct stat* st);
int _isatty(int fd);
int _open(const char *path);
int _close(int fd);
int _lseek(int fd, int ptr, int dir);
int _read(int fd, char* ptr, int len);
void *_sbrk(ptrdiff_t incr);
int _write(int fd, char* ptr, int len);

#ifdef __cplusplus
} //extern "C"
#endif

#endif //#ifndef _RETARGET_H__
