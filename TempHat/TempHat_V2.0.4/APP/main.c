#include "key.h"
#include "led.h"
#include "fan.h"
#include "tick.h"
#include "power.h"
#include "iwdg.h"
#include "analog.h"
#include "digitron.h"
#include "TempManage.h"

uint8_t opt2=0;
int main( void )
{
#ifdef STM8S003
    //设置内部高速时钟16M为主时钟
    CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);
    enableInterrupts();
    
//    #define OPT0_ROP_ADDR 0x4800                                            //option byte 0
//    uint8_t opt0 = FLASH_ReadOptionByte(OPT0_ROP_ADDR);
//    FLASH_Unlock(FLASH_MEMTYPE_DATA);
//    if(opt0 != 0xAA) FLASH_ProgramOptionByte(OPT0_ROP_ADDR, (uint8_t)0xAA); //设置读保护
//    FLASH_Lock(FLASH_MEMTYPE_DATA);
#else
    volatile bool release = true;
    if(release)                                     //如果需要调试，则复位芯片然后马上点仿真，在这里打断点并把release变量变成false。
    {
        delay1ms(1000);                             //延时1秒钟再禁止SWD功能
        Sysctrl_SetFunc(SysctrlSWDUseIOEn, TRUE);   //禁止SWD功能，把SWD作为普通IO
    }
#endif

    while(1)
    {
        if(pwr_is_lowpower_mode())
        {
            //iwdg_init();
            key_init();
            led_init();
            fan_init();
            pwr_init();
            digitron_init();
            analog_init();
            tick_init(); //HC32F003的库函数在初始化ADC时会重新设置SysTick，所以要放在ADC之后再初始化SysTick
            temp_manage_init();
            led_on(LED_GREEN);
            pwr_exit_lowpower_mode(); //防止多次初始化
        }
        else
        {
            //iwdg_feed();
            key_task_loop();
            pwr_task_loop();
            digitron_task_loop();
            analog_task_loop();
            temp_manage_task_loop();        
        }
    }
    return 0;
}
