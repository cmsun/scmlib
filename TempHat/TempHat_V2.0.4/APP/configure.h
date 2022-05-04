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

//������ص�ѹADCͨ��
#define ADC_BAT_GPIO_PORT               GPIOC
#define ADC_BAT_GPIO_PIN                GPIO_PIN_4
#define ADC_BAT_CHANNEL                 ADC1_CHANNEL_2
#define ADC_BAT_SCHMITTTRIG_CHANNEL     ADC1_SCHMITTTRIG_CHANNEL2
//��������Ƭ��ADCͨ��
#define ADC_SEM_GPIO_PORT               GPIOD
#define ADC_SEM_GPIO_PIN                GPIO_PIN_2
#define ADC_SEM_CHANNEL                 ADC1_CHANNEL_3
#define ADC_SEM_SCHMITTTRIG_CHANNEL     ADC1_SCHMITTTRIG_CHANNEL3
//�������µ�ADCͨ��
#define ADC_BODY_GPIO_PORT              GPIOD
#define ADC_BODY_GPIO_PIN               GPIO_PIN_3
#define ADC_BODY_CHANNEL                ADC1_CHANNEL_4
#define ADC_BODY_SCHMITTTRIG_CHANNEL    ADC1_SCHMITTTRIG_CHANNEL4
//���IICʹ�õ�IO��
#define IIC_SCL_GPIO_PORT               GPIOB
#define IIC_SCL_GPIO_PIN                GPIO_PIN_4
#define IIC_SDA_GPIO_PORT               GPIOB
#define IIC_SDA_GPIO_PIN                GPIO_PIN_5
//���Ʒ��ȵ�IO��
#define FAN_GPIO_PORT                   GPIOC
#define FAN_GPIO_PIN                    GPIO_PIN_3
//��ⰴ����IO��
#define KEY_GPIO_PORT                   GPIOA
#define KEY_GPIO_PIN                    GPIO_PIN_3
#define KEY_EXIT_PORT                   EXTI_PORT_GPIOA
//LED��
#define LED_BLUE_GPIO_PORT              GPIOD
#define LED_BLUE_GPIO_PIN               GPIO_PIN_6
#define LED_RED_GPIO_PORT               GPIOD
#define LED_RED_GPIO_PIN                GPIO_PIN_5
#define LED_GREEN_GPIO_PORT             GPIOD
#define LED_GREEN_GPIO_PIN              GPIO_PIN_4
//�����¶ȴ�����������Ƭ������ȵĵ�Դ��IO
#define PERIPHERALS_CTRL_GPIO_PORT      GPIOC
#define PERIPHERALS_CTRL_GPIO_PIN       GPIO_PIN_5
//���Ƽ��Ȼ��������IO�ڣ��Լ�PWMʹ�õ�IO��
#define TEMP_SWITCH_GPIO_PORT           GPIOC
#define TEMP_SWITCH_GPIO_PIN            GPIO_PIN_7
#define PWM_GPIO_PORT                   GPIOC
#define PWM_GPIO_PIN                    GPIO_PIN_6
//������Լ��������IO��
#define CHARGER_STDBY_GPIO_PORT         GPIOA
#define CHARGER_STDBY_GPIO_PIN          GPIO_PIN_1
#define CHARGER_CHRG_GPIO_PORT          GPIOA
#define CHARGER_CHRG_GPIO_PIN           GPIO_PIN_2

#else

#define NOP                             __nop
#define _GPIO_PORT_TypeDef              en_gpio_port_t
#define _GPIO_PIN_TypeDef               en_gpio_pin_t

//������ص�ѹADCͨ��
#define ADC_BAT_GPIO_PORT               GpioPort2
#define ADC_BAT_GPIO_PIN                GpioPin4
#define ADC_BAT_CHANNEL                 AdcExInputCH0
//��������Ƭ��ADCͨ��
#define ADC_SEM_GPIO_PORT               GpioPort3
#define ADC_SEM_GPIO_PIN                GpioPin2
#define ADC_SEM_CHANNEL                 AdcExInputCH2
//�������µ�ADCͨ��
#define ADC_BODY_GPIO_PORT              GpioPort3
#define ADC_BODY_GPIO_PIN               GpioPin3
#define ADC_BODY_CHANNEL                AdcExInputCH3
//���IICʹ�õ�IO��
#define IIC_SCL_GPIO_PORT               GpioPort1
#define IIC_SCL_GPIO_PIN                GpioPin4
#define IIC_SDA_GPIO_PORT               GpioPort1
#define IIC_SDA_GPIO_PIN                GpioPin5
//���Ʒ��ȵ�IO��
#define FAN_GPIO_PORT                   GpioPort2
#define FAN_GPIO_PIN                    GpioPin3
//��ⰴ����IO��
#define KEY_GPIO_PORT                   GpioPort0
#define KEY_GPIO_PIN                    GpioPin3
#define KEY_EXIT_PORT                   PORT0_IRQn
//LED��
#define LED_BLUE_GPIO_PORT              GpioPort3
#define LED_BLUE_GPIO_PIN               GpioPin6
#define LED_RED_GPIO_PORT               GpioPort3
#define LED_RED_GPIO_PIN                GpioPin5
#define LED_GREEN_GPIO_PORT             GpioPort3
#define LED_GREEN_GPIO_PIN              GpioPin4
//�����¶ȴ�����������Ƭ������ȵĵ�Դ��IO
#define PERIPHERALS_CTRL_GPIO_PORT      GpioPort2
#define PERIPHERALS_CTRL_GPIO_PIN       GpioPin5
//���Ƽ��Ȼ��������IO�ڣ��Լ�PWMʹ�õ�IO��
#define TEMP_SWITCH_GPIO_PORT           GpioPort2
#define TEMP_SWITCH_GPIO_PIN            GpioPin7
#define PWM_GPIO_PORT                   GpioPort2
#define PWM_GPIO_PIN                    GpioPin6
//������Լ��������IO��
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