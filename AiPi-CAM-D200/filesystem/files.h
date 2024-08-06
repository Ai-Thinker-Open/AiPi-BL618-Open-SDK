#ifndef __FILES_H_
#define __FILES_H_
#include "chip_include.h"
#ifdef __FILES_C_
#define FILES_EXT
#else
#define FILES_EXT   extern
#endif

FILES_EXT int filesystem_init(void);
FILES_EXT int take_photo(uint8_t *Buf, uint32_t len);
#endif
