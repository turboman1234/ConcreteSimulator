#ifndef _MBSLAVE_H
#define _MBSLAVE_H

#include "definitions.h"

// -------- Modbus Address ranges ---------------------
#define HOLDING_REGISTERS_NUMBER        			100
#define INPUTS_NUMBER                                           16
#define OUTPUTS_NUMBER                                          16
#define MAX_MODBUS_SLAVE_DEVICES                                10

#define PACKET_SIZE		                                256
#define RESPONSE_SIZE 	                                        256

#define INVALID_SLAVE_INDEX                                     0x0000FFFF


typedef struct Slave{
    unsigned char address;
    unsigned char inputs[INPUTS_NUMBER];
    unsigned char outputs[OUTPUTS_NUMBER];
    unsigned short holdingRegisters[HOLDING_REGISTERS_NUMBER];
    unsigned char responseBuffer[RESPONSE_SIZE];
    unsigned char recieveBuffer[PACKET_SIZE];
    BOOL isSlaveActive;
}ModBusSlaveUnit;


void InitNewMBSlaveDevices(void);
void ClearModBusSlaveMemory(unsigned char *pMemory, int size);
void CopyModBusMemory(unsigned char *source, unsigned char *destination, unsigned short cellsNumber);
BOOL MBSlaveAddressRecognition(unsigned char recieveAddress);

void MBInitHardwareAndProtocol(void);
void MBPollSlave( void );
void MBReceiveFSM( void );
void MB_handle_request( void );
void MBTimerExpired( void );
void MB_slave_transmit( void );
char process_cmd1(void);
char process_cmd3(void);
char process_cmd2(void);
char process_cmd5(void);
char process_cmd15(void);
char process_cmd16(void);

#endif
