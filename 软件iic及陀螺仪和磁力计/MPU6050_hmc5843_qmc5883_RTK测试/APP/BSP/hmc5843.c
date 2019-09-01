/*
********************************************************************************
*                              COPYRIGHT NOTICE
*                             Copyright (c) 2016
*                             All rights reserved
*
*  @fileName       : hmc5843.c
*  @author         : scm 351721714@qq.com
*  @create         : 2018/11/09 11:24:26
********************************************************************************
*/

#include <stdint.h>
#include <math.h>
#include <string.h>
#include "hmc5843.h"

#define HMC5843_DEV_ADDR    (0x1E)                      //I2C设备地址
#define HMC5843_WRITE_ADDR  ((HMC5843_DEV_ADDR<<1)|0)   //I2C写地址
#define HMC5843_READ_ADDR   ((HMC5843_DEV_ADDR<<1)|1)   //I2C读地址

#define HMC5843_REG_CFG_A   (0)     //配置寄存器A
#define HMC5843_REG_CFG_B   (1)     //配置寄存器B
#define HMC5843_REG_MODE    (2)     //模式寄存器
#define HMC5843_REG_X_MSB   (3)     //数据输出 X MSB 寄存器
#define HMC5843_REG_X_LSB   (4)     //数据输出 X LSB 寄存器
#define HMC5843_REG_Z_MSB   (5)     //数据输出 Z MSB 寄存器
#define HMC5843_REG_Z_LSB   (6)     //数据输出 Z LSB 寄存器
#define HMC5843_REG_Y_MSB   (7)     //数据输出 Y MSB 寄存器
#define HMC5843_REG_Y_LSB   (8)     //数据输出 Y LSB 寄存器
#define HMC5843_REG_STATE   (9)     //状态寄存器
#define HMC5843_REG_ID_A    (10)    //识别寄存器 A
#define HMC5843_REG_ID_B    (11)    //识别寄存器 B
#define HMC5843_REG_ID_C    (12)    //识别寄存器 C

uint16_t g_HMC5833_BIAS[3] = {0, 0, 0}; // X Y Z 轴的校正值

#if 1   //使用软件IIC

#include "iic.h"
_IIC_TypeDef hmc_iic;
_IIC_TypeDef *hmc_hiic = &hmc_iic;

int hmc5843_write_regs(uint8_t addr, uint8_t *data, uint16_t size)
{
    return iic_write_mem(hmc_hiic, HMC5843_DEV_ADDR, addr, data, size);
}

int hmc5843_read_regs(uint8_t addr, uint8_t *data, uint16_t size)
{
    return iic_read_mem(hmc_hiic, HMC5843_DEV_ADDR, addr, data, size);
}

#endif

#if 0 //使用STM32HAL库硬件IIC

#include "i2c.h"
#define HMC5843_I2C_HANDLE hi2c1

int hmc5843_write_regs(uint8_t addr, uint8_t *data, uint16_t size)
{
    HAL_StatusTypeDef result;
    result = HAL_I2C_Mem_Write(&HMC5843_I2C_HANDLE,
                                HMC5843_WRITE_ADDR,
                                addr,
                                sizeof(addr),
                                data,
                                size,
                                5);
    return result == HAL_OK ? 0 : 1;
}

int hmc5843_read_regs(uint8_t addr, uint8_t *data, int size)
{
    HAL_StatusTypeDef result;
    result = HAL_I2C_Mem_Read(&HMC5843_I2C_HANDLE,
                                HMC5843_WRITE_ADDR,
                                addr,
                                sizeof(addr),
                                data,
                                size,
                                5);
    return result == HAL_OK ? 0 : 1;
}

#endif

#if 0   //使用STM32STD库硬件IIC

#include "stm32f4xx_conf.h"
#define HMC5843_I2C_PORT    I2C1
#define IIC_TIMEOUT         (200)

static void IIC_hardware_init(void)  
{  
    /*GPIO初始化*/  
    GPIO_InitTypeDef GPIO_InitStructure;  
    /* 配置硬件IIC需要的变量 */   
    I2C_InitTypeDef I2C_InitStructure;   
  
    /* 使能与 IIC 有关的时钟 */  
    RCC_APB1PeriphClockCmd  (RCC_AHB1Periph_GPIOB, ENABLE);   
    if(HMC5843_I2C_PORT == I2C1)
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE); 
    else
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE); 

    /*I2C configuration*/
    if(HMC5843_I2C_PORT == I2C1)
    {
        GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_I2C1); //注意，此处不能合并写成GPIO_PinSource6|GPIO_PinSource7
        GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_I2C1);
    }
    else
    {
        GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_I2C2); //注意，此处不能合并写成GPIO_PinSource10|GPIO_PinSource11
        GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_I2C2);
    }        
 
    if(HMC5843_I2C_PORT == I2C1)
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7; 
    else
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;   
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;   
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;  
    GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;    
    GPIO_Init(GPIOB, &GPIO_InitStructure);   
  
    /*IIC外设初始化*/  
  
    /* I2C 配置 */  
    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;     //配置为普通IIC模式  
    I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;   
    I2C_InitStructure.I2C_OwnAddress1 = 0x00;   
    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;    //使能自动应答     
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;   
    I2C_InitStructure.I2C_ClockSpeed = 400000;   
    I2C_Init(HMC5843_I2C_PORT, &I2C_InitStructure);      
  
    /* 使能 I2C */  
    I2C_Cmd  (HMC5843_I2C_PORT, ENABLE);   
    /*允许应答模式*/  
    I2C_AcknowledgeConfig(HMC5843_I2C_PORT, ENABLE);     
}

#define i2c_timeout_return(flag) \
{ \
    if(timeout++ > IIC_TIMEOUT) \
    { \
        I2C_AcknowledgeConfig(HMC5843_I2C_PORT, DISABLE); \
        I2C_GenerateSTOP(HMC5843_I2C_PORT, ENABLE); \
        return flag; \
    } \
}

int hmc5843_write_regs(uint8_t addr, uint8_t *data, uint16_t size)
{
    int timeout = 0;
    int i = 0;

    while(I2C_GetFlagStatus(HMC5843_I2C_PORT, I2C_FLAG_BUSY))
    {
        i2c_timeout_return(1);
    }
   
    I2C_GenerateSTART(HMC5843_I2C_PORT, ENABLE);  
    while(!I2C_CheckEvent(HMC5843_I2C_PORT, I2C_EVENT_MASTER_MODE_SELECT))
    {
        i2c_timeout_return(2);
    }
  
    I2C_Send7bitAddress(HMC5843_I2C_PORT, HMC5843_WRITE_ADDR,I2C_Direction_Transmitter);  
    timeout = 0;
    while(!I2C_CheckEvent(HMC5843_I2C_PORT, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
    {
        i2c_timeout_return(3);
    } 
  
    I2C_SendData(HMC5843_I2C_PORT, addr);  
    timeout = 0;
    while(!I2C_CheckEvent(HMC5843_I2C_PORT, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
    {
        i2c_timeout_return(4);
    }        
 
    while(size--)
    {
        //I2C_AcknowledgeConfig(HMC5843_I2C_PORT, ENABLE);
        I2C_SendData(HMC5843_I2C_PORT, data[i++]);  
        timeout = 0;
        while(!I2C_CheckEvent(HMC5843_I2C_PORT, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
        {
            i2c_timeout_return(5);
        }
    }
 
    I2C_AcknowledgeConfig(HMC5843_I2C_PORT, DISABLE);
    I2C_GenerateSTOP(HMC5843_I2C_PORT, ENABLE); 
    
    return 0;
}

int hmc5843_read_regs(uint8_t addr, uint8_t *data, uint16_t size)
{
    int timeout = 0;
    int i = 0;

    while(I2C_GetFlagStatus(HMC5843_I2C_PORT, I2C_FLAG_BUSY))
    {
        i2c_timeout_return(1);
    }
  
    I2C_GenerateSTART(HMC5843_I2C_PORT, ENABLE);//起始信号  
    timeout = 0;
    while(!I2C_CheckEvent(HMC5843_I2C_PORT, I2C_EVENT_MASTER_MODE_SELECT))
    {
        i2c_timeout_return(2);
    }    
  
    I2C_Send7bitAddress(HMC5843_I2C_PORT, HMC5843_WRITE_ADDR, I2C_Direction_Transmitter);//发送设备地址+写信号  
    timeout = 0;
    while(!I2C_CheckEvent(HMC5843_I2C_PORT, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
    {
        i2c_timeout_return(3);
    }    
  
    I2C_SendData(HMC5843_I2C_PORT, addr);//发送存储单元地址，从0开始  
    timeout = 0;
    while(!I2C_CheckEvent(HMC5843_I2C_PORT, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
    {
        i2c_timeout_return(4);
    }        
  
    I2C_GenerateSTART(HMC5843_I2C_PORT,ENABLE);//起始信号  
    timeout = 0;
    while(!I2C_CheckEvent(HMC5843_I2C_PORT, I2C_EVENT_MASTER_MODE_SELECT))
    {
        i2c_timeout_return(5);
    }    
  
    I2C_Send7bitAddress(HMC5843_I2C_PORT, HMC5843_READ_ADDR, I2C_Direction_Receiver);//发送设备地址+读信号  
    timeout = 0;
    while(!I2C_CheckEvent(HMC5843_I2C_PORT, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
    {
        i2c_timeout_return(6);
    }     

    while(size--)
    {
        I2C_AcknowledgeConfig(HMC5843_I2C_PORT, ENABLE); 
        timeout = 0;
        while(!(I2C_CheckEvent(HMC5843_I2C_PORT, I2C_EVENT_MASTER_BYTE_RECEIVED)))
        {
            i2c_timeout_return(7);
        }    
        data[i++] = I2C_ReceiveData(HMC5843_I2C_PORT);//读出寄存器数据
    }
    
    I2C_AcknowledgeConfig(HMC5843_I2C_PORT, DISABLE); 
    I2C_GenerateSTOP(HMC5843_I2C_PORT, ENABLE);
  
    return 0;
}

#endif

int hmc5843_init(void)
{
    uint8_t ID[3] = {0};
    uint8_t REG_A = 0x14, REG_MODE = 0;
    uint8_t temp;

    iic_init(hmc_hiic, GPIOB, GPIO_PIN_6, GPIOB, GPIO_PIN_7);
 
    //读设备识别码
    hmc5843_read_regs(HMC5843_REG_ID_A, ID, 3);
    if(ID[0] != 0x48 || ID[1] != 0x34 || ID[2] != 0X33)
        return 1;
     
    if(hmc5843_write_regs(HMC5843_REG_CFG_A, &REG_A, 1))
        return 2;
    if(hmc5843_read_regs(HMC5843_REG_CFG_A, &temp, 1) || temp != REG_A)
        return 3;

    //设置连续采样工作模式
    if(hmc5843_write_regs(HMC5843_REG_MODE, &REG_MODE, 1))
        return 4;
    //检查连续采样工作模式是否设置成功
    if(hmc5843_read_regs(HMC5843_REG_MODE, &temp, 1) || temp != REG_MODE)
        return 5;
    
    return 0;
}

//获取hmc5843的三轴原始数据和三轴磁场强度数据（单位高斯）
int hmc5843_get_data(CompassRawData *raw, CompassGaussData *gauss)
{
    uint8_t data[6];
    CompassRawData comp = {0};
    int result = 0;
    
    result = hmc5843_read_regs(HMC5843_REG_X_MSB, data, 6);
    if(result == 0)
    {
        comp.x = (short)((data[0] << 8) | data[1]);
        comp.z = (short)((data[2] << 8) | data[3]);
        comp.y = (short)((data[4] << 8) | data[5]);
        
#if 0
    if(comp.x > 0x7fff)
        comp.x -= 0xffff;      

    if(comp.y > 0x7fff)
        comp.y -= 0xffff;

    if(comp.z > 0x7fff)
        comp.z -= 0xffff;
#endif
        
        comp.x -= g_HMC5833_BIAS[0];
        comp.y -= g_HMC5833_BIAS[1];
        comp.z -= g_HMC5833_BIAS[2];
    }
    else
    {
        comp.x = comp.y = comp.z = 0;
    }
	
	//经过校正后的磁力计数据
    if(raw != NULL)
    {
        memcpy((void *)raw, (void *)&comp, sizeof(comp));
    }
    
	
    //高斯数据。配置寄存器B默认情况下每1300个原始数据为1高斯，参考hmc5843数据手册。
    if(gauss != NULL)
    {
        gauss->x = (float)comp.x / 1300;
        gauss->y = (float)comp.y / 1300;
        gauss->z = (float)comp.z / 1300;
    }
    
    return result;
}

//获取角度，仅限于磁力计水平放置时有效。如果磁力计倾斜，需要用加速度计进行校正融合。
int hmc5843_get_direction(float *dir, float pitch, float roll)
{
    const float rad = 0.0174f;
    CompassRawData comp;

    if(hmc5843_get_data(&comp, NULL))
    {
        *dir = 0.0f;
        return -1;
    }
    
    //不经过加速度计校正倾斜度    
    /* *dir = atan2(comp.y, comp.x) * (180 / 3.14159265) + 180; */

    //经过加速度计校正倾斜度
    double hy = comp.y * cos(roll*rad) + comp.x * sin(roll*rad) * sin(pitch*rad) - comp.z * cos(pitch*rad) * sin(roll*rad);
    double hx = comp.x * cos(pitch*rad) + comp.z * sin(roll*rad);
    *dir = atan2(hy, hx) * (180 / 3.14159265) + 180;     

    return 0;
}
