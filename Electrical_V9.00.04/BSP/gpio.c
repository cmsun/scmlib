#include <stddef.h>
#include "stm32f10x_conf.h"
#include "gpio.h"
#include "debug.h"

EXIT_CALLBACK EXTI_IRQ_Table[16];

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

void GPIO_setITCallback(uint8_t GPIO_PinSourcex, void (*pfun)(void *), void *arg)
{
    _assert(IS_GPIO_PIN_SOURCE(GPIO_PinSourcex));

    EXTI_IRQ_Table[GPIO_PinSourcex].pfun = pfun;
    EXTI_IRQ_Table[GPIO_PinSourcex].arg = arg;
}

void EXTI0_IRQHandler(void)
{
    if((EXTI->PR & EXTI_Line0) && (EXTI->IMR & EXTI_Line0))
    {
        EXIT_CALLBACK *irq = &EXTI_IRQ_Table[0]; 
        if(irq->pfun != NULL)
            irq->pfun(irq->arg);
        EXTI->PR = EXTI_Line0;
    }
}

void EXTI1_IRQHandler(void)
{
    if((EXTI->PR & EXTI_Line1) && (EXTI->IMR & EXTI_Line1))
    {
        EXIT_CALLBACK *irq = &EXTI_IRQ_Table[1]; 
        if(irq->pfun != NULL)
            irq->pfun(irq->arg);
        EXTI->PR = EXTI_Line1;
    }
}

void EXTI2_IRQHandler(void)
{
    if((EXTI->PR & EXTI_Line2) && (EXTI->IMR & EXTI_Line2))
    {
        EXIT_CALLBACK *irq = &EXTI_IRQ_Table[2]; 
        if(irq->pfun != NULL)
            irq->pfun(irq->arg);
        EXTI->PR = EXTI_Line2;
    }
}

void EXTI3_IRQHandler(void)
{
    if((EXTI->PR & EXTI_Line3) && (EXTI->IMR & EXTI_Line3))
    {
        EXIT_CALLBACK *irq = &EXTI_IRQ_Table[3]; 
        if(irq->pfun != NULL)
            irq->pfun(irq->arg);
        EXTI->PR = EXTI_Line3;
    }
}

void EXTI4_IRQHandler(void)
{
    if((EXTI->PR & EXTI_Line4) && (EXTI->IMR & EXTI_Line4))
    {
        EXIT_CALLBACK *irq = &EXTI_IRQ_Table[4]; 
        if(irq->pfun != NULL)
            irq->pfun(irq->arg);
        EXTI->PR = EXTI_Line4;
    }
}

void EXTI9_5_IRQHandler(void)
{
    if((EXTI->PR & EXTI_Line5) && (EXTI->IMR & EXTI_Line5))
    {
        EXIT_CALLBACK *irq = &EXTI_IRQ_Table[5]; 
        if(irq->pfun != NULL)
            irq->pfun(irq->arg);
        EXTI->PR = EXTI_Line5;
    }

    if((EXTI->PR & EXTI_Line6) && (EXTI->IMR & EXTI_Line6))
    {
        EXIT_CALLBACK *irq = &EXTI_IRQ_Table[6]; 
        if(irq->pfun != NULL)
            irq->pfun(irq->arg);
        EXTI->PR = EXTI_Line6;
    }

    if((EXTI->PR & EXTI_Line7) && (EXTI->IMR & EXTI_Line7))
    {
        EXIT_CALLBACK *irq = &EXTI_IRQ_Table[7];
        if(irq->pfun != NULL)
            irq->pfun(irq->arg);
        EXTI->PR = EXTI_Line7;
    }

    if((EXTI->PR & EXTI_Line8) && (EXTI->IMR & EXTI_Line8))
    {
        EXIT_CALLBACK *irq = &EXTI_IRQ_Table[8];
        if(irq->pfun != NULL)
            irq->pfun(irq->arg);
        EXTI->PR = EXTI_Line8;
    }

    if((EXTI->PR & EXTI_Line9) && (EXTI->IMR & EXTI_Line9))
    {
        EXIT_CALLBACK *irq = &EXTI_IRQ_Table[9];
        if(irq->pfun != NULL)
            irq->pfun(irq->arg);
        EXTI->PR = EXTI_Line9;
    }
}

void EXTI15_10_IRQHandler(void)
{
    if((EXTI->PR & EXTI_Line10) && (EXTI->IMR & EXTI_Line10))
    {
        EXIT_CALLBACK *irq = &EXTI_IRQ_Table[10];
        if(irq->pfun != NULL)
            irq->pfun(irq->arg);
        EXTI->PR = EXTI_Line10;
    }

    if((EXTI->PR & EXTI_Line11) && (EXTI->IMR & EXTI_Line11))
    {
        EXIT_CALLBACK *irq = &EXTI_IRQ_Table[11];
        if(irq->pfun != NULL)
            irq->pfun(irq->arg);
        EXTI->PR = EXTI_Line11;
    }

    if((EXTI->PR & EXTI_Line12) && (EXTI->IMR & EXTI_Line12))
    {
        EXIT_CALLBACK *irq = &EXTI_IRQ_Table[12];
        if(irq->pfun != NULL)
            irq->pfun(irq->arg);
        EXTI->PR = EXTI_Line12;
    }

    if((EXTI->PR & EXTI_Line13) && (EXTI->IMR & EXTI_Line13))
    {
        EXIT_CALLBACK *irq = &EXTI_IRQ_Table[13];
        if(irq->pfun != NULL)
            irq->pfun(irq->arg);
        EXTI->PR = EXTI_Line13;
    }

    if((EXTI->PR & EXTI_Line14) && (EXTI->IMR & EXTI_Line14))
    {
        EXIT_CALLBACK *irq = &EXTI_IRQ_Table[14];
        if(irq->pfun != NULL)
            irq->pfun(irq->arg);
        EXTI->PR = EXTI_Line14;
    }

    if((EXTI->PR & EXTI_Line15) && (EXTI->IMR & EXTI_Line15))
    {
        EXIT_CALLBACK *irq = &EXTI_IRQ_Table[15];
        if(irq->pfun != NULL)
            irq->pfun(irq->arg);
        EXTI->PR = EXTI_Line15;
    }
}
