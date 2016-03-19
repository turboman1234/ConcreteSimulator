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

extern RCC_ClocksTypeDef MYCLOCKS;
extern ModBusSlaveUnit ModBusSlaves[MAX_MODBUS_SLAVE_DEVICES];

 void TestLEDs(void);
 void TestButtons(void);
 void TestSwitches(void);
 void TestTrimmers(void);
 void TestUSART3Sending();
 void TestRS232Slave(void);
 

 
int trimmerValue, dacValue = 0;
float volts = 0.0;
int trimmer1, trimmer2, trimmer3;

int main()
{   
    InitRCC();
    
    InitNewMBSlaveDevices();
    MBInitHardwareAndProtocol();
    InitVTimers();
    
    InitInertScaleSimulator();
    InitCementScale();
    InitWaterScale();
    InitCartSimulator();
    InitMixerSimulator();
    
    while(1)
    {
        CartSimulator();
        InertScaleSimulator();
        CementScaleSimulator();
        WaterScaleSimulator();
        MixerSimulator();
    }
    
    
    /*  TEST 0 - ADC and DAC
//    //ADC test
//    InitTrimmer(TRIMMER_1);
//    InitDAC(DAC_2, MIN_ANALOG_VALUE_12b);
//        
//    while(1)
//    {
//        
//        trimmerValue = GetTrimmerValue(TRIMMER_1);
//        
//        SetAnalogOutput(DAC_2, trimmerValue * 4);
//        
//        dacValue = GetAnalogOutput(DAC_2);
//        
//        volts = 2.979 * ((float)dacValue / (float)MAX_ANALOG_VALUE_12b);
//    } 
*/
    
    /*      TEST 1 - Running Light    
    TestLEDs();
    */
    
    /*      TEST 2 - press button - light led
    TestbuttonsAndSwitches();
    */
    
    /*      TEST 3 - control leds with switches
    TestSwitches();
    */
    
    /*      TEST 4 - control leds with trimmers
    TestTrimmers();
    */
    
    /*      TEST 5 - USART 3 sending
    TestUSART3Sending()
    */
    
    /*
            TEST 6 - test Read Coils (cmd_1), Write Single Coil (cmd_5), Write Multiple Coils (cmd_15) 
    TestRS232Slave();
    */
    
    
    

}


void TestUSART3Sending()
{    
    InitRCC();

    InitUSART3();
 
    while (1){
        
        while (USART_GetFlagStatus(USART3,USART_FLAG_TXE) == RESET);
        USART_SendData(USART3, 'a');    
    }
}



void TestRS232Slave(void)
{
    int i, led;
    
    InitRCC();
    InitVTimers();
    
//    InitTIM4();
//    InitUSART3();
    
    InitLED(LED_1);
    InitLED(LED_2);
    InitLED(LED_3);
    InitLED(LED_4);
    InitLED(LED_5);
    InitLED(LED_6);
    InitLED(LED_7);
    InitLED(LED_8);
    
    InitButton(BUTTON_7);
    
    InitNewMBSlaveDevices();
    
    RS232InitHardwareAndProtocol();
    
    while(1)
    {
        VTimerTask();
        
        RS232PollSlave();
        
        
        // Test cmd 1, 5, 15
        for(i = 0; i < 8; i++)// there are only 8 LEDs
        {
            led = LED_1 + i;
            if(ModBusSlaves[0].outputs[i] == ON)
            {
                SetLED(led, ON);
            }
            else
            {
                SetLED(led, OFF);
            }   
        }
        
        // Test cmd 2
        if(GetButtonState(BUTTON_7) == PRESSED)
        {
            ModBusSlaves[0].inputs[7] = PRESSED;
        }
        else
        {
            ModBusSlaves[0].inputs[7] = NOT_PRESSED;
        }
        
        
        
        RS232_slave_transmit();
    }
}

void TestLEDs(void)
 {  
    InitRCC();
    InitVTimers();
    
    InitLED(LED_1);
    InitLED(LED_2);
    InitLED(LED_3);
    InitLED(LED_4);
    InitLED(LED_5);
    InitLED(LED_6);
    InitLED(LED_7);
    InitLED(LED_8);
    
    int ledID = LED_1;
    
    SetVTimerValue(TIMER_1, 500);
    
    while(1)
    {
        if(IsVTimerElapsed(TIMER_1) == ELAPSED)
        {
            // set all LEDs to OFF state
            SetLED(LED_1, OFF);
            SetLED(LED_2, OFF);
            SetLED(LED_3, OFF);
            SetLED(LED_4, OFF);
            SetLED(LED_5, OFF);
            SetLED(LED_6, OFF);
            SetLED(LED_7, OFF);
            SetLED(LED_8, OFF);
            
            SetLED(ledID, ON);
            
            if(ledID == LED_8)
            {
                ledID = LED_1;
            }
            else
            {
                ledID = ledID + 1;
            }
            
            SetVTimerValue(TIMER_1, 500);
        }
                
        VTimerTask();
        
    }
 }

void TestButtons(void)
{
    InitRCC();
    
    InitButton(BUTTON_1);
    InitButton(BUTTON_2);
    InitButton(BUTTON_3);
    InitButton(BUTTON_4);
    InitButton(BUTTON_5);
    InitButton(BUTTON_6);
    InitButton(BUTTON_7);
    InitButton(BUTTON_8);
    
    InitLED(LED_1);
    InitLED(LED_2);
    InitLED(LED_3);
    InitLED(LED_4);
    InitLED(LED_5);
    InitLED(LED_6);
    InitLED(LED_7);
    InitLED(LED_8);
        
    int btn1, btn2, btn3, btn4, btn5, btn6, btn7, btn8;
    
    while(1)
    {       
        btn1 = GetButtonState(BUTTON_1);
        btn2 = GetButtonState(BUTTON_2);
        btn3 = GetButtonState(BUTTON_3);
        btn4 = GetButtonState(BUTTON_4);
        btn5 = GetButtonState(BUTTON_5);
        btn6 = GetButtonState(BUTTON_6);
        btn7 = GetButtonState(BUTTON_7);
        btn8 = GetButtonState(BUTTON_8);
        
        
        if(btn1 == PRESSED)
        {
            SetLED(LED_1, ON);
        }
        else
        {
            SetLED(LED_1, OFF);
        }
        if(btn2 == PRESSED)
        {
            SetLED(LED_2, ON);
        }
        else
        {
            SetLED(LED_2, OFF);
        }
        if(btn3 == PRESSED)
        {
            SetLED(LED_3, ON);
        }
        else
        {
            SetLED(LED_3, OFF);
        }
        if(btn4 == PRESSED)
        {
            SetLED(LED_4, ON);
        }
        else
        {
            SetLED(LED_4, OFF);
        }
        if(btn5 == PRESSED)
        {
            SetLED(LED_5, ON);
        }
        else
        {
            SetLED(LED_5, OFF);
        }
        if(btn6 == PRESSED)
        {
            SetLED(LED_6, ON);
        }
        else
        {
            SetLED(LED_6, OFF);
        }
        if(btn7 == PRESSED)
        {
            SetLED(LED_7, ON);
        }
        else
        {
            SetLED(LED_7, OFF);
        }
        if(btn8 == PRESSED)
        {
            SetLED(LED_8, ON);
        }
        else
        {
            SetLED(LED_8, OFF);
        }
        
    }
}

void TestSwitches(void)
{
     InitRCC();
     
     InitLED(LED_1);
     InitLED(LED_2);
     InitLED(LED_3);
     InitLED(LED_4);
     InitLED(LED_5);
     InitLED(LED_6);
     InitLED(LED_7);
     InitLED(LED_8);
     
     InitSwitch(SWITCH_1);
     InitSwitch(SWITCH_2);
     
     int switch1, switch2;
     
     while(1)
     {
         switch1 = GetSwitchState(SWITCH_1);
         switch2 = GetSwitchState(SWITCH_2);
         
         if(switch1 == ON && switch2 == OFF)
         {
             SetLED(LED_1, ON);
             SetLED(LED_2, ON);
             SetLED(LED_3, ON);
             SetLED(LED_4, ON);
             
             SetLED(LED_5, OFF);
             SetLED(LED_6, OFF);
             SetLED(LED_7, OFF);
             SetLED(LED_8, OFF);
         }
         else if(switch1 == OFF && switch2 == ON)
         {
             SetLED(LED_1, OFF);
             SetLED(LED_2, OFF);
             SetLED(LED_3, OFF);
             SetLED(LED_4, OFF);
             
             SetLED(LED_5, ON);
             SetLED(LED_6, ON);
             SetLED(LED_7, ON);
             SetLED(LED_8, ON);
         }
         else if(switch1 == ON && switch2 == ON)
         {
             SetLED(LED_1, ON);
             SetLED(LED_2, ON);
             SetLED(LED_3, ON);
             SetLED(LED_4, ON);
             
             SetLED(LED_5, ON);
             SetLED(LED_6, ON);
             SetLED(LED_7, ON);
             SetLED(LED_8, ON);
         }
         else 
         {
             SetLED(LED_1, OFF);
             SetLED(LED_2, OFF);
             SetLED(LED_3, OFF);
             SetLED(LED_4, OFF);
             
             SetLED(LED_5, OFF);
             SetLED(LED_6, OFF);
             SetLED(LED_7, OFF);
             SetLED(LED_8, OFF);
         }
     }
}

/*void ADCandDAC(void)
{
    int trimmerValue, dacValue = 0;
    float volts = 0.0;
    
    InitTrimmer(TRIMMER_1);

    InitDAC(DAC_2, MIN_ANALOG_VALUE_12b);
        
    while(1)
    {
        
        trimmerValue = GetTrimmerValue(TRIMMER_1);
        
        SetAnalogOutput(DAC_2, trimmerValue * 4);
        
        dacValue = GetAnalogOutput(DAC_2);
        
        volts = 2.979 * ((float)dacValue / (float)MAX_ANALOG_VALUE_12b);
    } 
}
*/


void TestTrimmers(void)
{
    InitTrimmer(TRIMMER_1);
    InitTrimmer(TRIMMER_2);
    InitTrimmer(TRIMMER_3);
    
    InitLED(LED_1);
    InitLED(LED_2);
    InitLED(LED_3);
    InitLED(LED_4);
    InitLED(LED_5);
    InitLED(LED_6);
    InitLED(LED_7);
    InitLED(LED_8);
    
        
    while(1)
    {
        trimmer1 = GetTrimmerValue(TRIMMER_1);
        trimmer2 = GetTrimmerValue(TRIMMER_2);
        trimmer3 = GetTrimmerValue(TRIMMER_3);
        
        if(trimmer1 < 500)
        {
            SetLED(LED_1, ON);
            SetLED(LED_2, OFF);
        }
        else
        {
            SetLED(LED_1, OFF);
            SetLED(LED_2, ON);            
        }
        
        if(trimmer2 < 500)
        {
            SetLED(LED_3, ON);
            SetLED(LED_4, OFF);
        }
        else
        {
            SetLED(LED_3, OFF);
            SetLED(LED_4, ON);            
        }
        
        if(trimmer3 < 500)
        {
            SetLED(LED_5, ON);
            SetLED(LED_6, OFF);
        }
        else
        {
            SetLED(LED_5, OFF);
            SetLED(LED_6, ON);            
        }
    }
}

