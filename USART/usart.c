#include "stm32f4xx_conf.h"
#include "definitions.h"
#include "VTimer.h"
#include "mbslave.h"
#include "rs232.h"
#include "usart.h"

/*      USART_2 is for ModBus communication

int modBusUnitType - MB_MASTER_UNIT or MB_SLAVE_UNIT

***MB_MASTER_UNIT - no need from interrupt controller
***MB_SLAVE_UNIT - interrupt controller is needed
*/
void InitUSART2(int modBusUnitType)
{
    GPIO_InitTypeDef MYGPIO;
    USART_InitTypeDef MYUSART;
    NVIC_InitTypeDef MYNVIC;
    
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
    
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);        
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);
    
    /* TX pin*/
    MYGPIO.GPIO_Pin = GPIO_Pin_2;
    MYGPIO.GPIO_Mode = GPIO_Mode_AF;
    MYGPIO.GPIO_PuPd = GPIO_PuPd_UP;
    MYGPIO.GPIO_OType = GPIO_OType_PP;
    MYGPIO.GPIO_Speed = GPIO_Speed_50MHz;
    
    GPIO_Init(GPIOA, &MYGPIO);
    
    /* RX pin */
    MYGPIO.GPIO_Pin = GPIO_Pin_3;
    MYGPIO.GPIO_Mode = GPIO_Mode_AF;
    MYGPIO.GPIO_PuPd = GPIO_PuPd_NOPULL;
    MYGPIO.GPIO_Speed = GPIO_Speed_50MHz;
    
    GPIO_Init(GPIOA, &MYGPIO);
    
    
    MYUSART.USART_BaudRate = USART_BAUD_RATE_19200;
    MYUSART.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    MYUSART.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    MYUSART.USART_Parity = USART_Parity_No;
    MYUSART.USART_StopBits = USART_StopBits_1;
    MYUSART.USART_WordLength = USART_WordLength_8b;
    USART_Init(USART2, &MYUSART);
    
    
    //When Usart is used for MB Master it must not be enable interrupt controller
    // MB Master do not use interruptions to read slave's response
    
    if(modBusUnitType == MB_MASTER_UNIT)
    {
        USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);
    }
    else if (modBusUnitType == MB_SLAVE_UNIT)
    {
        USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
        
        MYNVIC.NVIC_IRQChannel = USART2_IRQn;
        MYNVIC.NVIC_IRQChannelCmd = ENABLE;
        MYNVIC.NVIC_IRQChannelPreemptionPriority = 1;
        MYNVIC.NVIC_IRQChannelSubPriority = 0;
        USART_ClearFlag(USART2, USART_FLAG_RXNE);
        USART_ClearITPendingBit(USART2, USART_IT_RXNE);
        NVIC_Init(&MYNVIC);
    }
    
    
    USART_Cmd(USART2, ENABLE);	
}

//USART_3 is for serial communication (RS-232)
void InitUSART3(void)
{
    GPIO_InitTypeDef MYGPIO;
    USART_InitTypeDef MYUSART;
    NVIC_InitTypeDef MYNVIC;
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);    
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
    
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource8, GPIO_AF_USART3);        
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource9, GPIO_AF_USART3);
    
    //    /* TX pin and RX pin */
    //    MYGPIO.GPIO_Mode = GPIO_Mode_AF;
    //    MYGPIO.GPIO_Speed = GPIO_Speed_50MHz;
    //    MYGPIO.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
    //    MYGPIO.GPIO_PuPd = GPIO_PuPd_UP;
    //    MYGPIO.GPIO_OType = GPIO_OType_PP;
    
    //        /* TX pin*/
    //    MYGPIO.GPIO_Mode = GPIO_Mode_AF;
    //    MYGPIO.GPIO_Speed = GPIO_Speed_50MHz;
    //    MYGPIO.GPIO_Pin = GPIO_Pin_8;
    //    
    //    GPIO_Init(GPIOD, &MYGPIO);
    //    
    //    /* RX pin */
    //    MYGPIO.GPIO_Mode = GPIO_Mode_AF;
    //    MYGPIO.GPIO_Speed = GPIO_Speed_50MHz;
    //    MYGPIO.GPIO_Pin = GPIO_Pin_9;
    //    
    //    GPIO_Init(GPIOD, &MYGPIO);
    
    MYGPIO.GPIO_Pin = GPIO_Pin_9; // rx
    MYGPIO.GPIO_Mode = GPIO_Mode_AF;
    MYGPIO.GPIO_PuPd = GPIO_PuPd_NOPULL;
    MYGPIO.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOD, &MYGPIO);
    
    MYGPIO.GPIO_Pin = GPIO_Pin_8 ; // tx
    MYGPIO.GPIO_Mode = GPIO_Mode_AF;
    MYGPIO.GPIO_PuPd = GPIO_PuPd_UP;
    MYGPIO.GPIO_OType = GPIO_OType_PP;
    MYGPIO.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOD, &MYGPIO);    
    
    //USART_DeInit(USART3);
    
    MYUSART.USART_BaudRate = USART_BAUD_RATE_19200;
    MYUSART.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    MYUSART.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    MYUSART.USART_Parity = USART_Parity_No;
    MYUSART.USART_StopBits = USART_StopBits_1;
    MYUSART.USART_WordLength = USART_WordLength_8b;
    USART_Init(USART3, &MYUSART);
    
    
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
    
    MYNVIC.NVIC_IRQChannel = USART3_IRQn;
    MYNVIC.NVIC_IRQChannelCmd = ENABLE;
    MYNVIC.NVIC_IRQChannelPreemptionPriority = 1;
    MYNVIC.NVIC_IRQChannelSubPriority = 1;
    NVIC_Init(&MYNVIC);
    
    USART_ClearFlag(USART3, USART_FLAG_RXNE);
    USART_ClearITPendingBit(USART3, USART_IT_RXNE);
    
    USART_Cmd(USART3, ENABLE);
}


unsigned char recieveMyUSART(int usartID)
{
    assert_param(IS_USART_ID_VALID(usartID));
    
    unsigned char recvtemp;
    USART_TypeDef* USARTx;
    
    switch(usartID)
    {
    case USART_2:
        USARTx = USART2;
        break;
    case USART_3:
        USARTx = USART3;
        break;
    }
    
    if(USART_GetFlagStatus(USARTx, USART_FLAG_RXNE) == SET)
    {
        recvtemp = USART_ReceiveData(USARTx);
        return recvtemp;
    }
    return -1;
}


//char *data - pointer to data which will be send
//unsigned char count - number of sending data bytes
//int usartID - USART_2 for ModBus and USART_3 fot serial communication RS232
//int timerType - TIMER_XX, MB_MASTER_TIMER, TIMER_COMMUNICATION ...
//int miliseconds - timeout between frames sending - T_10_MS, T_100_MS, T_1_S...
unsigned char sendMyUSART(char *data, unsigned char count, int usartID, int timerType, int miliseconds)
{
    assert_param(IS_USART_ID_VALID(usartID));
    
    unsigned char bytesSend = 0;	
    USART_TypeDef* USARTx;
    
    switch(usartID)
    {
    case USART_2:
        USARTx = USART2;
        break;
    case USART_3:
        USARTx = USART3;
        break;
    }
    
    while(count--)
    {
        SetVTimerValue(timerType, miliseconds);
        
        //check if TX buffer is empty
        while(USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET && IsVTimerElapsed(timerType) == NOT_ELAPSED);
        if(IsVTimerElapsed(timerType) == NOT_ELAPSED)
        {
            USART_SendData(USARTx, *data);
        }
        else 
        {
            break;
        }
        data++;
        bytesSend ++;
    }
    
    
    SetVTimerValue(timerType, miliseconds);
    
    //check if TX is done
    while(USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET &&  IsVTimerElapsed(timerType) == NOT_ELAPSED);
    
    return bytesSend;
}

//This handler is connected with ModBus Slave devices - it recieves requests
void USART2_IRQHandler(void)
{
    MBReceiveFSM();
    
    USART_ClearFlag(USART2, USART_FLAG_RXNE);
    USART_ClearITPendingBit(USART2, USART_IT_RXNE);
}

//This handler is connected with RS232 Slave devices - it recieves requests
void USART3_IRQHandler(void)
{
    RS232ReceiveFSM();
    
    USART_ClearFlag(USART3, USART_FLAG_RXNE);
    USART_ClearITPendingBit(USART3, USART_IT_RXNE);
}
