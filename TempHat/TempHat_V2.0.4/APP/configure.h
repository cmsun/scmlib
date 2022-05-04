#ifndef __CONFIGURE_H
#define __CONFIGURE_H

#ifdef STM8S003
#include "stm8s_conf.h"
#else
#include <stdint.h>
#include "gpio.h"
#include "adc.h"
#include "bgr.h"
#include "wdt.h"
#include "lpm.h"
#include "adt.h"
#endif

#ifdef STM8S003

#define NOP                             nop
#define _GPIO_PORT_TypeDef              GPIO_TypeDef*
#define _GPIO_PIN_TypeDef               GPIO_Pin_TypeDef

//测量电池电压ADC通道
#define ADC_BAT_GPIO_PORT               GPIOC
#define ADC_BAT_GPIO_PIN                GPIO_PIN_4
#define ADC_BAT_CHANNEL                 ADC1_CHANNEL_2
#define ADC_BAT_SCHMITTTRIG_CHANNEL     ADC1_SCHMITTTRIG_CHANNEL2
//测量制冷片的ADC通道
#define ADC_SEM_GPIO_PORT               GPIOD
#define ADC_SEM_GPIO_PIN                GPIO_PIN_2
#define ADC_SEM_CHANNEL                 ADC1_CHANNEL_3
#define ADC_SEM_SCHMITTTRIG_CHANNEL     ADC1_SCHMITTTRIG_CHANNEL3
//测量体温的ADC通道
#define ADC_BODY_GPIO_PORT              GPIOD
#define ADC_BODY_GPIO_PIN               GPIO_PIN_3
#define ADC_BODY_CHANNEL                ADC1_CHANNEL_4
#define ADC_BODY_SCHMITTTRIG_CHANNEL    ADC1_SCHMITTTRIG_CHANNEL4
//软件IIC使用的IO口
#define IIC_SCL_GPIO_PORT               GPIOB
#define IIC_SCL_GPIO_PIN                GPIO_PIN_4
#define IIC_SDA_GPIO_PORT               GPIOB
#define IIC_SDA_GPIO_PIN                GPIO_PIN_5
//控制风扇的IO口
#define FAN_GPIO_PORT                   GPIOC
#define FAN_GPIO_PIN                    GPIO_PIN_3
//检测按键的IO口
#define KEY_GPIO_PORT                   GPIOA
#define KEY_GPIO_PIN                    GPIO_PIN_3
#define KEY_EXIT_PORT                   EXTI_PORT_GPIOA
//LED灯
#define LED_BLUE_GPIO_PORT              GPIOD
#define LED_BLUE_GPIO_PIN               GPIO_PIN_6
#define LED_RED_GPIO_PORT               GPIOD
#define LED_RED_GPIO_PIN                GPIO_PIN_5
#define LED_GREEN_GPIO_PORT             GPIOD
#define LED_GREEN_GPIO_PIN              GPIO_PIN_4
//控制温度传感器、制冷片、电风扇的电源的IO
#define PERIPHERALS_CTRL_GPIO_PORT      GPIOC
#define PERIPHERALS_CTRL_GPIO_PIN       GPIO_PIN_5
//控制加热或者制冷的IO口，以及PWM使用的IO口
#define TEMP_SWITCH_GPIO_PORT           GPIOC
#define TEMP_SWITCH_GPIO_PIN            GPIO_PIN_7
#define PWM_GPIO_PORT                   GPIOC
#define PWM_GPIO_PIN                    GPIO_PIN_6
//检测充电以及充满电的IO口
#define CHARGER_STDBY_GPIO_PORT         GPIOA
#define CHARGER_STDBY_GPIO_PIN          GPIO_PIN_1
#define CHARGER_CHRG_GPIO_PORT          GPIOA
#define CHARGER_CHRG_GPIO_PIN           GPIO_PIN_2

#else

#define NOP                             __nop
#define _GPIO_PORT_TypeDef              en_gpio_port_t
#define _GPIO_PIN_TypeDef               en_gpio_pin_t

//测量电池电压ADC通道
#define ADC_BAT_GPIO_PORT               GpioPort2
#define ADC_BAT_GPIO_PIN                GpioPin4
#define ADC_BAT_CHANNEL                 AdcExInputCH0
//测量制冷片的ADC通道
#define ADC_SEM_GPIO_PORT               GpioPort3
#define ADC_SEM_GPIO_PIN                GpioPin2
#define ADC_SEM_CHANNEL                 AdcExInputCH2
//测量体温的ADC通道
#define ADC_BODY_GPIO_PORT              GpioPort3
#define ADC_BODY_GPIO_PIN               GpioPin3
#define ADC_BODY_CHANNEL                AdcExInputCH3
//软件IIC使用的IO口
#define IIC_SCL_GPIO_PORT               GpioPort1
#define IIC_SCL_GPIO_PIN                GpioPin4
#define IIC_SDA_GPIO_PORT               GpioPort1
#define IIC_SDA_GPIO_PIN                GpioPin5
//控制风扇的IO口
#define FAN_GPIO_PORT                   GpioPort2
#define FAN_GPIO_PIN                    GpioPin3
//检测按键的IO口
#define KEY_GPIO_PORT                   GpioPort0
#define KEY_GPIO_PIN                    GpioPin3
#define KEY_EXIT_PORT                   PORT0_IRQn
//LED灯
#define LED_BLUE_GPIO_PORT              GpioPort3
#define LED_BLUE_GPIO_PIN               GpioPin6
#define LED_RED_GPIO_PORT               GpioPort3
#define LED_RED_GPIO_PIN                GpioPin5
#define LED_GREEN_GPIO_PORT             GpioPort3
#define LED_GREEN_GPIO_PIN              GpioPin4
//控制温度传感器、制冷片、电风扇的电源的IO
#define PERIPHERALS_CTRL_GPIO_PORT      GpioPort2
#define PERIPHERALS_CTRL_GPIO_PIN       GpioPin5
//控制加热或者制冷的IO口，以及PWM使用的IO口
#define TEMP_SWITCH_GPIO_PORT           GpioPort2
#define TEMP_SWITCH_GPIO_PIN            GpioPin7
#define PWM_GPIO_PORT                   GpioPort2
#define PWM_GPIO_PIN                    GpioPin6
//检测充电以及充满电的IO口
#define CHARGER_STDBY_GPIO_PORT         GpioPort0
#define CHARGER_STDBY_GPIO_PIN          GpioPin1
#define CHARGER_CHRG_GPIO_PORT          GpioPort0
#define CHARGER_CHRG_GPIO_PIN           GpioPin2

#endif

void _GPIO_Init_OutPut(_GPIO_PORT_TypeDef port, _GPIO_PIN_TypeDef pin);
void _GPIO_Init_InPut(_GPIO_PORT_TypeDef port, _GPIO_PIN_TypeDef pin);
void _GPIO_WriteHigh(_GPIO_PORT_TypeDef port, _GPIO_PIN_TypeDef pin);
void _GPIO_WriteLow(_GPIO_PORT_TypeDef port, _GPIO_PIN_TypeDef pin);
void _GPIO_Toggle(_GPIO_PORT_TypeDef port, _GPIO_PIN_TypeDef pin);
uint8_t _GPIO_ReadInputPin(_GPIO_PORT_TypeDef port, _GPIO_PIN_TypeDef pin);

#endif