#ifndef __ADC_H
#define __ADC_H

void Init_ADC1(void);
void Init_ADC2(void);
void Init_ADC3(void);
u16 GetADCValue(ADC_TypeDef* ADCx, int channel);

#endif
