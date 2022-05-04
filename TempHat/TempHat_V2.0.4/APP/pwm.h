#ifndef __pwm_H
#define __pwm_H

#include <stdint.h>
#include "configure.h"

typedef enum {
    PWM_IDLE_STATE_RESET,
    PWM_IDLE_STATE_SET
} pwm_IDLE_STATE;

void pwm_init(uint16_t period, float duty_cycles);
void pwm_set_period(uint16_t period);
void pwm_set_duty_cycles(float duty_cycles);
void pwm_enable(void);
void pwm_disable(pwm_IDLE_STATE state);
void pwm_period_call(void);

#endif