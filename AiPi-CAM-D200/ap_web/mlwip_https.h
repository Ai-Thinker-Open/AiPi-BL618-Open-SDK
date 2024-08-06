#ifndef __MLWIP_HTTPS_H_
#define __MLWIP_HTTPS_H_
#include "chip_include.h"
#define u32 uint32_t
#define u16 uint16_t
#define u8 uint8_t
typedef void *at_os_mutex_t;

typedef struct myparm
{
    int sc;
    u8 *buf;
}MYPARM;

extern char *datajpeg_buf;
extern uint32_t datajpeg_len;

void mhttp_server_init();

#endif
