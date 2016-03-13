#include "stm32f4xx.h"
#include "stdlib.h"
#include "definitions.h"
#include "userLibrary.h"
#include "VTimer.h"
#include "simulators.h"


//Inputs
unsigned char doseSandCmd;
unsigned char doseGravelCmd;
unsigned char doseCementCmd;
unsigned char doseWaterCmd;
unsigned char emptyCementCmd;
unsigned char emptyWaterCmd;
unsigned char openCloseMixerCmd;
unsigned char skipCartUpCmd;
unsigned char skipCartDownCmd;

//Outputs
unsigned char emptyCementFbk;
unsigned char emptyWaterFbk;
unsigned char openCloseMixerFbk;
unsigned char cartIsUpFbk;
unsigned char cartIsReadyFbk;
unsigned char cartIsDownFbk;
unsigned char emergencyStopBtnFbk;
unsigned char looseSkipCartRopeFbk;
float currentScaleValue, oldScaleValue;

//State Variable
tInertScaleStates inertScaleState;

//Variables
BOOL cartIsEmpty;


void InitInertScaleSimulator(void)
{
    inertScaleState = eInertScaleIdle;
}

void InertScaleSimulator(void)
{
    switch(inertScaleState)
    {
    case eInertScaleIdle:
        {
            //it is assumed cart is over the scale
            oldScaleValue = 0.0;
            currentScaleValue = 0.0;
            cartIsEmpty = TRUE;
            
            //read needed sensors
            doseSandCmd = GetDigitalInput(DOSE_SAND_INPUT);
            doseGravelCmd = GetDigitalInput(DOSE_GRAVEL_INPUT);
            
            //predicats checking for change current state
            
            //1. Start to dose but cart is not on the scale
            if(cartIsDownFbk == OFF && (doseSandCmd == ON || doseGravelCmd == ON))
            {
                inertScaleState = eInertScaleAlarm;
            }
            //2. Cart is not on the scale
            else if(cartIsDownFbk == OFF)
            {
                inertScaleState = eInertScaleEmptying;
            }
            //3. Dosing Sand And Dosing Gravel
            else if(doseSandCmd == ON && doseGravelCmd == ON)
            {
                inertScaleState = eInertScaleDoseSandAndGravel;
                SetVTimerValue(DOSE_SAND_TIMER, T_10_MS);
                SetVTimerValue(DOSE_GRAVEL_TIMER, T_10_MS);
            }
            //4. Dose Sand
            else if(doseSandCmd == ON)
            {
                inertScaleState = eInertScaleDoseSand;
                SetVTimerValue(DOSE_SAND_TIMER, T_10_MS);
            }
            //5. Dose Gravel
            else if(doseGravelCmd == ON)
            {
                inertScaleState = eInertScaleDoseGravel;
                SetVTimerValue(DOSE_GRAVEL_TIMER, T_10_MS);
            }
            break;
        }
        
    case eInertScaleDoseSand:
        {
            //Sand is filling with constant velocity.
            if(IsVTimerElapsed(DOSE_SAND_TIMER) == ELAPSED)
            {
                currentScaleValue = currentScaleValue + SAND_VELOCITY;
                oldScaleValue = currentScaleValue;
                cartIsEmpty = FALSE;
                SetVTimerValue(DOSE_SAND_TIMER, T_10_MS);
            }
            
            //read needed sensors
            doseSandCmd = GetDigitalInput(DOSE_SAND_INPUT);
            doseGravelCmd = GetDigitalInput(DOSE_GRAVEL_INPUT);
            
            //predicats checking for change current state
            
            //1. Dosing but cart is moved away from the scale
            if(cartIsDownFbk == OFF)
            {
                inertScaleState = eInertScaleAlarm;
            }
            //2. Stop dose sand
            else if(doseSandCmd == OFF)
            {
                currentScaleValue = (float)(rand() % 10); //10 = 10 * SAND_VELOCITY
                oldScaleValue = currentScaleValue;
                
                inertScaleState = eInertScaleCalming;
            }
            //3. Dose sand and gravel
            else if(doseSandCmd == ON && doseGravelCmd == ON)
            {
                SetVTimerValue(DOSE_SAND_TIMER, T_10_MS);
                SetVTimerValue(DOSE_GRAVEL_TIMER, T_10_MS);// does second timer need?????????????????????????????????does time have to be the same????????????????????
                
                inertScaleState = eInertScaleDoseSandAndGravel;
            }
            break;
        }
        
    case eInertScaleDoseGravel:
        {
            //Gravel is filling with constant velocity.
            if(IsVTimerElapsed(DOSE_GRAVEL_TIMER) == ELAPSED)
            {
                currentScaleValue = currentScaleValue + GRAVEL_VELOCITY;
                oldScaleValue = currentScaleValue;
                cartIsEmpty = FALSE;
                SetVTimerValue(DOSE_GRAVEL_TIMER, T_10_MS);
            }
            
            //read needed sensors
            doseSandCmd = GetDigitalInput(DOSE_SAND_INPUT);
            doseGravelCmd = GetDigitalInput(DOSE_GRAVEL_INPUT);
            
            //predicats checking for change current state
            
            //1. Dosing but cart is moved away from the scale
            if(cartIsDownFbk == OFF)
            {
                inertScaleState = eInertScaleAlarm;
            }
            //2. Stop dose gravel
            else if(doseGravelCmd == OFF)
            {
                currentScaleValue = (float)(rand() % 20); //20 = 10 * GRAVEL_VELOCITY
                oldScaleValue = currentScaleValue;
                
                inertScaleState = eInertScaleCalming;
            }
            //3. Dose sand and gravel
            else if(doseSandCmd == ON && doseGravelCmd == ON)
            {
                SetVTimerValue(DOSE_SAND_TIMER, T_10_MS);
                SetVTimerValue(DOSE_GRAVEL_TIMER, T_10_MS);// does second timer need?????????????????????????????????does time have to be the same????????????????????
                
                inertScaleState = eInertScaleDoseSandAndGravel;
            }
            break;
        }
        
    case eInertScaleDoseSandAndGravel:
        {
            //Sand and gravel are filling with constant velocity.
            if(IsVTimerElapsed(DOSE_SAND_TIMER) == ELAPSED)
            {
                currentScaleValue = currentScaleValue + SAND_VELOCITY;
                oldScaleValue = currentScaleValue;
                cartIsEmpty = FALSE;
                SetVTimerValue(DOSE_SAND_TIMER, T_10_MS);
            }
            
            if(IsVTimerElapsed(DOSE_GRAVEL_TIMER) == ELAPSED)
            {
                currentScaleValue = currentScaleValue + GRAVEL_VELOCITY;
                oldScaleValue = currentScaleValue;
                cartIsEmpty = FALSE;
                SetVTimerValue(DOSE_GRAVEL_TIMER, T_10_MS);
            }    
            
            //read needed sensors
            doseSandCmd = GetDigitalInput(DOSE_SAND_INPUT);
            doseGravelCmd = GetDigitalInput(DOSE_GRAVEL_INPUT);
            
            //predicats checking for change current state
            
            //1. Dosing but cart is moved away from the scale
            if(cartIsDownFbk == OFF)
            {
                inertScaleState = eInertScaleAlarm;
            }
            //2. Stop dose gravel but still dose sand
            else if(doseGravelCmd == OFF && doseSandCmd == ON)
            {
                currentScaleValue = (float)(rand() % 20); //20 = 10 * GRAVEL_VELOCITY
                oldScaleValue = currentScaleValue;
                
                SetVTimerValue(DOSE_SAND_TIMER, T_10_MS);
                
                inertScaleState = eInertScaleDoseSand;
            }
            //3. Stop dose sand but still dose gravel
            else if(doseGravelCmd == ON && doseSandCmd == OFF)
            {
                currentScaleValue = (float)(rand() % 10); //10 = 10 * SAND_VELOCITY
                oldScaleValue = currentScaleValue;
                
                SetVTimerValue(DOSE_GRAVEL_TIMER, T_10_MS);
                
                inertScaleState = eInertScaleDoseGravel;
            }
            //4. Stop dose sand and gravel
            else if(doseSandCmd == OFF && doseGravelCmd == OFF)
            {
                currentScaleValue = (float)((rand() % 10) + (rand() % 20));            
                oldScaleValue = currentScaleValue;
                
                inertScaleState = eInertScaleCalming;
            }
            break;
        }

    case eInertScaleCalming:
        {
            //Scale value is constant
            
            // read needed sensors
            doseSandCmd = GetDigitalInput(DOSE_SAND_INPUT);
            doseGravelCmd = GetDigitalInput(DOSE_GRAVEL_INPUT);
            
            //predicats checking for change current state
            
            //1. Start to dose but cart is not on the scale
            if(cartIsDownFbk == OFF && (doseSandCmd == ON || doseGravelCmd == ON))
            {
                inertScaleState = eInertScaleAlarm;
            }
            //2. Cart is not on the scale
            else if(cartIsDownFbk == OFF)
            {
                inertScaleState = eInertScaleEmptying;
            }
            //3. Dosing Sand And Dosing Gravel
            else if(doseSandCmd == ON && doseGravelCmd == ON)
            {
                inertScaleState = eInertScaleDoseSandAndGravel;
                SetVTimerValue(DOSE_SAND_TIMER, T_10_MS);
                SetVTimerValue(DOSE_GRAVEL_TIMER, T_10_MS);
            }
            //4. Dose Sand
            else if(doseSandCmd == ON)
            {
                inertScaleState = eInertScaleDoseSand;
                SetVTimerValue(DOSE_SAND_TIMER, T_10_MS);
            }
            //5. Dose Gravel
            else if(doseGravelCmd == ON)
            {
                inertScaleState = eInertScaleDoseGravel;
                SetVTimerValue(DOSE_GRAVEL_TIMER, T_10_MS);
            }
            
            break;
        }
        
    case eInertScaleEmptying:
        {
            //Cart is moved from the scale
            currentScaleValue = -CART_WEIGHT;
            
            // read needed sensors
            doseSandCmd = GetDigitalInput(DOSE_SAND_INPUT);
            doseGravelCmd = GetDigitalInput(DOSE_GRAVEL_INPUT);
            
            //predicats checking for change current state
            
            //1. Start to dose but cart is not on the scale
            if(doseSandCmd == ON || doseGravelCmd == ON)
            {
                inertScaleState = eInertScaleAlarm;
            }
            //2. Cart is down but it is not empty
            else if(cartIsDownFbk == ON && cartIsEmpty == FALSE)
            {
                inertScaleState = eInertScaleReFilling;
            }
            //3. Cart is down and it is empty
            else if(cartIsDownFbk == ON && cartIsEmpty == TRUE)
            {
                inertScaleState = eInertScaleIdle;
            }
            
            break;
        }
        
    case eInertScaleReFilling:
        {
            //Cart has not been empted and the scale is initialized with its old value
            currentScaleValue = oldScaleValue;
            
            // read needed sensors
            doseSandCmd = GetDigitalInput(DOSE_SAND_INPUT);
            doseGravelCmd = GetDigitalInput(DOSE_GRAVEL_INPUT);
            
            //predicats checking for change current state
            
            //1. Start to dose but cart is not on the scale
            if(cartIsDownFbk == OFF && (doseSandCmd == ON || doseGravelCmd == ON))
            {
                inertScaleState = eInertScaleAlarm;
            }
            //2. Cart is not on the scale
            else if(cartIsDownFbk == OFF)
            {
                inertScaleState = eInertScaleEmptying;
            }
            //3. Dosing Sand And Dosing Gravel
            else if(doseSandCmd == ON && doseGravelCmd == ON)
            {
                inertScaleState = eInertScaleDoseSandAndGravel;
                SetVTimerValue(DOSE_SAND_TIMER, T_10_MS);
                SetVTimerValue(DOSE_GRAVEL_TIMER, T_10_MS);
            }
            //4. Dose Sand
            else if(doseSandCmd == ON)
            {
                inertScaleState = eInertScaleDoseSand;
                SetVTimerValue(DOSE_SAND_TIMER, T_10_MS);
            }
            //5. Dose Gravel
            else if(doseGravelCmd == ON)
            {
                inertScaleState = eInertScaleDoseGravel;
                SetVTimerValue(DOSE_GRAVEL_TIMER, T_10_MS);
            }
            
            break;
        }
        
    case eInertScaleAlarm:
        //Print message to display and wait for restart simulator
        break;
    }
}
