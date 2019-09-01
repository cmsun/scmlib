#include "stm32f10x.h"
#include "modbus.h"

void TIM3_Init(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //时钟使能
    //定时器TIM3初始化5毫秒中断一次
    TIM_TimeBaseStructure.TIM_Period = 50 - 1;                  //设置在下一个更新事件装入活动的自动重装载寄存器周期的值
    TIM_TimeBaseStructure.TIM_Prescaler = 7200 - 1;             //设置用来作为TIMx时钟频率除数的预分频值
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;     //设置时钟分割:TDTS = Tck_tim
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //TIM向上计数模式
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);             //根据指定的参数初始化TIMx的时间基数单位
    //中断优先级NVIC设置
    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;           //TIM3中断
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; //先占优先级1级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;        //从优先级3级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           //IRQ通道被使能
    NVIC_Init(&NVIC_InitStructure);                           //初始化NVIC寄存器
    
    TIM_ClearITPendingBit(TIM3, TIM_IT_Update);//清除更新中断请求位，否则会产即进入中断
    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE); //使能指定的TIM3中断,允许更新中断
    //TIM_Cmd(TIM3, ENABLE); //使能TIMx
}

__weak void TIM3Callback(void) {}

void TIM3_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
    {
        TIM3Callback();
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
    }
}
