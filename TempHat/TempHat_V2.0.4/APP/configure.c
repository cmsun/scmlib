#include "configure.h"

void _GPIO_Init_OutPut(_GPIO_PORT_TypeDef port, _GPIO_PIN_TypeDef pin)
{
#ifdef STM8S003
    GPIO_Init(port, pin, GPIO_MODE_OUT_PP_LOW_SLOW);
#else
    stc_gpio_cfg_t stcGpioCfg;
    ///< 打开GPIO外设时钟门控
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE); 
    ///< 端口方向配置
    stcGpioCfg.enDir = GpioDirOut;
    ///< 端口上下拉配置
    stcGpioCfg.enPu = GpioPuDisable;
    stcGpioCfg.enPd = GpioPdDisable;
    ///< GPIO IO 端口初始化
    Gpio_Init(port, pin, &stcGpioCfg);
#endif
}

void _GPIO_Init_InPut(_GPIO_PORT_TypeDef port, _GPIO_PIN_TypeDef pin)
{
#ifdef STM8S003
    GPIO_Init(port, pin, GPIO_MODE_IN_PU_NO_IT);
#else
    stc_gpio_cfg_t stcGpioCfg;
    ///< 打开GPIO外设时钟门控
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE); 
    ///< 端口方向配置
    stcGpioCfg.enDir = GpioDirIn;
    ///< 端口上下拉配置
    stcGpioCfg.enPu = GpioPuDisable;
    stcGpioCfg.enPd = GpioPdDisable;
    ///< 端口开漏输出配置->开漏输出关闭
    stcGpioCfg.enOD = GpioOdDisable;
    ///< GPIO IO 端口初始化
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