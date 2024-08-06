#ifndef __BL618_WIFI_H_
#define __BL618_WIFI_H_
#include "chip_include.h"
#ifdef __BL618_WIFI_C_
#define BL618_WIFI_EXT
#else
#define BL618_WIFI_EXT extern
#endif

#define BL618_WIFI_STACK_SIZE (1024 * 2)
#define BL618_WIFI_TASK_PRIORITY_FW (25)

// wifi 初始化
BL618_WIFI_EXT void bl618_wifi_init(void);
BL618_WIFI_EXT uint8_t bl618_wifi_connect(char *ssid, char *passwd);
BL618_WIFI_EXT void bl618_wifi_callback_register(void (*bl618_wifi_callback)(void), uint32_t code);
BL618_WIFI_EXT void bl618_wifi_callback_unregister(uint32_t code);
BL618_WIFI_EXT uint8_t bl618_wifi_ap_start(uint8_t *ssid, uint8_t *pwd);
#endif
