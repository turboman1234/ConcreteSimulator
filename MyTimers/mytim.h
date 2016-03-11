#ifndef __MYTIM_H
#define __MYTIM_H

void InitTIM2(void);
void TIM2_IRQHandler(void);

void InitTIM3(void);
void ReInitModBusTimer(unsigned short miliseconds);
void ModBusTimerEnable(unsigned short miliseconds);
void ModBusTimerDisable(void);
void TIM3_IRQHandler(void);

void InitTIM4(void);
void ReInitRS232Timer(unsigned short miliseconds);
void RS232TimerEnable(unsigned short miliseconds);
void RS232TimerDisable(void);
void TIM4_IRQHandler(void);

#endif
