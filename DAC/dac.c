#include "stm32f4xx_conf.h"
#include "dac.h"

//init DAC - chanel 1, 12 bits

// initValue - 0 - 4095
void Init_DAC_1(int initValue)
{
    DAC_InitTypeDef  DAC_InitStructure;
    
    //Set DAC clock
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);
    
    /* DAC channel1 Configuration */
    DAC_InitStructure.DAC_Trigger = DAC_Trigger_Software;
    DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
    DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
    
    
    //DAC_DeInit(); 
    DAC_Init(DAC_Channel_1, &DAC_InitStructure);
    DAC_Cmd(DAC_Channel_1, ENABLE);
    DAC_SetChannel1Data(DAC_Align_12b_R, initValue);
    DAC_SoftwareTriggerCmd(DAC_Channel_1, ENABLE);
}



//init DAC - chanel 2, 12 bits

// initValue - 0 - 4095
void Init_DAC_2(int initValue)
{
    DAC_InitTypeDef  DAC_InitStructure;
    
    //Set DAC clock
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);
    
    /* DAC channel2 Configuration */
    DAC_InitStructure.DAC_Trigger = DAC_Trigger_Software;
    DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
    DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
    
    
    //DAC_DeInit(); 
    DAC_Init(DAC_Channel_2, &DAC_InitStructure);
    DAC_Cmd(DAC_Channel_2, ENABLE);
    DAC_SetChannel2Data(DAC_Align_12b_R, initValue);
    DAC_SoftwareTriggerCmd(DAC_Channel_2, ENABLE);
}

