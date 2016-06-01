#ifndef __SIMULATORS_H
#define __SIMULATORS_H

//Inputs (controlled signals)
#define DOSE_SAND_INPUT                         INPUT_1
#define DOSE_GRAVEL_INPUT                       INPUT_2
#define DOSE_CEMENT_INPUT                       INPUT_3
#define DOSE_WATER_INPUT                        INPUT_4
#define EMPTY_CEMENT_INPUT                      INPUT_5
#define EMPTY_WATER_INPUT                       INPUT_6
#define OPEN_CLOSE_MIXER_INPUT                  INPUT_7
#define SKIP_CART_UP_INPUT                      INPUT_8
#define SKIP_CART_DOWN_INPUT                    INPUT_9

//Outputs (sensors' signals)
#define EMPTY_CEMENT_OUTPUT                     OUTPUT_1
#define EMPTY_WATER_OUTPUT                      OUTPUT_2        
#define OPEN_CLOSE_MIXER_OUTPUT                 OUTPUT_3
#define SKIP_CART_UP_OUTPUT                     OUTPUT_4
#define SKIP_CART_DOWN_OUTPUT                   OUTPUT_5
#define SKIP_CART_READY_OUTPUT                  OUTPUT_6
#define LOOSE_SKIP_CART_ROPE_OUTPUT             OUTPUT_7
#define INERT_SCALE                             ModBusSlaves[0]
#define WATER_SCALE                             ModBusSlaves[1]
#define CONCRETE_SCALE                          DAC_1

//LEDs
#define CEMENT_VALVE_LED                        LED_1
#define WATER_VALVE_LED                         LED_2
#define MIXER_VALVE_LED                         LED_3
#define CART_IS_DOWN_LED                        LED_4
#define CART_IS_READY_LED                       LED_5
#define CART_IS_UP_LED                          LED_6
#define ALARM_LED_1                             LED_7
#define ALARM_LED_2                             LED_8

//Timers
#define DOSE_SAND_TIMER                         TIMER_1
#define DOSE_GRAVEL_TIMER                       TIMER_2
#define DOSE_CEMENT_TIMER                       TIMER_3
#define DOSE_WATER_TIMER                        TIMER_4
#define CART_TIMER                              TIMER_5
#define ALARM_TIMER                             TIMER_6

//Simulators' constants
#define CART_WEIGHT                             650
#define SAND_VELOCITY                           5
#define SAND_MAX_TAIL_LENGHT                    10                       
#define GRAVEL_VELOCITY                         10
#define GRAVEL_MAX_TAIL_LENGHT                  20
#define CEMENT_VELOCITY                         1
#define CEMENT_MAX_TAIL_LENGHT                  3
#define WATER_VELOCITY                          2
#define WATER_MAX_TAIL_LENGHT                   5
#define MAX_SEC_BETWEEN_DOWN_AND_READY_STATE    10
#define MAX_SEC_BETWEEN_UP_AND_READY_STATE      5
#define MAX_CEMENT_SCALE_VALUE                  4000
#define MAX_WATER_SCALE_VALUE                   4000
#define MAX_INERT_SCALE_VALUE                   10000

//Error codes
#define EVERYTHING_IS_OK                        0xFFFF
#define INERT_SCALE_IS_OVERFILLED               11
#define WATER_SCALE_IS_OVERFILLED               22
#define CEMENT_SCALE_IS_OVERFILLED              33
#define LOOSE_CART_ROPE                         44
#define CART_COMMAND_ERROR                      55

//Error messages


//Types - it is used inductive sensor which return logical 1 when current valve is closed and logical 0 - when it is opened
#define OPENED                                  OFF
#define CLOSED                                  ON

//Inert Scale's states
typedef enum{
    eInertScaleIdle = 0xA0,
    eInertScaleDoseSand,
    eInertScaleDoseGravel,
    eInertScaleDoseSandAndGravel,
    eInertScaleCalming,
    eInertScaleEmptying,
    eInertScaleReFilling,
    eInertScaleAlarm
}tInertScaleStates;

//Cement and water scales' states
typedef enum{
    eIdle = 0xB0,
    eDosing,
    eCalming,
    eEmptying,
    eScaleIsOverfilled
}tCementAndWaterScaleStates;

//Cart's states
typedef enum{
    eDown = 0xC0,
    eMovingDownReadyDown,
    eReady,
    eMovingReadyUpReady,
    eUp,
    eAlarm
}tCartStates;

//Mixer's states
typedef enum{
    eMixerIsClosed = 0xD0,
    eMixerIsOpened
}tMixerStates;


void InitConcreteSimulatorPeripheral(void);
void InitInertScaleSimulator(void);
void InitCementScale(void);
void InitWaterScale(void);
void InitCartSimulator(void);
void InitMixerSimulator(void);

void InertScaleSimulator(void);
void CementScaleSimulator(void);
void WaterScaleSimulator(void);
void CartSimulator(void);
void MixerSimulator(void);
void SetLEDs(void);

#endif
