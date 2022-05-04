#ifndef __TICK_H
#define __TICK_H

#include <stdint.h>
#include "configure.h"

void tick_init(void);
void tick_increase(void);
uint32_t tick_value(void);
void tick_delay(uint32_t delay);

#endif