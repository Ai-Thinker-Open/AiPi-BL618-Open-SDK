#define __QYQ_MODULE_BUTTON_C_
#include "qyq_module_button.h"

static int8_t qyq_module_button_get_buttonvalue(struct qyq_module_button_type *this, uint16_t *value)
{
    // 判断状态是否有变化
    static uint8_t reservestatus = 0;
    uint16_t timercountvalue = 0;

    if (this == null)
    {
        return -1;
    }

    if (value == null)
    {
        return -1;
    }

    this->qyq_module_button_config->qyq_module_button_current_status = this->qyq_module_button_config->qyq_module_button_statusin();
    // printf("status value is %d\r\n", this->qyq_module_button_config->qyq_module_button_current_status);

    if (this->qyq_module_button_config->qyq_module_button_last_status != this->qyq_module_button_config->qyq_module_button_current_status)
    {
        this->qyq_module_button_config->qyq_module_button_last_status = this->qyq_module_button_config->qyq_module_button_current_status;
        this->qyq_module_button_config->qyq_module_button_longtimer_count = 0;
        return 0;
    }
    // 去抖动
    if (this->qyq_module_button_config->qyq_module_button_longtimer_count < 2)
    {
        this->qyq_module_button_config->qyq_module_button_longtimer_count++;
        return 0;
    }
    if (this->qyq_module_button_config->qyq_module_button_current_status) // 判断当前状态
    {
        reservestatus = this->qyq_module_button_config->qyq_module_button_last_status;
        if (this->qyq_module_button_config->qyq_module_button_longtimer_value != null)
        {
            timercountvalue = this->qyq_module_button_config->qyq_module_button_longtimer_value[reservestatus - 1];
        }
        else
        {
            timercountvalue = 10000;
        }
        if (this->qyq_module_button_config->qyq_module_button_longtimer_count < timercountvalue)
        {
            this->qyq_module_button_config->qyq_module_button_longtimer_count++;
        }

        if (this->qyq_module_button_config->qyq_module_button_longtimer_count >= timercountvalue)
        {
            // 返回长按按键
            if (!this->qyq_module_button_config->qyq_module_button_process_flag)
            {
                this->qyq_module_button_config->qyq_module_button_pres_flag = 0;
                this->qyq_module_button_config->qyq_module_button_process_flag = 1;
                *value = (2 * this->qyq_module_button_config->qyq_module_button_number + reservestatus);
                return 1;
            }
        }
        else
        {
            if (this->qyq_module_button_config->qyq_module_button_pres_flag)
            {
                // 设置双击按键标志
                if (this->qyq_module_button_config->qyq_module_button_contimer_value != null)
                {
                    timercountvalue = this->qyq_module_button_config->qyq_module_button_contimer_value[reservestatus - 1];
                }
                else
                {
                    timercountvalue = 0;
                }
                if ((this->qyq_module_button_config->qyq_module_button_contimer_count != 0) &&
                    (this->qyq_module_button_config->qyq_module_button_contimer_count < timercountvalue))
                {
                    if (!this->qyq_module_button_config->qyq_module_button_process_flag)
                    {
                        this->qyq_module_button_config->qyq_module_button_process_flag = 1;
                        *value = (1 * this->qyq_module_button_config->qyq_module_button_number + reservestatus);
                        return 1;
                    }
                }
            }
            else
            {
                this->qyq_module_button_config->qyq_module_button_pres_flag = 1;
                this->qyq_module_button_config->qyq_module_button_contimer_count = 0;
            }
        }
    }
    else
    {
        // 等待双击按键是否执行
        this->qyq_module_button_config->qyq_module_button_longtimer_count = 0;
        if (this->qyq_module_button_config->qyq_module_button_contimer_value != null)
        {
            if (reservestatus != 0)
            {
                timercountvalue = this->qyq_module_button_config->qyq_module_button_contimer_value[reservestatus - 1];
            }
        }
        else
        {
            timercountvalue = 0;
        }
        if (this->qyq_module_button_config->qyq_module_button_contimer_count < timercountvalue)
        {
            this->qyq_module_button_config->qyq_module_button_contimer_count++;
            return 0;
        }
        // 返回单击按键并清除设置状态
        if ((this->qyq_module_button_config->qyq_module_button_pres_flag == 1) && (this->qyq_module_button_config->qyq_module_button_process_flag == 0))
        {
            this->qyq_module_button_config->qyq_module_button_pres_flag = 0;
            this->qyq_module_button_config->qyq_module_button_process_flag = 1;
            *value = reservestatus;
            return 1;
        }
        else
        {
            this->qyq_module_button_config->qyq_module_button_pres_flag = 0;
            this->qyq_module_button_config->qyq_module_button_process_flag = 0;
        }
    }

    return 0;
}

int8_t qyq_module_button_create(qyq_module_button_type_t *this, qyq_module_button_config_t *qyq_module_button_config)
{
    if (this == null)
    {
        return -1;
    }
    if (qyq_module_button_config == null)
    {
        return -1;
    }

    this->qyq_module_button_config = qyq_module_button_config;

    this->qyq_module_button_config->qyq_module_button_current_status = 0;
    this->qyq_module_button_config->qyq_module_button_last_status = 0;
    this->qyq_module_button_config->qyq_module_button_process_flag = 0;
    this->qyq_module_button_config->qyq_module_button_pres_flag = 0;
    this->qyq_module_button_config->qyq_module_button_contimer_count = 0;
    this->qyq_module_button_config->qyq_module_button_longtimer_count = 0;

    this->qyq_module_button_get_buttonvalue = qyq_module_button_get_buttonvalue;

    return 0;
}
