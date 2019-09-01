#include "system_task.h"
#include "led.h"
#include "debug.h"
#include "modbus.h"
#include "eeprom.h"
#include "rtc.h"
#include "includes.h"

//任务堆栈
OS_STK SUPER_TASK_STK[SUPER_TASK_STK_SIZE];
OS_STK EEPROM_TASK_STK[EEPROM_TASK_STK_SIZE];
OS_STK EXTERNAL_TASK_STK[EXTERNAL_TASK_STK_SIZE];
OS_STK INTERNAL_TASK_STK[INTERNAL_TASK_STK_SIZE];

void sEeproomTask(void *pdata)
{
    uint16_t param[35] = {0};
    
    eeprom_read(0, (void *)param, sizeof(param));   //从eeprom中读取配置参数
    memcpy(mbHoldReg, param, sizeof(param));        //把参数复制到modbus寄存器中
    
    while(1)
    {
        //如果重新设置了参数，则保存到eeprom中
        if(memcmp(mbHoldReg, param, sizeof(param)) != 0)
        {
            eeprom_write(0, (void *)mbHoldReg, sizeof(param));
            eeprom_read(0, (void *)param, sizeof(param));
            if(memcmp(mbHoldReg, param, sizeof(param)) != 0)
                memcpy(mbHoldReg, param, sizeof(param));
        }
        
        OSTimeDly(5);
    }
}

void sExternalTask(void *pdata)
{
    while(1)
    {
        led1_troggle();
        mb_poll();
        OSTimeDly(25);
    }
}

void sInternalTask(void *pdata)
{
    while(1)
    {
        OSTimeDly(10);
    }
}


void sSuperTask(void *pdata)
{
    OS_CPU_SR cpu_sr = 0;
	pdata = pdata; 
  	OS_ENTER_CRITICAL();			//进入临界区(无法被中断打断)    
 	OSTaskCreate(sEeproomTask, NULL, STK_ADDR(EEPROM_TASK_STK), EEPROM_TASK_PRIORITY);
 	OSTaskCreate(sExternalTask, NULL, STK_ADDR(EXTERNAL_TASK_STK), EXTERNAL_TASK_PRIORITY);
 	OSTaskCreate(sInternalTask, NULL, STK_ADDR(INTERNAL_TASK_STK), INTERNAL_TASK_PRIORITY);
	// OSTaskSuspend(SUPER_TASK_PRIORITY);              //挂起起始任务.
	OS_EXIT_CRITICAL();				//退出临界区(可以被中断打断)

    while(1)
    {
        OSTimeDly(1);
    }
}

void system_init(void)
{
    led_init();
    mb_init(MODBUS_RTU);
    eeprom_init();
    RTC_Initialize();
}
