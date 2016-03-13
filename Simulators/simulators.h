#ifndef __SIMULATORS_H
#define __SIMULATORS_H

//Inputs
#define DOSE_SAND_INPUT                         INPUT_1
#define DOSE_GRAVEL_INPUT                       INPUT_2
#define DOSE_CEMENT_INPUT                       INPUT_3
#define DOSE_WATER_INPUT                        INPUT_4
#define EMPTY_CEMENT_INPUT                      INPUT_5
#define EMPTY_WATER_INPUT                       INPUT_6
#define OPEN_CLOSE_MIXER_INPUT                  INPUT_7
#define SKIP_CART_UP_INPUT                      INPUT_8
#define SKIP_CART_DOWN_INPUT                    INPUT_9

//Outputs
#define EMPTY_CEMENT_OUTPUT                     OUTPUT_1
#define EMPTY_WATER_OUTPUT                      OUTPUT_2        
#define OPEN_CLOSE_MIXER_OUTPUT                 OUTPUT_3
#define SKIP_CART_UP_OUTPUT                     OUTPUT_4
#define SKIP_CART_DOWN_OUTPUT                   OUTPUT_5
#define SKIP_CART_READY_OUTPUT                  OUTPUT_6
#define EMRGENCY_STOP_BUTTON_OUTPUT             OUTPUT_7
#define LOOSE_SKIP_CART_ROPE_OUTPUT             OUTPUT_8

//Timers
#define DOSE_SAND_TIMER                         TIMER_1
#define DOSE_GRAVEL_TIMER                       TIMER_2

//Simulators' constants
#define CART_WEIGHT                             650.0
#define SAND_VELOCITY                           1
#define GRAVEL_VELOCITY                         2

//Inert Scale's states
typedef enum{
    eInertScaleIdle = 0xAA,
    eInertScaleDoseSand,
    eInertScaleDoseGravel,
    eInertScaleDoseSandAndGravel,
    eInertScaleCalming,
    eInertScaleEmptying,
    eInertScaleReFilling,
    eInertScaleAlarm
}tInertScaleStates;

//



void InertScaleSimulator(void);
void InitInertScaleSimulator(void);

#endif
