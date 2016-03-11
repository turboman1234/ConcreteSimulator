#ifndef _RS232_H
#define _RS232_H

BOOL RS232SlaveAddressRecognition(unsigned char recieveAddress);
void RS232InitHardwareAndProtocol(void);
void RS232PollSlave( void );
void RS232ReceiveFSM( void );
void RS232TimerExpired( void );
void RS232_handle_request( void );
void RS232_slave_transmit( void );
char RS232_process_cmd1(void);
char RS232_process_cmd3(void);
char RS232_process_cmd2(void);
char RS232_process_cmd5(void);
char RS232_process_cmd15(void);
char RS232_process_cmd16(void);

#endif
