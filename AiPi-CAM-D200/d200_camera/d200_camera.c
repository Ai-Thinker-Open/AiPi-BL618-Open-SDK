#define __D200_CAMERA_C_
#include "d200_camera.h"

// 设备接口
static struct bflb_device_s *i2c0;
static struct bflb_device_s *cam0;
static struct bflb_device_s *mjpeg;
struct image_sensor_config_s *sensor_config;
struct bflb_cam_config_s cam_config;
static uint8_t receive_status = 0; // 接收状态，用于判断是否接收完成
static uint8_t jpg_head_buf[800] = {0};
static uint32_t jpg_head_len;
static uint8_t camera_mode = 0;
volatile uint8_t *pic_addr;
volatile uint32_t pic_size = 0;
void *camera_muxsem_handle = NULL;

#define DVP_JPEG_ROW_NUM (16)

// buf变量
uint8_t __attribute__((section(".psram_noinit"), aligned(64))) dvp_buffer[1600 * 2 * DVP_JPEG_ROW_NUM];
uint8_t __attribute__((section(".psram_noinit"), aligned(32))) mjpeg_buffer[500 * 1024];
uint8_t __attribute__((section(".psram_noinit"), aligned(32))) mjpeg_output_buffer[500 * 1024];

static void mjpeg_isr(int irq, void *arg)
{
    uint8_t *pic;
    uint32_t jpeg_len;

    uint32_t intstatus = bflb_mjpeg_get_intstatus(mjpeg);

    bflb_cam_stop(cam0);
    bflb_mjpeg_stop(mjpeg);

    if (intstatus & MJPEG_INTSTS_ONE_FRAME)
    {
        bflb_mjpeg_int_clear(mjpeg, MJPEG_INTCLR_ONE_FRAME);

        receive_status = 1; // 接收状态置为1
    }
}

// D200 电源初始化
void d200_power_init(void)
{
    struct bflb_device_s *gpio;

    gpio = bflb_device_get_by_name("gpio");

    /* DVP0 GPIO init */
    bflb_gpio_init(gpio, GPIO_PIN_19, GPIO_OUTPUT | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_2);
}

// D200 电源开
void d200_power_on(void)
{
    struct bflb_device_s *gpio;

    gpio = bflb_device_get_by_name("gpio");

    bflb_gpio_reset(gpio, GPIO_PIN_19);
}

// D200 电源关
void d200_power_off(void)
{
    struct bflb_device_s *gpio;

    camera_mode = 0;
    bflb_cam_stop(cam0);
    bflb_mjpeg_stop(mjpeg);

    gpio = bflb_device_get_by_name("gpio");

    bflb_gpio_set(gpio, GPIO_PIN_19);
}

// D200 复位
void d200_dvp_rst(void)
{
    struct bflb_device_s *gpio;

    gpio = bflb_device_get_by_name("gpio");

    bflb_gpio_reset(gpio, GPIO_PIN_3);
    bflb_mtimer_delay_ms(10);
    bflb_gpio_set(gpio, GPIO_PIN_3);
    bflb_mtimer_delay_ms(10);
}

// D200 dvp 初始化
void d200_dvp_gpio_init(void)
{
    struct bflb_device_s *gpio;

    gpio = bflb_device_get_by_name("gpio");

    /* I2C GPIO */
    bflb_gpio_init(gpio, GPIO_PIN_0, GPIO_FUNC_I2C0 | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_1, GPIO_FUNC_I2C0 | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    /* Reset GPIO */
    bflb_gpio_init(gpio, GPIO_PIN_3, GPIO_OUTPUT | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);

    /* MCLK GPIO */
    bflb_gpio_init(gpio, GPIO_PIN_23, GPIO_FUNC_CLKOUT | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);

    /* DVP0 GPIO */
    bflb_gpio_init(gpio, GPIO_PIN_24, GPIO_FUNC_CAM | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_25, GPIO_FUNC_CAM | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_26, GPIO_FUNC_CAM | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_27, GPIO_FUNC_CAM | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_28, GPIO_FUNC_CAM | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_29, GPIO_FUNC_CAM | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_30, GPIO_FUNC_CAM | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_31, GPIO_FUNC_CAM | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_32, GPIO_FUNC_CAM | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_33, GPIO_FUNC_CAM | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_34, GPIO_FUNC_CAM | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
}

// d200驱动扫描
int8_t d200_drive_scan(void)
{
    i2c0 = bflb_device_get_by_name("i2c0");
    cam0 = bflb_device_get_by_name("cam0");

    if (image_sensor_scan(i2c0, &sensor_config))
    {
        printf("\r\nSensor name: %s\r\n", sensor_config->name);
    }
    else
    {
        printf("\r\nError! Can't identify sensor!\r\n");
        return -1;
    }
    return 0;
}

// D200 摄像头启动
void d200_camera_start(void)
{
    camera_mode = 1;
    bflb_cam_start(cam0);
    bflb_mjpeg_start(mjpeg);
}

// D200 摄像头结束
void d200_camera_stop(void)
{
    camera_mode = 0;
    bflb_cam_stop(cam0);
    bflb_mjpeg_stop(mjpeg);
}

// D200 信号量初始化
void d200_semap_init(void)
{
    camera_muxsem_handle = xSemaphoreCreateMutex();
    if (NULL != camera_muxsem_handle)
    {
        printf("camera_muxsem_handle creat success!\r\n");
    }
}

// D200 信号量上锁
void d200_semap_lock(void)
{
    if (camera_muxsem_handle != NULL)
    {
        xSemaphoreTake(camera_muxsem_handle, portMAX_DELAY);
    }
}

// D200 信号量开锁
void d200_semap_uplock(void)
{
    if (camera_muxsem_handle != NULL)
    {
        xSemaphoreGive(camera_muxsem_handle);
    }
}

// D200 摄像头初始化
void d200_camera_init(void)
{
    d200_dvp_gpio_init();

    // DVP 复位
    d200_dvp_rst();
    // 驱动扫描
    d200_drive_scan();

    memcpy(&cam_config, sensor_config, IMAGE_SENSOR_INFO_COPY_SIZE);
    cam_config.with_mjpeg = true;
    cam_config.input_source = CAM_INPUT_SOURCE_DVP;
    cam_config.output_format = CAM_OUTPUT_FORMAT_AUTO;
    cam_config.output_bufaddr = dvp_buffer;
    cam_config.output_bufsize = cam_config.resolution_x * 2 * DVP_JPEG_ROW_NUM;

    printf("dvp x: %d\r\n", cam_config.resolution_x);
    printf("dvp y: %d\r\n", cam_config.resolution_y);

    bflb_cam_init(cam0, &cam_config);
}

// 摄像头camera_mjpege初始化
void d200_camera_mjpeg_init(void)
{
    mjpeg = bflb_device_get_by_name("mjpeg");

    struct bflb_mjpeg_config_s config;

    config.format = MJPEG_FORMAT_YUV422_YUYV; // 格式
    config.quality = 50;                      // 质量
    config.rows = DVP_JPEG_ROW_NUM;           // 这个参数有什么用处呢
    config.resolution_x = cam_config.resolution_x;
    config.resolution_y = cam_config.resolution_y;
    config.input_bufaddr0 = (uint32_t)dvp_buffer;
    config.input_bufaddr1 = 0;
    config.output_bufaddr = (uint32_t)mjpeg_buffer;
    config.output_bufsize = sizeof(mjpeg_buffer);
    config.input_yy_table = NULL; /* use default table */
    config.input_uv_table = NULL; /* use default table */

    bflb_mjpeg_init(mjpeg, &config);

    jpg_head_len = JpegHeadCreate(YUV_MODE_422, 50, cam_config.resolution_x, cam_config.resolution_y, jpg_head_buf);
    bflb_mjpeg_fill_jpeg_header_tail(mjpeg, jpg_head_buf, jpg_head_len);

    bflb_mjpeg_tcint_mask(mjpeg, false);
    bflb_irq_attach(mjpeg->irq_num, mjpeg_isr, NULL);
    bflb_irq_enable(mjpeg->irq_num);
}

// camera 运行
void d200_camera_run(void)
{
    uint8_t *pic;
    uint32_t jpeg_len;

    if (receive_status == 1)
    {
        receive_status = 0;

        jpeg_len = bflb_mjpeg_get_frame_info(mjpeg, &pic);

        bflb_mjpeg_pop_one_frame(mjpeg);

        bflb_l1c_dcache_invalidate_range(pic, jpeg_len);

        xSemaphoreTake(camera_muxsem_handle, portMAX_DELAY);
        for (int i = 0; i < jpeg_len; i++)
        {
            mjpeg_output_buffer[i] = pic[i];
        }
        pic_size = jpeg_len;
        xSemaphoreGive(camera_muxsem_handle);
        // printf("camera length:%d\r\n", pic_size);

        if (camera_mode == 1)
        {
            bflb_mjpeg_update_input_output_buff(mjpeg, dvp_buffer, NULL, mjpeg_buffer, sizeof(mjpeg_buffer));
            bflb_mjpeg_start(mjpeg);
            bflb_cam_start(cam0);
        }
    }
}

// 输出buff初始化
void d200_camera_output_buff_init(void)
{
    pic_addr = (uint32_t)mjpeg_output_buffer;
}

// 获取输出buff的地址
uint8_t *d200_camera_output_buff_addr(void)
{
    return pic_addr;
}

// 获取输出buff的长度
uint32_t d200_camera_output_buff_length(void)
{
    return pic_size;
}

// 摄像头系统初始化
void d200_camera_system_init(void)
{
    // d200 摄像头输出buf初始化
    d200_camera_output_buff_init();

    // 摄像头初始化
    d200_camera_init();

    // 摄像头mjpeg初始化
    d200_camera_mjpeg_init();
}

// D200 系统初始化
void bl618_camera_task(void *msg)
{
    // 电源初始化
    d200_power_init();

    // 摄像头电源 开
    d200_power_on();

    // 信号量初始化
    d200_semap_init();

    // 摄像头系统初始化
    d200_camera_system_init();

    // 摄像头启动
    d200_camera_start();

    while (1)
    {
        d200_camera_run();
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
