#ifndef SYSTEM_TASK_H
#define SYSTEM_TASK_H

#include "includes.h"

#define SUPER_TASK_PRIORITY     0
#define EEPROM_TASK_PRIORITY    1
#define EXTERNAL_TASK_PRIORITY  2
#define INTERNAL_TASK_PRIORITY  3

#define SUPER_TASK_STK_SIZE     64
#define EEPROM_TASK_STK_SIZE    64
#define EXTERNAL_TASK_STK_SIZE  64
#define INTERNAL_TASK_STK_SIZE  64

#define STK_SIZE(stk) (sizeof(stk)/sizeof(stk[0]))
#define STK_ADDR(stk) ((OS_STK *)&stk[STK_SIZE(stk)-1])

void system_init(void);
void sSuperTask(void *pdata);
extern OS_STK SUPER_TASK_STK[SUPER_TASK_STK_SIZE];

#endif

