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

#define STM32F103
//#define STM32F407

#if defined(USE_STDPERIPH_DRIVER)   //如果使用标准库
#if defined(STM32F103)
#include "stm32f10x_conf.h"
#endif
#if defined(STM32F407)
#include "stm32f4xx_conf.h"
#endif
#else
#if defined(STM32F103)
#include "stm32f1xx_hal.h"
#endif
#if defined(STM32F407)
#include "stm32f4xx_hal.h"
#endif
#endif

typedef struct _IIC_TypeDef
{
    GPIO_TypeDef *SCL_Port;
    uint32_t SCL_Pin;
    GPIO_TypeDef *SDA_Port;
    uint32_t SDA_Pin;
    uint32_t SDA_Pin_No;
} _IIC_TypeDef;

void iic_init(_IIC_TypeDef *hiic, GPIO_TypeDef *scl_port, uint32_t scl_pin, GPIO_TypeDef *sda_port, uint32_t sda_pin);
void iic_start(_IIC_TypeDef *hiic);
void iic_stop(_IIC_TypeDef *hiic);
int iic_wait_ack(_IIC_TypeDef *hiic);
void iic_ack(_IIC_TypeDef *hiic);
void iic_nack(_IIC_TypeDef *hiic);
void iic_send_byte(_IIC_TypeDef *hiic, uint8_t data);
uint8_t iic_read_byte(_IIC_TypeDef *hiic);
int iic_write_mem(_IIC_TypeDef *hiic, uint8_t dev, uint8_t addr, uint8_t *data, uint8_t size);
int iic_read_mem(_IIC_TypeDef *hiic, uint8_t dev, uint8_t addr, uint8_t *data, uint8_t size);

#endif /* end of include guard: IIC_H */
