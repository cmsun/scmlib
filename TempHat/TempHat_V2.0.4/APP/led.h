#ifndef __LED_H
#define __LED_H

#include "configure.h"

typedef enum {
    LED_BLUE,
    LED_RED,
    LED_GREEN
} LED_ENUM;

void led_init(void);
void led_on(LED_ENUM led);
void led_off(LED_ENUM led);
void led_toggle(LED_ENUM led);

#endif