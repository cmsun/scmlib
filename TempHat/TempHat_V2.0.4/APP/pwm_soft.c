#include <stdbool.h>
#include <stdint.h>
#include "pwm.h"

typedef struct {
    bool cmd;               //PWMʹ�ܣ�1 PWMʧ�ܣ�0
    uint16_t period;        //PWM����
    float duty_cycles;      //PWMռ�ձȣ�0% - 100%
    uint16_t reverse_point; //��ƽ��ת��λ��
    uint16_t call_cnt;      //һ�������ڵĵ��ô���
} PWM_TypeDef;

PWM_TypeDef pwm;

//period:��ʾ����pwm_period_call()����period����Ϊһ��PWM���ڡ�
void pwm_init(uint16_t period, float duty_cycles)
{
    pwm.cmd = false;
    pwm.period = period;
    pwm.duty_cycles = duty_cycles;
    pwm.reverse_point = (uint16_t)(pwm.period*pwm.duty_cycles + 0.5f);
    
    _GPIO_Init_OutPut(PWM_GPIO_PORT, PWM_GPIO_PIN);
}

static void pwm_set_pin_state(uint8_t state)
{
    if(state == 0)
    {
        _GPIO_WriteLow(PWM_GPIO_PORT, PWM_GPIO_PIN);
    }
    else
    {
        _GPIO_WriteHigh(PWM_GPIO_PORT, PWM_GPIO_PIN);
    }
}

void pwm_set_period(uint16_t period)
{
    pwm.period = period;
    pwm.reverse_point = (uint16_t)(pwm.period*pwm.duty_cycles + 0.5f);
    pwm.call_cnt = 0;
}

void pwm_set_duty_cycles(float duty_cycles)
{
    if(duty_cycles < 0)
        duty_cycles = 0;
    else if(duty_cycles > 1)
        duty_cycles = 1;
    
    pwm.duty_cycles = duty_cycles;
    pwm.reverse_point = (uint16_t)(pwm.period*pwm.duty_cycles + 0.5f);
    pwm.call_cnt = 0;
}

void pwm_enable(void)
{
    pwm.cmd = true;
}

void pwm_disable(pwm_IDLE_STATE state)
{
    pwm.cmd = false;
    pwm.call_cnt = 0;
    pwm_set_pin_state(state);
}

void pwm_period_call(void)
{
    if(pwm.cmd == true)
    {      
        if(pwm.call_cnt++ < pwm.reverse_point)
            pwm_set_pin_state(1);
        else
            pwm_set_pin_state(0);
        pwm.call_cnt %= pwm.period;
    }
    else
    {
        pwm.call_cnt = 0;
    }
}