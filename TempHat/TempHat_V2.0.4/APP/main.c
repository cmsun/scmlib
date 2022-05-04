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
    //�����ڲ�����ʱ��16MΪ��ʱ��
    CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);
    enableInterrupts();
    
//    #define OPT0_ROP_ADDR 0x4800                                            //option byte 0
//    uint8_t opt0 = FLASH_ReadOptionByte(OPT0_ROP_ADDR);
//    FLASH_Unlock(FLASH_MEMTYPE_DATA);
//    if(opt0 != 0xAA) FLASH_ProgramOptionByte(OPT0_ROP_ADDR, (uint8_t)0xAA); //���ö�����
//    FLASH_Lock(FLASH_MEMTYPE_DATA);
#else
    volatile bool release = true;
    if(release)                                     //�����Ҫ���ԣ���λоƬȻ�����ϵ���棬�������ϵ㲢��release�������false��
    {
        delay1ms(1000);                             //��ʱ1�����ٽ�ֹSWD����
        Sysctrl_SetFunc(SysctrlSWDUseIOEn, TRUE);   //��ֹSWD���ܣ���SWD��Ϊ��ͨIO
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
            tick_init(); //HC32F003�Ŀ⺯���ڳ�ʼ��ADCʱ����������SysTick������Ҫ����ADC֮���ٳ�ʼ��SysTick
            temp_manage_init();
            led_on(LED_GREEN);
            pwr_exit_lowpower_mode(); //��ֹ��γ�ʼ��
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
