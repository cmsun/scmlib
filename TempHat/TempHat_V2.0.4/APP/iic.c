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

#include "iic.h"

typedef enum {
    _SDA_READ = 0,
    _SDA_WRITE = 1
} _SDA_MODE;

void iic_init(_IIC_TypeDef *hiic,
              _GPIO_PORT_TypeDef scl_port, _GPIO_PIN_TypeDef scl_pin,
              _GPIO_PORT_TypeDef sda_port, _GPIO_PIN_TypeDef sda_pin)
{    
    hiic->SCL_Port = scl_port;
    hiic->SCL_Pin = scl_pin;
    hiic->SDA_Port = sda_port;
    hiic->SDA_Pin = sda_pin;

    _GPIO_Init_OutPut(scl_port, scl_pin);
    _GPIO_Init_OutPut(sda_port, sda_pin);
}

void iic_delay_us(uint32_t n)
{
    for(uint32_t i = 0; i < n; ++i)
    {
        NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
        NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
    }  
}

//设置SDA线的输入输出模式
static inline void iic_sda_mode(_IIC_TypeDef *hiic, _SDA_MODE mode)
{
    if(mode == _SDA_READ)
        _GPIO_Init_InPut(hiic->SDA_Port, hiic->SDA_Pin);
    else
        _GPIO_Init_OutPut(hiic->SDA_Port, hiic->SDA_Pin);
}

//读取SDA线电平信号
static inline int iic_sda_read(_IIC_TypeDef *hiic)
{
    return _GPIO_ReadInputPin(hiic->SDA_Port, hiic->SDA_Pin);
}

//设置SDA线电平信号
static inline void iic_sda_set(_IIC_TypeDef *hiic, int status)
{
    if(status == 0)
        _GPIO_WriteLow(hiic->SDA_Port, hiic->SDA_Pin);
    else
        _GPIO_WriteHigh(hiic->SDA_Port, hiic->SDA_Pin);
}

//设置SCL线电平信号
static inline void iic_scl_set(_IIC_TypeDef *hiic, int status)
{
    if(status == 0)
        _GPIO_WriteLow(hiic->SCL_Port, hiic->SCL_Pin);
    else
        _GPIO_WriteHigh(hiic->SCL_Port, hiic->SCL_Pin);
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

int iic_write_mem(_IIC_TypeDef *hiic, uint8_t dev, uint8_t addr, uint8_t *data, uint16_t size)
{   
    iic_start(hiic);
    iic_send_byte(hiic, (dev<<1)|0);
    _IIC_WAIT_ACK(hiic, 1);
    
    iic_send_byte(hiic, addr);
    _IIC_WAIT_ACK(hiic, 2);
    for(uint16_t i = 0; i < size; ++i)
    {
        iic_send_byte(hiic, data[i]);
        _IIC_WAIT_ACK(hiic, 3);
    }
    
    iic_stop(hiic);
    
    return 0;
}

int iic_read_mem(_IIC_TypeDef *hiic, uint8_t dev, uint8_t addr, uint8_t *data, uint16_t size)
{   
    iic_start(hiic);
    iic_send_byte(hiic, (dev<<1)|0);
    _IIC_WAIT_ACK(hiic, 1);
    iic_send_byte(hiic, addr);
    _IIC_WAIT_ACK(hiic, 2);
    
    iic_start(hiic);
    iic_send_byte(hiic, (dev<<1)|1);
    _IIC_WAIT_ACK(hiic, 3);
    for(uint16_t i = 0; i < size;)
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

int iic_write_mem_addr16bit(_IIC_TypeDef *hiic, uint8_t dev, uint16_t addr, uint8_t *data, uint16_t size)
{
    iic_start(hiic);
    iic_send_byte(hiic, (dev<<1)|0);               //发送设备地址和写命令
    iic_wait_ack(hiic);
    iic_send_byte(hiic, addr>>8);                  //发送存储地址高字节
    iic_wait_ack(hiic);
    iic_send_byte(hiic, addr&0xff);                //发送存储地址低字节
    iic_wait_ack(hiic);
    for(int i = 0; i < size; ++i)
    {
        iic_send_byte(hiic, data[i]);              //写入数据
        iic_wait_ack(hiic);
    }
    iic_stop(hiic);
    
    return 0;
}
    
int iic_read_mem_addr16bit(_IIC_TypeDef *hiic, uint8_t dev, uint16_t addr, uint8_t *data, uint16_t size)
{
    iic_start(hiic);
    iic_send_byte(hiic, (dev<<1)|0);               //发送设备地址和写命令
    iic_wait_ack(hiic);
    iic_send_byte(hiic, addr>>8);                  //发送存储地址高字节
    iic_wait_ack(hiic);
    iic_send_byte(hiic, addr&0xff);                //发送存储地址低字节
    iic_wait_ack(hiic);
    
    iic_start(hiic);
    iic_send_byte(hiic, (dev<<1)|1);                //进入接收模式
    iic_wait_ack(hiic);
    for(int i = 0; i < size; ++i)
    {
        data[i] = iic_read_byte(hiic);
        if(i < size)
            iic_ack(hiic);
        else
            iic_nack(hiic);
    }
    iic_stop(hiic);
    
    return 0;
}
