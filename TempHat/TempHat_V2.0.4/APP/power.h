#ifndef __POWER_H
#define __POWER_H

#include "stdint.h"
#include "stdbool.h"
#include "configure.h"

void pwr_init(void);
void pwr_weakup(void);
void pwr_set_lowpower_mode(void);
void pwr_exit_lowpower_mode(void);
bool pwr_is_lowpower_mode(void);
bool pwr_battery_is_fully_charged(void);
bool pwr_battery_is_charging(void);
void pwr_temp_control_standby(void);
void pwr_temp_control_cool(float duty_cycles);
void pwr_temp_control_heat(float duty_cycles);
void pwr_task_loop(void);

#endif