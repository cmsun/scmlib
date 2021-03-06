/*
********************************************************************************
*                              COPYRIGHT NOTICE
*                             Copyright (c) 2016
*                             All rights reserved
*
*  @fileName       : iic.h
*  @author         : scm 351721714@qq.com
*  @create         : 2019/01/08 16:00:09
********************************************************************************
*/

#ifndef IIC_H
#define IIC_H

//#define STM32F10x
#define STM32F4xx

//#if defined(STM32F10x)
//#include "stm32f10x_conf.h"
//#endif

#if defined(STM32F4xx)
//#include "stm32f4xx_conf.h"
#include "stm32f4xx_hal.h"
#endif

typedef struct _IIC_TypeDef
{
    GPIO_TypeDef *SCL_Port;
    uint32_t SCL_Pin;
    GPIO_TypeDef *SDA_Port;
    uint32_t SDA_Pin;
    uint32_t SDA_Pin_No;
} _IIC_TypeDef;

void iic_delay_us(uint32_t n);
void iic_init(_IIC_TypeDef *hiic, GPIO_TypeDef *scl_port, uint32_t scl_pin, GPIO_TypeDef *sda_port, uint32_t sda_pin);
void iic_start(_IIC_TypeDef *hiic);
void iic_stop(_IIC_TypeDef *hiic);
int iic_wait_ack(_IIC_TypeDef *hiic);
void iic_ack(_IIC_TypeDef *hiic);
void iic_nack(_IIC_TypeDef *hiic);
void iic_send_byte(_IIC_TypeDef *hiic, uint8_t data);
uint8_t iic_read_byte(_IIC_TypeDef *hiic);
//addr用uint8_t来表示地址可以表示0~255总共256个地址，但是size如果使用uint8_t表示则一次最多只能读取
//255个字节，因为uint8_t能表示的最大整数为255。如果用立即数256或者多个字节的变量给size参数赋值256，
//则变量size得到的值为0，这样就会产生BUG。
int iic_write_mem(_IIC_TypeDef *hiic, uint8_t dev, uint8_t addr, uint8_t *data, uint16_t size);
int iic_read_mem(_IIC_TypeDef *hiic, uint8_t dev, uint8_t addr, uint8_t *data, uint16_t size);
//IIC读写16位地址的IIC设备
int iic_write_mem_addr16bit(_IIC_TypeDef *hiic, uint8_t dev, uint16_t addr, uint8_t *data, uint16_t size);
int iic_read_mem_addr16bit(_IIC_TypeDef *hiic, uint8_t dev, uint16_t addr, uint8_t *data, uint16_t size);

#endif /* end of include guard: IIC_H */
