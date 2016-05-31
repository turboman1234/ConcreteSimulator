#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"
#include "system_stm32f4xx.h"
#include "definitions.h"
#include "rcc.h"
#include "initPeripheral.h"
#include "userLibrary.h"
#include "VTimer.h"
#include "usart.h"
#include "mytim.h"
#include "serial.h"
#include "mbslave.h"
#include "rs232.h"
#include "simulators.h"
#include "LCD.h"

extern RCC_ClocksTypeDef MYCLOCKS;
extern ModBusSlaveUnit ModBusSlaves[MAX_MODBUS_SLAVE_DEVICES];


//Test Finctions
//void TestInputsAndLeds(void);
//void InitInputsAndLEDs(void);
//

int main()
{   
    InitRCC();
    InitNewMBSlaveDevices();
    MBInitHardwareAndProtocol();
    InitVTimers();
    
    InitConcreteSimulatorPeripheral();
    InitInertScaleSimulator();
    InitCementScale();
    InitWaterScale();
    InitCartSimulator();
    InitMixerSimulator();
    InitLCD();
    
//    InitInputsAndLEDs();    
    
    while(1)
    {
        CartSimulator();
        InertScaleSimulator();
        CementScaleSimulator();
        WaterScaleSimulator();
        MixerSimulator();
        SetLEDs();
        MBPollSlave();
        MB_slave_transmit();
        
        if(IsVTimerElapsed(LCD_REFRESH_TIMER) == ELAPSED)
        {
//            LCDprint("Text1Text2Text3Text4Text5Text6Text7Txet8Text9Text0abcdefghijklmnopqrstuvwxyz!@#$");
            LCDprint((const uint8_t *)"Cement scale: 123.45Water scale: 456.78 Inert scale: 1111.25Mix time rest: 4h23m");
            SetVTimerValue(LCD_REFRESH_TIMER, 10000);
        }
        
//        TestInputsAndLeds();
    }
    
    
}

//int led1, led2, led3, led4, led5, led6, led7, led8;
//
//int i1, i2, i3, i4, i5, i6, i7, i8, i9;
//
//void TestInputsAndLeds(void)
//{
//    i1 = GetDigitalInput(INPUT_9);
//    i2 = GetDigitalInput(INPUT_10);
//    i3 = GetDigitalInput(INPUT_11);
//    i4 = GetDigitalInput(INPUT_12);
//    i5 = GetDigitalInput(INPUT_13);
//    i6 = GetDigitalInput(INPUT_14);
//    i7 = GetDigitalInput(INPUT_15);
//    i8 = GetDigitalInput(INPUT_16);
//    
//    SetLED(LED_1, i1);
//    SetLED(LED_2, i2);
//    SetLED(LED_3, i3);
//    SetLED(LED_4, i4);
//    SetLED(LED_5, i5);
//    SetLED(LED_6, i6);
//    SetLED(LED_7, i7);
//    SetLED(LED_8, i8);    
//}
//
//void InitInputsAndLEDs(void)
//{
//    InitLED(LED_1);
//    InitLED(LED_2);
//    InitLED(LED_3);
//    InitLED(LED_4);
//    InitLED(LED_5);
//    InitLED(LED_6);
//    InitLED(LED_7);
//    InitLED(LED_8);
//    
//    InitInput(INPUT_1);
//    InitInput(INPUT_2);
//    InitInput(INPUT_3);
//    InitInput(INPUT_4);
//    InitInput(INPUT_5);
//    InitInput(INPUT_6);
//    InitInput(INPUT_7);
//    InitInput(INPUT_8);
//    InitInput(INPUT_9);
//    InitInput(INPUT_10);
//    InitInput(INPUT_11);
//    InitInput(INPUT_12);
//    InitInput(INPUT_13);
//    InitInput(INPUT_14);
//    InitInput(INPUT_15);
//    InitInput(INPUT_16);
//    
//}
//

