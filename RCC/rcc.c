#include "stm32f4xx_conf.h"
#include "rcc.h"

RCC_ClocksTypeDef MYCLOCKS;

void InitRCC(void)
{
    RCC_DeInit();
    
    // set needed frequencies
    
    //start HSI oscilator at 16, MHz
    RCC_HSICmd(ENABLE);
    
    //wait for HSI oscilator start working
    while(RCC_GetFlagStatus(RCC_FLAG_HSIRDY) != SET);
    
    //HSI is choosen for system clock source
    RCC_SYSCLKConfig(RCC_SYSCLKSource_HSI);
    
    RCC_GetClocksFreq(&MYCLOCKS);
    
    // Configure PLL
    //source: HSI
    //PLL_M: 16
    //PLL_N: 200
    //PLL_P: 2
    //PLL_Q: 1
    // SystemCoreClock = ((INPUT_CLOCK (HSE_OR_HSI_IN_HZ) / PLL_M) * PLL_N) / PLL_P
    // SystemCoreClock = ((16 000 000 / 16) * 200) / 2 = 100, MHz
    RCC_PLLConfig(RCC_PLLSource_HSI, PLL_M, PLL_N, PLL_P, PLL_Q);
    
    // Activate PLL
    RCC_PLLCmd(ENABLE);
    
    // Wait until the PLL is ready
    while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);
    
    // Select PLL as system clock source
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
    
    //Set AHB clock = systemCoreClock = 100, MHz
    RCC_HCLKConfig(RCC_SYSCLK_Div1);
    
    //Set APB1 clock = AHB clock / 2 = 50, MHz
    RCC_PCLK1Config(RCC_HCLK_Div2);
    
    //Set APB2 clock = AHB clock / 4 = 25, MHz
    RCC_PCLK2Config(RCC_HCLK_Div4);
    
    // Show current FREQ in Hz (for DEBUG purposes)
    RCC_GetClocksFreq(&MYCLOCKS);
    
    //SystemCoreClockUpdate();
}


