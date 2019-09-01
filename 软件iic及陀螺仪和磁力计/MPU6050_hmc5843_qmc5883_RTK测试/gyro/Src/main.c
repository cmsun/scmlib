
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2019 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f4xx_hal.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"

/* USER CODE BEGIN Includes */
#include <math.h>
#include "mpuxxxx.h"
#include "debug.h"
#include "eMPL_outputs.h"
#include "packet.h"
#include "hmc5843.h"
#include "MahonyAHRS.h"
#include "GPS.h"
#include "iic.h"
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */
unsigned char *mpl_key = (unsigned char*)"eMPL 5.1";
short sensors;
EulerAngle euler;
/* Quaternion quat; */
Acceleration accel;
long mpl_data[9];
int8_t accuracy;
unsigned long timestamp;
CompassGaussData comp;
float compass_angle;
int result = 0;
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  *
  * @retval None
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_USART1_UART_Init();
  MX_SPI2_Init();
  /* USER CODE BEGIN 2 */

  Mahony_Init(100);
  GPS_init();
  __HAL_UART_ENABLE_IT(&huart1, UART_IT_RXNE);
  
//  while((result = hmc5843_init()) != 0)
//  {
//	  log_i("HMC5840 initialize failed: %d...!\r\n", result);
//	  HAL_Delay(100);
//  }
  
  while((result = mpuxxxx_mpu_dmp_init()) != 0)
  {
    log_i("MPU6050 dmp initialize failed: %d... \r\n", result);
    HAL_Delay(100);
  }
	
  while((result = mpuxxxx_mpl_init()) != 0)
  {
    log_i("MPU6515 mpl initialize failed: %d... \r\n", result);
    HAL_Delay(1000);
  }
	
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */
    HAL_GPIO_TogglePin(ARM_LEDG_GPIO_Port, ARM_LEDG_Pin);
      
	GPS_poll(); //GPS 数据分析
	 
#if 0
	Gyroscope gyro;
	Acceleration accel;
	CompassGaussData comp;
	static Gyroscope s_gyro;
	static Acceleration s_accel;
	static CompassGaussData s_comp;
	short sensors;
	
	//由于调用初始化函数Mahony_Init(100)时设置了数据融合的频率为100Hz
	//所以Mahony_update()函数的调用频率也必须为100Hz，否则对陀螺仪角速
	//度的积分时间将会不准确，从而导致大的积分误差。
	HAL_Delay(7); //IIC读取数据需要大量时间，所以延时7ms而不是10ms。
	  
	result = mpuxxxx_dmp_get_scale_data(&gyro, &accel, &sensors);
	if((result == 0) && (sensors & INV_XYZ_GYRO))
		memcpy(&s_gyro, &gyro, sizeof(gyro));
	if((result == 0) && (sensors & INV_XYZ_ACCEL))
		memcpy(&s_accel, &accel, sizeof(accel));
	result = hmc5843_get_data(NULL, &comp);
	if(result == 0)
		memcpy(&s_comp, &comp, sizeof(comp));
	Mahony_update(s_gyro.x, s_gyro.y, s_gyro.z, s_accel.x, s_accel.y, s_accel.z, s_comp.x, s_comp.y, s_comp.z);
	Mahony_computeAngles();
	euler.pitch = getPitch();
	euler.roll = getRoll();
	euler.yaw = getYaw();

	hmc5843_get_direction(&compass_angle, euler.pitch, euler.roll);
#endif

#if 1
	const float q30 = (float)(1<<30), q16 = (float)(1<<16);
	
	HAL_Delay(10);
	
	mpuxxxx_mpl_poll();
  mpuxxxx_mpl_get_euler(&euler);
  log_i("mpl euler pitch:%f roll:%f yaw:%f\r\n", euler.pitch, euler.roll, euler.yaw);

	if(inv_get_sensor_type_euler(mpl_data, &accuracy, (inv_time_t *)&timestamp) == 1)
	{
		euler.pitch = mpl_data[0]/q16;
		euler.roll = mpl_data[1]/q16;
		euler.yaw = mpl_data[2]/q16;
		log_i("mpl euler pitch:%f roll:%f yaw:%f\r\n", euler.pitch, euler.roll, euler.yaw);	  
	}
	if(inv_get_sensor_type_quat(mpl_data, &accuracy, (inv_time_t *)&timestamp) == 1)
	{
		//q30格式转换为浮点数
		float q0 = mpl_data[0] / q30;
		float q1 = mpl_data[1] / q30;
		float q2 = mpl_data[2] / q30;
		float q3 = mpl_data[3] / q30;
		//计算得到俯仰角/横滚角/航向角
		euler.pitch = asin(-2 * q1 * q3 + 2 * q0* q2)* 57.3;									// pitch
		euler.roll  = atan2(2 * q2 * q3 + 2 * q0 * q1, -2 * q1 * q1 - 2 * q2* q2 + 1)* 57.3;	// roll
		euler.yaw   = atan2(2*(q1*q2 + q0*q3),q0*q0+q1*q1-q2*q2-q3*q3) * 57.3;					//yaw
		log_i("mpl euler pitch:%f roll:%f yaw:%f\r\n", euler.pitch, euler.roll, euler.yaw);
		log_i("mpl quat: %f %f %f %f\r\n", mpl_data[0]/q30, mpl_data[1]/q30, mpl_data[2]/q30, mpl_data[3]/q30);
		  
		eMPL_send_quat(mpl_data);
	}
	if(inv_get_sensor_type_gyro(mpl_data, &accuracy, (inv_time_t*)&timestamp) == 1)
	{
		log_i("mpl gyro: %f %f %f\r\n", mpl_data[0]/q16, mpl_data[1]/q16, mpl_data[2]/q16);
	}
	if(inv_get_sensor_type_accel(mpl_data, &accuracy, (inv_time_t*)&timestamp) == 1)
	{
		accel.x = mpl_data[0] / q16;
		accel.y = mpl_data[1] / q16;
		accel.z = mpl_data[2] / q16;
		log_i("mpl accel: %f %f %f\r\n", mpl_data[0]/q16, mpl_data[1]/q16, mpl_data[2]/q16);
	}
#endif
  }
  /* USER CODE END 3 */

}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

    /**Configure the main internal regulator output voltage 
    */
  __HAL_RCC_PWR_CLK_ENABLE();

  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  file: The file name as string.
  * @param  line: The line in file as a number.
  * @retval None
  */
void _Error_Handler(char *file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
