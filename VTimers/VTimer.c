/*
 * Copyright (c) 2010 All Right Reserved
 * Project: Multi Beverage Brewer
 * Author: Emanuil Markov
 * Summary: VTimers module.
 */
#include "stm32f4xx_conf.h"
#include "definitions.h"
#include "mytim.h"
#include "VTimer.h"

volatile u32 arrVTimers[MAX_TIMER_COUNT];
u16 vtimersRestartCount;
extern volatile u32 timerCounter;


void InitVTimers(void)
{
    int i;
   
    ResetTimerCounter();
    vtimersRestartCount = 0;
    
    for (i = 0; i < MAX_TIMER_COUNT; i++)
    {
        arrVTimers[i] = 0;
    }
    
    InitTIM2();
    EnableVTimers();
}

void EnableVTimers(void)
{
    TIM_Cmd(TIM2, ENABLE);
}

void DisableVTimers(void)
{
    TIM_Cmd(TIM2, DISABLE);
}

int ConvertTimerIDToVTimerIndex(int timerID)
{
    assert_param(IS_TIMER_ID_VALID(timerID));
    int index;
    
    //convert timerID to index for VTimers array
    index = timerID % TIMER_1; //TIMER_1 is first timer's id
    
    return index;
}

void SetVTimerValue(int timerID, u32 ticks)
{
    int index = ConvertTimerIDToVTimerIndex(timerID);
    
    arrVTimers[index] = mGetTimerCounter() + ticks;
}

u32 GetVTimerValue(int timerID)
{
    int index = ConvertTimerIDToVTimerIndex(timerID);
    
    if (mGetTimerCounter() < arrVTimers[index])
    {
    	return arrVTimers[timerID] - mGetTimerCounter();
    }
    else
    {
        return 0;
    }
}

void ClearVTimer(int timerID)
{
    int index = ConvertTimerIDToVTimerIndex(timerID);
    
    arrVTimers[index] = 0;
}

int IsVTimerElapsed(int timerID)
{
    int index = ConvertTimerIDToVTimerIndex(timerID);
    
    if(arrVTimers[index] <= mGetTimerCounter())
    {
        return ELAPSED;
    }
    else
    {
        return NOT_ELAPSED;
    }
}

void VTimersReset(void)
{
    u32 tick = mGetTimerCounter(),
    newTick = ResetTimerCounter();
    
    vtimersRestartCount++;
    
    for (int i = 0; i < MAX_TIMER_COUNT; i++)
    {
        if (arrVTimers[i] > tick)
        {
            arrVTimers[i] = newTick + (arrVTimers[i] - tick);
        }
        else if (arrVTimers[i] != 0)
        {
            arrVTimers[i] = 0;
        }
    }
}

void VTimerTask(void)
{
    if (mGetTimerCounter() > 0xffff0000)
    {
        VTimersReset();
    }
}

u32 ResetTimerCounter(void)
{
    return timerCounter = 1;
}

u32 GetTimerCounter(void)
{
    return timerCounter;
}


