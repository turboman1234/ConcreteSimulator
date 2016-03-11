#ifndef __INITPERIPHERAL_H
#define __INITPERIPHERAL_H

#define IS_BUTTON_ID_VALID (BUTTON_ID)         (((BUTTON_ID) == BUTTON_1) || \
                                                ((BUTTON_ID) == BUTTON_2) || \
                                                ((BUTTON_ID) == BUTTON_3) || \
                                                ((BUTTON_ID) == BUTTON_4) || \
                                                ((BUTTON_ID) == BUTTON_5) || \
                                                ((BUTTON_ID) == BUTTON_6) || \
                                                ((BUTTON_ID) == BUTTON_7) || \
                                                ((BUTTON_ID) == BUTTON_8))

#define IS_LED_ID_VALID (LED_ID)               (((LED_ID) == LED_1) || \
                                                ((LED_ID) == LED_2) || \
                                                ((LED_ID) == LED_3) || \
                                                ((LED_ID) == LED_4) || \
                                                ((LED_ID) == LED_5) || \
                                                ((LED_ID) == LED_6) || \
                                                ((LED_ID) == LED_7) || \
                                                ((LED_ID) == LED_8))

#define IS_TRIMMER_ID_VALID (TRIMMER_ID)       (((TRIMMER_ID) == TRIMMER_1) || \
                                                ((TRIMMER_ID) == TRIMMER_2) || \
                                                ((TRIMMER_ID) == TRIMMER_3))

#define IS_SWITCH_ID_VALID (SWITCH_ID)         (((SWITCH_ID) == SWITCH_1) || \
                                                ((SWITCH_ID) == SWITCH_2))
        
#define IS_INPUT_ID_VALID (INPUT_ID)          (((INPUT_ID) == INPUT_1) || \
                                                ((INPUT_ID) == INPUT_2) || \
                                                ((INPUT_ID) == INPUT_3) || \
                                                ((INPUT_ID) == INPUT_4) || \
                                                ((INPUT_ID) == INPUT_5) || \
                                                ((INPUT_ID) == INPUT_6) || \
                                                ((INPUT_ID) == INPUT_7) || \
                                                ((INPUT_ID) == INPUT_8) || \
                                                ((INPUT_ID) == INPUT_9) || \
                                                ((INPUT_ID) == INPUT_10) || \
                                                ((INPUT_ID) == INPUT_11) || \
                                                ((INPUT_ID) == INPUT_12) || \
                                                ((INPUT_ID) == INPUT_13) || \
                                                ((INPUT_ID) == INPUT_14) || \
                                                ((INPUT_ID) == INPUT_15) || \
                                                ((INPUT_ID) == INPUT_16))

#define IS_OUTPUT_ID_VALID (OUTPUT_ID)         (((OUTPUT_ID) == OUTPUT_1) || \
                                                ((OUTPUT_ID) == OUTPUT_2) || \
                                                ((OUTPUT_ID) == OUTPUT_3) || \
                                                ((OUTPUT_ID) == OUTPUT_4) || \
                                                ((OUTPUT_ID) == OUTPUT_5) || \
                                                ((OUTPUT_ID) == OUTPUT_6) || \
                                                ((OUTPUT_ID) == OUTPUT_7) || \
                                                ((OUTPUT_ID) == OUTPUT_8) || \
                                                ((OUTPUT_ID) == OUTPUT_9) || \
                                                ((OUTPUT_ID) == OUTPUT_10) || \
                                                ((OUTPUT_ID) == OUTPUT_11) || \
                                                ((OUTPUT_ID) == OUTPUT_12) || \
                                                ((OUTPUT_ID) == OUTPUT_13) || \
                                                ((OUTPUT_ID) == OUTPUT_14) || \
                                                ((OUTPUT_ID) == OUTPUT_15) || \
                                                ((OUTPUT_ID) == OUTPUT_16))

#define IS_ADC_ID_VALID (ADC_ID)               (((ADC_ID) == ADC_1) || \
                                                ((ADC_ID) == ADC_2))

#define IS_DAC_ID_VALID (DAC_ID)               (((DAC_ID) == DAC_1) || \
                                                ((DAC_ID) == DAC_2))

#define IS_UART2_ID_VALID (UART2_ID)           (((UART2_ID) == U2_Rx) || \
                                                ((UART2_ID) == U2_Tx))




void InitButton(int buttonID);
void InitLED(int ledID);
void InitSwitch(int switchID);
void InitInput(int inputID);
void InitOutput(int outputID);
void InitTrimmer(int trimmerID);
void InitADC(int adcID);
void InitDAC(int dacID, int initValue);

#endif
