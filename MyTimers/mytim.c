#include "stm32f4xx_conf.h"
#include "mbslave.h"
#include "rs232.h"
#include "mytim.h"


extern volatile u32 timerCounter = 1;

//TIM2 is for VTimer library
void InitTIM2(void)
{
    TIM_TimeBaseInitTypeDef TIM_2_TimeBaseInitStruct;
    NVIC_InitTypeDef MYNVIC;
    
    //AHB clock = 50, MHz
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    
    // Configure TIM2 IRQ
    MYNVIC.NVIC_IRQChannel = TIM2_IRQn;
    MYNVIC.NVIC_IRQChannelCmd = ENABLE;
    MYNVIC.NVIC_IRQChannelPreemptionPriority = 0;
    MYNVIC.NVIC_IRQChannelSubPriority = 0;
    NVIC_Init(&MYNVIC);
   
    TIM_DeInit(TIM2);
    
    //TIM_2 clock = 50, MHz / prescaler = 50 000 000 / 500 = 100 000, Hz
    //time = TIM_2 period * (1 / TIM_2 clock) = 100 * (1 / 100 000) = 0.001, s
       
    TIM_2_TimeBaseInitStruct.TIM_Prescaler = 500;
    TIM_2_TimeBaseInitStruct.TIM_Period = 100;
    TIM_2_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1; // 0
    TIM_2_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_2_TimeBaseInitStruct.TIM_RepetitionCounter = 0;
    
    TIM_TimeBaseInit(TIM2, &TIM_2_TimeBaseInitStruct);
    
    TIM_ClearFlag(TIM2, TIM_FLAG_Update);
    TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
}

void TIM2_IRQHandler(void)
{
    timerCounter = timerCounter + 1;
    TIM_ClearFlag(TIM2, TIM_FLAG_Update);
    TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
}


//TIM_3 is for ModBus communication
void InitTIM3(void)
{
    TIM_TimeBaseInitTypeDef TIM_3_TimeBaseInitStruct;
    NVIC_InitTypeDef MYNVIC;
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    
    // Configure TIM3 IRQ
    MYNVIC.NVIC_IRQChannel = TIM3_IRQn;
    MYNVIC.NVIC_IRQChannelCmd = ENABLE;
    MYNVIC.NVIC_IRQChannelPreemptionPriority = 0;
    MYNVIC.NVIC_IRQChannelSubPriority = 1;
    NVIC_Init(&MYNVIC);	
    
    TIM_DeInit(TIM3);
    
    //TIM_3 clock = 50, MHz / prescaler = 50 000 000 / 50 000 = 1000, Hz
    //time = TIM_3 period * (1 / TIM_3 clock) = 1 * (1 / 1000) = 0.001, s
       
    TIM_3_TimeBaseInitStruct.TIM_Prescaler = 50000;
    TIM_3_TimeBaseInitStruct.TIM_Period = 1;
    TIM_3_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1; // 0
    TIM_3_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_3_TimeBaseInitStruct.TIM_RepetitionCounter = 0;
    
    TIM_TimeBaseInit(TIM3, &TIM_3_TimeBaseInitStruct);
    
    TIM_ClearFlag(TIM3, TIM_FLAG_Update);
    TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
    
    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
    
    //TIM3 is not enabled from init function
    //ENABLE command is set by ModBusTimerEnable()
}

void ReInitModBusTimer(unsigned short miliseconds)
{
    TIM_TimeBaseInitTypeDef MYTIM;
    
    TIM_DeInit(TIM3);
    
    MYTIM.TIM_ClockDivision = TIM_CKD_DIV1;
    MYTIM.TIM_CounterMode = TIM_CounterMode_Up;
    MYTIM.TIM_Prescaler = 50000;
    MYTIM.TIM_Period = miliseconds;
    MYTIM.TIM_RepetitionCounter = 0;
    
    TIM_TimeBaseInit(TIM3, &MYTIM);
        
    TIM_ClearFlag(TIM3, TIM_FLAG_Update);
    TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
    
    TIM_Cmd(TIM3, ENABLE);
}

void ModBusTimerEnable(unsigned short miliseconds)
{    
    TIM_Cmd(TIM3, DISABLE);
    
    ReInitModBusTimer(miliseconds);
}

void ModBusTimerDisable(void)
{
    TIM_Cmd(TIM3, DISABLE);
}


void TIM3_IRQHandler(void)
{
    TIM_Cmd(TIM3, DISABLE);
        
    MBTimerExpired();
    
    TIM_ClearFlag(TIM3, TIM_FLAG_Update);
    TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
}


//TIM_4 is for RS232 communication
void InitTIM4(void)
{
    TIM_TimeBaseInitTypeDef TIM_4_TimeBaseInitStruct;
    NVIC_InitTypeDef MYNVIC;
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
    
    // Configure TIM4 IRQ
    MYNVIC.NVIC_IRQChannel = TIM4_IRQn;
    MYNVIC.NVIC_IRQChannelCmd = ENABLE;
    MYNVIC.NVIC_IRQChannelPreemptionPriority = 0;
    MYNVIC.NVIC_IRQChannelSubPriority = 2;
    NVIC_Init(&MYNVIC);	
    
    TIM_DeInit(TIM4);
    
    //TIM_4 clock = 50, MHz / prescaler = 50 000 000 / 50 000 = 1000, Hz
    //time = TIM_4 period * (1 / TIM_4 clock) = 1 * (1 / 1000) = 0.001, s
       
    TIM_4_TimeBaseInitStruct.TIM_Prescaler = 50000;
    TIM_4_TimeBaseInitStruct.TIM_Period = 1;
    TIM_4_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1; // 0
    TIM_4_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_4_TimeBaseInitStruct.TIM_RepetitionCounter = 0;
    
    TIM_TimeBaseInit(TIM4, &TIM_4_TimeBaseInitStruct);
    
    TIM_ClearFlag(TIM4, TIM_FLAG_Update);
    TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
    
    TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
    
    //TIM4 is not enabled from init function
    //ENABLE command is set by RS232TimerEnable()
}

void ReInitRS232Timer(unsigned short miliseconds)
{
    TIM_TimeBaseInitTypeDef MYTIM;
    
    TIM_DeInit(TIM4);
    
    MYTIM.TIM_ClockDivision = TIM_CKD_DIV1;
    MYTIM.TIM_CounterMode = TIM_CounterMode_Up;
    MYTIM.TIM_Prescaler = 50000;
    MYTIM.TIM_Period = miliseconds;
    MYTIM.TIM_RepetitionCounter = 0;
    
    TIM_TimeBaseInit(TIM4, &MYTIM);
        
    TIM_ClearFlag(TIM4, TIM_FLAG_Update);
    TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
    TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
    
    TIM_Cmd(TIM4, ENABLE);
}

void RS232TimerEnable(unsigned short miliseconds)
{    
    TIM_Cmd(TIM4, ENABLE);
    
    ReInitRS232Timer(miliseconds);
}

void RS232TimerDisable(void)
{
    TIM_Cmd(TIM4, DISABLE);
}


void TIM4_IRQHandler(void)
{
    TIM_Cmd(TIM4, DISABLE);
   
    RS232TimerExpired();
    
    TIM_ClearFlag(TIM4, TIM_FLAG_Update);
    TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
}
