#define __BL618_WIFI_C_
#include "bl618_wifi.h"

// #include "custom.h"
typedef void (*bl618_wifi_callback_t)(void);

static TaskHandle_t bl618_wifi_handle;
static uint32_t bl618_wifi_sta_connect_status = 0;
static wifi_conf_t conf = {
    .country_code = "CN",
};

static bl618_wifi_callback_t p_bl618_wifi_got_ip_callback = NULL;
static bl618_wifi_callback_t p_bl618_wifi_disconnect_callback = NULL;
static bl618_wifi_callback_t p_bl618_wifi_connect_callback = NULL;
static bl618_wifi_callback_t p_bl618_wifi_scan_callback = NULL;
static bl618_wifi_callback_t p_bl618_wifi_init_callback = NULL;
static bl618_wifi_callback_t p_bl618_wifi_on_ap_started_callback = NULL;
static bl618_wifi_callback_t p_bl618_wifi_on_ap_stopped_callback = NULL;
static bl618_wifi_callback_t p_bl618_wifi_on_ap_sta_add_callback = NULL;
static bl618_wifi_callback_t p_bl618_wifi_on_ap_sta_del_callback = NULL;

uint8_t bl618_wifi_connect(char *ssid, char *passwd)
{
    // 判断参数是否正确
    if (NULL == ssid || 0 == strlen(ssid))
    {
        return 1;
    }

    // 获取STA连接状态
    if (wifi_mgmr_sta_state_get() == 1)
    {
        wifi_sta_disconnect();
    }

    // 判断WiFi连接
    if (wifi_sta_connect(ssid, passwd, NULL, NULL, 0, 0, 0, 1))
    {
        return 4;
    }

    return 0;
}

uint8_t bl618_wifi_ap_start(uint8_t *ssid, uint8_t *pwd)
{
    wifi_mgmr_ap_params_t config = {0};

    config.channel = 3;
    config.key = pwd;
    config.ssid = ssid;
    config.use_dhcpd = 1;

    if (wifi_mgmr_conf_max_sta(1) != 0)
    {
        return 5;
    }

    return wifi_mgmr_ap_start(&config);
}

void bl618_wifi_init(void)
{
    // tcp协议栈初始化
    tcpip_init(NULL, NULL);

    /* enable wifi clock */
    printf("Starting wifi ...");

    GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_IP_WIFI_PHY | GLB_AHB_CLOCK_IP_WIFI_MAC_PHY | GLB_AHB_CLOCK_IP_WIFI_PLATFORM);
    GLB_AHB_MCU_Software_Reset(GLB_AHB_MCU_SW_WIFI);

    /* set ble controller EM Size */

    GLB_Set_EM_Sel(GLB_WRAM160KB_EM0KB);

    if (0 != rfparam_init(0, NULL, 0))
    {
        printf("PHY RF init failed!");
        return 0;
    }

    printf("PHY RF init success!");

    extern void interrupt0_handler(void);
    bflb_irq_attach(WIFI_IRQn, (irq_callback)interrupt0_handler, NULL);
    bflb_irq_enable(WIFI_IRQn);

    xTaskCreate(wifi_main, (char *)"fw", BL618_WIFI_STACK_SIZE, NULL, BL618_WIFI_TASK_PRIORITY_FW, &bl618_wifi_handle);
}

void wifi_event_handler(uint32_t code)
{
    switch (code)
    {
    case CODE_WIFI_ON_INIT_DONE:
    {
        wifi_mgmr_init(&conf);
        if (p_bl618_wifi_init_callback != NULL)
        {
            p_bl618_wifi_init_callback();
        }
    }
    break;
    case CODE_WIFI_ON_MGMR_DONE:
    {
        printf("CODE_WIFI_ON_MGMR_DONE \r\n");
    }
    break;
    case CODE_WIFI_ON_SCAN_DONE:
    {
        if (p_bl618_wifi_scan_callback != NULL)
        {
            p_bl618_wifi_scan_callback();
        }
    }
    break;
    case CODE_WIFI_ON_CONNECTED:
    {
        if (p_bl618_wifi_connect_callback != NULL)
        {
            p_bl618_wifi_connect_callback();
        }
    }
    break;
    case CODE_WIFI_ON_GOT_IP:
    {
        if (p_bl618_wifi_got_ip_callback != NULL)
        {
            p_bl618_wifi_got_ip_callback();
        }
    }
    break;
    case CODE_WIFI_ON_DISCONNECT:
    {
        if (p_bl618_wifi_disconnect_callback != NULL)
        {
            p_bl618_wifi_disconnect_callback();
        }
    }
    break;
    case CODE_WIFI_ON_AP_STARTED:
    {
        if (p_bl618_wifi_on_ap_started_callback != NULL)
        {
            p_bl618_wifi_on_ap_started_callback();
        }
    }
    break;
    case CODE_WIFI_ON_AP_STOPPED:
    {
        if (p_bl618_wifi_on_ap_stopped_callback != NULL)
        {
            p_bl618_wifi_on_ap_stopped_callback();
        }
    }
    break;
    case CODE_WIFI_ON_AP_STA_ADD:
    {
        if (p_bl618_wifi_on_ap_sta_add_callback != NULL)
        {
            p_bl618_wifi_on_ap_sta_add_callback();
        }
    }
    break;
    case CODE_WIFI_ON_AP_STA_DEL:
    {
        if (p_bl618_wifi_on_ap_sta_del_callback != NULL)
        {
            p_bl618_wifi_on_ap_sta_del_callback();
        }
    }
    break;
    default:
    {
    }
    break;
    }
}

void bl618_wifi_callback_register(void (*bl618_wifi_callback)(void), uint32_t code)
{
    switch (code)
    {
    case CODE_WIFI_ON_INIT_DONE:
    {
        if (p_bl618_wifi_init_callback == NULL)
        {
            p_bl618_wifi_init_callback = bl618_wifi_callback;
        }
    }
    break;
    case CODE_WIFI_ON_MGMR_DONE:
    {
    }
    break;
    case CODE_WIFI_ON_SCAN_DONE:
    {
        if (p_bl618_wifi_scan_callback == NULL)
        {
            p_bl618_wifi_scan_callback = bl618_wifi_callback;
        }
    }
    break;
    case CODE_WIFI_ON_CONNECTED:
    {
        if (p_bl618_wifi_connect_callback == NULL)
        {
            p_bl618_wifi_connect_callback = bl618_wifi_callback;
        }
    }
    break;
    case CODE_WIFI_ON_GOT_IP:
    {
        if (p_bl618_wifi_got_ip_callback == NULL)
        {
            p_bl618_wifi_got_ip_callback = bl618_wifi_callback;
        }
    }
    break;
    case CODE_WIFI_ON_DISCONNECT:
    {
        if (p_bl618_wifi_disconnect_callback == NULL)
        {
            p_bl618_wifi_disconnect_callback = bl618_wifi_callback;
        }
    }
    break;
    case CODE_WIFI_ON_AP_STARTED:
    {
        if (p_bl618_wifi_on_ap_started_callback == NULL)
        {
            p_bl618_wifi_on_ap_started_callback = bl618_wifi_callback;
        }
    }
    break;
    case CODE_WIFI_ON_AP_STOPPED:
    {
        if (p_bl618_wifi_on_ap_stopped_callback == NULL)
        {
            p_bl618_wifi_on_ap_stopped_callback = bl618_wifi_callback;
        }
    }
    break;
    case CODE_WIFI_ON_AP_STA_ADD:
    {
        if (p_bl618_wifi_on_ap_sta_add_callback == NULL)
        {
            p_bl618_wifi_on_ap_sta_add_callback = bl618_wifi_callback;
        }
    }
    break;
    case CODE_WIFI_ON_AP_STA_DEL:
    {
        if (p_bl618_wifi_on_ap_sta_del_callback == NULL)
        {
            p_bl618_wifi_on_ap_sta_del_callback = bl618_wifi_callback;
        }
    }
    break;
    default:
    {
    }
    break;
    }
}

void bl618_wifi_callback_unregister(uint32_t code)
{
    switch (code)
    {
    case CODE_WIFI_ON_INIT_DONE:
    {
        p_bl618_wifi_init_callback = NULL;
    }
    break;
    case CODE_WIFI_ON_MGMR_DONE:
    {
    }
    break;
    case CODE_WIFI_ON_SCAN_DONE:
    {
        p_bl618_wifi_scan_callback = NULL;
    }
    break;
    case CODE_WIFI_ON_CONNECTED:
    {
        p_bl618_wifi_connect_callback = NULL;
    }
    break;
    case CODE_WIFI_ON_GOT_IP:
    {
        p_bl618_wifi_got_ip_callback = NULL;
    }
    break;
    case CODE_WIFI_ON_DISCONNECT:
    {
        p_bl618_wifi_disconnect_callback = NULL;
    }
    break;
    case CODE_WIFI_ON_AP_STARTED:
    {
        p_bl618_wifi_on_ap_started_callback = NULL;
    }
    break;
    case CODE_WIFI_ON_AP_STOPPED:
    {
        p_bl618_wifi_on_ap_stopped_callback = NULL;
    }
    break;
    case CODE_WIFI_ON_AP_STA_ADD:
    {
        p_bl618_wifi_on_ap_sta_add_callback = NULL;
    }
    break;
    case CODE_WIFI_ON_AP_STA_DEL:
    {
        p_bl618_wifi_on_ap_sta_del_callback = NULL;
    }
    break;
    default:
    {
    }
    break;
    }
}
