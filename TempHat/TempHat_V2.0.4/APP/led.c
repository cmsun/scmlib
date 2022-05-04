#include "led.h"

void led_init(void)
{
    _GPIO_Init_OutPut(LED_BLUE_GPIO_PORT, LED_BLUE_GPIO_PIN);
    _GPIO_Init_OutPut(LED_RED_GPIO_PORT, LED_RED_GPIO_PIN);
    _GPIO_Init_OutPut(LED_GREEN_GPIO_PORT, LED_GREEN_GPIO_PIN);
    
    _GPIO_WriteHigh(LED_BLUE_GPIO_PORT, LED_BLUE_GPIO_PIN);
    _GPIO_WriteHigh(LED_RED_GPIO_PORT, LED_RED_GPIO_PIN);
    _GPIO_WriteHigh(LED_GREEN_GPIO_PORT, LED_GREEN_GPIO_PIN);
}

void led_on(LED_ENUM led)
{
    if(led == LED_BLUE)
        _GPIO_WriteLow(LED_BLUE_GPIO_PORT, LED_BLUE_GPIO_PIN);
    else if(led == LED_RED)
        _GPIO_WriteLow(LED_RED_GPIO_PORT, LED_RED_GPIO_PIN);
    else if(led == LED_GREEN)
        _GPIO_WriteLow(LED_GREEN_GPIO_PORT, LED_GREEN_GPIO_PIN);
}

void led_off(LED_ENUM led)
{
    if(led == LED_BLUE)
        _GPIO_WriteHigh(LED_BLUE_GPIO_PORT, LED_BLUE_GPIO_PIN);
    else if(led == LED_RED)
        _GPIO_WriteHigh(LED_RED_GPIO_PORT, LED_RED_GPIO_PIN);
    else if(led == LED_GREEN)
        _GPIO_WriteHigh(LED_GREEN_GPIO_PORT, LED_GREEN_GPIO_PIN);
}

void led_toggle(LED_ENUM led)
{
    if(led == LED_BLUE)
        _GPIO_Toggle(LED_BLUE_GPIO_PORT, LED_BLUE_GPIO_PIN);
    else if(led == LED_RED)
        _GPIO_Toggle(LED_RED_GPIO_PORT, LED_RED_GPIO_PIN);
    else if(led == LED_GREEN)
        _GPIO_Toggle(LED_GREEN_GPIO_PORT, LED_GREEN_GPIO_PIN);
}