/*
********************************************************************************
*                              COPYRIGHT NOTICE
*                             Copyright (c) 2016
*                             All rights reserved
*
*  @fileName       : _adc.c
*  @author         : scm 351721714@qq.com
*  @create         : 2018/09/01 09:33:44
********************************************************************************
*/

#include "stm32f10x_adc.h"
#include "stm32f10x_gpio.h"

typedef struct {
    GPIO_TypeDef *GPIOx;
    uint16_t GPIO_Pin_x;
} ADC_Pin_Map;

static const ADC_Pin_Map ADC1_Pin_Table[] = {
    {GPIOA, GPIO_Pin_0},	//ADC_IN0
    {GPIOA, GPIO_Pin_1},	//ADC_IN1
    {GPIOA, GPIO_Pin_2},	//ADC_IN2
    {GPIOA, GPIO_Pin_3},	//ADC_IN3
    {GPIOA, GPIO_Pin_4},	//ADC_IN4
    {GPIOA, GPIO_Pin_5},	//ADC_IN5
    {GPIOA, GPIO_Pin_6},	//ADC_IN6
    {GPIOA, GPIO_Pin_7},	//ADC_IN7
    {GPIOB, GPIO_Pin_0},	//ADC_IN8
    {GPIOB, GPIO_Pin_1},	//ADC_IN9
    {GPIOC, GPIO_Pin_0},    //ADC_IN10
    {GPIOC, GPIO_Pin_1},    //ADC_IN11
    {GPIOC, GPIO_Pin_2},    //ADC_IN12
    {GPIOC, GPIO_Pin_3},    //ADC_IN13
    {GPIOC, GPIO_Pin_4},    //ADC_IN14
    {GPIOC, GPIO_Pin_5},    //ADC_IN15
};

static const ADC_Pin_Map ADC3_Pin_Table[] = {
    {GPIOA, GPIO_Pin_0},	//ADC_IN0
    {GPIOA, GPIO_Pin_1},	//ADC_IN1
    {GPIOA, GPIO_Pin_2},	//ADC_IN2
    {GPIOA, GPIO_Pin_3},	//ADC_IN3
    {GPIOF, GPIO_Pin_6},	//ADC_IN4
    {GPIOF, GPIO_Pin_7},	//ADC_IN5
    {GPIOF, GPIO_Pin_8},	//ADC_IN6
    {GPIOF, GPIO_Pin_9},	//ADC_IN7
    {GPIOF, GPIO_Pin_10},	//ADC_IN8
    {0, 0},             	//ADC_IN9
    {GPIOC, GPIO_Pin_0},    //ADC_IN10
    {GPIOC, GPIO_Pin_1},    //ADC_IN11
    {GPIOC, GPIO_Pin_2},    //ADC_IN12
    {GPIOC, GPIO_Pin_3},    //ADC_IN13
};

void adc_init(ADC_TypeDef *ADCx,
            uint8_t ADC_Channel_x,
            FunctionalState scanMode,
            FunctionalState continueMode)
{
    //使能ADC时钟。
    if(ADCx == ADC1)
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
    else if(ADCx == ADC2)
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC2, ENABLE);
    else if(ADCx == ADC3)
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC3, ENABLE);

    /*
     * 设置ADC时钟分频因子。ADC时钟ADCCLK大于14M会导致转换不准，
     * 所以只能选择6分频和8分频：72M/6=12M, 72/8=9M
     */
    RCC_ADCCLKConfig(RCC_PCLK2_Div6);
   
    //ADC复位
    ADC_DeInit(ADCx);
    //ADC初始化 
    ADC_InitTypeDef adc_init;
    adc_init.ADC_Mode = ADC_Mode_Independent;                   //ADC模式。
    adc_init.ADC_ScanConvMode = scanMode;                       //是否开启扫描模式(扫描模式用于多通道采集)
    adc_init.ADC_ContinuousConvMode = continueMode;             //是否循环转换。
    adc_init.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;  //是否使用外部触发转换。
    adc_init.ADC_DataAlign = ADC_DataAlign_Right;               //数据对齐。
    adc_init.ADC_NbrOfChannel = ADC_Channel_x;                  //要转换的通道数目。
    ADC_Init(ADCx, &adc_init);
    ADC_Cmd(ADCx, ENABLE);
   
    //使用ADC前要校准，以提高转换精确度
    ADC_ResetCalibration(ADCx);
    while(ADC_GetResetCalibrationStatus(ADCx))
        ;
    ADC_StartCalibration(ADCx);
    while(ADC_GetCalibrationStatus(ADCx))
        ;
}

void adc_regularChannelConfig(ADC_TypeDef *ADCx,
        uint8_t ADC_Channel_x,
        uint8_t Rank,
        uint8_t ADC_SampleTime_x)
{
    GPIO_InitTypeDef init;
    init.GPIO_Mode = GPIO_Mode_AIN;
    init.GPIO_Speed = GPIO_Speed_50MHz;

    if(ADCx == ADC1 || ADCx == ADC2)
    {
        assert_param(ADC_Channel_x <= ADC_Channel_15);
        init.GPIO_Pin = ADC1_Pin_Table[ADC_Channel_x].GPIO_Pin_x;
        GPIO_Init(ADC1_Pin_Table[ADC_Channel_x].GPIOx, &init);
        /* GPIO_Initialize(ADC1_Pin_Table[ADC_Channel_x].GPIOx, ADC1_Pin_Table[ADC_Channel_x].GPIO_Pin_x, GPIO_Mode_AIN, GPIO_Speed_50MHz); */
    }
    else
    {
        assert_param(ADC_Channel_x <= ADC_Channel_13 && ADC_Channel_x != ADC_Channel_9);
        init.GPIO_Pin = ADC3_Pin_Table[ADC_Channel_x].GPIO_Pin_x;
        GPIO_Init(ADC3_Pin_Table[ADC_Channel_x].GPIOx, &init);
    }
   
    ADC_RegularChannelConfig((ADC_TypeDef *)ADCx, ADC_Channel_x, Rank, ADC_SampleTime_x);
}

void adc_regularDMAConfig(ADC_TypeDef *ADCx, uint16_t *buff, uint16_t size)
{
    //1到16个通道
    assert_param(size >= 1 && size <= 16);
    //只有ADC1和ADC3有DMA功能，ADC2没有DMA功能
    assert_param((uint32_t)ADCx == ADC1_BASE || (uint32_t)ADCx == ADC3_BASE);  
   
    DMA_InitTypeDef dma_init;
    dma_init.DMA_PeripheralBaseAddr = (uint32_t)&ADCx->DR;
    dma_init.DMA_MemoryBaseAddr = (uint32_t)buff;
    dma_init.DMA_DIR = DMA_DIR_PeripheralSRC;
    dma_init.DMA_BufferSize = size;
    dma_init.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    dma_init.DMA_MemoryInc = DMA_MemoryInc_Enable;
    dma_init.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    dma_init.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    dma_init.DMA_Mode = DMA_Mode_Circular;
    dma_init.DMA_Priority = DMA_Priority_High;
    dma_init.DMA_M2M = DMA_M2M_Disable;
   
    if(ADCx == ADC1)
    {
        RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
        DMA_DeInit(DMA1_Channel1);
        DMA_Init(DMA1_Channel1, &dma_init);
        DMA_Cmd(DMA1_Channel1, ENABLE);
    }
    else
    {
        RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);
        DMA_DeInit(DMA2_Channel5);
        DMA_Init(DMA2_Channel5, &dma_init);
        DMA_Cmd(DMA2_Channel5, ENABLE);
    }
   
    ADC_DMACmd(ADCx, ENABLE);
}
