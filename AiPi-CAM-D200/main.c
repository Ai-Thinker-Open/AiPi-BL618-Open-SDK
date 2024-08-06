#define __MAIN_C_
#include "chip_include.h"
#include "app_include.h"

static TaskHandle_t bl618_freeheap_task_handle;
static TaskHandle_t bl618_camera_task_handle;
static TaskHandle_t bl618_wifi_task_handle;
static TaskHandle_t bl618_button_task_handle;

qyq_module_button_type_t qyq_module_button;
qyq_module_button_config_t qyq_module_button_config;
uint16_t qyq_module_button_contimer_value[8] = {20, 20, 20, 20, 20, 20, 20, 20}; // 短按与双击的间隔时间
uint16_t qyq_module_button_longtimer_value[1] = {200};                           // 长按时间值s
struct bflb_device_s *gpio;

volatile static uint8_t wifi_status = 0;
volatile static uint8_t led_status = 0;
extern uint8_t *pic_addr;
extern uint32_t pic_size;
extern uint8_t s_led_status;

static int8_t qyq_module_tcpclient_write_block(int sockfd, const uint8_t *buf, uint32_t size, uint32_t blocksize)
{
    int total_sent = 0;
    int bytes_sent;

    if (sockfd < 0)
    {
        return -1;
    }

    // 分段发送消息
    while (total_sent < size)
    {
        // 计算本次发送的字节数
        int remaining = size - total_sent;
        int to_send = remaining > blocksize ? blocksize : remaining;

        // 发送消息片段
        bytes_sent = send(sockfd, buf + total_sent, to_send, 0);
        if (bytes_sent == -1)
        {

            return -1;
        }

        total_sent += bytes_sent;
    }
    return 0;
}

void bl618_wifi_init_process(void)
{
    printf("bl618_wifi_init_process\r\n");
}

void bl618_wifi_got_ip_process(void)
{
    printf("bl618_wifi_got_ip_process\r\n");
    wifi_status = 1;
}

void bl618_wifi_disconnect_process(void)
{
    printf("bl618_wifi_got_ip_process\r\n");
    wifi_status = 0;
}

static void bl618_wifi_task(void *pvParameters)
{
    uint8_t data = 0;
    int sock = 0;
    vTaskDelay(pdMS_TO_TICKS(3000));
    bl618_wifi_callback_register(bl618_wifi_got_ip_process, CODE_WIFI_ON_GOT_IP);
    bl618_wifi_callback_register(bl618_wifi_init_process, CODE_WIFI_ON_INIT_DONE);
    bl618_wifi_callback_register(bl618_wifi_disconnect_process, CODE_WIFI_ON_DISCONNECT);

    // vTaskDelay(pdMS_TO_TICKS(3000));
    printf("bl618 wifi connecct\r\n");

    while (1)
    {
        if (wifi_status)
        {
            mhttp_server_init();
        }

        vTaskDelay(pdMS_TO_TICKS(3000));
        // d200_semap_lock();
        // take_photo(pic_addr, pic_size);
        // d200_semap_uplock();
    }
}

void bl618_freeheap_task(void *msg)
{
    size_t freeHeapSize;

    vTaskDelay(pdMS_TO_TICKS(3000));

    while (1)
    {
        freeHeapSize = kfree_size();
        printf("Free heap size: %u bytes\n", freeHeapSize);
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

uint8_t qyq_module_button_statusin(void) // 需要适配，按键输入引脚
{
    if (bflb_gpio_read(gpio, GPIO_PIN_2) == 1)
    {
        return 1;
    }

    return 0;
}

void bl618_button_task(void *msg)
{
    uint16_t value = 0;

    gpio = bflb_device_get_by_name("gpio");

    bflb_gpio_init(gpio, GPIO_PIN_2, GPIO_INPUT | GPIO_PULLDOWN | GPIO_SMT_EN | GPIO_DRV_0);
    bflb_gpio_init(gpio, GPIO_PIN_16, GPIO_OUTPUT | GPIO_PULLDOWN | GPIO_SMT_EN | GPIO_DRV_1);

    // bflb_gpio_init(gpio, GPIO_PIN_2, GPIO_INPUT | GPIO_FLOAT | GPIO_SMT_EN | GPIO_DRV_0);

    printf("bl618_button_task\r\n");
    qyq_module_button_config.qyq_module_button_contimer_value = null;
    qyq_module_button_config.qyq_module_button_longtimer_value = qyq_module_button_longtimer_value;
    qyq_module_button_config.qyq_module_button_number = 1;
    qyq_module_button_config.qyq_module_button_statusin = qyq_module_button_statusin;
    qyq_module_button_create(&qyq_module_button, &qyq_module_button_config);

    while (1)
    {
        if (qyq_module_button.qyq_module_button_get_buttonvalue(&qyq_module_button, &value) != 0)
        {
            switch (value)
            {
            case 1:
                if(s_led_status)
                {
                    bflb_gpio_set(gpio, GPIO_PIN_16);
                    vTaskDelay(pdMS_TO_TICKS(300));
                    take_photo(pic_addr,pic_size);
                    bflb_gpio_reset(gpio, GPIO_PIN_16);
                }
                else
                {
                    take_photo(pic_addr,pic_size);
                }
                break;
            case 3:
                // led_status = !led_status;
                s_led_status = !s_led_status;

                printf("led_ctrl:%d\r\n",s_led_status);
                // if(led_status)
                // {
                //     bflb_gpio_set(gpio, GPIO_PIN_16);
                // }
                // else
                // {
                //     bflb_gpio_reset(gpio, GPIO_PIN_16);
                // }
                break;
            default:
                break;
            }
            // printf("button value is %d\r\n", value);
        }

        // 按键扫描时间 10ms
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

int main(void)
{
    // 板卡初始化
    board_init();

    bl618_wifi_init();

    filesystem_init();

    bl616_uart_init();

    xTaskCreate(bl618_wifi_task, (char *)"bl618_wifi_task", 4096, NULL, 22, &bl618_wifi_task_handle);
    xTaskCreate(bl618_button_task, (char *)"bl618_button_task", 4096, NULL, 23, &bl618_button_task_handle);
    // xTaskCreate(bl618_freeheap_task, (char *)"bl618_freeheap_task", 4096, NULL, 21, &bl618_freeheap_task_handle);
    xTaskCreate(bl618_camera_task, (char *)"bl618_camera_task", 4096, NULL, 20, &bl618_camera_task_handle);

    vTaskStartScheduler();

    while (1)
    {
    }
}
