#include "configure.h"

void _GPIO_Init_OutPut(_GPIO_PORT_TypeDef port, _GPIO_PIN_TypeDef pin)
{
#ifdef STM8S003
    GPIO_Init(port, pin, GPIO_MODE_OUT_PP_LOW_SLOW);
#else
    stc_gpio_cfg_t stcGpioCfg;
    ///< ��GPIO����ʱ���ſ�
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE); 
    ///< �˿ڷ�������
    stcGpioCfg.enDir = GpioDirOut;
    ///< �˿�����������
    stcGpioCfg.enPu = GpioPuDisable;
    stcGpioCfg.enPd = GpioPdDisable;
    ///< GPIO IO �˿ڳ�ʼ��
    Gpio_Init(port, pin, &stcGpioCfg);
#endif
}

void _GPIO_Init_InPut(_GPIO_PORT_TypeDef port, _GPIO_PIN_TypeDef pin)
{
#ifdef STM8S003
    GPIO_Init(port, pin, GPIO_MODE_IN_PU_NO_IT);
#else
    stc_gpio_cfg_t stcGpioCfg;
    ///< ��GPIO����ʱ���ſ�
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE); 
    ///< �˿ڷ�������
    stcGpioCfg.enDir = GpioDirIn;
    ///< �˿�����������
    stcGpioCfg.enPu = GpioPuDisable;
    stcGpioCfg.enPd = GpioPdDisable;
    ///< �˿ڿ�©�������->��©����ر�
    stcGpioCfg.enOD = GpioOdDisable;
    ///< GPIO IO �˿ڳ�ʼ��
    Gpio_Init(port, pin, &stcGpioCfg);
#endif
}

void _GPIO_WriteHigh(_GPIO_PORT_TypeDef port, _GPIO_PIN_TypeDef pin)
{
#ifdef STM8S003
    GPIO_WriteHigh(port, pin);
#else
    Gpio_WriteOutputIO(port, pin, TRUE);
#endif
}

void _GPIO_WriteLow(_GPIO_PORT_TypeDef port, _GPIO_PIN_TypeDef pin)
{
#ifdef STM8S003
    GPIO_WriteLow(port, pin);
#else
    Gpio_WriteOutputIO(port, pin, FALSE);
#endif
}

void _GPIO_Toggle(_GPIO_PORT_TypeDef port, _GPIO_PIN_TypeDef pin)
{
#ifdef STM8S003
    port->ODR ^= pin;
#else
    *((volatile uint32_t *)((uint32_t)&M0P_GPIO->P0OUT + port)) ^= ((1UL)<<(pin));
#endif
}

uint8_t _GPIO_ReadInputPin(_GPIO_PORT_TypeDef port, _GPIO_PIN_TypeDef pin)
{
#ifdef STM8S003
    return GPIO_ReadInputPin(port, pin);
#else
    return Gpio_GetInputIO(port, pin);
#endif
}