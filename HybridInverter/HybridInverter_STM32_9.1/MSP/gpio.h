/*
 * ODR，输出数据寄存器(可读可写)：既能控制管脚为高电平，也能控制管脚为低电平。
 * 写1则GPIO管脚对应位输出高电平，写0输出低电平。低16位使用，高16位保留。
 * 
 * IDR，输入数据寄存器(只读)：每个APB2时钟周期捕捉I/O管脚上的数据。IDR反映IO管脚
 * 的真实状态：比如GPIOA的ODR设置Bit0为1使GPIOA的Pin0输出高电平，但对应管脚被接地。
 * 此时读取ODR的Bit0得到的值为1说明GPIOA的Pin0输出了高电平，读取IDR得到的值为0说明
 * 当前管脚的真实电平为低电平。低16位使用，高16位保留。
 *
 * BSRR, 32位置位/复位寄存器(只写)：既能控制管脚为高电平，也能控制管脚为低电平。
 * 对寄存器低16bit写1对应管脚为高电平，对寄存器高 16bit 写1对应管脚为低电平。写0无动作。
 * 
 * BRR，16位复位寄存器(只写)：只能改变管脚状态为低电平，对寄存器管脚对应位写 1 相
 * 应管脚会为低电平。写 0 无动作。低16位使用，高16位保留。
 */

#ifndef GPIO_H
#define GPIO_H

#include "stm32f10x_conf.h"

typedef struct {
    void (*pfun)(void *);
    void *arg;
} EXIT_CALLBACK;

void GPIO_Initialize(GPIO_TypeDef *GPIOx,
        uint16_t GPIO_Pin_x,
        GPIOMode_TypeDef GPIO_Mode_x,
        GPIOSpeed_TypeDef GPIO_Speed_x);

void GPIO_EXTI_Init(uint8_t GPIO_PortSourceGPIOx,
        uint8_t GPIO_PinSourcex,
        EXTITrigger_TypeDef EXTI_Trigger_x,
        EXTIMode_TypeDef EXTI_Mode_x);

void GPIO_setNVICPriority(uint8_t GPIO_PinSourcex,
        uint8_t NVIC_IRQChannelPreemptionPriority,
        uint8_t NVIC_IRQChannelSubPriority);

void GPIO_ITConfig(uint8_t GPIO_PinSourcex, FunctionalState NewState);

void GPIO_setITCallback(uint8_t GPIO_PinSourcex, void (*pfun)(void *), void *arg);

/**********************位带操作,实现类似51的GPIO控制功能***********************/
/***************具体实现思想,参考<<CM3权威指南>>第五章(87页~92页)**************/
//IO口操作宏定义
#define BITBAND(addr, bitnum) ((addr&0xF0000000)+0x2000000+((addr&0xFFFFF)<<5)+(bitnum<<2)) 
#define MEM_ADDR(addr) *((volatile unsigned long  *)(addr)) 
#define BIT_ADDR(addr, bitnum) MEM_ADDR(BITBAND(addr, bitnum)) 

//IO口地址映射
#define GPIOA_ODR_Addr    (GPIOA_BASE+12) //0x4001080C 
#define GPIOB_ODR_Addr    (GPIOB_BASE+12) //0x40010C0C 
#define GPIOC_ODR_Addr    (GPIOC_BASE+12) //0x4001100C 
#define GPIOD_ODR_Addr    (GPIOD_BASE+12) //0x4001140C 
#define GPIOE_ODR_Addr    (GPIOE_BASE+12) //0x4001180C 
#define GPIOF_ODR_Addr    (GPIOF_BASE+12) //0x40011A0C    
#define GPIOG_ODR_Addr    (GPIOG_BASE+12) //0x40011E0C    

#define GPIOA_IDR_Addr    (GPIOA_BASE+8) //0x40010808 
#define GPIOB_IDR_Addr    (GPIOB_BASE+8) //0x40010C08 
#define GPIOC_IDR_Addr    (GPIOC_BASE+8) //0x40011008 
#define GPIOD_IDR_Addr    (GPIOD_BASE+8) //0x40011408 
#define GPIOE_IDR_Addr    (GPIOE_BASE+8) //0x40011808 
#define GPIOF_IDR_Addr    (GPIOF_BASE+8) //0x40011A08 
#define GPIOG_IDR_Addr    (GPIOG_BASE+8) //0x40011E08 
 
//IO口操作,只对单一的IO口!
//确保n的值小于16!
#define PAout(n)   BIT_ADDR(GPIOA_ODR_Addr, n)  //输出 
#define PAin(n)    BIT_ADDR(GPIOA_IDR_Addr, n)  //输入 

#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr, n)  //输出 
#define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr, n)  //输入 

#define PCout(n)   BIT_ADDR(GPIOC_ODR_Addr, n)  //输出 
#define PCin(n)    BIT_ADDR(GPIOC_IDR_Addr, n)  //输入 

#define PDout(n)   BIT_ADDR(GPIOD_ODR_Addr, n)  //输出 
#define PDin(n)    BIT_ADDR(GPIOD_IDR_Addr, n)  //输入 

#define PEout(n)   BIT_ADDR(GPIOE_ODR_Addr, n)  //输出 
#define PEin(n)    BIT_ADDR(GPIOE_IDR_Addr, n)  //输入

#define PFout(n)   BIT_ADDR(GPIOF_ODR_Addr, n)  //输出 
#define PFin(n)    BIT_ADDR(GPIOF_IDR_Addr, n)  //输入

#define PGout(n)   BIT_ADDR(GPIOG_ODR_Addr, n)  //输出 
#define PGin(n)    BIT_ADDR(GPIOG_IDR_Addr, n)  //输入

#endif
