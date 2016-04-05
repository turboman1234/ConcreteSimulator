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
    
    while(1)
    {
        CartSimulator();
        InertScaleSimulator();
        CementScaleSimulator();
        WaterScaleSimulator();
        MixerSimulator();
        SetLEDs();
    }
    
    
}

