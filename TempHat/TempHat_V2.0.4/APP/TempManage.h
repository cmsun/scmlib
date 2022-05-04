#ifndef __TEMPMANAGE__H
#define __TEMPMANAGE__H

#include "pid.h"
#include "configure.h"

#define HEAT_LIMIT 50.0f
#define COOL_LIMIT 10.0f

typedef enum {
    TEMP_CTRL_STANDBY,
    TEMP_CTRL_COOL,
    TEMP_CTRL_HEAT,
    TEMP_CTRL_SLEEP,
}TempControl_enum;

typedef struct TempManage_TypeDef {
    const float BodyTemp;
    TempControl_enum ctrl;
    PID_TypeDef pid;
} TempManage_TypeDef;

void temp_manage_init(void);
void temp_manage_task_loop(void);

#endif