#include "pid.h"

#define RANGE(x, min, max) ((x)<(min)?(min):(x)>(max)?(max):(x))

void pid_init(PID_TypeDef *pid)
{
    pid->Kp = PID_DEFAULT_KP;
    pid->Ki = PID_DEFAULT_KI;
    pid->Kd = PID_DEFAULT_KD;
    pid->biase = 0;
    pid->intergral = 0;
    pid->limit_min = PID_DEFAULT_MIN;
    pid->limit_max = PID_DEFAULT_MAX;
}

void pid_set_param(PID_TypeDef *pid, float p, float i, float d)
{
    pid->Kp = p;
    pid->Ki = i;
    pid->Kd = d;
}

void pid_set_limit(PID_TypeDef *pid, float min, float max)
{
    pid->limit_min = min;
    pid->limit_max = max;
}

void pid_reset(PID_TypeDef *pid)
{
    pid->biase = 0;
    pid->intergral = 0;
}

float pid_process(PID_TypeDef *pid, float setpoint, float value)
{
    float result;
    float currbiase;
    
    currbiase = setpoint - value;
    pid->intergral += currbiase;
    result = pid->Kp*currbiase + pid->Ki*pid->intergral + pid->Kd*(currbiase - pid->biase);
    pid->biase = currbiase;
    
    result = RANGE(result, pid->limit_min, pid->limit_max);
    //限制积分的增长，防止控制系统不灵敏的情况下过度积分，导致系统控制控制不灵敏的情况变得更严重。
    pid->intergral = RANGE(pid->intergral, -PID_INTERGRAL_RANGE, PID_INTERGRAL_RANGE);
    
    return result;
}