#ifndef __USART_H
#define __USART_H


#define USART_BAUD_RATE_2400            2400
#define USART_BAUD_RATE_9600            9600
#define USART_BAUD_RATE_19200           19200
#define USART_BAUD_RATE_38400           38400
#define USART_BAUD_RATE_57600           57600
#define USART_BAUD_RATE_115200          115200

#define MB_MASTER_UNIT                  0xAAAA
#define MB_SLAVE_UNIT                   0xBBBB

void InitUSART2(int modBusUnitType);
void InitUSART3(void);
unsigned char recieveMyUSART(int usartID);
unsigned char sendMyUSART(char *data, unsigned char count, int usartID, int timerType, int miliseconds);
void USART2_IRQHandler(void);
void USART3_IRQHandler(void);

#endif

