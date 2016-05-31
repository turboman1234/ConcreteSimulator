#ifndef __VTIMER_H
#define __VTIMER_H

#define mGetTimerCounter()		GetTimerCounter()
#define MAX_TIMER_COUNT                 20

void InitVTimers(void);
void EnableVTimers(void);
void DisableVTimers(void);
int ConvertTimerIDToVTimerIndex(int timerID);
void SetVTimerValue(int timerID, u32 ticks);
u32 GetVTimerValue(int timerID);
void ClearVTimer(int timerID);
int IsVTimerElapsed(int timerID);
void VTimersReset(void);
void VTimerTask(void);
u32 ResetTimerCounter(void);
u32 GetTimerCounter(void);


#endif
