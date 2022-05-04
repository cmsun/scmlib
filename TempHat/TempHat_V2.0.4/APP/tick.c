#include <stdint.h>
#include "pwm.h"

volatile uint32_t Tick = 0;

void tick_init(void)
{
#ifdef STM8S003
    TIM4_TimeBaseInit(TIM4_PRESCALER_128, 125); //16 MHz / 128 = 125 000 Hz
    TIM4_ClearFlag(TIM4_FLAG_UPDATE);
    TIM4_ITConfig(TIM4_IT_UPDATE, ENABLE);
    TIM4_Cmd(ENABLE);
#else
    SysTick_Config(SystemCoreClock/1000);
#endif
}

//��ʱ���ж��е������������ÿһ���� sysTick �����Լ�1
void tick_increase(void)
{
    Tick++;
}

uint32_t tick_value(void)
{
    return Tick;
}

void tick_delay(uint32_t delay)
{
    uint32_t t = Tick;
    while(Tick - t < delay)
        ;
}

#ifndef STM8S003
//systick�жϺ���
void SysTick_IRQHandler(void)
{
    tick_increase();
    pwm_period_call();
}
#endif

