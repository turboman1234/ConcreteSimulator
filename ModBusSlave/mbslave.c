#include "stm32f4xx_conf.h"
#include "definitions.h"
#include "mbcrc.h"
#include "mbslave.h"
#include "serial.h"
#include "mytim.h"
#include "usart.h"


static volatile unsigned short MBRcvBufferPos;
static volatile unsigned short MBSndBufferPos;
static int ActiveSlaveIndex = INVALID_SLAVE_INDEX;


extern ModBusSlaveUnit ModBusSlaves[MAX_MODBUS_SLAVE_DEVICES];


ModBusSlaveUnit ModBusSlaves[MAX_MODBUS_SLAVE_DEVICES];
unsigned char RecieveBuffer[PACKET_SIZE];
unsigned char ResponseBuffer[RESPONSE_SIZE];

typedef enum
{
    EV_FRAME_RECEIVED,          /*!< Frame received. */
    EV_EXECUTE,                 /*!< Execute function. */
} eMBEventType;

typedef enum
{
    STATE_RX_IDLE,              /*!< Receiver is in idle state. */
    STATE_RX_RCV,               /*!< Frame is beeing received. */
} eMBRcvState;

typedef enum
{
    STATE_TX_IDLE,              /*!< Transmitter is in idle state. */
    STATE_TX_XMIT               /*!< Transmitter is in transfer state. */
} eMBSndState;

volatile eMBEventType MBQueuedEvent;
volatile BOOL    MBEventInQueue;

static volatile eMBSndState MBSndState;
static volatile eMBRcvState MBRcvState;

// ..................UART_RX..........................

/*
    Initialize all Slaves
*/
void InitNewMBSlaveDevices(void)
{
    int i;
    
    for(i = 0; i < MAX_MODBUS_SLAVE_DEVICES; i++)
    {
        ModBusSlaves[i].address = i + 1;
        ModBusSlaves[i].isSlaveActive = FALSE;
        ClearModBusSlaveMemory(ModBusSlaves[i].inputs, INPUTS_NUMBER);
        ClearModBusSlaveMemory(ModBusSlaves[i].outputs, OUTPUTS_NUMBER);
        ClearModBusSlaveMemory((unsigned char *)ModBusSlaves[i].holdingRegisters, HOLDING_REGISTERS_NUMBER * sizeof(unsigned short));
        ClearModBusSlaveMemory(ModBusSlaves[i].recieveBuffer, PACKET_SIZE);
        ClearModBusSlaveMemory(ModBusSlaves[i].responseBuffer, RESPONSE_SIZE);
    }
}

/*
    Clear means fillinig with 0 'size' count in pointed memory

    pMemory - pointer to memory which must be cleared
    size - number of cells from wanted memory

    When it is needed to be cleared HoldingRegisters (shor data type) firs casting, next size = sizeof(short) * HoldRegNumb;
*/
void ClearModBusSlaveMemory(unsigned char *pMemory, int size)
{
    int  i;
    
    for(i = 0; i < size; i++)
    {
        *pMemory = 0;
        pMemory++;
    }
}

/*
    This function copies 'cellsNumber' from 'source' in 'destination'
*/

void CopyModBusMemory(unsigned char *source, unsigned char *destination, unsigned short cellsNumber)
{
    int i;
    
    unsigned char *src, *dst;
    src = source;
    dst = destination;
        
    for(i = 0; i < cellsNumber; i++)
    {
        dst[i] = src[i];
    }
}

/*
    This function recognizes for which of all slaves is addressed the current message and sets global int ActiveSlaveAddress with found slave.
    If it's not found slave ActiveSlaveAddress = INVALID_SLAVE_ADDRESS
    The function returns TRUE - recieve address is valid, FALSE - recive address is not valid
    unsigned char recieveAddress - first byte from input message
*/
BOOL MBSlaveAddressRecognition(unsigned char recieveAddress)
{
    int i;
    
    BOOL isRecieveAddressValid = FALSE;
    
    for(i = 0; i < MAX_MODBUS_SLAVE_DEVICES; i++)
    {
        if(ModBusSlaves[i].address == recieveAddress)
        {
            ModBusSlaves[i].isSlaveActive = TRUE;
            isRecieveAddressValid = TRUE;
            ActiveSlaveIndex = i;
            break;
        }
    }
    
    return isRecieveAddressValid;
}

void MBInitHardwareAndProtocol(void)
{    
    MBRcvState = STATE_RX_IDLE;
    MBSndState = STATE_TX_IDLE;  
    
    InitNewMBSlaveDevices();
    
    InitUSART2(MB_SLAVE_UNIT);
    InitTIM3();
}	

void MBPollSlave( void )
{
    static unsigned char RcvAddress;
    unsigned int crc;
    BOOL isRcvAddressValid;
    
    eMBEventType eEvent;
    
    if( MBEventInQueue )
    {
        eEvent = MBQueuedEvent;
        MBEventInQueue = FALSE;
        
        switch ( eEvent )
        {
        case EV_FRAME_RECEIVED:
            { 
                RcvAddress = RecieveBuffer[0];
                isRcvAddressValid = MBSlaveAddressRecognition(RcvAddress);
                
                if(isRcvAddressValid == TRUE)
                {	
                    //copy global recieveBuffer in active slave's recieveBuffer for regular slave simulation
                    CopyModBusMemory(RecieveBuffer, ModBusSlaves[ActiveSlaveIndex].recieveBuffer, MBRcvBufferPos);
                    
                    crc = usMBCRC16((unsigned char *) ModBusSlaves[ActiveSlaveIndex].recieveBuffer, MBRcvBufferPos);
                    if( crc == 0 )
                    {
                        MBEventInQueue = TRUE;
                        MBQueuedEvent = EV_EXECUTE;     
                    } 
                }
                break;
            }
        case EV_EXECUTE:
            {	
                MB_handle_request();
                break;
            }   
        }
    }
}

void MBReceiveFSM( void )
{
    signed char Byte;
        
    Byte = (signed char)GetByte(USART_2);
    
    switch ( MBRcvState )
    {		
    case STATE_RX_IDLE:
        MBRcvBufferPos = 0;
        
        RecieveBuffer[MBRcvBufferPos++] = Byte;
        MBRcvState = STATE_RX_RCV;
        
        ModBusTimerEnable(T_10_MS); // 10 ms are enought for waiting one frame by 19200 baude rate
        break;
        
    case STATE_RX_RCV:
        if( MBRcvBufferPos < PACKET_SIZE )
        {
            RecieveBuffer[MBRcvBufferPos++] = Byte;
        }
        ModBusTimerEnable(T_10_MS);
        break;
    }	
}

void MB_handle_request( void )
{
    int mblen;
    unsigned int crc;
    
    mblen = 0;
    
    if(ModBusSlaves[ActiveSlaveIndex].recieveBuffer[0] == ModBusSlaves[ActiveSlaveIndex].address)
    {
        switch(ModBusSlaves[ActiveSlaveIndex].recieveBuffer[1])
        {
        case 1: //Read Coil Status
            mblen = process_cmd1();
            break;
        case 2: //Read Discrete Input
            mblen = process_cmd2();
            break;
        case 3: //Read Holding Registeers
            mblen = process_cmd3();
            break;
        case 5: //Force Single Coil
            mblen = process_cmd5();
            break;
        case 15: //Force Multiple Coils
            mblen = process_cmd15();
            break;
        case 16: //Preset Multiple Registers
            mblen = process_cmd16();
            break;
        default:
            return;
        }
        MBSndState = STATE_TX_XMIT;
    }
    
    if( MBSndState == STATE_TX_XMIT  )
    {
        crc = usMBCRC16(ResponseBuffer, mblen);
        ResponseBuffer[mblen + 0] = (unsigned char) crc;
        ResponseBuffer[mblen + 1] = (unsigned char) (crc >> 8);	
        MBSndBufferPos = mblen + 2; //2 - CRC_LEN
        
        //copy global ResponseBuffer in active slave's response buffer for regular slave simulation.
        CopyModBusMemory(ResponseBuffer, ModBusSlaves[ActiveSlaveIndex].responseBuffer, MBSndBufferPos);
    }
}

void MBTimerExpired( void )
{
    switch ( MBRcvState )
    {
    case STATE_RX_RCV:
        {
            MBEventInQueue = TRUE;
            MBQueuedEvent = EV_FRAME_RECEIVED;
            break;
        }
    default:  ;
    }
    ModBusTimerDisable();
    MBRcvState = STATE_RX_IDLE;
}

void MB_slave_transmit( void )
{
    if( MBSndState == STATE_TX_XMIT )
    {
        OutString(ResponseBuffer, MBSndBufferPos, USART_2, MB_SLAVE_TIMER, T_10_MS);
        MBSndState = STATE_TX_IDLE;
        
        //Clearing Recive buffers
        ClearModBusSlaveMemory(RecieveBuffer, PACKET_SIZE);
        ClearModBusSlaveMemory(ModBusSlaves[ActiveSlaveIndex].recieveBuffer, PACKET_SIZE);
        
        //Clearing Response
        ClearModBusSlaveMemory(ResponseBuffer, RESPONSE_SIZE);
        ClearModBusSlaveMemory(ModBusSlaves[ActiveSlaveIndex].responseBuffer, RESPONSE_SIZE);
    }
}

//Read Coil Status
char process_cmd1(void)
{
    unsigned short outputs = 0x0000, i;
    unsigned short temp;
    unsigned char bytesCount;
    
    if(RecieveBuffer[2] != 0) 
    {
        return 0; //check START ADDRESS HI is 0
    }
    if(RecieveBuffer[3] >= OUTPUTS_NUMBER) 
    {
        return 0;  //check START ADDRESS LO is [0:15]
    }
    if(RecieveBuffer[4] != 0) 
    {
        return 0; //check No of POINTS HI is 0
    }
    if((OUTPUTS_NUMBER - RecieveBuffer[3]) < RecieveBuffer[5]) 
    {
        return 0; //check No of POINTS LO
    }
    
    //read all cois' states
    for(i = 0; i < OUTPUTS_NUMBER; i++)
    {
        outputs |= (ModBusSlaves[ActiveSlaveIndex].outputs[i] & 0x01) << i;
    }
    
    //take desired action
    outputs = outputs >> RecieveBuffer[3];
    temp = (unsigned short)((1 << RecieveBuffer[5]) - 1);
    outputs &= temp;
    bytesCount = (RecieveBuffer[5] / 8) ? 2 : 1;
    
    //compose response
    ResponseBuffer[0] = RecieveBuffer[0]; // SLAVEID - same (already confirmed)
    ResponseBuffer[1] = RecieveBuffer[1]; // COMMANDID - same (already confirmed)
    ResponseBuffer[2] = bytesCount; // BYTECOUNT - is at max 2 bytes as we have 16 outputs
    ResponseBuffer[3] = (unsigned char)(outputs & 0x00FF); //get LO Byte
    
    if(bytesCount == 2)
    {
        ResponseBuffer[4] = (unsigned char)(outputs >> 8); //get HI Byte
    }
    
    return bytesCount + 3; //length of response;
}

//Read Discrete Input
char process_cmd2(void)
{
    unsigned short inputs = 0x0000, i;
    unsigned short temp;
    unsigned char bytesCount;
    
    if(RecieveBuffer[2] != 0) 
    {
        return 0; //check START ADDRESS HI is 0
    }
    if(RecieveBuffer[3] >= INPUTS_NUMBER) 
    {
        return 0;  //check START ADDRESS LO is [0:15]
    }
    if(RecieveBuffer[4] != 0) 
    {
        return 0; //check No of POINTS HI is 0
    }
    if((INPUTS_NUMBER - RecieveBuffer[3]) < RecieveBuffer[5]) 
    {
        return 0; //check No of POINTS LO
    }
    
    //read all cois' states
    for(i = 0; i < INPUTS_NUMBER; i++)
    {
        inputs |= (ModBusSlaves[ActiveSlaveIndex].inputs[i] & 0x01) << i;
    }
    
    //take desired action
    inputs = inputs >> RecieveBuffer[3];
    temp = (unsigned short)((1 << RecieveBuffer[5]) - 1);
    inputs &= temp;
    bytesCount = (RecieveBuffer[5] / 8) ? 2 : 1;
    
    //compose response
    ResponseBuffer[0] = RecieveBuffer[0]; // SLAVEID - same (already confirmed)
    ResponseBuffer[1] = RecieveBuffer[1]; // COMMANDID - same (already confirmed)
    ResponseBuffer[2] = bytesCount; // BYTECOUNT - is at max 2 bytes as we have 16 outputs
    ResponseBuffer[3] = (unsigned char)(inputs & 0x00FF); //get LO Byte
    
    if(bytesCount == 2)
    {
        ResponseBuffer[4] = (unsigned char)(inputs >> 8); //get HI Byte
    }
    
    return bytesCount + 3; //length of response;
}

//Read Holding Registeers
char process_cmd3(void)
{
    int i;
    
    if(RecieveBuffer[2] != 0) 
    {
        return 0; //check START ADDRESS HI is 0
    }
    if(RecieveBuffer[3] >= HOLDING_REGISTERS_NUMBER) 
    {
        return 0;  //check START ADDRESS LO is < HOLDING_REGISTERS_NUMBER
    }
    if(RecieveBuffer[4] != 0) 
    {
        return 0; //check no of points hi is 0
    }
    if((HOLDING_REGISTERS_NUMBER - RecieveBuffer[3]) < RecieveBuffer[5]) 
    {
        return 0; //check No of POINTS LO
    }    
    
    //compose response
    ResponseBuffer[0] = RecieveBuffer[0]; // SLAVEID - same (already confirmed)
    ResponseBuffer[1] = RecieveBuffer[1]; // COMMANDID - same (already confirmed)
    ResponseBuffer[2] = RecieveBuffer[5] * 2; // BYTECOUNT - is at max 200 bytes as we have 100 Hold. Reg.
    
    for(i = 0; i < RecieveBuffer[5]; i ++)
    {
        ResponseBuffer[3 + i * 2] = ModBusSlaves[ActiveSlaveIndex].holdingRegisters[i + RecieveBuffer[3]] >> 8;
        ResponseBuffer[4 + i * 2] = ModBusSlaves[ActiveSlaveIndex].holdingRegisters[i + RecieveBuffer[3]];
    }
    
    return 3 + RecieveBuffer[5] * 2;
}

//Force Single Coil
char process_cmd5(void)
{
    if(RecieveBuffer[2] != 0)
    {
        return 0; //check COIL ADDRESS HI is 0
    }
    if(RecieveBuffer[3] >= OUTPUTS_NUMBER) 
    {
        return 0;  //check COIL ADDRESS LO is [0:15]
    }
    if(RecieveBuffer[4] != 0xFF && RecieveBuffer[4] != 0x00) 
    {
        return 0; //check DATA HI
    }
    if(RecieveBuffer[5] != 0x00) 
    {
        return 0; //check DATA LO
    }
    
    //take desired action
    if(RecieveBuffer[4] == 0xFF)
    {
        ModBusSlaves[ActiveSlaveIndex].outputs[RecieveBuffer[3]] = 0x01;
    }
    else 
    {
        ModBusSlaves[ActiveSlaveIndex].outputs[RecieveBuffer[3]] = 0x00;
    }
    
    //compose response
    ResponseBuffer[0] = RecieveBuffer[0];
    ResponseBuffer[1] = RecieveBuffer[1];
    ResponseBuffer[2] = RecieveBuffer[2];
    ResponseBuffer[3] = RecieveBuffer[3];
    ResponseBuffer[4] = RecieveBuffer[4];
    ResponseBuffer[5] = RecieveBuffer[5];
    
    return 6;
}

//Force Multiple Coils
char process_cmd15(void)
{
    unsigned char bytesCount, i, j, currentByte, coilsCount, startAddress, ucSize;
    
    if(RecieveBuffer[2] != 0) 
    {
        return 0; //check COIL ADDRESS HI
    }
    if(RecieveBuffer[3] >= OUTPUTS_NUMBER) 
    {
        return 0; //check COIL ADDRESS LO is [0:15]
    }
    if(RecieveBuffer[4] != 0) 
    {
        return 0; //check QUANTITY HI
    }
    if((OUTPUTS_NUMBER - RecieveBuffer[3]) < RecieveBuffer[5]) 
    {
        return 0; //check QUANTITY LO
    }
    if(RecieveBuffer[5] > OUTPUTS_NUMBER) 
    {
        return 0; //check QUANTITY Coils - it must be less or equal then OUTPUTS_NUMBER
    }
    if(RecieveBuffer[6] > 2 || RecieveBuffer[6] < 1) //max Coils' number is 16 which is 2 Bytes
    {
        return 0; // check BYTE COUNT
    }
    
    ucSize = sizeof(unsigned char) * 8; //bits in 1 unsigned char
    startAddress = RecieveBuffer[3];
    coilsCount = RecieveBuffer[5];
    bytesCount = RecieveBuffer[6];
    
    for(i = 0; i < bytesCount; i++)
    {
        currentByte = RecieveBuffer[7 + i];
        
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
                ModBusSlaves[ActiveSlaveIndex].outputs[startAddress + i * ucSize + j] = (currentByte & (1 << j)) >> j; 
                coilsCount --;
            }
            else
            {
                break;
            }
        }
    }
    
    //compose response
    ResponseBuffer[0] = RecieveBuffer[0];
    ResponseBuffer[1] = RecieveBuffer[1];
    ResponseBuffer[2] = RecieveBuffer[2];
    ResponseBuffer[3] = RecieveBuffer[3];
    ResponseBuffer[4] = RecieveBuffer[4];
    ResponseBuffer[5] = RecieveBuffer[5];
    
    return 6;
}

//Preset Multiple Registers
char process_cmd16(void)
{
    unsigned char i;
    
    if(RecieveBuffer[2] != 0) 
    {
        return 0; //check START ADDRESS HI is 0
    }
    if(RecieveBuffer[3] >= HOLDING_REGISTERS_NUMBER) 
    {
        return 0;  //check START ADDRESS LO is < HOLDING_REGISTERS_NUMBER
    }
    if(RecieveBuffer[4] != 0) 
    {
        return 0; //check no of points hi is 0
    }
    if((HOLDING_REGISTERS_NUMBER - RecieveBuffer[3]) < RecieveBuffer[5]) 
    {
        return 0; //check No of POINTS LO
    }  
    
    for (i = 0; i < RecieveBuffer[5]; i ++)
    {
        ModBusSlaves[ActiveSlaveIndex].holdingRegisters[RecieveBuffer[3] + i] = (unsigned short)RecieveBuffer[7 + i * 2] << 8;
        ModBusSlaves[ActiveSlaveIndex].holdingRegisters[RecieveBuffer[3] + i] |= RecieveBuffer[8 + i * 2];
    }	
    
    //compose response
    ResponseBuffer[0] = RecieveBuffer[0];
    ResponseBuffer[1] = RecieveBuffer[1];
    ResponseBuffer[2] = RecieveBuffer[2];
    ResponseBuffer[3] = RecieveBuffer[3];
    ResponseBuffer[4] = RecieveBuffer[4];
    ResponseBuffer[5] = RecieveBuffer[5];
    
    return 6;
}
