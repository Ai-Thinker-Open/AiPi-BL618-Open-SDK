#ifndef __CHIP_INCLUDE_H_
#define __CHIP_INCLUDE_H_

// 系统初始化头文件
#include "stdint.h"
#include "chip_types.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "mem.h"
#include "bflb_irq.h"
#include "bl616_glb.h"
#include "bflb_mtimer.h"
#include "board.h"
#include "bflb_i2c.h"
#include "bflb_cam.h"
#include "image_sensor.h"
#include "bflb_mjpeg.h"
#include "jpeg_head.h"

//wifi 头文件
#include <lwip/tcpip.h>
#include <lwip/sockets.h>
#include <lwip/netdb.h>

// #include "export/bl_fw_api.h"
#include "bl_fw_api.h"
#include "wifi_mgmr_ext.h"
#include "wifi_mgmr.h"

//wifi 头文件
#include "bl616_glb.h"
#include "bflb_gpio.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

// 文件相关
#include "fatfs_diskio_register.h"
#include "ff.h"


#endif
