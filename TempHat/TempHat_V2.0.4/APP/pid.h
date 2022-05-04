#ifndef __PID_H
#define __PID_H

#define PID_DEFAULT_KP      (1E-1f)
#define PID_DEFAULT_KI      (1E-10f)
#define PID_DEFAULT_KD      0
#define PID_DEFAULT_MIN     (-0.6f)
#define PID_DEFAULT_MAX     (+0.6f)
#define PID_INTERGRAL_RANGE (150.0f)

typedef struct {
    float Kp;
    float Ki;
    float Kd;
    float biase;
    float intergral;
    float limit_min;
    float limit_max;
} PID_TypeDef;

void pid_init(PID_TypeDef *pid);
void pid_set_limit(PID_TypeDef *pid, float min, float max);
void pid_reset(PID_TypeDef *pid);
float pid_process(PID_TypeDef *pid, float setpoint, float value);

#endif