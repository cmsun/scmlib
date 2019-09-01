/*
********************************************************************************
*                              COPYRIGHT NOTICE
*                             Copyright (c) 2016
*                             All rights reserved
*
*  @fileName       : mpuxxxx.h
*  @author         : scm 351721714@qq.com
*  @create         : 2018/10/12 23:55:10
********************************************************************************
*/

#ifndef MPUXXXX_H
#define MPUXXXX_H

#include <stdint.h>
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h"
#include "stm32f4xx_hal.h"
#include "debug.h"

//#define USE_SPI 1 //使用SPI通信而不是I2C通信

#ifdef USE_SPI
#include "spi.h"
#else
#include "iic.h"
#endif

#if defined(MPU9150) || defined(MPU9250)
#define COMPASS_ENABLED 1
#endif

#if defined(MPU6050) && defined(USE_SPI)
#error "MPU6050 do not suport SPI!\r\n"
#endif

//#define AK89xx_BYPASS 1

typedef struct EularAngle
{
    float pitch;
    float roll;
    float yaw;
} EulerAngle;

typedef struct MPU_Coordinate
{
	float x;
	float y;
	float z;
} MPU_Coordinate;

typedef MPU_Coordinate Gyroscope;
typedef MPU_Coordinate Acceleration;

#if defined(USE_SPI)
int mpuxxxx_spi_write_reg(uint8_t reg, uint8_t value);
int mpuxxxx_spi_read_reg(uint8_t reg, uint8_t *value);
int mpuxxxx_spi_write_regs(uint8_t addr, uint8_t reg, uint8_t len, uint8_t *buf);
int mpuxxxx_spi_read_regs(uint8_t addr, uint8_t reg, uint8_t len, uint8_t *buf);
#else
uint8_t mpuxxxx_i2c_write_regs(uint8_t addr, uint8_t reg, uint8_t len, uint8_t *buf);
uint8_t mpuxxxx_i2c_read_regs(uint8_t addr, uint8_t reg, uint8_t len, uint8_t *buf);
#endif

int mpuxxxx_mpu_dmp_init(void);
int mpuxxxx_mpl_init(void);
int mpuxxxx_mpl_poll(void);
int mpuxxxx_mpl_get_euler(EulerAngle *_euler);
int mpuxxxx_dmp_get_euler(EulerAngle *_euler);
int mpuxxxx_dmp_get_scale_data(Gyroscope *gyro, Acceleration *accel, short *sensors);

void mpu9250_Init(void);


//以下是mpu官方库移植需要实现的函数

static inline int i2c_write(uint8_t addr, uint8_t reg, uint8_t len, uint8_t *buf)
{
#if defined(USE_SPI)
    return mpuxxxx_spi_write_regs(addr, reg, len, buf);
#else
	return mpuxxxx_i2c_write_regs(addr, reg, len, buf);
#endif
}

static inline int i2c_read(uint8_t addr, uint8_t reg, uint8_t len, uint8_t *buf)
{
#if defined(USE_SPI)
    return mpuxxxx_spi_read_regs(addr, reg, len, buf);
#else
	return mpuxxxx_i2c_read_regs(addr, reg, len, buf);
#endif
}

static inline void delay_ms(uint32_t cnt)
{
	HAL_Delay(cnt);
}

static inline int get_ms(unsigned long *timestamp)
{
	*timestamp = (unsigned long)HAL_GetTick();
	return 0;
}

#define log_i(...) debug_printf(__VA_ARGS__)

#define log_e(...) debug_printf(__VA_ARGS__)


#endif /* end of include guard: MPUXXXX_H */
