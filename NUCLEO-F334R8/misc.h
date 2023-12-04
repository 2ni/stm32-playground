#ifndef _MISC_H__
#define _MISC_H__

#include <stm32f334x8.h>
#include <string.h>
#include <stdio.h>

static inline void uint32_to_hex(uint32_t value, char* string) {
  sprintf(string, "%08lx", value);
}

/*
 * get uid as string
 * char[27] uidstr
 */
static inline void get_uid(char* uidstr) {
  strcpy(uidstr, "0x");
  unsigned long *id = (unsigned long *)0x1FFFF7AC;
  uint32_to_hex(id[0], uidstr+2);
  uint32_to_hex(id[1], uidstr+10);
  uint32_to_hex(id[2], uidstr+18);
  uidstr[26] = '\0';
}

#endif // _MISC_H__
