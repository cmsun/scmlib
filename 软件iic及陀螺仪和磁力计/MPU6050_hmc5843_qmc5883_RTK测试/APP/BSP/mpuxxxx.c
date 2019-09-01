/*
********************************************************************************
*                              COPYRIGHT NOTICE
*                             Copyright (c) 2016
*                             All rights reserved
*
*  @fileName       : mpuxxxx.c
*  @author         : scm 351721714@qq.com
*  @create         : 2018/10/12 23:55:06
********************************************************************************
*/

#include <stdint.h>
#include <math.h>
#include <string.h>
#include "main.h"
#include "debug.h"

#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h"
#include "invensense.h"
#include "invensense_adv.h"
#include "eMPL_outputs.h"
#include "mltypes.h"
#include "mpu.h"
#include "log.h"
#include "hmc5843.h"
#include "mpuxxxx.h"

#define DEFAULT_MPU_HZ	100
#define COMPASS_READ_MS	100

#define DEFAULT_GYRO_FSR        2000    //陀螺仪量程
#define DEFAULT_ACCEL_FSR       2       //加速计量程
#define DEFAULT_COMPASS_FSR             //磁力计量程
#define DEFAULT_LPF             5       //低通滤波值

int mpuxxxx_new_data = 0;       //mpu是否产生新的数据

//陀螺仪方向设置
static signed char gyro_orientation[9] = { 1, 0, 0,
                                           0, 1, 0,
                                           0, 0, 1};
#if defined(COMPASS_ENABLED)
//磁力计方向设置
static signed char compass_orientation[9] = { 0, 1, 0,
                                              1, 0, 0,
											  0, 0, -1};
#endif

#if defined(USE_SPI)
											  
#define MPU_SPI_Handler			hspi2
#define MPU_SPI_CS_GPIO_Port	SPI2_CS_GPIO_Port
#define MPU_SPI_CS_Pin			SPI2_CS_Pin
											  
//MPU6515 SPI 片选使能
static inline void mpuxxxx_spi_cs_low(void)
{
    HAL_GPIO_WritePin(MPU_SPI_CS_GPIO_Port, MPU_SPI_CS_Pin, GPIO_PIN_RESET);
}

//MPU6515 SPI 片选失能
static inline void mpuxxxx_spi_cs_high(void)
{
    HAL_GPIO_WritePin(MPU_SPI_CS_GPIO_Port, MPU_SPI_CS_Pin, GPIO_PIN_SET);
}

//MPU6515写单个寄存器
int mpuxxxx_spi_write_reg(uint8_t reg, uint8_t value)
{
    //spi cs 拉低，进行一次写操作
    mpuxxxx_spi_cs_low();

    //发送寄存器地址
    if(HAL_SPI_Transmit(&MPU_SPI_Handler, &reg, sizeof(reg), 100) != HAL_OK)
    {
        mpuxxxx_spi_cs_high();
        return 1;
    }

    //从当前地址写寄存器的值
    if(HAL_SPI_Transmit(&MPU_SPI_Handler, &value, sizeof(value), 100) != HAL_OK)
    {
        mpuxxxx_spi_cs_high();
        return 2;
    }

    //spi cs 拉高，结束一次写操作
    mpuxxxx_spi_cs_high();
    return 0;
}

//MPU6515读单个寄存器
int mpuxxxx_spi_read_reg(uint8_t reg, uint8_t *value)
{
    //地址最高位置1，表明对该地址进行读操作
    reg |= 0x80;

    //spi cs 拉低，进行一次读操作
    mpuxxxx_spi_cs_low();

    //发送寄存器地址
    if(HAL_SPI_Transmit(&MPU_SPI_Handler, &reg, sizeof(reg), 100) != HAL_OK)
    {
        mpuxxxx_spi_cs_high();
        return 1;
    }

    //从当前地址读寄存器的值
    if(HAL_SPI_Receive(&MPU_SPI_Handler, value, 1, 100) != HAL_OK)
    {
        mpuxxxx_spi_cs_high();
        return 2;
    }

    //spi cs 拉高，结束一次读操作
    mpuxxxx_spi_cs_high();
    return 0;
}

//MPU6515写多个寄存器
int mpuxxxx_spi_write_regs(uint8_t addr, uint8_t reg, uint8_t len, uint8_t *buf)
{
    //spi cs 拉低，进行一次写操作
    mpuxxxx_spi_cs_low();

    //发送寄存器地址
    if(HAL_SPI_Transmit(&MPU_SPI_Handler, &reg, sizeof(reg), 100) != HAL_OK)
    {
        mpuxxxx_spi_cs_high();
        return 1;
    }

    //从当前地址开始，定一个或多个寄存器
    for(int i = 0; i < len; ++i)
    {
        if(HAL_SPI_Transmit(&MPU_SPI_Handler, buf + i, 1, 100) != HAL_OK)
        {
            mpuxxxx_spi_cs_high();
            return 2;
        }
    }

    //spi cs 拉高，结束一次写操作
    mpuxxxx_spi_cs_high();
    return 0;
}

//MPU6515读多个寄存器
int mpuxxxx_spi_read_regs(uint8_t addr, uint8_t reg, uint8_t len, uint8_t *buf)
{
    //地址最高位置1，表明对该地址进行读操作
    reg |= 0x80;

    //spi cs 拉低，进行一次读操作
    mpuxxxx_spi_cs_low();

    //发送寄存器地址
    if(HAL_SPI_Transmit(&MPU_SPI_Handler, &reg, sizeof(reg), 100) != HAL_OK)
    {
        mpuxxxx_spi_cs_high();
        return 1;
    }

    //从当前地址开始，读一个或多个寄存器
    for(int i = 0; i < len; ++i)
    {
        if(HAL_SPI_Receive(&MPU_SPI_Handler, buf + i, 1, 100) != HAL_OK)
        {
            mpuxxxx_spi_cs_high();
            return 2;
        }
    }

    //spi cs 拉高，结束一次读操作
    mpuxxxx_spi_cs_high();
    return 0;
}

#else //使用I2C

#include "iic.h"
_IIC_TypeDef iic;
_IIC_TypeDef *hiic = &iic;

uint8_t mpuxxxx_i2c_write_regs(uint8_t addr, uint8_t reg, uint8_t len, uint8_t *buf)
{
    return iic_write_mem(hiic, addr, reg, buf, len);	
} 

uint8_t mpuxxxx_i2c_read_regs(uint8_t addr,uint8_t reg, uint8_t len, uint8_t *buf)
{ 
    return iic_read_mem(hiic, addr, reg, buf, len);
}

#endif //USE_SPI


//MPU6515自检，并校验
//返回值:0,正常 其他,失败
//找平校准的方法：将设备放在水平面，用函数mpu_run_6500_self_test找出偏移值，记录下来。
//以后每将启动不再调用mpu_run_6500_self_test函数，而是将记录的偏移值写入offset寄存器，或者写入mpl库中。
int mpuxxxx_run_self_test(void)
{	
    //经过多次验证，以下的偏移值产生的数据波动最小
	long gyro[3] = {-134336, 112686, -11136}, accel[3] = {620, 1250, 1463};
    int result;

#if defined(MPU6050)
    if((result = mpu_run_self_test(gyro, accel)) == 0x07)
#else
    if((result = mpu_run_6500_self_test(gyro, accel, 0)) == 0x07)
#endif
	{
		//debug_printf("gyro bias: %ld %ld %ld\r\n", gyro[0], gyro[1], gyro[2]);
		//debug_printf("accel bias: %ld %ld %ld\r\n", accel[0], accel[1], accel[2]);

#if 0
        //将偏移校准数据写入MPL库中
        float gyro_sens = 0;
        mpu_get_gyro_sens(&gyro_sens);
        gyro[0] = (long)(gyro[0] * gyro_sens);
        gyro[1] = (long)(gyro[1] * gyro_sens);
        gyro[2] = (long)(gyro[2] * gyro_sens);
        dmp_set_gyro_bias(gyro);
        //------------------------------------
        unsigned short accel_sens = 16384;
        mpu_get_accel_sens(&accel_sens);
        accel[0] *= accel_sens;
        accel[1] *= accel_sens;
        accel[2] *= accel_sens;
        dmp_set_accel_bias(accel);
#endif

#if 1
        //将校准数据写入偏移存器中
        for(int i = 0; i < 3; ++i)
        {
            gyro[i] = (long)(gyro[i] * 32.8f);  //conver to +-1000dps
            gyro[i] >>= 16;
            accel[i] *= (float)2048;            //convert to +-16G
            accel[i] >>= 16;
        }
        mpu_set_gyro_bias_reg(gyro);
#if defined(MPU6050)
        mpu_set_accel_bias_6050_reg(accel);
#else
        mpu_set_accel_bias_6500_reg(accel);
#endif
#endif

		return 0;
	}
	else
	{
		if(result & 0x01)
		{
			log_i("gyro self test failed!\r\n");
		}
		
		if(result & 0x02)
		{
			log_i("accel self test failed!\r\n");
		}
		
		if(result & 0x04)
		{
			log_i("compass self test failed!\r\n");
		}

        return 1;
	}
}

//mpuxxxx初始化
//返回0:正常 返回其他:失败
int mpuxxxx_mpu_dmp_init(void)
{
	struct int_param_s int_param;
    unsigned short rate, lpf;
	unsigned short gyro_fsr;
	unsigned char accel_fsr;
    
    iic_init(hiic, GPIOB, GPIO_PIN_6, GPIOB, GPIO_PIN_7);
	
	if(mpu_init(&int_param))										    //初始化MPU6515
        return 1;
#ifdef COMPASS_ENABLED
    if(mpu_set_sensors(INV_XYZ_GYRO | INV_XYZ_ACCEL | INV_XYZ_COMPASS))	//设置所需要的传感器
#else
	if(mpu_set_sensors(INV_XYZ_GYRO | INV_XYZ_ACCEL))
#endif
		return 2;
    if(mpu_configure_fifo(INV_XYZ_GYRO | INV_XYZ_ACCEL))				//设置FIFO
		return 3;
    if(mpu_set_sample_rate(DEFAULT_MPU_HZ))							    //设置采样率
        return 4;
#ifdef COMPASS_ENABLED
    if(mpu_set_compass_sample_rate(1000 / COMPASS_READ_MS))
        return 5;
#endif
    if(mpu_set_gyro_fsr(DEFAULT_GYRO_FSR))								//设置陀螺仪满量程范围
        return 6;
    if(mpu_set_accel_fsr(DEFAULT_ACCEL_FSR))							//设置加速度计满量程范围
        return 7;
    if(mpu_set_lpf(DEFAULT_LPF))										//设置数字低通滤波器
        return 8;
    if(mpu_get_sample_rate(&rate) || (rate != DEFAULT_MPU_HZ))
		return 9;
    if(mpu_get_gyro_fsr(&gyro_fsr) || (gyro_fsr != DEFAULT_GYRO_FSR))
        return 10;
    if(mpu_get_accel_fsr(&accel_fsr) || (accel_fsr != DEFAULT_ACCEL_FSR))
        return 11;
	if(mpu_get_lpf(&lpf) || (lpf != DEFAULT_LPF))
		return 12;
    if(dmp_load_motion_driver_firmware())							    //加载dmp固件
		return 13;
    if(dmp_set_orientation(inv_orientation_matrix_to_scalar(gyro_orientation)))//设置陀螺仪方向
        return 14;
    if(dmp_enable_feature(DMP_FEATURE_GYRO_CAL | DMP_FEATURE_SEND_CAL_GYRO | DMP_FEATURE_SEND_RAW_ACCEL | DMP_FEATURE_6X_LP_QUAT))	//设置dmp功能
		return 15;
    if(dmp_set_fifo_rate(DEFAULT_MPU_HZ))							    //设置DMP输出速率(最大不超过200Hz)
		return 16;
	if(dmp_set_interrupt_mode(DMP_INT_CONTINUOUS))						//使能中断模式
		return 17;
	if(dmp_enable_gyro_cal(1))										    //使能陀螺仪自我校验
		return 18;
    if(mpuxxxx_run_self_test())                                         //自检，找平。并把偏移值写入offset寄存器，或者写入mpl库
        return 19;
    if(mpu_set_dmp_state(1))										    //使能DMP
		return 20;

	return 0;
}

//mpl库初始化
//返回0：成功 返回其它：失败
int mpuxxxx_mpl_init(void)
{
    if(inv_init_mpl())
        return 1;
    if(inv_enable_quaternion())			//compute 6-axis quaternions
        return 2;
    if(inv_enable_9x_sensor_fusion())	//compute 9-axis quaternions
        return 3;
    if(inv_enable_fast_nomot())			//当静止状态时重新校准陀螺仪的初始偏移值
        return 4;
    if(inv_enable_gyro_tc())			//当温度变化时自动更新陀螺仪的初始偏移值
        return 5;

#ifdef COMPASS_ENABLED
    /* Compass calibration algorithms. */
    if(inv_enable_vector_compass_cal())
        return 6;
    if(inv_enable_magnetic_disturbance())
        return 7;
#endif

    if(inv_enable_eMPL_outputs())
        return 8;
    if(inv_start_mpl())
        return 9;

    inv_set_gyro_sample_rate(1000000L / DEFAULT_GYRO_FSR);
    inv_set_accel_sample_rate(1000000L / DEFAULT_ACCEL_FSR);
#ifdef COMPASS_ENABLED
    inv_set_compass_sample_rate(COMPASS_READ_MS * 1000L);
#endif

    inv_set_gyro_orientation_and_scale(
            inv_orientation_matrix_to_scalar(gyro_orientation),
            (long)DEFAULT_GYRO_FSR<<15);
    inv_set_accel_orientation_and_scale(
            inv_orientation_matrix_to_scalar(gyro_orientation),
            (long)DEFAULT_ACCEL_FSR<<15);
#ifdef COMPASS_ENABLED
    unsigned short compass_fsr;
    mpu_get_compass_fsr(&compass_fsr);
    inv_set_compass_orientation_and_scale(
            inv_orientation_matrix_to_scalar(compass_orientation),
            (long)compass_fsr<<15);
#endif

    return 0;
}

//mpuxxxx中断回调函数
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_PIN)
{
	mpuxxxx_new_data = 1;
}

//mpl库的轮询函数，一但发现有新的数据就从dmp中读出并写入mpl库进行数据融合。
//轮询的频率要大于等于mpu的采样速度，否则得出的数据很不准确。
int mpuxxxx_mpl_poll(void)
{
    short gyro[3], accel_short[3], sensors;
    long accel[3], quat[4];
    unsigned char more;
    long temperature;
    unsigned long timestamp;

	if(mpuxxxx_new_data == 0)
		return 1;
	else
		mpuxxxx_new_data = 0;
	
    if(dmp_read_fifo(gyro, accel_short, quat, &timestamp, &sensors, &more))
        return 2;

    if(sensors & INV_XYZ_GYRO)
    {
        if(inv_build_gyro(gyro, timestamp))
			return 3;
    }

    if(sensors & INV_XYZ_ACCEL)
    {
        accel[0] = (long)accel_short[0];
        accel[1] = (long)accel_short[1];
        accel[2] = (long)accel_short[2];
        if(inv_build_accel(accel, 0, timestamp))
			return 4;
    }

    if(sensors & INV_WXYZ_QUAT)
    {
        if(inv_build_quat(quat, 0, timestamp))
			return 5;
    }

    if(mpu_get_temperature(&temperature, &timestamp))
	{
		return 6;
	}
    if(inv_build_temp(temperature, timestamp))
	{
		return 7;
	}

#ifdef COMPASS_ENABLED
    short compass_short[3];
    long compass[3];
	mpu_set_bypass(1);
    if(mpu_get_compass_reg(compass_short, &timestamp) == 0)
	{
		compass[0] = (long)compass_short[0];
		compass[1] = (long)compass_short[1];
		compass[2] = (long)compass_short[2];
		inv_build_compass(compass, 0, timestamp);
	}
	mpu_set_bypass(0);
#endif

    inv_execute_on_data();

    return 0;
}

//从mpl库中获取欧拉角
int mpuxxxx_mpl_get_euler(EulerAngle *_euler)
{
    long mpl_data[4];
    int8_t accuracy;
    unsigned long timestamp;
	
	if(mpuxxxx_new_data == 0)
        return 0;
    else
        mpuxxxx_new_data = 0;

#if 0
    const float q30 = (float)(1<<30);
    if(inv_get_sensor_type_quat(mpl_data, &accuracy, (inv_time_t *)&timestamp) == 1)
    {
        //q30格式转换为浮点数
        float q0 = mpl_data[0] / q30;
        float q1 = mpl_data[1] / q30;
        float q2 = mpl_data[2] / q30;
        float q3 = mpl_data[3] / q30;
        //计算得到俯仰角/横滚角/航向角
        _euler->pitch = asin(-2*q1*q3 + 2*q0*q2) * 57.3;
        _euler->roll  = atan2(2*q2*q3 + 2*q0*q1, -2*q1*q1 - 2*q2*q2 + 1) * 57.3;
        _euler->yaw   = atan2(2*(q1*q2 + q0*q3), q0*q0 + q1*q1 - q2*q2 - q3*q3) * 57.3;
    }
#endif

#if 1
    const float q16 = (float)(1<<16);
    if(inv_get_sensor_type_euler(mpl_data, &accuracy, (inv_time_t *)&timestamp) == 1)
    {
        _euler->pitch = mpl_data[0] / q16;
        _euler->roll  = mpl_data[1] / q16;
        _euler->yaw   = mpl_data[2] / q16;
    }
#endif

	return 0;
}

//得到dmp处理后的数据(注意,本函数需要比较多堆栈,局部变量有点多)
//pitch:俯仰角 精度:0.1°   范围:-90.0°  <---> +90.0°
//roll:横滚角  精度:0.1°   范围:-180.0° <---> +180.0°
//yaw:航向角   精度:0.1°   范围:-180.0° <---> +180.0°
//返回0：无数据 返回其它：有数据
int mpuxxxx_dmp_get_euler(EulerAngle *_euler)
{
    long quat[4];
    short gyro[3], accel[3], sensors;
    unsigned long sensor_timestamp;
    unsigned char more;
    float q0 = 0.0f, q1 = 0.0f, q2 = 0.0f, q3 = 0.0f;

    if(mpuxxxx_new_data == 0)
        return 1;
    else
        mpuxxxx_new_data = 0;

    //从DMP中读取数据
    int result = dmp_read_fifo(gyro, accel, quat, &sensor_timestamp, &sensors, &more);

    /* Unlike gyro and accel, quaternions are written to the FIFO in the body frame, q30.
     * The orientation is set by the scalar passed to dmp_set_orientation during initialization. 
     */
    if((result == 0) && (sensors & INV_WXYZ_QUAT))
    {
        //q30格式转换为浮点数
        q0 = quat[0] / 1073741824.0f;
        q1 = quat[1] / 1073741824.0f;
        q2 = quat[2] / 1073741824.0f;
        q3 = quat[3] / 1073741824.0f;

        //计算得到俯仰角/横滚角/航向角
        _euler->pitch = asin(-2*q1*q3 + 2*q0*q2) * 57.3;
        _euler->roll  = atan2(2*q2*q3 + 2*q0*q1, -2*q1*q1 - 2*q2*q2 + 1) * 57.3;
        _euler->yaw   = atan2(2*(q1*q2 + q0*q3), q0*q0 + q1*q1 - q2*q2 - q3*q3) * 57.3;
    }

    return 0;
}

int mpuxxxx_dmp_get_scale_data(Gyroscope *_gyro, Acceleration *_accel, short *sensors)
{
    long quat[4];
    short gyro[3], accel[3];
    unsigned long sensor_timestamp;
    unsigned char more;
	
	if(mpuxxxx_new_data == 0)
        return 1;
    else
        mpuxxxx_new_data = 0;

    //从DMP中读取数据
    int result = dmp_read_fifo(gyro, accel, quat, &sensor_timestamp, sensors, &more);
	
	//陀螺仪数据转化成角速度（单位 弧度/秒）
	//当陀螺仪的FSR为2000时，scale factor 为 16.4
	if(_gyro && (result == 0) && (*sensors & INV_XYZ_GYRO))
	{
		_gyro->x = gyro[0] / 16.4f;
		_gyro->y = gyro[1] / 16.4f;
		_gyro->z = gyro[2] / 16.4f;
	}
	
	//加速度计原始数据转化成重力加速度（单位G: 1G为9.8米每秒）
	//当回速度计的FSR为2G时，scale factor 为 16384
	if(_accel && (result == 0) && (*sensors & INV_XYZ_ACCEL))
    {
        _accel->x = (float)accel[0] / 16384;
        _accel->y = (float)accel[1] / 16384;
        _accel->z = (float)accel[2] / 16384;
    }
	
	return 0;
}