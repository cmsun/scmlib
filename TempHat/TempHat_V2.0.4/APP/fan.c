#include "fan.h"

void fan_init(void)
{
    _GPIO_Init_OutPut(FAN_GPIO_PORT, FAN_GPIO_PIN);
}

void fan_on(void)
{
    _GPIO_WriteHigh(FAN_GPIO_PORT, FAN_GPIO_PIN);
}

void fan_off(void)
{
    _GPIO_WriteLow(FAN_GPIO_PORT, FAN_GPIO_PIN);
}