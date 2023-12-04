#include "retarget.h"

// inspired by Core/Src/syscalls.c from a project setup with STM32CubeIDE

int _fstat(int fd, struct stat *st) {
  (void) fd;
  st->st_mode = S_IFCHR;
  return 0;
}

int _isatty(int fd) {
  (void) fd;
  return 1;
}

int _open(const char *path) {
  (void) path;
  return -1;
}

int _close(int fd) {
  (void) fd;
  return -1;
}

int _lseek(int fd, int ptr, int dir) {
  (void) fd, (void) ptr, (void) dir;
  return 0;
}

int _read(int fd, char *ptr, int len) {
  (void) fd, (void) ptr, (void) len;
  return 0;
}

static uint8_t *__sbrk_heap_end = NULL;

void *_sbrk(ptrdiff_t incr) {
  extern uint8_t _end; /* Symbol defined in the linker script */
  extern uint8_t _estack; /* Symbol defined in the linker script */
  extern uint32_t _Min_Stack_Size; /* Symbol defined in the linker script */
  const uint32_t stack_limit = (uint32_t)&_estack - (uint32_t)&_Min_Stack_Size;
  const uint8_t *max_heap = (uint8_t *)stack_limit;
  uint8_t *prev_heap_end;

  /* Initialize heap end at first call */
  if (NULL == __sbrk_heap_end)
  {
    __sbrk_heap_end = &_end;
  }

  /* Protect heap from growing into the reserved MSP stack */
  if (__sbrk_heap_end + incr > max_heap)
  {
    errno = ENOMEM;
    return (void *)-1;
  }

  prev_heap_end = __sbrk_heap_end;
  __sbrk_heap_end += incr;

  return (void *)prev_heap_end;
}

int _write(int file, char *ptr, int len) {
  (void) file;

  int i;
  for (i = 0; i < len; i++) {
    ITM_SendChar(*ptr++);
  }
  return len;
}
