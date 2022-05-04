#include "key.h"
#include "led.h"
#include "fan.h"
#include "pid.h"
#include "tick.h"
#include "power.h"
#include "analog.h"
#include "digitron.h"
#include "TempManage.h"

TempManage_TypeDef TempManage = {
    .BodyTemp = 37.3f,
    .ctrl = TEMP_CTRL_SLEEP,
};

void temp_manage_init(void)
{
    pid_init(&TempManage.pid);
}

void temp_manage_task_loop(void)
{
    float temp, duty_cycles = 0;
    static uint32_t tick = 0, delay2sleep = 0;
    KEY_ACTION key;

    if(tick_value() - tick > 50)
    {
        tick = tick_value();
        
        temp = analog_semiconductor_temp();
        digitron_set_value(analog_body_temp());
        
        key = key_action();
        if(key == KEY_ACTION_CLICK)
        {
            if(TempManage.ctrl == TEMP_CTRL_SLEEP)
            {
#ifdef STM8S003
                TempManage.ctrl = TEMP_CTRL_STANDBY;
#endif
            }
            else
            {
                TempManage.ctrl++;
                TempManage.ctrl %= 3;
                pid_reset(&TempManage.pid);
            }
        }
        else if(key == KEY_ACTION_DOUBLE_CLICK)
        {
            if(TempManage.ctrl == TEMP_CTRL_SLEEP)
                TempManage.ctrl = TEMP_CTRL_STANDBY;
        }
        else if(key == KEY_ACTION_LONG_PRESS)
        {
            delay2sleep = 0;
            TempManage.ctrl = TEMP_CTRL_SLEEP;
            pid_reset(&TempManage.pid);
            pwr_set_lowpower_mode();
        }

        if(TempManage.ctrl == TEMP_CTRL_SLEEP) //被唤醒后
        {
            if(delay2sleep++ > 10) //被唤醒1秒内没有检测到双击就继续休眠
            {
                delay2sleep = 0;
                pwr_set_lowpower_mode();
            }
        }
        else if(TempManage.ctrl == TEMP_CTRL_STANDBY)
        {
            pwr_temp_control_standby();
        }
        else if(TempManage.ctrl == TEMP_CTRL_HEAT)
        {
            duty_cycles = pid_process(&TempManage.pid, HEAT_LIMIT, temp);
            pwr_temp_control_heat(duty_cycles);

        }
        else if(TempManage.ctrl == TEMP_CTRL_COOL)
        {
            duty_cycles = pid_process(&TempManage.pid, COOL_LIMIT, temp);
            pwr_temp_control_cool(duty_cycles);
        }
    }
}