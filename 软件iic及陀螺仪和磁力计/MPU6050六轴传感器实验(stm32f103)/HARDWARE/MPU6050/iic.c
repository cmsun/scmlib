/*
********************************************************************************
*                              COPYRIGHT NOTICE
*                             Copyright (c) 2016
*                             All rights reserved
*
*  @fileName       : iic.c
*  @author         : scm 351721714@qq.com
*  @create         : 2019/01/08 15:59:41
********************************************************************************
*/

#include <math.h>
#include "iic.h"

void iic_delay_us(int n)
{
    for(int i = 0; i < n; ++i)
    {
        __NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
        __NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
        __NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
        __NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
        __NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
        __NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
        __NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
//        __NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
//        __NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
//        __NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
//        __NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
//        __NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
//        __NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
//        __NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
//        __NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
//        __NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
//        __NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
    }  
}

void iic_init(_IIC_TypeDef *hiic, GPIO_TypeDef *scl_port, uint32_t scl_pin, GPIO_TypeDef *sda_port, uint32_t sda_pin)
{    
    hiic->SCL_Port = scl_port;
    hiic->SCL_Pin = scl_pin;
    hiic->SDA_Port = sda_port;
    hiic->SDA_Pin = sda_pin;
    hiic->SDA_Pin_No = (log(hiic->SDA_Pin) / log(2));
#if defined(STM32F103)
    if(hiic->SDA_Pin > 0x0080)  //GPIO_Pin_7
    {
        hiic->SDA_Pin_No -= 8;
    }
#endif
    
#if defined(USE_STDPERIPH_DRIVER)   //如果使用标准库
    uint32_t scl_pin_number, sda_pin_number;   
    scl_pin_number = (log(hiic->SCL_Pin) / log(2));
    sda_pin_number = hiic->SDA_Pin_No;
#if defined(STM32F103)              //如果是stm32f103芯片
    GPIO_InitTypeDef GPIO_InitStructure;    
    //使能SCL和SDA管脚的时钟
    RCC_APB2PeriphClockCmd(1<<scl_pin_number, ENABLE);
    RCC_APB2PeriphClockCmd(1<<sda_pin_number, ENABLE);
    //SCL和SDA都初始化为上拉输出模式
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    //初始化SCL管脚
    GPIO_InitStructure.GPIO_Pin = hiic->SCL_Pin;
    GPIO_Init(hiic->SCL_Port, &GPIO_InitStructure);
    //初始化SDA管脚  
    GPIO_InitStructure.GPIO_Pin = hiic->SDA_Pin;
    GPIO_Init(hiic->SDA_Port, &GPIO_InitStructure);
#endif
#if defined(STM32F407)              //如果是stm32f407芯片
//    GPIO_InitTypeDef GPIO_InitStructure;
//    RCC_AHB1PeriphClockCmd(hiic->SCL_Port, ENABLE);
//    RCC_AHB1PeriphClockCmd(hiic->SDA_Port, ENABLE);
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
//    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
//    GPIO_InitStructure.GPIO_Pin = hiic->SCL_Pin;
//    GPIO_Init(hiic->SCL_Port, &GPIO_InitStructure);
//    GPIO_InitStructure.GPIO_Pin = hiic->SDA_Pin;
//    GPIO_Init(hiic->SCL_Port, &GPIO_InitStructure);
#endif
#endif
}

typedef enum {
    _SDA_READ = 0,
    _SDA_WRITE = 1
} _SDA_MODE;

//设置SDA线的输入输出模式
static inline void iic_sda_mode(_IIC_TypeDef *hiic, _SDA_MODE mode)
{
#if defined(STM32F103)
    uint32_t temp = (mode == _SDA_READ) ? (8<<(hiic->SDA_Pin_No*4)) : (3<<(hiic->SDA_Pin_No*4));
    __IO uint32_t *reg = (hiic->SCL_Pin <= 0x0080) ? &hiic->SDA_Port->CRL : &hiic->SDA_Port->CRH; 
    *reg &= ~(0xf << (hiic->SDA_Pin_No*4));
    *reg |= temp;
#else
    hiic->SDA_Port->MODER &= ~(3<<(hiic->SDA_Pin_No*2));
    hiic->SDA_Port->MODER |= mode<<(hiic->SDA_Pin_No*2);
#endif
}

//读取SDA线电平信号
static inline int iic_sda_read(_IIC_TypeDef *hiic)
{
    return ((hiic->SDA_Port->IDR & hiic->SDA_Pin) != 0);
}

//设置SDA线电平信号
static inline void iic_sda_set(_IIC_TypeDef *hiic, int status)
{
    status == 0 ? (hiic->SDA_Port->ODR &= ~(hiic->SDA_Pin)) : (hiic->SDA_Port->ODR |= hiic->SDA_Pin);    
}

//设置SCL线电平信号
static inline void iic_scl_set(_IIC_TypeDef *hiic, int status)
{
    status == 0 ? (hiic->SCL_Port->ODR &= ~(hiic->SCL_Pin)) : (hiic->SCL_Port->ODR |= hiic->SCL_Pin); 
}

void iic_start(_IIC_TypeDef *hiic)
{
    //START:when CLK is high,DATA change form high to low
    iic_sda_set(hiic, 1);
    iic_scl_set(hiic, 1);
    iic_delay_us(1);
    iic_sda_set(hiic, 0);
    iic_delay_us(1);
    //钳住IIC总线
    iic_scl_set(hiic, 0);
}

void iic_stop(_IIC_TypeDef *hiic)
{
    //STOP:when CLK is high,DATA change form low to high
    iic_sda_set(hiic, 0);
    iic_scl_set(hiic, 1);
    iic_delay_us(1);
    iic_sda_set(hiic, 1);
    iic_delay_us(1);		
}

//返回0：接收应答成功
//返回1：接收应答失败
int iic_wait_ack(_IIC_TypeDef *hiic)
{
    int timeout = 0;

    iic_sda_set(hiic, 1);       //将SDA线置高电平，以确保正确接收应答信号
    iic_sda_mode(hiic, _SDA_READ);
    
    while(iic_sda_read(hiic))   //接收应答信号
    {
        if(++timeout > 250)
        {
            iic_sda_mode(hiic, _SDA_WRITE);
            return 1;
        }
    }   
    //产生一个时钟，表明应答信号已经被读取
    iic_scl_set(hiic, 1);
    iic_delay_us(1);
    iic_scl_set(hiic, 0);

    iic_sda_mode(hiic, _SDA_WRITE);    

    return 0;
}

void iic_ack(_IIC_TypeDef *hiic)
{
    //CLK高电平时，DATA为低电平     
    iic_sda_set(hiic, 0);
    iic_scl_set(hiic, 1); 
    iic_delay_us(1);
    //钳住IIC总线
    iic_scl_set(hiic, 0);
}

void iic_nack(_IIC_TypeDef *hiic)
{
    //CLK高电平时，DATA为高电平
    iic_sda_set(hiic, 1);    
    iic_scl_set(hiic, 1);
    iic_delay_us(1);
    //钳住IIC总线
    iic_scl_set(hiic, 0);
}

void iic_send_byte(_IIC_TypeDef *hiic, uint8_t data)
{
    for(int i = 7; i >= 0; --i)
    {
        iic_scl_set(hiic, 0);
        //iic_delay_us(1);
        iic_sda_set(hiic, ((data >> i) & 0x01));
        //iic_delay_us(1);
        iic_scl_set(hiic, 1);
        iic_delay_us(1);
    }
    //钳住IIC总线，并给接收方的ACK信号产生时钟。
    iic_scl_set(hiic, 0);
    
    //这个延时是必须的。发送方发送完一个字节后拉低SCL线，使得
    //接收方产生一个ACK信号。但这只说明接收方接收到了数据，并
    //不说明已经完成存储，因为写入速度一般很慢。如果发送完一个
    //字节并收到ACK信号后马上又发送下一个字节，就会由于数据覆
    //盖而产生写入错误。
    iic_delay_us(1);
}

uint8_t iic_read_byte(_IIC_TypeDef *hiic)
{
    uint8_t rec = 0;
    iic_sda_mode(hiic, _SDA_READ);
    for(int i = 7; i >= 0; --i)
    {
        iic_scl_set(hiic, 0);
        iic_delay_us(4);
        iic_scl_set(hiic, 1);
        iic_delay_us(1);
        if(iic_sda_read(hiic) != 0)
        {
            rec |= 1<<i;
        }
    }
    //钳住IIC总线
    iic_scl_set(hiic, 0);
    iic_sda_mode(hiic, _SDA_WRITE);
    return rec;
}

#define _IIC_WAIT_ACK(h, flag) \
    {                          \
        if (iic_wait_ack(h))   \
        {                      \
            iic_stop(h);       \
            return flag;       \
        }                      \
    }

int iic_write_mem(_IIC_TypeDef *hiic, uint8_t dev, uint8_t addr, uint8_t *data, uint8_t size)
{   
    iic_start(hiic);
    iic_send_byte(hiic, (dev<<1)|0);
    _IIC_WAIT_ACK(hiic, 1);
    
    iic_send_byte(hiic, addr);
    _IIC_WAIT_ACK(hiic, 2);
    for(int i = 0; i < size; ++i)
    {
        iic_send_byte(hiic, data[i]);
        _IIC_WAIT_ACK(hiic, 3);
    }
    
    iic_stop(hiic);
    
    return 0;
}

int iic_read_mem(_IIC_TypeDef *hiic, uint8_t dev, uint8_t addr, uint8_t *data, uint8_t size)
{   
    iic_start(hiic);
    iic_send_byte(hiic, (dev<<1)|0);
    _IIC_WAIT_ACK(hiic, 1);
    iic_send_byte(hiic, addr);
    _IIC_WAIT_ACK(hiic, 2);
    
    iic_start(hiic);
    iic_send_byte(hiic, (dev<<1)|1);
    _IIC_WAIT_ACK(hiic, 3);
    for(int i = 0; i < size;)
    {
        data[i] = iic_read_byte(hiic);
        if(++i < size)
        {
            iic_ack(hiic);
        }
        else
        {
            iic_nack(hiic);
        }
    }
    
    iic_stop(hiic);
    
    return 0;
}
