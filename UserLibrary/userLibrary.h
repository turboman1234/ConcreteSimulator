#ifndef __USERLIBRARY_H
#define __USERLIBRARY_H

int GetButtonState(int buttonNumber);
int GetDigitalInput(int inputNumber);
int GetSwitchState(int switchNumber);
int GetLEDState(int ledID);
int GetOutputState(int outputID);
int GetTrimmerValue(int trimmerNumber);
int GetAnalogInput(int adcNumber);
int GetAnalogOutput(int dacNumber);
void SetDigitalOutput(int outputID, int state);
void SetLED(int ledID, int state);
void SetAnalogOutput(int dacOutput, int value);

#endif
