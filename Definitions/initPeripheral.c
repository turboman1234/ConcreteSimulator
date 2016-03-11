#include "stm32f4xx_conf.h"
#include "definitions.h"
#include "adc.h"
#include "dac.h"
#include "initPeripheral.h"


/* Configure GPIO pin as button input 
  int buttonID - BUTTON_1 ... BUTTON_8
*/
void InitButton(int buttonID)
{
    assert_param(IS_BUTTON_ID_VALID(buttonID));
    
    GPIO_InitTypeDef GPIO_InitStructure;
    
    switch (buttonID)
    {
    case BUTTON_1:
        
        /* BUTTON_1 ----> pin PC6 */
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
        
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
        
        GPIO_Init(GPIOC, &GPIO_InitStructure);
        
        break;
        
    case BUTTON_2:
        /* BUTTON_2 ----> pin PC8 */
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
        
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
        
        GPIO_Init(GPIOC, &GPIO_InitStructure);
        
        break;
        
    case BUTTON_3:
        /* BUTTON_3 ----> pin PC9 */
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
        
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
        
        GPIO_Init(GPIOC, &GPIO_InitStructure);
        break;
        
    case BUTTON_4:
        /* BUTTON_4 ----> pin PD11 */
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
        
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
        
        GPIO_Init(GPIOD, &GPIO_InitStructure);
        break;
        
    case BUTTON_5:
        /* BUTTON_5 ----> pin PA8 */
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
        
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
        
        GPIO_Init(GPIOA, &GPIO_InitStructure);
        break;
        
    case BUTTON_6:
        /* BUTTON_6 ----> pin PD0 */
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
        
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
        
        GPIO_Init(GPIOD, &GPIO_InitStructure);
        break;
        
    case BUTTON_7:
        /* BUTTON_7 ----> pin PE1 */
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
        
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
        
        GPIO_Init(GPIOE, &GPIO_InitStructure);
        break;
        
    case BUTTON_8:
        /* BUTTON_8 ----> pin PE0 */
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
        
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
        
        GPIO_Init(GPIOE, &GPIO_InitStructure);
        break;        
    }
}

/* Configure GPIO pin as LED output 
  int ledID - LED_1 ... LED_8
*/
void InitLED(int ledID)
{
    assert_param(IS_LED_ID_VALID(ledID));
 
    GPIO_InitTypeDef GPIO_InitStructure;
       
    switch (ledID)
    {
    case LED_1:
        
        /* LED_1 ----> pin PD15 */
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
        
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
        GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        
        GPIO_Init(GPIOD, &GPIO_InitStructure);
        
        break;
        
    case LED_2:
        /* LED_2 ----> pin PD13 */
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
        
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
        GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        
        GPIO_Init(GPIOD, &GPIO_InitStructure);
        
        break;
        
    case LED_3:
        /* LED_3 ----> pin PD14 */
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
        
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
        GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        
        GPIO_Init(GPIOD, &GPIO_InitStructure);
        break;
        
    case LED_4:
        /* LED_4 ----> pin PD12 */
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
        
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
        GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        
        GPIO_Init(GPIOD, &GPIO_InitStructure);
        break;
        
    case LED_5:
        /* LED_5 ----> pin PC11 */
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
        
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
        GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        
        GPIO_Init(GPIOC, &GPIO_InitStructure);
        break;
        
    case LED_6:
        /* LED_6 ----> pin PD2 */
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
        
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
        GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        
        GPIO_Init(GPIOD, &GPIO_InitStructure);
        break;
        
    case LED_7:
        /* LED_7 ----> pin PB3 */
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
        
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
        GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        
        GPIO_Init(GPIOB, &GPIO_InitStructure);
        break;
        
    case LED_8:
        /* LED_8 ----> pin PB7 */
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
        
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
        GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        
        GPIO_Init(GPIOB, &GPIO_InitStructure);
        break;        
    }
}

/* Configure GPIO pin as Switch input 
  int swtich - SWITCH_1, SWITCH_2
*/
void InitSwitch(int switchID)
{
    assert_param(IS_SWITCH_ID_VALID(switchID));
    
    GPIO_InitTypeDef GPIO_InitStructure;
    
    switch (switchID)
    {
    case SWITCH_1:
        
        /* SWITCH_1 ----> pin PE1 */
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
        
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
        
        GPIO_Init(GPIOE, &GPIO_InitStructure);
        
        break;
        
    case SWITCH_2:
        /* SWITCH_2 ----> pin PE0 */
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
        
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
        
        GPIO_Init(GPIOE, &GPIO_InitStructure);
        
        break;
    }        
}

/* Configure GPIO pin as digital input 
  int inputID - INPUT_1 ... INPUT_16
*/
void InitInput(int inputID)
{
    assert_param(IS_INPUT_ID_VALID(inputID));
    
    GPIO_InitTypeDef GPIO_InitStructure;
        
    switch (inputID)
    {
    case INPUT_1:
        
        /* INPUT_1 ----> pin PC13 */
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
        
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
        
        GPIO_Init(GPIOC, &GPIO_InitStructure);
        
        break;
        
    case INPUT_2:
        /* INPUT_2 ----> pin PE5 */
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
        
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
        
        GPIO_Init(GPIOE, &GPIO_InitStructure);
        
        break;
        
    case INPUT_3:
        /* INPUT_3 ----> pin PB8 */
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
        
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
        
        GPIO_Init(GPIOB, &GPIO_InitStructure);
        break;
        
    case INPUT_4:
        /* INPUT_4 ----> pin PB4 */
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
        
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
        
        GPIO_Init(GPIOB, &GPIO_InitStructure);
        break;
        
    case INPUT_5:
        /* INPUT_5 ----> pin PD7 */
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
        
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
        
        GPIO_Init(GPIOD, &GPIO_InitStructure);
        break;
        
    case INPUT_6:
        /* INPUT_6 ----> pin PD3 */
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
        
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
        
        GPIO_Init(GPIOD, &GPIO_InitStructure);
        break;
        
    case INPUT_7:
        /* INPUT_7 ----> pin PD1 */
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
        
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
        
        GPIO_Init(GPIOD, &GPIO_InitStructure);
        break;
        
    case INPUT_8:
        /* INPUT_8 ----> pin PA10 */
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
        
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
        
        GPIO_Init(GPIOA, &GPIO_InitStructure);
        break;        
        
    case INPUT_9:
        
        /* INPUT_9 ----> pin PE6 */
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
        
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
        
        GPIO_Init(GPIOE, &GPIO_InitStructure);
        
        break;
        
    case INPUT_10:
        /* INPUT_10 ----> pin PE4 */
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
        
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
        
        GPIO_Init(GPIOE, &GPIO_InitStructure);
        
        break;
        
    case INPUT_11:
        /* INPUT_11 ----> pin PE2 */
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
        
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
        
        GPIO_Init(GPIOE, &GPIO_InitStructure);
        break;
        
    case INPUT_12:
        /* INPUT_12 ----> pin PB6 */
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
        
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
        
        GPIO_Init(GPIOB, &GPIO_InitStructure);
        break;
        
    case INPUT_13:
        /* INPUT_13 ----> pin PB5 */
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
        
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
        
        GPIO_Init(GPIOB, &GPIO_InitStructure);
        break;
        
    case INPUT_14:
        /* INPUT_14 ----> pin PD6 */
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
        
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
        
        GPIO_Init(GPIOD, &GPIO_InitStructure);
        break;
        
    case INPUT_15:
        /* INPUT_15 ----> pin PD4 */
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
        
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
        
        GPIO_Init(GPIOD, &GPIO_InitStructure);
        break;
        
    case INPUT_16:
        /* INPUT_16 ----> pin PC12 */
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
        
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
        
        GPIO_Init(GPIOC, &GPIO_InitStructure);
        break;        
        
    }
}

/* Configure GPIO pin as digital output 
  int outputID - OUTPUT_1 ... OUTPUT_16
*/
void InitOutput(int outputID)
{
    assert_param(IS_OUTPUT_ID_VALID(outputID));
 
    GPIO_InitTypeDef GPIO_InitStructure;
    
    switch (outputID)
    {
    case OUTPUT_1:
        
        /* OUTPUT_1 ----> pin PB1 */
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
        
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
        GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        
        GPIO_Init(GPIOB, &GPIO_InitStructure);
        
        break;
        
    case OUTPUT_2:
        /* OUTPUT_2 ----> pin PE7 */
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
        
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
        GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        
        GPIO_Init(GPIOE, &GPIO_InitStructure);
        
        break;
        
    case OUTPUT_3:
        /* OUTPUT_3 ----> pin PE9 */
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
        
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
        GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        
        GPIO_Init(GPIOE, &GPIO_InitStructure);
        break;
        
    case OUTPUT_4:
        /* OUTPUT_4 ----> pin PB11 */
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
        
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
        GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        
        GPIO_Init(GPIOB, &GPIO_InitStructure);
        break;
        
    case OUTPUT_5:
        /* OUTPUT_5 ----> pin PE15 */
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
        
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
        GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        
        GPIO_Init(GPIOE, &GPIO_InitStructure);
        break;
        
    case OUTPUT_6:
        /* OUTPUT_6 ----> pin PE13 */
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
        
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
        GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        
        GPIO_Init(GPIOE, &GPIO_InitStructure);
        break;
        
    case OUTPUT_7:
        /* OUTPUT_7 ----> pin PE11 */
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
        
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
        GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        
        GPIO_Init(GPIOE, &GPIO_InitStructure);
        break;
        
    case OUTPUT_8:
        /* OUTPUT_8 ----> pin PC3 */
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
        
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
        GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        
        GPIO_Init(GPIOC, &GPIO_InitStructure);
        break;        
        
    case OUTPUT_9:
        
        /* OUTPUT_9 ----> pin PB0 */
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
        
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
        GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        
        GPIO_Init(GPIOB, &GPIO_InitStructure);
        
        break;
        
    case OUTPUT_10:
        /* OUTPUT_10 ----> pin PD10 */
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
        
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
        GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        
        GPIO_Init(GPIOD, &GPIO_InitStructure);
        
        break;
        
    case OUTPUT_11:
        /* OUTPUT_11 ----> pin PE8 */
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
        
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
        GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        
        GPIO_Init(GPIOE, &GPIO_InitStructure);
        break;
        
    case OUTPUT_12:
        /* OUTPUT_12 ----> pin PE10 */
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
        
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
        GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        
        GPIO_Init(GPIOE, &GPIO_InitStructure);
        break;
        
    case OUTPUT_13:
        /* OUTPUT_13 ----> pin PB10 */
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
        
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
        GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        
        GPIO_Init(GPIOB, &GPIO_InitStructure);
        break;
        
    case OUTPUT_14:
        /* OUTPUT_14 ----> pin PE14 */
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
        
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
        GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        
        GPIO_Init(GPIOE, &GPIO_InitStructure);
        break;
        
    case OUTPUT_15:
        /* OUTPUT_15 ----> pin PE12 */
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
        
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
        GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        
        GPIO_Init(GPIOE, &GPIO_InitStructure);
        break;
        
    case OUTPUT_16:
        /* OUTPUT_16 ----> pin PA9 */
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
        
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
        GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        
        GPIO_Init(GPIOA, &GPIO_InitStructure);
        break;        
    }
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
/* Configure GPIO pin as analog input for Trimmer 

  int trimmerID - TRIMMER_1, TRIMMER_2, TRIMMER_3
*/
void InitTrimmer(int trimmerID)
{   
    assert_param(IS_TRIMMER_ID_VALID(trimmerID));
    
    GPIO_InitTypeDef GPIO_InitStructure;
    
    switch (trimmerID)
    {
    case TRIMMER_1:
        
        /* TRIMMER_1 ----> pin PC1 */
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
        
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
        
        GPIO_Init(GPIOC, &GPIO_InitStructure);
        
        break;
        
    case TRIMMER_2:
        /* TRIMMER_2 ----> pin PA1 */
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
        
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
        
        GPIO_Init(GPIOA, &GPIO_InitStructure);
        
        break;
        
    case TRIMMER_3:
        /* TRIMMER_3 ----> pin PC2 */
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
        
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
        
        GPIO_Init(GPIOC, &GPIO_InitStructure);
        break;
    }
    
    Init_ADC3();
}

/* Configure GPIO pin as ADC input
  int adcID - ADC_1, ADC_2
*/
void InitADC(int adcID)
{
    assert_param(IS_ADC_ID_VALID(adcID));
    
    GPIO_InitTypeDef GPIO_InitStructure;
    
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
    
    switch (adcID)
    {
    case ADC_1:
        
        /* ADC_1 ----> pin PC4 */       
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
        
        GPIO_Init(GPIOC, &GPIO_InitStructure);
        
        Init_ADC1();
        
        break;
        
    case ADC_2:
        
        /* ADC_2 ----> pin PC5 */
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
        
        GPIO_Init(GPIOC, &GPIO_InitStructure);
        
        Init_ADC2();
        
        break;
    }
}

/* Configure GPIO pin as DAC output
  int dacID - DAC_1, DAC_2
  int initValue - 0 - 4095
*/
void InitDAC(int dacID, int initValue)
{
    assert_param(IS_DAC_ID_VALID(dacID));
    
    GPIO_InitTypeDef GPIO_InitStructure;
    
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    
    //Set GPIOA clock
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

    switch(dacID)
    {
    case DAC_1:        
        
        /* DAC_1 ----> pin PA4 */
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
        GPIO_Init(GPIOA, &GPIO_InitStructure);
        
        Init_DAC_1(initValue);
        break;
        
    case DAC_2:
        
        /* DAC_2 ----> pin PA5 */
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
        GPIO_Init(GPIOA, &GPIO_InitStructure);
        
        Init_DAC_2(initValue);
        break;
    }
}


