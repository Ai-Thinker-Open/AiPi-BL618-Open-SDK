#define __BL616_UART_C_
#include "bl616_uart.h"
#include "qyq_frame_at.h"

qyq_frame_at_type_t qyq_frame_at;
qyq_frame_at_config_t qyq_frame_at_config;
static TaskHandle_t bl618_uart_handle;
static TaskHandle_t bl618_uart_tick_handle;
static TaskHandle_t bl618_uart_rec_handle;
volatile struct bflb_device_s *uartx;
volatile struct bflb_device_s *gpio;
static uint8_t uart1_read_buf[1024];

uint8_t s_led_status = 0;

char *repeat_string(const char *str, int length)
{
    int str_len = strlen(str);
    int repeat_count = length / str_len + 1;

    char *result = (char *)malloc((length + 1) * sizeof(char));
    if (result == NULL)
    {
        return NULL; // 内存分配失败
    }

    int pos = 0;
    for (int i = 0; i < repeat_count; i++)
    {
        for (int j = 0; j < str_len; j++)
        {
            if (pos < length)
            {
                result[pos++] = str[j];
            }
            else
            {
                break;
            }
        }
    }
    result[length] = '\0'; // 确保字符串以空字符结尾

    return result;
}

void remove_trailing_newline(char *str)
{
    int len = strlen(str);
    if (len > 0 && str[len - 1] == '\n')
    {
        str[len - 1] = '\0';
        len--;
    }
    if (len > 0 && str[len - 1] == '\r')
    {
        str[len - 1] = '\0';
    }
}

int8_t bl616_sta_connect(unsigned int argc, const char **argv)
{
    printf("%s\r\n", argv[0]);
    // 检查参数个数是否正确
    if (argc != 2)
    {
        return -1; // 返回错误码
    }

    uint8_t *ssid_str = repeat_string(argv[0], strlen(argv[0]));
    // printf("pss length:%d",strlen(argv[1]));
    uint8_t *pwd_str = repeat_string(argv[1], strlen(argv[1]));

    remove_trailing_newline(ssid_str);
    remove_trailing_newline(pwd_str);
    remove_trailing_newline(pwd_str);
    remove_trailing_newline(pwd_str);
    remove_trailing_newline(pwd_str);

    printf("%s", pwd_str);
    printf("length:%d\r\n", strlen(pwd_str));

    if (bl618_wifi_connect(ssid_str, pwd_str) != 0)
    {
        printf("bl618 wifi connecct fail!\r\n");
    }

    return 0; // 成功返回0
}

int8_t bl616_led_set(unsigned int argc, const char **argv)
{
    uint8_t write_value;
    // printf("%s\r\n", argv[0]);
    // 检查参数个数是否正确
    if (argc != 1)
    {
        return -1; // 返回错误码
    }

    uint8_t *led_str = repeat_string(argv[0], strlen(argv[0]));

    remove_trailing_newline(led_str);
    remove_trailing_newline(led_str);
    remove_trailing_newline(led_str);
    remove_trailing_newline(led_str);

    // 解析AT指令参数

    // 检查输入参数是否为有效的整数
    if (sscanf(led_str, "%hhu", &write_value) != 1)
    {
        return -1; // 返回参数错误码
    }

    s_led_status = write_value;

    // printf("led status :%d", s_led_status);

    return 0; // 成功返回0
}

void aiio_wifi_sta_ip_get(uint32_t *ip, uint32_t *gw, uint32_t *mask)
{
    uint32_t *dns;
    if (wifi_mgmr_sta_state_get())
    {
        return wifi_sta_ip4_addr_get(ip, mask, gw, dns);
    }
    else
    {
        *ip = 0;
        *gw = 0;
        *mask = 0;
    }
}

void print_ip(uint32_t ip)
{
    printf("%d.%d.%d.%d\r\n",
           ip & 0xFF,
           (ip >> 8) & 0xFF,
           (ip >> 16) & 0xFF,
           (ip >> 24) & 0xFF);
}

int8_t bl616_sta_find_ip(unsigned int argc, const char **argv)
{
    uint32_t ip, gw, mask;
    aiio_wifi_sta_ip_get(&ip, &gw, &mask);
    print_ip(ip);
    return 0;
}

qyq_frame_at_list_t qyq_frame_at_list[] = {
    {"+WJAP", 5, null, bl616_sta_find_ip, bl616_sta_connect, null},
    {"+LED", 4, null, null, bl616_led_set, null},
};

void qyq_frame_at_write(uint8_t *buf, uint16_t length)
{
    uint16_t i = 0;
    for (i = 0; i < length; i++)
    {
        bflb_uart_putchar(uartx, buf[i]);
    }
}

// 1ms定时时钟
void bl616_uart1_tick_task(void *msg)
{
    while (1)
    {
        qyq_frame_at.qyq_frame_at_tick(&qyq_frame_at);
        vTaskDelay(pdMS_TO_TICKS(1));
    }
    return NULL;
}

void bl616_uart1_at_rec_task(void *msg)
{
    int ch;

    while (1)
    {
        ch = bflb_uart_getchar(uartx);
        if (ch != -1)
        {
            // printf("0x%02x ", ch);
            qyq_frame_at.qyq_frame_at_recv(&qyq_frame_at, ch);
        }

        vTaskDelay(pdMS_TO_TICKS(1));
    }
}

void bl616_at_init(void)
{
    qyq_frame_at_config.qyq_frame_at_list = qyq_frame_at_list;
    qyq_frame_at_config.qyq_frame_at_list_size = (sizeof(qyq_frame_at_list) / sizeof(qyq_frame_at_list_t));

    qyq_frame_at_config.qyq_frame_at_rx_buf = uart1_read_buf;
    qyq_frame_at_config.qyq_frame_at_rx_size = sizeof(uart1_read_buf);
    qyq_frame_at_config.qyq_frame_at_write = qyq_frame_at_write;

    if (qyq_frame_at_create(&qyq_frame_at, &qyq_frame_at_config) != 0)
    {
        printf("qyq_frame_at_create is fail!\r\n");
    }

    if (qyq_frame_at.qyq_frame_at_init(&qyq_frame_at) != 0)
    {
        printf("qyq_frame_at_init is fail!\r\n");
    }

    xTaskCreate(bl616_uart1_tick_task, (char *)"bl616_uart1_tick_task", 1152, NULL, 15, &bl618_uart_tick_handle);
    xTaskCreate(bl616_uart1_at_rec_task, (char *)"bl616_uart1_at_rec_task", 1152, NULL, 14, &bl618_uart_rec_handle);
}


void bl616_uart0_init(void)
{
    uartx = bflb_device_get_by_name("uart0");
}

// 串口运行任务
void bl616_uart_task(void)
{
    // AT 框架初始化
    bl616_at_init();

    // 串口初始化
    bl616_uart0_init();

    while (1)
    {
        qyq_frame_at.qyq_frame_at_run(&qyq_frame_at);
        vTaskDelay(pdMS_TO_TICKS(5));
    }
}

void bl616_uart_init(void)
{
    xTaskCreate(bl616_uart_task, (char *)"bl616_uart_task", 512, NULL, 20, &bl618_uart_handle);
}
