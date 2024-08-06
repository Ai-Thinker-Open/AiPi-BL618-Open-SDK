#ifndef __D200_CAMERA_H_
#define __D200_CAMERA_H_
#include "chip_include.h"
#ifdef __D200_CAMERA_C_
#define D200_CAMERA
#else
#define D200_CAMERA extern
#endif

D200_CAMERA void d200_camera_system_init(void);            // 摄像头系统初始化
D200_CAMERA uint8_t *d200_camera_output_buff_addr(void);   // 获取摄像头数据地址
D200_CAMERA uint32_t d200_camera_output_buff_length(void); // 获取摄像头数据长度
D200_CAMERA void d200_semap_init(void);                    // 信号量初始化
D200_CAMERA void d200_semap_lock(void);                    // 信号量获取(用于数据处理)
D200_CAMERA void d200_semap_uplock(void);                  // 信号量释放(用于数据处理)
D200_CAMERA void d200_camera_start(void);                  // 摄像头启动
D200_CAMERA void d200_camera_stop(void);                   // 摄像头停止
D200_CAMERA void d200_power_init(void);                    // 摄像头电源初始化
D200_CAMERA void d200_power_on(void);                      // 电源 开
D200_CAMERA void d200_power_off(void);                     // 电源 关
D200_CAMERA void d200_camera_run(void);                    // 摄像头运行

D200_CAMERA void bl618_camera_task(void *msg); // 摄像头任务，可以直接创建一个任务，然后就可以获取摄像头的数据

#endif
