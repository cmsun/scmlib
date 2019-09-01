#include <stddef.h>
#include "stm32f10x_conf.h"
#include "gpio.h"
#include "debug.h"

void GPIO_Initialize(GPIO_TypeDef *GPIOx,
        uint16_t GPIO_Pin_x,
        GPIOMode_TypeDef GPIO_Mode_x,
        GPIOSpeed_TypeDef GPIO_Speed_x)
{
    _assert(IS_GPIO_PIN(GPIO_Pin_x));
    _assert(IS_GPIO_MODE(GPIO_Mode_x));
    _assert(IS_GPIO_SPEED(GPIO_Speed_x));

    if((uint32_t)GPIOx == GPIOA_BASE)
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    else if((uint32_t)GPIOx == GPIOB_BASE)
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    else if((uint32_t)GPIOx == GPIOC_BASE)
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    else if((uint32_t)GPIOx == GPIOD_BASE)
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
    else if((uint32_t)GPIOx == GPIOE_BASE)
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
    else if((uint32_t)GPIOx == GPIOF_BASE)
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF, ENABLE);

    GPIO_InitTypeDef init;
    init.GPIO_Pin = GPIO_Pin_x;
    init.GPIO_Speed = GPIO_Speed_x;
    init.GPIO_Mode = GPIO_Mode_x;
    GPIO_Init(GPIOx, &init);
}

void GPIO_EXTI_Init(uint8_t GPIO_PortSourceGPIOx,
        uint8_t GPIO_PinSourcex,
        EXTITrigger_TypeDef EXTI_Trigger_x,
        EXTIMode_TypeDef EXTI_Mode_x)
{
    _assert(IS_GPIO_PIN_SOURCE(GPIO_PinSourcex));
    _assert(IS_EXTI_TRIGGER(EXTI_Trigger_x));
    _assert(IS_EXTI_MODE(EXTI_Mode_x));

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOx, GPIO_PinSourcex);

    EXTI_InitTypeDef exti_init;
    exti_init.EXTI_Line = (1 << GPIO_PinSourcex);
    exti_init.EXTI_Mode = EXTI_Mode_x;
    exti_init.EXTI_Trigger = EXTI_Trigger_x;
    exti_init.EXTI_LineCmd = ENABLE;
    EXTI_Init(&exti_init);
}

void GPIO_setNVICPriority(uint8_t GPIO_PinSourcex,
        uint8_t NVIC_IRQChannelPreemptionPriority,
        uint8_t NVIC_IRQChannelSubPriority)
{
    _assert(IS_GPIO_PIN_SOURCE(GPIO_PinSourcex));

    NVIC_InitTypeDef nvic_init;
    nvic_init.NVIC_IRQChannel = (GPIO_PinSourcex <= 4
            ? GPIO_PinSourcex + 6
            : (GPIO_PinSourcex <= 9 ? EXTI9_5_IRQn : EXTI15_10_IRQn));
    nvic_init.NVIC_IRQChannelPreemptionPriority = NVIC_IRQChannelPreemptionPriority;
    nvic_init.NVIC_IRQChannelSubPriority = NVIC_IRQChannelSubPriority;
    nvic_init.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic_init);                      
}

void GPIO_ITConfig(uint8_t GPIO_PinSourcex, FunctionalState NewState)
{
    _assert(IS_GPIO_PIN_SOURCE(GPIO_PinSourcex));

    uint8_t IRQChannel = GPIO_PinSourcex <= 4
        ? GPIO_PinSourcex + 6
        : (GPIO_PinSourcex <= 9 ? EXTI9_5_IRQn : EXTI15_10_IRQn);

    if(NewState != DISABLE)
        NVIC->ISER[IRQChannel>>0x05] = (uint32_t)0x01<<(IRQChannel & (uint8_t)0x1F);
    else
        NVIC->ICER[IRQChannel>>0x05] = (uint32_t)0x01<<(IRQChannel & (uint8_t)0x1F);
}

__weak void EXTI_Line0_Callback(void) {}
__weak void EXTI_Line1_Callback(void) {}
__weak void EXTI_Line2_Callback(void) {}
__weak void EXTI_Line3_Callback(void) {}
__weak void EXTI_Line4_Callback(void) {}
__weak void EXTI_Line5_Callback(void) {}
__weak void EXTI_Line6_Callback(void) {}
__weak void EXTI_Line7_Callback(void) {}
__weak void EXTI_Line8_Callback(void) {}
__weak void EXTI_Line9_Callback(void) {}
__weak void EXTI_Line10_Callback(void) {}
__weak void EXTI_Line11_Callback(void) {}
__weak void EXTI_Line12_Callback(void) {}
__weak void EXTI_Line13_Callback(void) {}
__weak void EXTI_Line14_Callback(void) {}
__weak void EXTI_Line15_Callback(void) {}

void EXTI0_IRQHandler(void)
{
    if((EXTI->PR & EXTI_Line0) && (EXTI->IMR & EXTI_Line0))
    {
        EXTI_Line0_Callback();
        EXTI->PR = EXTI_Line0;
    }
}

void EXTI1_IRQHandler(void)
{
    if((EXTI->PR & EXTI_Line1) && (EXTI->IMR & EXTI_Line1))
    {
        EXTI_Line1_Callback();
        EXTI->PR = EXTI_Line1;
    }
}

void EXTI2_IRQHandler(void)
{
    if((EXTI->PR & EXTI_Line2) && (EXTI->IMR & EXTI_Line2))
    {
        EXTI_Line2_Callback();
        EXTI->PR = EXTI_Line2;
    }
}

void EXTI3_IRQHandler(void)
{
    if((EXTI->PR & EXTI_Line3) && (EXTI->IMR & EXTI_Line3))
    {
        EXTI_Line3_Callback();
        EXTI->PR = EXTI_Line3;
    }
}

void EXTI4_IRQHandler(void)
{
    if((EXTI->PR & EXTI_Line4) && (EXTI->IMR & EXTI_Line4))
    {
        EXTI_Line4_Callback();
        EXTI->PR = EXTI_Line4;
    }
}

void EXTI9_5_IRQHandler(void)
{
    if((EXTI->PR & EXTI_Line5) && (EXTI->IMR & EXTI_Line5))
    {
        EXTI_Line5_Callback();
        EXTI->PR = EXTI_Line5;
    }

    if((EXTI->PR & EXTI_Line6) && (EXTI->IMR & EXTI_Line6))
    {
        EXTI_Line6_Callback();
        EXTI->PR = EXTI_Line6;
    }

    if((EXTI->PR & EXTI_Line7) && (EXTI->IMR & EXTI_Line7))
    {
        EXTI_Line7_Callback();
        EXTI->PR = EXTI_Line7;
    }

    if((EXTI->PR & EXTI_Line8) && (EXTI->IMR & EXTI_Line8))
    {
        EXTI_Line8_Callback();
        EXTI->PR = EXTI_Line8;
    }

    if((EXTI->PR & EXTI_Line9) && (EXTI->IMR & EXTI_Line9))
    {
        EXTI_Line9_Callback();
        EXTI->PR = EXTI_Line9;
    }
}

void EXTI15_10_IRQHandler(void)
{
    if((EXTI->PR & EXTI_Line10) && (EXTI->IMR & EXTI_Line10))
    {
        EXTI_Line10_Callback();
        EXTI->PR = EXTI_Line10;
    }

    if((EXTI->PR & EXTI_Line11) && (EXTI->IMR & EXTI_Line11))
    {
        EXTI_Line11_Callback();
        EXTI->PR = EXTI_Line11;
    }

    if((EXTI->PR & EXTI_Line12) && (EXTI->IMR & EXTI_Line12))
    {
        EXTI_Line12_Callback();
        EXTI->PR = EXTI_Line12;
    }

    if((EXTI->PR & EXTI_Line13) && (EXTI->IMR & EXTI_Line13))
    {
        EXTI_Line13_Callback();
        EXTI->PR = EXTI_Line13;
    }

    if((EXTI->PR & EXTI_Line14) && (EXTI->IMR & EXTI_Line14))
    {
        EXTI_Line14_Callback();
        EXTI->PR = EXTI_Line14;
    }

    if((EXTI->PR & EXTI_Line15) && (EXTI->IMR & EXTI_Line15))
    {
        EXTI_Line15_Callback();
        EXTI->PR = EXTI_Line15;
    }
}
