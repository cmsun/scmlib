#include "led.h"
#include "fan.h"
#include "tick.h"
#include "PWM.h"
#include "analog.h"
#include "digitron.h"
#include "power.h"

bool LowPowerMode = true;

void pwr_init(void)
{
    pwm_init(10, 0.5f);
    _GPIO_Init_OutPut(TEMP_SWITCH_GPIO_PORT, TEMP_SWITCH_GPIO_PIN);
    _GPIO_Init_OutPut(PERIPHERALS_CTRL_GPIO_PORT, PERIPHERALS_CTRL_GPIO_PIN);
    _GPIO_Init_InPut(CHARGER_STDBY_GPIO_PORT, CHARGER_STDBY_GPIO_PIN);
    _GPIO_Init_InPut(CHARGER_CHRG_GPIO_PORT, CHARGER_CHRG_GPIO_PIN);
    
    pwm_disable(PWM_IDLE_STATE_RESET);
    _GPIO_WriteLow(TEMP_SWITCH_GPIO_PORT, TEMP_SWITCH_GPIO_PIN);
    _GPIO_WriteLow(PERIPHERALS_CTRL_GPIO_PORT, PERIPHERALS_CTRL_GPIO_PIN);
}

void pwr_set_lowpower_mode(void)
{
    LowPowerMode = true;
    
    fan_off();
    led_off(LED_GREEN);
    led_off(LED_RED);
    led_off(LED_BLUE);
    digitron_disable();
    //BDR6121B电机芯片的INA和INB口必须同时给0，否则即使MOS管Q2截止也会让VCC_5V会有4.3V的电压。
    pwm_disable(PWM_IDLE_STATE_RESET);
    _GPIO_WriteLow(TEMP_SWITCH_GPIO_PORT, TEMP_SWITCH_GPIO_PIN);
    //MOS管Q2截止，断掉VCC_5V电源
    _GPIO_WriteHigh(PERIPHERALS_CTRL_GPIO_PORT, PERIPHERALS_CTRL_GPIO_PIN);
    
#ifdef STM8S003
    GPIO_Init(KEY_GPIO_PORT, KEY_GPIO_PIN, GPIO_MODE_IN_FL_IT);
    EXTI_SetExtIntSensitivity(KEY_EXIT_PORT, EXTI_SENSITIVITY_FALL_ONLY);
    EXTI_SetTLISensitivity(EXTI_TLISENSITIVITY_FALL_ONLY);
    __halt();
#else
    Gpio_EnableIrq(KEY_GPIO_PORT, KEY_GPIO_PIN, GpioIrqFalling);
    EnableNvic(KEY_EXIT_PORT, IrqLevel3, TRUE);
    Lpm_GotoDeepSleep(FALSE); //FALSE表示退出IO中断后不再继续休眠，TRUE表示退出IO中断后马继续休眠。
#endif
}

void pwr_exit_lowpower_mode(void)
{
    LowPowerMode = false;
}

bool pwr_is_lowpower_mode(void)
{
    return LowPowerMode;
}

bool pwr_is_fully_charged(void)
{
    return (bool)(_GPIO_ReadInputPin(CHARGER_STDBY_GPIO_PORT, CHARGER_STDBY_GPIO_PIN) == 0);
}

bool pwr_is_charging(void)
{
    return (bool)(_GPIO_ReadInputPin(CHARGER_CHRG_GPIO_PORT, CHARGER_CHRG_GPIO_PIN) == 0);
}

void pwr_temp_control_standby(void)
{
    fan_off();
    led_off(LED_RED);
    led_off(LED_BLUE);
    digitron_enable();
    pwm_disable(PWM_IDLE_STATE_RESET);
    _GPIO_WriteLow(TEMP_SWITCH_GPIO_PORT, TEMP_SWITCH_GPIO_PIN);
    _GPIO_WriteLow(PERIPHERALS_CTRL_GPIO_PORT, PERIPHERALS_CTRL_GPIO_PIN);
}

void pwr_temp_control_cool(float duty_cycles)
{
    led_off(LED_RED);
    led_on(LED_BLUE);
    
    _GPIO_WriteLow(TEMP_SWITCH_GPIO_PORT, TEMP_SWITCH_GPIO_PIN);
    if(duty_cycles < -0.01f)
    {
        fan_on();
        pwm_set_duty_cycles(-duty_cycles);
        pwm_enable();
    }
    else
    {
        fan_off();
        pwm_disable(PWM_IDLE_STATE_RESET);
    }
}

void pwr_temp_control_heat(float duty_cycles)
{
    fan_off();
    led_on(LED_RED);
    led_off(LED_BLUE);
    
    _GPIO_WriteHigh(TEMP_SWITCH_GPIO_PORT, TEMP_SWITCH_GPIO_PIN);
    if(duty_cycles > 0.01f)
    {
        pwm_set_duty_cycles(1-duty_cycles);
        pwm_enable();
    }
    else
    {        
        pwm_disable(PWM_IDLE_STATE_SET);
    }
}

void pwr_task_loop(void)
{
    static uint32_t tick = 0, cnt = 0;
    if(tick_value() - tick > 100)
    {
        tick = tick_value();
        if(pwr_is_fully_charged())
        {
            led_on(LED_GREEN);
        }
        else if(pwr_is_charging())
        {
            if(++cnt == 10)
            {
                cnt = 0;
                led_toggle(LED_GREEN);
            }
        }
        else if(analog_battery_voltage() < 3.0f)
        {
            led_toggle(LED_GREEN);
        }
        else
        {
            led_off(LED_GREEN);
        }
    }
}

#ifndef STM8S003
void Port0_IRQHandler(void)
{
    if(TRUE == Gpio_GetIrqStatus(KEY_GPIO_PORT, KEY_GPIO_PIN))
    {
        Gpio_ClearIrq(KEY_GPIO_PORT, KEY_GPIO_PIN);
    }
}
#endif