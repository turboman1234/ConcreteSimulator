#include "stm32f4xx_conf.h"
#include "definitions.h"
#include "mbcrc.h"
#include "mbslave.h"
#include "serial.h"
#include "mytim.h"
#include "rs232.h"
#include "usart.h"


static volatile unsigned short RS232RcvBufferPos;
static volatile unsigned short RS232SndBufferPos;
static int RS232ActiveSlaveIndex = INVALID_SLAVE_INDEX;


extern ModBusSlaveUnit ModBusSlaves[MAX_MODBUS_SLAVE_DEVICES];
unsigned char RS232RecieveBuffer[PACKET_SIZE];
unsigned char RS232ResponseBuffer[RESPONSE_SIZE];

typedef enum
{
    EV_FRAME_RECEIVED,          /*!< Frame received. */
    EV_EXECUTE,                 /*!< Execute function. */
} eEventType;

typedef enum
{
    STATE_RX_IDLE,              /*!< Receiver is in idle state. */
    STATE_RX_RCV,               /*!< Frame is beeing received. */
} eRcvState;

typedef enum
{
    STATE_TX_IDLE,              /*!< Transmitter is in idle state. */
    STATE_TX_XMIT               /*!< Transmitter is in transfer state. */
} eSndState;

volatile eEventType QueuedEvent;
volatile BOOL    EventInQueue;

static volatile eSndState SndState;
static volatile eRcvState RcvState;

// ..................UART_RX..........................


/*
    This function recognizes for which of all slaves is addressed the current message and sets global int ActiveSlaveAddress with found slave.
    If it's not found slave ActiveSlaveAddress = INVALID_SLAVE_ADDRESS
    The function returns TRUE - recieve address is valid, FALSE - recive address is not valid
    unsigned char recieveAddress - first byte from input message
*/
BOOL RS232SlaveAddressRecognition(unsigned char recieveAddress)
{
    int i;
    
    BOOL isRecieveAddressValid = FALSE;
    
    for(i = 0; i < MAX_MODBUS_SLAVE_DEVICES; i++)
    {
        if(ModBusSlaves[i].address == recieveAddress)
        {
            ModBusSlaves[i].isSlaveActive = TRUE;
            isRecieveAddressValid = TRUE;
            RS232ActiveSlaveIndex = i;
            break;
        }
    }
    
    return isRecieveAddressValid;
}

void RS232InitHardwareAndProtocol(void)
{   
    RcvState = STATE_RX_IDLE;
    SndState = STATE_TX_IDLE;  
    
    InitTIM4();
    InitUSART3();
}	

void RS232PollSlave( void )
{
    static unsigned char RcvAddress;
    unsigned int crc;
    BOOL isRcvAddressValid;
    
    eEventType eEvent;
    
    if( EventInQueue )
    {
        eEvent = QueuedEvent;
        EventInQueue = FALSE;
        
        switch ( eEvent )
        {
        case EV_FRAME_RECEIVED:
            { 
                RcvAddress = RS232RecieveBuffer[0];
                isRcvAddressValid = RS232SlaveAddressRecognition(RcvAddress);
                
                if(isRcvAddressValid == TRUE)
                {	
                    //copy global recieveBuffer in active slave's recieveBuffer for regular slave simulation
                    CopyModBusMemory(RS232RecieveBuffer, ModBusSlaves[RS232ActiveSlaveIndex].recieveBuffer, RS232RcvBufferPos);
                    
                    crc = usMBCRC16((unsigned char *) ModBusSlaves[RS232ActiveSlaveIndex].recieveBuffer, RS232RcvBufferPos);
                    if( crc == 0 )
                    {
                        EventInQueue = TRUE;
                        QueuedEvent = EV_EXECUTE;     
                    } 
                }
                break;
            }    
        case EV_EXECUTE:
            {	
                RS232_handle_request();
                break;
            }
        }
    }
}

void RS232ReceiveFSM( void )
{
    signed char Byte;
        
    Byte = (signed char)GetByte(USART_3);
    
    switch ( RcvState )
    {		
    case STATE_RX_IDLE:
        RS232RcvBufferPos = 0;
        
        RS232RecieveBuffer[RS232RcvBufferPos++] = Byte;
        RcvState = STATE_RX_RCV;
        
        RS232TimerEnable(T_10_MS);// 10 ms are enought for waiting one frame by 19200 baude rate
        break;
        
    case STATE_RX_RCV:
        if( RS232RcvBufferPos < PACKET_SIZE )
        {
            RS232RecieveBuffer[RS232RcvBufferPos++] = Byte;
        }
        RS232TimerEnable(T_10_MS);
        break;
    }	
}

void RS232_handle_request( void )
{
    int mblen;
    unsigned int crc;
    
    mblen = 0;
    
    if(ModBusSlaves[RS232ActiveSlaveIndex].recieveBuffer[0] == ModBusSlaves[RS232ActiveSlaveIndex].address)
    {
        switch(ModBusSlaves[RS232ActiveSlaveIndex].recieveBuffer[1])
        {
        case 1:                                 // Read Coil Status
            mblen = RS232_process_cmd1();
            break;
        case 2:                                // Read Discrete Input
            mblen = RS232_process_cmd2();
            break;
        case 3:                                // Read Holding Registeers
            mblen = RS232_process_cmd3();
            break;
        case 5:                                // Force Single Coil
            mblen = RS232_process_cmd5();
            break;
        case 15:                               // Force Multiple Coils
            mblen = RS232_process_cmd15();
            break;
        case 16:                               // Preset Multiple Registers
            mblen = RS232_process_cmd16();
            break;
        default:
            return;
        }
        SndState = STATE_TX_XMIT;
    }
    
    if( SndState == STATE_TX_XMIT  )
    {
        crc = usMBCRC16(RS232ResponseBuffer, mblen);
        RS232ResponseBuffer[mblen + 0] = (unsigned char) crc;
        RS232ResponseBuffer[mblen + 1] = (unsigned char) (crc >> 8);	
        RS232SndBufferPos = mblen + 2; //2 - CRC_LEN
                
        //copy global RS232ResponseBuffer in active slave's response buffer for regular slave simulation.
        CopyModBusMemory(RS232ResponseBuffer, ModBusSlaves[RS232ActiveSlaveIndex].responseBuffer, RS232SndBufferPos);
    }
}

void RS232TimerExpired( void )
{
    switch ( RcvState )
    {
    case STATE_RX_RCV:
        {
            EventInQueue = TRUE;
            QueuedEvent = EV_FRAME_RECEIVED;
            break;
        }
    default:  ;
    }
    RS232TimerDisable();
    RcvState = STATE_RX_IDLE;
}

void RS232_slave_transmit( void )
{
    if( SndState == STATE_TX_XMIT )
    {
        OutString(RS232ResponseBuffer, RS232SndBufferPos, USART_3, RS232_TIMER, T_10_MS);
        SndState = STATE_TX_IDLE;
        
        //Clearing Recive buffers
        ClearModBusSlaveMemory(RS232RecieveBuffer, PACKET_SIZE);
        ClearModBusSlaveMemory(ModBusSlaves[RS232ActiveSlaveIndex].recieveBuffer, PACKET_SIZE);
        
        //Clearing Response
        ClearModBusSlaveMemory(RS232ResponseBuffer, RESPONSE_SIZE);
        ClearModBusSlaveMemory(ModBusSlaves[RS232ActiveSlaveIndex].responseBuffer, RESPONSE_SIZE);
    }
}

//Read Coil Status
char RS232_process_cmd1(void)
{
    unsigned short outputs = 0x0000, i;
    unsigned short temp;
    unsigned char bytesCount;
    
    if(RS232RecieveBuffer[2] != 0) 
    {
        return 0; //check START ADDRESS HI is 0
    }
    if(RS232RecieveBuffer[3] >= OUTPUTS_NUMBER) 
    {
        return 0;  //check START ADDRESS LO is [0:15]
    }
    if(RS232RecieveBuffer[4] != 0) 
    {
        return 0; //check No of POINTS HI is 0
    }
    if((OUTPUTS_NUMBER - RS232RecieveBuffer[3]) < RS232RecieveBuffer[5]) 
    {
        return 0; //check No of POINTS LO
    }
    
    //read all cois' states
    for(i = 0; i < OUTPUTS_NUMBER; i++)
    {
        outputs |= (ModBusSlaves[RS232ActiveSlaveIndex].outputs[i] & 0x01) << i;
    }
    
    //take desired action
    outputs = outputs >> RS232RecieveBuffer[3];
    temp = (unsigned short)((1 << RS232RecieveBuffer[5]) - 1);
    outputs &= temp;
    bytesCount = (RS232RecieveBuffer[5] / 8) ? 2 : 1;
    
    //compose response
    RS232ResponseBuffer[0] = RS232RecieveBuffer[0]; // SLAVEID - same (already confirmed)
    RS232ResponseBuffer[1] = RS232RecieveBuffer[1]; // COMMANDID - same (already confirmed)
    RS232ResponseBuffer[2] = bytesCount; // BYTECOUNT - is at max 2 bytes as we have 16 outputs
    RS232ResponseBuffer[3] = (unsigned char)(outputs & 0x00FF); //get LO Byte
    
    if(bytesCount == 2)
    {
        RS232ResponseBuffer[4] = (unsigned char)(outputs >> 8); //get HI Byte
    }
    
    return bytesCount + 3; //length of response;
}

//Read Discrete Input
char RS232_process_cmd2(void)
{
    unsigned short inputs = 0x0000, i;
    unsigned short temp;
    unsigned char bytesCount;
    
    if(RS232RecieveBuffer[2] != 0) 
    {
        return 0; //check START ADDRESS HI is 0
    }
    if(RS232RecieveBuffer[3] >= INPUTS_NUMBER) 
    {
        return 0;  //check START ADDRESS LO is [0:15]
    }
    if(RS232RecieveBuffer[4] != 0) 
    {
        return 0; //check No of POINTS HI is 0
    }
    if((INPUTS_NUMBER - RS232RecieveBuffer[3]) < RS232RecieveBuffer[5]) 
    {
        return 0; //check No of POINTS LO
    }
    
    //read all cois' states
    for(i = 0; i < INPUTS_NUMBER; i++)
    {
        inputs |= (ModBusSlaves[RS232ActiveSlaveIndex].inputs[i] & 0x01) << i;
    }
    
    //take desired action
    inputs = inputs >> RS232RecieveBuffer[3];
    temp = (unsigned short)((1 << RS232RecieveBuffer[5]) - 1);
    inputs &= temp;
    bytesCount = (RS232RecieveBuffer[5] / 8) ? 2 : 1;
    
    //compose response
    RS232ResponseBuffer[0] = RS232RecieveBuffer[0]; // SLAVEID - same (already confirmed)
    RS232ResponseBuffer[1] = RS232RecieveBuffer[1]; // COMMANDID - same (already confirmed)
    RS232ResponseBuffer[2] = bytesCount; // BYTECOUNT - is at max 2 bytes as we have 16 outputs
    RS232ResponseBuffer[3] = (unsigned char)(inputs & 0x00FF); //get LO Byte
    
    if(bytesCount == 2)
    {
        RS232ResponseBuffer[4] = (unsigned char)(inputs >> 8); //get HI Byte
    }
    
    return bytesCount + 3; //length of response;
}

//Read Holding Registeers
char RS232_process_cmd3(void)
{
    int i;
    
    if(RS232RecieveBuffer[2] != 0) 
    {
        return 0; //check START ADDRESS HI is 0
    }
    if(RS232RecieveBuffer[3] >= HOLDING_REGISTERS_NUMBER) 
    {
        return 0;  //check START ADDRESS LO is < HOLDING_REGISTERS_NUMBER
    }
    if(RS232RecieveBuffer[4] != 0) 
    {
        return 0; //check no of points hi is 0
    }
    if((HOLDING_REGISTERS_NUMBER - RS232RecieveBuffer[3]) < RS232RecieveBuffer[5]) 
    {
        return 0; //check No of POINTS LO
    }    
    
    //compose response
    RS232ResponseBuffer[0] = RS232RecieveBuffer[0]; // SLAVEID - same (already confirmed)
    RS232ResponseBuffer[1] = RS232RecieveBuffer[1]; // COMMANDID - same (already confirmed)
    RS232ResponseBuffer[2] = RS232RecieveBuffer[5] * 2; // BYTECOUNT - is at max 200 bytes as we have 100 Hold. Reg.
    
    for(i = 0; i < RS232RecieveBuffer[5]; i ++)
    {
        RS232ResponseBuffer[3 + i * 2] = ModBusSlaves[RS232ActiveSlaveIndex].holdingRegisters[i + RS232RecieveBuffer[3]] >> 8;
        RS232ResponseBuffer[4 + i * 2] = ModBusSlaves[RS232ActiveSlaveIndex].holdingRegisters[i + RS232RecieveBuffer[3]];
    }
    
    return 3 + RS232RecieveBuffer[5] * 2;
}

//Force Single Coil
char RS232_process_cmd5(void)
{
    if(RS232RecieveBuffer[2] != 0)
    {
        return 0; //check COIL ADDRESS HI is 0
    }
    if(RS232RecieveBuffer[3] >= OUTPUTS_NUMBER) 
    {
        return 0;  //check COIL ADDRESS LO is [0:15]
    }
    if(RS232RecieveBuffer[4] != 0xFF && RS232RecieveBuffer[4] != 0x00) 
    {
        return 0; //check DATA HI
    }
    if(RS232RecieveBuffer[5] != 0x00) 
    {
        return 0; //check DATA LO
    }
    
    //take desired action
    if(RS232RecieveBuffer[4] == 0xFF)
    {
        ModBusSlaves[RS232ActiveSlaveIndex].outputs[RS232RecieveBuffer[3]] = 0x01;
    }
    else 
    {
        ModBusSlaves[RS232ActiveSlaveIndex].outputs[RS232RecieveBuffer[3]] = 0x00;
    }
    
    //compose response
    RS232ResponseBuffer[0] = RS232RecieveBuffer[0];
    RS232ResponseBuffer[1] = RS232RecieveBuffer[1];
    RS232ResponseBuffer[2] = RS232RecieveBuffer[2];
    RS232ResponseBuffer[3] = RS232RecieveBuffer[3];
    RS232ResponseBuffer[4] = RS232RecieveBuffer[4];
    RS232ResponseBuffer[5] = RS232RecieveBuffer[5];
    
    return 6;
}

//Force Multiple Coils
char RS232_process_cmd15(void)
{
    unsigned char bytesCount, i, j, currentByte, coilsCount, startAddress, ucSize;
    
    if(RS232RecieveBuffer[2] != 0) 
    {
        return 0; //check COIL ADDRESS HI
    }
    if(RS232RecieveBuffer[3] >= OUTPUTS_NUMBER) 
    {
        return 0; //check COIL ADDRESS LO is [0:15]
    }
    if(RS232RecieveBuffer[4] != 0) 
    {
        return 0; //check QUANTITY HI
    }
    if((OUTPUTS_NUMBER - RS232RecieveBuffer[3]) < RS232RecieveBuffer[5]) 
    {
        return 0; //check QUANTITY LO
    }
    if(RS232RecieveBuffer[5] > OUTPUTS_NUMBER) 
    {
        return 0; //check QUANTITY Coils - it must be less or equal then OUTPUTS_NUMBER
    }
    if(RS232RecieveBuffer[6] > 2 || RS232RecieveBuffer[6] < 1) //max Coils' number is 16 which is 2 Bytes
    {
        return 0; // check BYTE COUNT
    }
    
    ucSize = sizeof(unsigned char)*8; //bits in 1 unsigned char
    startAddress = RS232RecieveBuffer[3];
    coilsCount = RS232RecieveBuffer[5];
    bytesCount = RS232RecieveBuffer[6];
    
    for(i = 0; i < bytesCount; i++)
    {
        currentByte = RS232RecieveBuffer[7 + i];
        
        for(j = 0; j < ucSize; j++)
        {
            if(coilsCount > 0)
            {
                // j = 3 =>
                // 0011 1010 - current byte
                //&
                // 0000 1000 - generated mask with j
                //-----------
                // 0000 1000 ----> 0000 0001 - result 
                ModBusSlaves[RS232ActiveSlaveIndex].outputs[startAddress + i * ucSize + j] = (currentByte & (1 << j)) >> j; 
                coilsCount --;
            }
            else
            {
                break;
            }
        }
    }
    
    //compose response
    RS232ResponseBuffer[0] = RS232RecieveBuffer[0];
    RS232ResponseBuffer[1] = RS232RecieveBuffer[1];
    RS232ResponseBuffer[2] = RS232RecieveBuffer[2];
    RS232ResponseBuffer[3] = RS232RecieveBuffer[3];
    RS232ResponseBuffer[4] = RS232RecieveBuffer[4];
    RS232ResponseBuffer[5] = RS232RecieveBuffer[5];
    
    return 6;
}

//Preset Multiple Registers
char RS232_process_cmd16(void)
{
    unsigned char i;
    
    if(RS232RecieveBuffer[2] != 0) 
    {
        return 0; //check START ADDRESS HI is 0
    }
    if(RS232RecieveBuffer[3] >= HOLDING_REGISTERS_NUMBER) 
    {
        return 0;  //check START ADDRESS LO is < HOLDING_REGISTERS_NUMBER
    }
    if(RS232RecieveBuffer[4] != 0) 
    {
        return 0; //check no of points hi is 0
    }
    if((HOLDING_REGISTERS_NUMBER - RS232RecieveBuffer[3]) < RS232RecieveBuffer[5]) 
    {
        return 0; //check No of POINTS LO
    }  
    
    for (i = 0; i < RS232RecieveBuffer[5]; i ++)
    {
        ModBusSlaves[RS232ActiveSlaveIndex].holdingRegisters[RS232RecieveBuffer[3] + i] = (unsigned short)RS232RecieveBuffer[7 + i * 2] << 8;
        ModBusSlaves[RS232ActiveSlaveIndex].holdingRegisters[RS232RecieveBuffer[3] + i] |= RS232RecieveBuffer[8 + i * 2];
    }	
    
    //compose response
    RS232ResponseBuffer[0] = RS232RecieveBuffer[0];
    RS232ResponseBuffer[1] = RS232RecieveBuffer[1];
    RS232ResponseBuffer[2] = RS232RecieveBuffer[2];
    RS232ResponseBuffer[3] = RS232RecieveBuffer[3];
    RS232ResponseBuffer[4] = RS232RecieveBuffer[4];
    RS232ResponseBuffer[5] = RS232RecieveBuffer[5];
    
    return 6;
}
