#include <stdio.h>
#include "stm32f4xx.h"
#include "stdlib.h"
#include "rcc.h"
#include "initPeripheral.h"
#include "definitions.h"
#include "userLibrary.h"
#include "VTimer.h"
#include "mbslave.h"
#include "LCD.h"
#include "simulators.h"


extern ModBusSlaveUnit ModBusSlaves[MAX_MODBUS_SLAVE_DEVICES];
char SimulatorStatusMessage[21] = {'O', 'k', '\0'};

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
unsigned char emptyCementValve;
unsigned char emptyWaterValve;
unsigned char openCloseMixerFbk;
unsigned char cartIsUpFbk;
unsigned char cartIsReadyFbk;
unsigned char cartIsDownFbk;
unsigned char looseSkipCartRopeFbk;
extern short currentScaleValue = 0, oldScaleValue = 0, cementScaleValue = 0, waterScaleValue = 0;

//State Variable
tInertScaleStates inertScaleState;
tCementAndWaterScaleStates cementScaleState;
tCementAndWaterScaleStates waterScaleState;
tCartStates cartState;
tMixerStates mixerState;

//Variables
BOOL cartIsEmpty;
BOOL cartIsInSteadyState;
unsigned char secondsBetweenStates;
unsigned char lastMixerCmd;
unsigned char alarmLedState = OFF;
unsigned short inertScaleAlarmIdentifier = EVERYTHING_IS_OK;

void InitConcreteSimulatorPeripheral(void)
{
    InitRCC();
    InitVTimers();
    
    //Init Inputs
    InitInput(DOSE_SAND_INPUT);
    InitInput(DOSE_GRAVEL_INPUT);
    InitInput(DOSE_CEMENT_INPUT);
    InitInput(DOSE_WATER_INPUT);
    InitInput(EMPTY_CEMENT_INPUT);
    InitInput(EMPTY_WATER_INPUT);
    InitInput(OPEN_CLOSE_MIXER_INPUT);
    InitInput(SKIP_CART_UP_INPUT);
    InitInput(SKIP_CART_DOWN_INPUT);
        
    //Init Outputs
    InitOutput(EMPTY_CEMENT_OUTPUT);
    InitOutput(EMPTY_WATER_OUTPUT);
    InitOutput(OPEN_CLOSE_MIXER_OUTPUT);
    InitOutput(SKIP_CART_UP_OUTPUT);
    InitOutput(SKIP_CART_DOWN_OUTPUT);
    InitOutput(SKIP_CART_READY_OUTPUT);
    InitOutput(LOOSE_SKIP_CART_ROPE_OUTPUT);
    InitDAC(CONCRETE_SCALE, 0);
    
    //Init LEDs
    InitLED(CEMENT_VALVE_LED);
    InitLED(WATER_VALVE_LED);
    InitLED(MIXER_VALVE_LED);
    InitLED(CART_IS_DOWN_LED);
    InitLED(CART_IS_READY_LED);
    InitLED(CART_IS_UP_LED);
    InitLED(ALARM_LED_1);
    InitLED(ALARM_LED_2);
    
    InitNewMBSlaveDevices();
    MBInitHardwareAndProtocol();
    
}

void InitCementScale(void)
{
    emptyCementValve = CLOSED;
    SetDigitalOutput(EMPTY_CEMENT_OUTPUT, emptyCementValve);
    cementScaleState = eIdle;
}

void InitWaterScale(void)
{
    emptyWaterValve = CLOSED;
    SetDigitalOutput(EMPTY_WATER_OUTPUT, emptyWaterValve);
    waterScaleState = eIdle;
}

void InitCartSimulator(void)
{
    cartIsDownFbk = ON;
    cartIsReadyFbk = OFF;
    cartIsUpFbk = OFF;
    
    SetDigitalOutput(SKIP_CART_DOWN_OUTPUT, cartIsDownFbk);
    SetDigitalOutput(SKIP_CART_READY_OUTPUT, cartIsReadyFbk);
    SetDigitalOutput(SKIP_CART_UP_OUTPUT, cartIsUpFbk);
    
    cartIsEmpty = TRUE;
    
    cartState = eDown;
}

void InitMixerSimulator(void)
{
    mixerState = eMixerIsClosed;

    lastMixerCmd = OFF;    
    openCloseMixerFbk = CLOSED;
    SetDigitalOutput(OPEN_CLOSE_MIXER_OUTPUT, openCloseMixerFbk);
}

void InitInertScaleSimulator(void)
{
    inertScaleState = eInertScaleIdle;
    
    looseSkipCartRopeFbk = OFF;
    SetDigitalOutput(LOOSE_SKIP_CART_ROPE_OUTPUT, looseSkipCartRopeFbk);
}



void InertScaleSimulator(void)
{
    switch(inertScaleState)
    {
    case eInertScaleIdle:
        {
            //it is assumed cart is over the scale
            oldScaleValue = 0;
            currentScaleValue = 0;
            
            //read needed sensors
            doseSandCmd = GetDigitalInput(DOSE_SAND_INPUT);
            doseGravelCmd = GetDigitalInput(DOSE_GRAVEL_INPUT);
            
            //predicats checking for change current state
            
            //1. Start to dose but cart is not on the scale
            if(cartIsDownFbk == OFF && (doseSandCmd == ON || doseGravelCmd == ON))
            {
                inertScaleState = eInertScaleAlarm;
                inertScaleAlarmIdentifier = LOOSE_CART_ROPE;
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
                SetVTimerValue(DOSE_SAND_TIMER, T_500_MS);
                SetVTimerValue(DOSE_GRAVEL_TIMER, T_500_MS);
            }
            //4. Dose Sand
            else if(doseSandCmd == ON)
            {
                inertScaleState = eInertScaleDoseSand;
                SetVTimerValue(DOSE_SAND_TIMER, T_500_MS);
            }
            //5. Dose Gravel
            else if(doseGravelCmd == ON)
            {
                inertScaleState = eInertScaleDoseGravel;
                SetVTimerValue(DOSE_GRAVEL_TIMER, T_500_MS);
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
                SetVTimerValue(DOSE_SAND_TIMER, T_500_MS);
            }
            
            //read needed sensors
            doseSandCmd = GetDigitalInput(DOSE_SAND_INPUT);
            doseGravelCmd = GetDigitalInput(DOSE_GRAVEL_INPUT);
            
            //predicats checking for change current state
            
            //1. Dosing but cart is moved away from the scale
            if(cartIsDownFbk == OFF)
            {
                inertScaleState = eInertScaleAlarm;
                inertScaleAlarmIdentifier = LOOSE_CART_ROPE;
            }
            //2. Inert Scale is overfilled
            else if(currentScaleValue > MAX_INERT_SCALE_VALUE)
            {
                inertScaleState = eInertScaleAlarm;
                inertScaleAlarmIdentifier = INERT_SCALE_IS_OVERFILLED;   
            }
            //3. Stop dose sand
            else if(doseSandCmd == OFF)
            {
                currentScaleValue = currentScaleValue + (rand() % SAND_MAX_TAIL_LENGHT);
                oldScaleValue = currentScaleValue;
                
                inertScaleState = eInertScaleCalming;
            }
            //4. Dose sand and gravel
            else if(doseSandCmd == ON && doseGravelCmd == ON)
            {
                SetVTimerValue(DOSE_SAND_TIMER, T_500_MS);
                SetVTimerValue(DOSE_GRAVEL_TIMER, T_500_MS);// does second timer need?????????????????????????????????does time have to be the same????????????????????
                
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
                SetVTimerValue(DOSE_GRAVEL_TIMER, T_500_MS);
            }
            
            //read needed sensors
            doseSandCmd = GetDigitalInput(DOSE_SAND_INPUT);
            doseGravelCmd = GetDigitalInput(DOSE_GRAVEL_INPUT);
            
            //predicats checking for change current state
            
            //1. Dosing but cart is moved away from the scale
            if(cartIsDownFbk == OFF)
            {
                inertScaleState = eInertScaleAlarm;
                inertScaleAlarmIdentifier = LOOSE_CART_ROPE;
            }
            //2. Inert Scale is overfilled
            else if(currentScaleValue > MAX_INERT_SCALE_VALUE)
            {
                inertScaleState = eInertScaleAlarm;
                inertScaleAlarmIdentifier = INERT_SCALE_IS_OVERFILLED;   
            }
            //3. Stop dose gravel
            else if(doseGravelCmd == OFF)
            {
                currentScaleValue = currentScaleValue + (rand() % GRAVEL_MAX_TAIL_LENGHT);
                oldScaleValue = currentScaleValue;
                
                inertScaleState = eInertScaleCalming;
            }
            //4. Dose sand and gravel
            else if(doseSandCmd == ON && doseGravelCmd == ON)
            {
                SetVTimerValue(DOSE_SAND_TIMER, T_500_MS);
                SetVTimerValue(DOSE_GRAVEL_TIMER, T_500_MS);// does second timer need?????????????????????????????????does time have to be the same????????????????????
                
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
                SetVTimerValue(DOSE_SAND_TIMER, T_500_MS);
            }
            
            if(IsVTimerElapsed(DOSE_GRAVEL_TIMER) == ELAPSED)
            {
                currentScaleValue = currentScaleValue + GRAVEL_VELOCITY;
                oldScaleValue = currentScaleValue;
                cartIsEmpty = FALSE;
                SetVTimerValue(DOSE_GRAVEL_TIMER, T_500_MS);
            }    
            
            //read needed sensors
            doseSandCmd = GetDigitalInput(DOSE_SAND_INPUT);
            doseGravelCmd = GetDigitalInput(DOSE_GRAVEL_INPUT);
            
            //predicats checking for change current state
            
            //1. Dosing but cart is moved away from the scale
            if(cartIsDownFbk == OFF)
            {
                inertScaleState = eInertScaleAlarm;
                inertScaleAlarmIdentifier = LOOSE_CART_ROPE;
            }
            //2. Inert Scale is overfilled
            else if(currentScaleValue > MAX_INERT_SCALE_VALUE)
            {
                inertScaleState = eInertScaleAlarm;
                inertScaleAlarmIdentifier = INERT_SCALE_IS_OVERFILLED;   
            }
            //3. Stop dose gravel but still dose sand
            else if(doseGravelCmd == OFF && doseSandCmd == ON)
            {
                currentScaleValue = currentScaleValue + (rand() % GRAVEL_MAX_TAIL_LENGHT);
                oldScaleValue = currentScaleValue;
                
                SetVTimerValue(DOSE_SAND_TIMER, T_500_MS);
                
                inertScaleState = eInertScaleDoseSand;
            }
            //4. Stop dose sand but still dose gravel
            else if(doseGravelCmd == ON && doseSandCmd == OFF)
            {
                currentScaleValue = currentScaleValue + (rand() % SAND_MAX_TAIL_LENGHT);
                oldScaleValue = currentScaleValue;
                
                SetVTimerValue(DOSE_GRAVEL_TIMER, T_500_MS);
                
                inertScaleState = eInertScaleDoseGravel;
            }
            //5. Stop dose sand and gravel
            else if(doseSandCmd == OFF && doseGravelCmd == OFF)
            {
                currentScaleValue = currentScaleValue + (rand() % SAND_MAX_TAIL_LENGHT) + (rand() % GRAVEL_MAX_TAIL_LENGHT);            
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
                inertScaleAlarmIdentifier = LOOSE_CART_ROPE;
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
                SetVTimerValue(DOSE_SAND_TIMER, T_500_MS);
                SetVTimerValue(DOSE_GRAVEL_TIMER, T_500_MS);
            }
            //4. Dose Sand
            else if(doseSandCmd == ON)
            {
                inertScaleState = eInertScaleDoseSand;
                SetVTimerValue(DOSE_SAND_TIMER, T_500_MS);
            }
            //5. Dose Gravel
            else if(doseGravelCmd == ON)
            {
                inertScaleState = eInertScaleDoseGravel;
                SetVTimerValue(DOSE_GRAVEL_TIMER, T_500_MS);
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
                inertScaleAlarmIdentifier = LOOSE_CART_ROPE;
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
                inertScaleAlarmIdentifier = LOOSE_CART_ROPE;
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
                SetVTimerValue(DOSE_SAND_TIMER, T_500_MS);
                SetVTimerValue(DOSE_GRAVEL_TIMER, T_500_MS);
            }
            //4. Dose Sand
            else if(doseSandCmd == ON)
            {
                inertScaleState = eInertScaleDoseSand;
                SetVTimerValue(DOSE_SAND_TIMER, T_500_MS);
            }
            //5. Dose Gravel
            else if(doseGravelCmd == ON)
            {
                inertScaleState = eInertScaleDoseGravel;
                SetVTimerValue(DOSE_GRAVEL_TIMER, T_500_MS);
            }
            
            break;
        }
        
    case eInertScaleAlarm:
        {
            //Print message to display and wait for restart simulator
            if(inertScaleAlarmIdentifier == LOOSE_CART_ROPE)
            {
                looseSkipCartRopeFbk = ON;
                SetDigitalOutput(LOOSE_SKIP_CART_ROPE_OUTPUT, looseSkipCartRopeFbk);
                
                SetVTimerValue(ALARM_TIMER, T_500_MS);
                SetLED(ALARM_LED_1, alarmLedState);
                SetLED(ALARM_LED_2, alarmLedState);
                
                sprintf(SimulatorStatusMessage, "Sim. status: %5s%2d", "ALARM", LOOSE_CART_ROPE);
                LCDsetCursor(0, 3); 
                LCDprint(SimulatorStatusMessage);
            }
            else if(inertScaleAlarmIdentifier == INERT_SCALE_IS_OVERFILLED)
            {
                SetLED(ALARM_LED_1, ON);
                SetLED(ALARM_LED_2, ON);
                
                sprintf(SimulatorStatusMessage, "Sim. status: %5s%2d", "ALARM", INERT_SCALE_IS_OVERFILLED);
                LCDsetCursor(0, 3); 
                LCDprint(SimulatorStatusMessage);
            }            

            while(1)
            {
                if(inertScaleAlarmIdentifier == LOOSE_CART_ROPE && IsVTimerElapsed(ALARM_TIMER) == ELAPSED)
                {
                    alarmLedState = !alarmLedState;
                    SetLED(ALARM_LED_1, alarmLedState);
                    SetLED(ALARM_LED_2, alarmLedState);
                    SetVTimerValue(ALARM_TIMER, T_500_MS);
                }
            }
            
            break;
        }
    }
    
    INERT_SCALE.holdingRegisters[0] = currentScaleValue;
}


void CementScaleSimulator(void)
{
    switch(cementScaleState)
    {
    case eIdle:
        {
            cementScaleValue = 0;

            //set feedback sensor value
            emptyCementValve = CLOSED;
            SetDigitalOutput(EMPTY_CEMENT_OUTPUT, emptyCementValve);            

            //read needed sensors
            doseCementCmd = GetDigitalInput(DOSE_CEMENT_INPUT);
            emptyCementCmd = GetDigitalInput(EMPTY_CEMENT_INPUT);        
            
            //check predicates
            
            //1. If cement valve is opened, cement scale could not work properly (it will not dose cement)
            if(emptyCementCmd == ON)
            {
                cementScaleState = eEmptying;
            }
            //2. Dose cement 
            else if(doseCementCmd == ON)
            {
                cementScaleState = eDosing;
                SetVTimerValue(DOSE_CEMENT_TIMER, T_500_MS);
            }
            break;
        }
    case eDosing:
        {
            if(IsVTimerElapsed(DOSE_CEMENT_TIMER) == ELAPSED)
            {
                cementScaleValue = cementScaleValue + CEMENT_VELOCITY;
                SetVTimerValue(DOSE_CEMENT_TIMER, T_500_MS);
            }

            //set feedback sensor value
            emptyCementValve = CLOSED;
            SetDigitalOutput(EMPTY_CEMENT_OUTPUT, emptyCementValve);
            
            //read needed sensors
            doseCementCmd = GetDigitalInput(DOSE_CEMENT_INPUT);
            emptyCementCmd = GetDigitalInput(EMPTY_CEMENT_INPUT);
            
            //check predicates
            
            //1. If cement valve is opened, cement scale could not work properly (it will not dose cement)
            if(emptyCementCmd == ON)
            {
                cementScaleState = eEmptying;
            }
            //2. If cement scale is overfilled
            else if(cementScaleValue > MAX_CEMENT_SCALE_VALUE)
            {
                cementScaleState = eScaleIsOverfilled;
            }
            //3. Stop cement dosing
            else if(doseCementCmd == OFF)
            {
                cementScaleState = eCalming;
                
                cementScaleValue = cementScaleValue + (rand() % CEMENT_MAX_TAIL_LENGHT); 
            }
            
            break;
        }
    case eCalming:
        {
            //cement scale value is constant
            
            //set feedback sensor value
            emptyCementValve = CLOSED;
            SetDigitalOutput(EMPTY_CEMENT_OUTPUT, emptyCementValve);
            
            //read needed sensors
            doseCementCmd = GetDigitalInput(DOSE_CEMENT_INPUT);
            emptyCementCmd = GetDigitalInput(EMPTY_CEMENT_INPUT);
            
            //check predicates
            
            //1. If cement valve is opened, dosing quantity falls 
            if(emptyCementCmd == ON)
            {
                cementScaleState = eEmptying;
            }
            //2.Dose cement
            else if(doseCementCmd == ON)
            {
                cementScaleState = eDosing;
                
                SetVTimerValue(DOSE_CEMENT_TIMER, T_500_MS);
            }            
            
            break;
        }
    case eEmptying:
        {
            //cement scale is emptying
            cementScaleValue = 0;
            
            //set feedback sensor value
            emptyCementValve = OPENED;
            SetDigitalOutput(EMPTY_CEMENT_OUTPUT, emptyCementValve);
            
            //read needed sensors
            emptyCementCmd = GetDigitalInput(EMPTY_CEMENT_INPUT);
            
            //check predicates
            
            //1. If cement valve is opened, dosing quantity falls 
            if(emptyCementCmd == OFF)
            {
                cementScaleState = eIdle;
            }
            
            break;
        }
    case eScaleIsOverfilled:
        {
            SetLED(ALARM_LED_1, ON);
            SetLED(ALARM_LED_2, OFF); 
            
            sprintf(SimulatorStatusMessage, "Sim. status: %5s%2d", "ALARM", CEMENT_SCALE_IS_OVERFILLED);
            LCDsetCursor(0, 3); 
            LCDprint(SimulatorStatusMessage);
            
            while(1);        

        }
    }
    
    SetAnalogOutput(CONCRETE_SCALE, cementScaleValue);
}

void WaterScaleSimulator(void)
{
    switch(waterScaleState)
    {
    case eIdle:
        {
            waterScaleValue = 0;

            //set feedback sensor value
            emptyWaterValve = CLOSED;
            SetDigitalOutput(EMPTY_WATER_OUTPUT, emptyWaterValve);            

            //read needed sensors
            doseWaterCmd = GetDigitalInput(DOSE_WATER_INPUT);
            emptyWaterCmd = GetDigitalInput(EMPTY_WATER_INPUT);        
            
            //check predicates
            
            //1. If water valve is opened, water scale could not work properly (it will not dose water)
            if(emptyWaterCmd == ON)
            {
                waterScaleState = eEmptying;
            }
            //2. Dose water 
            else if(doseWaterCmd == ON)
            {
                waterScaleState = eDosing;
                SetVTimerValue(DOSE_WATER_TIMER, T_500_MS);
            }
            break;
        }
    case eDosing:
        {
            if(IsVTimerElapsed(DOSE_WATER_TIMER) == ELAPSED)
            {
                waterScaleValue = waterScaleValue + WATER_VELOCITY;
                SetVTimerValue(DOSE_WATER_TIMER, T_500_MS);
            }

            //set feedback sensor value
            emptyWaterValve = CLOSED;
            SetDigitalOutput(EMPTY_WATER_OUTPUT, emptyWaterValve);            
            
            //read needed sensors
            doseWaterCmd = GetDigitalInput(DOSE_WATER_INPUT);
            emptyWaterCmd = GetDigitalInput(EMPTY_WATER_INPUT);        
                        
            //check predicates
            
            //1. If water valve is opened, water scale could not work properly (it will not dose water)
            if(emptyWaterCmd == ON)
            {
                waterScaleState = eEmptying;
            }
            //2. If water scale is overfilled
            else if(waterScaleValue > MAX_WATER_SCALE_VALUE)
            {
                waterScaleState = eScaleIsOverfilled;
            }
            //2. Stop water dosing
            else if(doseWaterCmd == OFF)
            {
                waterScaleState = eCalming;
                
                waterScaleValue = waterScaleValue + rand() % WATER_MAX_TAIL_LENGHT;
            }
            
            break;
        }
    case eCalming:
        {
            //water scale value is constant
            
            //set feedback sensor value
            emptyWaterValve = CLOSED;
            SetDigitalOutput(EMPTY_WATER_OUTPUT, emptyWaterValve);            
            
            //read needed sensors
            doseWaterCmd = GetDigitalInput(DOSE_WATER_INPUT);
            emptyWaterCmd = GetDigitalInput(EMPTY_WATER_INPUT);        

            //check predicates
            
            //1. If water valve is opened, dosing quantity falls 
            if(emptyWaterCmd == ON)
            {
                waterScaleState = eEmptying;
            }
            //2.Dose water
            else if(doseWaterCmd == ON)
            {
                waterScaleState = eDosing;
                
                SetVTimerValue(DOSE_WATER_TIMER, T_500_MS);
            }            
            
            break;
        }
    case eEmptying:
        {
            //water scale is emptying
            waterScaleValue = 0;
            
            //set feedback sensor value
            emptyWaterValve = OPENED;
            SetDigitalOutput(EMPTY_WATER_OUTPUT, emptyWaterValve);
            
            //read needed sensors
            emptyWaterCmd = GetDigitalInput(EMPTY_WATER_INPUT);
            
            //check predicates
            
            //1. If water valve is opened, dosing quantity falls 
            if(emptyWaterCmd == OFF)
            {
                waterScaleState = eIdle;
            }
            
            break;
        }
    case eScaleIsOverfilled:
        {
            SetLED(ALARM_LED_1, OFF);
            SetLED(ALARM_LED_2, ON);
            
            sprintf(SimulatorStatusMessage, "Sim. status: %5s%2d", "ALARM", WATER_SCALE_IS_OVERFILLED);
            LCDsetCursor(0, 3); 
            LCDprint(SimulatorStatusMessage);
            
            while(1);
        }
    }
    
    WATER_SCALE.holdingRegisters[0] = waterScaleValue;
}

void CartSimulator(void)
{    
    switch(cartState)
    {
    case eDown:
        {
            //set sensors    
            cartIsDownFbk = ON;
            cartIsReadyFbk = OFF;
            cartIsUpFbk = OFF;
            
            SetDigitalOutput(SKIP_CART_DOWN_OUTPUT, cartIsDownFbk);
            SetDigitalOutput(SKIP_CART_READY_OUTPUT, cartIsReadyFbk);
            SetDigitalOutput(SKIP_CART_UP_OUTPUT, cartIsUpFbk);
            
            cartIsInSteadyState = TRUE;
            
            skipCartUpCmd = GetDigitalInput(SKIP_CART_UP_INPUT);
            skipCartDownCmd = GetDigitalInput(SKIP_CART_DOWN_INPUT);
            
            if(skipCartUpCmd == ON && skipCartDownCmd == ON)
            {
                cartState = eAlarm;
            }
            else if(skipCartUpCmd == ON)
            {
                SetVTimerValue(CART_TIMER, T_1_S);
                secondsBetweenStates = MAX_SEC_BETWEEN_DOWN_AND_READY_STATE; //because in moving state it will decrease to 0
                cartState = eMovingDownReadyDown;
            }
            
            break;
        }
    case eMovingDownReadyDown:
        {
            //set sensors           
            cartIsDownFbk = OFF;
            cartIsReadyFbk = OFF;
            cartIsUpFbk = OFF;
            
            SetDigitalOutput(SKIP_CART_DOWN_OUTPUT, cartIsDownFbk);
            SetDigitalOutput(SKIP_CART_READY_OUTPUT, cartIsReadyFbk);
            SetDigitalOutput(SKIP_CART_UP_OUTPUT, cartIsUpFbk);
            
            cartIsInSteadyState = FALSE;
            
            //read input commands
            skipCartUpCmd = GetDigitalInput(SKIP_CART_UP_INPUT);
            skipCartDownCmd = GetDigitalInput(SKIP_CART_DOWN_INPUT);
            
            if(skipCartUpCmd == ON && skipCartDownCmd == ON)
            {
                cartState = eAlarm;
            }
            else if(skipCartUpCmd == ON)
            {
                if(IsVTimerElapsed(CART_TIMER) == ELAPSED)
                {
                    if(secondsBetweenStates > 0)
                    {
                        secondsBetweenStates = secondsBetweenStates - 1;
                        SetVTimerValue(CART_TIMER, T_1_S);
                    }
                    else
                    {
                        cartState = eReady;
                    }
                }
            }
            else if(skipCartDownCmd == ON)
            {
                if(IsVTimerElapsed(CART_TIMER) == ELAPSED)
                {
                    if(secondsBetweenStates < MAX_SEC_BETWEEN_DOWN_AND_READY_STATE)
                    {
                        secondsBetweenStates = secondsBetweenStates + 1;
                        SetVTimerValue(CART_TIMER, T_1_S);
                    }
                    else
                    {
                        cartState = eDown;
                    }
                }
            }
            
            break;
        }
    case eReady:
        {   
            //set sensors   
            cartIsDownFbk = OFF;
            cartIsReadyFbk = ON;
            cartIsUpFbk = OFF;
            
            SetDigitalOutput(SKIP_CART_DOWN_OUTPUT, cartIsDownFbk);
            SetDigitalOutput(SKIP_CART_READY_OUTPUT, cartIsReadyFbk);
            SetDigitalOutput(SKIP_CART_UP_OUTPUT, cartIsUpFbk);
            
            if(cartIsInSteadyState == FALSE)
            {
                cartIsInSteadyState = TRUE;
                SetVTimerValue(CART_TIMER, T_1_S);
                //simulate cart's and sensor's inertion and wait for controller reaction
                while(IsVTimerElapsed(CART_TIMER) == NOT_ELAPSED);
            }
            
            //read input commands
            skipCartUpCmd = GetDigitalInput(SKIP_CART_UP_INPUT);
            skipCartDownCmd = GetDigitalInput(SKIP_CART_DOWN_INPUT);
            
            if(skipCartUpCmd == ON && skipCartDownCmd == ON)
            {
                cartState = eAlarm;
            }
            else if(skipCartUpCmd == ON)
            {
                cartState = eMovingReadyUpReady;
                SetVTimerValue(CART_TIMER, T_1_S);
                secondsBetweenStates = MAX_SEC_BETWEEN_UP_AND_READY_STATE; // because in moving state it will decrease to 0
            }
            else if(skipCartDownCmd == ON)
            {
                cartState = eMovingDownReadyDown;
                SetVTimerValue(CART_TIMER, T_1_S);
                secondsBetweenStates = 0; //because in moving state it will increase to Max Seconds Constant
            }
            
            break;
        }
    case eMovingReadyUpReady:
        {
            //set sensors 
            cartIsDownFbk = OFF;
            cartIsReadyFbk = OFF;
            cartIsUpFbk = OFF;
            
            SetDigitalOutput(SKIP_CART_DOWN_OUTPUT, cartIsDownFbk);
            SetDigitalOutput(SKIP_CART_READY_OUTPUT, cartIsReadyFbk);
            SetDigitalOutput(SKIP_CART_UP_OUTPUT, cartIsUpFbk);
            
            cartIsInSteadyState = FALSE;
            
            //read input commands
            skipCartUpCmd = GetDigitalInput(SKIP_CART_UP_INPUT);
            skipCartDownCmd = GetDigitalInput(SKIP_CART_DOWN_INPUT);
            
            if(skipCartUpCmd == ON && skipCartDownCmd == ON)
            {
                cartState = eAlarm;
            }
            else if(skipCartUpCmd == ON)
            {
                if(IsVTimerElapsed(CART_TIMER) == ELAPSED)
                {
                    if(secondsBetweenStates > 0)
                    {
                        secondsBetweenStates = secondsBetweenStates - 1;
                        SetVTimerValue(CART_TIMER, T_1_S);
                    }
                    else
                    {
                        cartState = eUp;
                    }
                }
            }
            else if(skipCartDownCmd == ON)
            {
                if(IsVTimerElapsed(CART_TIMER) == ELAPSED)
                {
                    if(secondsBetweenStates < MAX_SEC_BETWEEN_UP_AND_READY_STATE)
                    {
                        secondsBetweenStates = secondsBetweenStates + 1;
                        SetVTimerValue(CART_TIMER, T_1_S);
                    }
                    else
                    {
                        cartState = eReady;
                    }
                }
            }
            break;
        }
    case eUp:
        {
            //set sensors
            cartIsDownFbk = OFF;
            cartIsReadyFbk = OFF;
            cartIsUpFbk = ON;
            
            SetDigitalOutput(SKIP_CART_DOWN_OUTPUT, cartIsDownFbk);
            SetDigitalOutput(SKIP_CART_READY_OUTPUT, cartIsReadyFbk);
            SetDigitalOutput(SKIP_CART_UP_OUTPUT, cartIsUpFbk);
            
            cartIsEmpty = TRUE;
            cartIsInSteadyState = TRUE;
            
            //read input commands
            skipCartUpCmd = GetDigitalInput(SKIP_CART_UP_INPUT);
            skipCartDownCmd = GetDigitalInput(SKIP_CART_DOWN_INPUT);
            
            if(skipCartUpCmd == ON && skipCartDownCmd == ON)
            {
                cartState = eAlarm;
            }
            else if(skipCartDownCmd == ON)
            {
                cartState = eMovingReadyUpReady;
                SetVTimerValue(CART_TIMER, T_1_S);
                secondsBetweenStates = 0; //because in moving state it will increase to Max Seconds Constant
            }
            
            break;
        }
    case eAlarm:
        {
            //must wait until reset system
            SetVTimerValue(ALARM_TIMER, T_1_S);
            SetLED(ALARM_LED_1, alarmLedState);
            SetLED(ALARM_LED_2, alarmLedState);
            
            sprintf(SimulatorStatusMessage, "Sim. status: %5s%2d", "ALARM", CART_COMMAND_ERROR);
            LCDsetCursor(0, 3); 
            LCDprint(SimulatorStatusMessage);
            
            while(1)
            {
                if(IsVTimerElapsed(ALARM_TIMER) == ELAPSED)
                {
                    alarmLedState = !alarmLedState;
                    SetLED(ALARM_LED_1, alarmLedState);
                    SetLED(ALARM_LED_2, alarmLedState);
                    
                    SetVTimerValue(ALARM_TIMER, T_1_S);
                }
            }
            break;
        }
    }
}

void MixerSimulator(void)
{
    switch(mixerState)
    {
    case eMixerIsClosed:
        {
            openCloseMixerFbk = CLOSED;
            SetDigitalOutput(OPEN_CLOSE_MIXER_OUTPUT, openCloseMixerFbk);
            
            openCloseMixerCmd = GetDigitalInput(OPEN_CLOSE_MIXER_INPUT);
            
            if(openCloseMixerCmd == OFF && lastMixerCmd == ON)
            {
                lastMixerCmd = OFF;
            }          
            else if(openCloseMixerCmd == ON && lastMixerCmd == OFF)
            {
                lastMixerCmd = ON;
                mixerState = eMixerIsOpened;
            }
            
            break;
        }
    case eMixerIsOpened:
        {
            openCloseMixerFbk = OPENED;
            SetDigitalOutput(OPEN_CLOSE_MIXER_OUTPUT, openCloseMixerFbk);
            
            openCloseMixerCmd = GetDigitalInput(OPEN_CLOSE_MIXER_INPUT);
            
            if(openCloseMixerCmd == OFF && lastMixerCmd == ON)
            {
                lastMixerCmd = OFF;
            }          
            else if(openCloseMixerCmd == ON && lastMixerCmd == OFF)
            {
                lastMixerCmd = ON;
                mixerState = eMixerIsClosed;
            }
            break;
        }
    }
}

void SetLEDs(void)
{
    SetLED(CEMENT_VALVE_LED, emptyCementValve);
    SetLED(WATER_VALVE_LED, emptyWaterValve);
    SetLED(MIXER_VALVE_LED, openCloseMixerFbk);
    SetLED(CART_IS_DOWN_LED, cartIsDownFbk);
    SetLED(CART_IS_READY_LED, cartIsReadyFbk);
    SetLED(CART_IS_UP_LED, cartIsUpFbk);
}

