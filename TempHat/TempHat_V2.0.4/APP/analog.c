#include <stdint.h>
#include "tick.h"
#include "analog.h"

#ifdef STM8S003
#define ADC_RESOLUTION  1024    //ADC分辨率
#define ADC_Rain        2000    //ADC阻抗，把NTC电阻去掉，然后计算出的值
#else
#define ADC_RESOLUTION  4096
#define ADC_Rain        2000    //ADC阻抗
#endif

#define MIN(x, y) ((x)<(y)?(x):(y))
#define MAX(x, y) ((x)>(y)?(x):(y))

Analog_TypeDef Analog;

const float TempMap[] = {
27.4936, 26.3245, 25.2119, 24.1527, 23.1442, 22.1835, 21.2682, 20.3959, 19.5644,
18.7714, 18.0151, 17.2935, 16.6048, 15.9475, 15.3198, 14.7203, 14.1475, 13.6003,
13.0772, 12.5771, 12.0988, 11.6413, 11.2037, 10.7848, 10.3839, 10.0000, 9.6324,
9.2802,  8.9428,  8.6195,  8.3096,  8.0124,  7.7275,  7.4541,  7.1919,  6.9403,
6.6987,  6.4669,  6.2442,  6.0304,  5.8250,  5.6276,  5.4380,  5.2557,  5.0804,
4.9119,  4.7498,  4.5939,  4.4439,  4.2995,  4.1605,  4.0268,  3.8980,  3.7739,
3.6544,  3.5393,  3.4248,  3.3215,  3.2185,  3.1191,  3.0234,  2.9310,  2.8419,
2.7559,  2.6729,  2.5929,  2.5156,  2.4410,  2.3690,  2.2994,  2.2322,  2.1673,
};

#ifdef STM8S003

void analog_init(void)
{
    Analog.Vbat = 3.7f;
    Analog.Vcc = 5.0f;
    Analog.semtemp = 0;
    Analog.bodytemp = 0;
    
    smooth_filter_init(&Analog.sf_sem);
    smooth_filter_init(&Analog.sf_body);
    
    GPIO_Init(ADC_BAT_GPIO_PORT, ADC_BAT_GPIO_PIN, GPIO_MODE_IN_FL_NO_IT);
    GPIO_Init(ADC_SEM_GPIO_PORT, ADC_SEM_GPIO_PIN, GPIO_MODE_IN_FL_NO_IT);
    GPIO_Init(ADC_BODY_GPIO_PORT, ADC_BODY_GPIO_PIN, GPIO_MODE_IN_FL_NO_IT);
}

static uint16_t adc_get_channel_value(ADC1_Channel_TypeDef channel,
                                      ADC1_SchmittTrigg_TypeDef schmitt)
{
    const uint32_t cnt = 100; 
    uint32_t sum = 0;
    uint16_t value, min = 0, max = 0;
    
    ADC1_DeInit();
    ADC1_Init(ADC1_CONVERSIONMODE_SINGLE , channel, ADC1_PRESSEL_FCPU_D18,
              ADC1_EXTTRIG_TIM, DISABLE, ADC1_ALIGN_RIGHT, schmitt, DISABLE);
    ADC1_Cmd(ENABLE);
    for(uint32_t i = 0; i < cnt; ++i)
    {
        ADC1_StartConversion();
        while(ADC1_GetFlagStatus(ADC1_FLAG_EOC) == RESET)
            ;
        ADC1_ClearFlag(ADC1_FLAG_EOC);
        if(i != 0) //舍弃第一次转换的结果
        {
            value = ADC1_GetConversionValue();
            sum += value;
            min = MIN(min, value);
            max = MAX(max, value);
        }
    }
    return (sum-min-max)/(cnt-3);
}

static inline void update_vcc_adc_value(void)
{
    Analog.adc_vcc = adc_get_channel_value(ADC1_CHANNEL_7, ADC1_SCHMITTTRIG_CHANNEL7);
}

static inline void update_battery_voltage_adc_value(void)
{
    Analog.adc_vbat = adc_get_channel_value(ADC_BAT_CHANNEL, ADC_BAT_SCHMITTTRIG_CHANNEL);
}

static inline void update_semtemp_adc_value(void)
{
    Analog.adc_sem = adc_get_channel_value(ADC_SEM_CHANNEL, ADC_SEM_SCHMITTTRIG_CHANNEL);
}

static inline void update_bodytemp_adc_value(void)
{
    Analog.adc_body = adc_get_channel_value(ADC_BODY_CHANNEL, ADC_BODY_SCHMITTTRIG_CHANNEL);
}

#else

stc_adc_cfg_t stcAdcCfg;
stc_adc_norm_cfg_t stcAdcNormCfg;

void analog_init(void)
{
    Analog.Vbat = 3.7f;
    Analog.Vcc = 5.0f;
    Analog.semtemp = 0;
    Analog.bodytemp = 0;
    
    smooth_filter_init(&Analog.sf_sem);
    smooth_filter_init(&Analog.sf_body);
    
    //GPIO配置成模拟输入模式
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE);    //GPIO 外设时钟使能
    Gpio_SetAnalogMode(ADC_BAT_GPIO_PORT, ADC_BAT_GPIO_PIN);
    Gpio_SetAnalogMode(ADC_SEM_GPIO_PORT, ADC_SEM_GPIO_PIN);
    Gpio_SetAnalogMode(ADC_BODY_GPIO_PORT, ADC_BODY_GPIO_PIN);
    //ADC配置
    Sysctrl_SetPeripheralGate(SysctrlPeripheralAdcBgr, TRUE);  //ADCBGR 外设时钟使能
    Adc_Enable();
    Bgr_BgrEnable();
    stcAdcCfg.enAdcOpMode = AdcNormalMode;          //单次采样模式
    stcAdcCfg.enAdcClkSel = AdcClkSysTDiv1;         //PCLK
    stcAdcCfg.enAdcSampTimeSel = AdcSampTime12Clk;  //12个采样时钟
    stcAdcCfg.enAdcRefVolSel = RefVolSelAVDD;       //参考电压:VADD
    stcAdcCfg.bAdcInBufEn = TRUE;                   //使用1.2基准电压必须使能输入增益
    stcAdcCfg.u32AdcRegHighThd = 0u;                //比较阈值上门限
    stcAdcCfg.u32AdcRegLowThd = 0u;                 //比较阈值下门限
    stcAdcCfg.enAdcTrig0Sel = AdcTrigDisable;       //ADC转换自动触发设置
    stcAdcCfg.enAdcTrig1Sel = AdcTrigDisable;
    Adc_Init(&stcAdcCfg);
}

static uint16_t adc_get_channel_value(en_adc_samp_ch_sel_t channel)
{
    const uint32_t cnt = 100; 
    uint32_t sum = 0;
    uint16_t value, min = 0, max = 0;

    stcAdcNormCfg.enAdcNormModeCh = channel;
    stcAdcNormCfg.bAdcResultAccEn = FALSE;
    Adc_ConfigNormMode(&stcAdcCfg, &stcAdcNormCfg);
    for(uint32_t i = 0; i < cnt; ++i)
    {
        Adc_Start();
        while(FALSE != Adc_PollBusyState())
            ;
        if(i != 0) //舍弃第一次转换的结果
        {
            Adc_GetResult(&value);
            sum += value;
            min = MIN(min, value);
            max = MAX(max, value);
        }
    }
    return (sum-min-max)/(cnt-3);
}

static inline void update_vcc_adc_value(void)
{
    Analog.adc_vcc = adc_get_channel_value(AdcVref1P2Input);
}

static inline void update_battery_voltage_adc_value(void)
{
    Analog.adc_vbat = adc_get_channel_value(ADC_BAT_CHANNEL);
}

static inline void update_semtemp_adc_value(void)
{
    Analog.adc_sem = adc_get_channel_value(ADC_SEM_CHANNEL);
}

static inline void update_bodytemp_adc_value(void)
{
    Analog.adc_body = adc_get_channel_value(ADC_BODY_CHANNEL);
}

#endif

float V = 0;
float R;
static float adc_value_to_temp(uint16_t adc_raw)
{
    float Rp = 0, Rntc = 0;     //Rp是NTC电阻和ADC阻抗的并联值。
    float Vadc = 0, temp = 0;   //Vadc是ADC采集到的电压，也就是NTC电阻两端的电压。

    V = Vadc = Analog.Vcc * adc_raw / ADC_RESOLUTION;
    Rp = 6.8f * Vadc / (Analog.Vcc - Vadc);
    R = Rntc = ADC_Rain * Rp / (ADC_Rain - Rp);
    for(int i = 0; i < sizeof(TempMap)/sizeof(TempMap[0])-1; i++)
    {
        if(Rntc <= TempMap[i] && Rntc > TempMap[i+1])
        {
            temp = i + (TempMap[i]-Rntc) / (TempMap[i]-TempMap[i+1]);
            break;
        }
    }
            
    return temp;
}

float analog_battery_voltage(void)
{
    return Analog.Vbat;
}

float analog_semiconductor_temp(void)
{
    return Analog.semtemp;
}

float analog_body_temp(void)
{
    return Analog.bodytemp;
}

void analog_task_loop(void)
{
    static uint32_t tick = 0;
    float temp;
    
    if(tick_value() - tick > 1000)
    {
        tick = tick_value();
        
        update_vcc_adc_value();
        update_battery_voltage_adc_value();
        update_semtemp_adc_value();
        update_bodytemp_adc_value();
        
        Analog.Vcc = 1.2f * ADC_RESOLUTION / Analog.adc_vcc;
        Analog.Vbat = Analog.Vcc * Analog.adc_vbat / ADC_RESOLUTION;
        
        temp = adc_value_to_temp(Analog.adc_sem);
        Analog.semtemp = smooth_filter(&Analog.sf_sem, temp);  
        
        temp = adc_value_to_temp(Analog.adc_body);
        Analog.bodytemp = smooth_filter(&Analog.sf_body, temp); 
    }
}