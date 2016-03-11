#include "mbcrc.h"

unsigned int usMBCRC16(unsigned char *Frame, int length)
{
	unsigned int CRC, carry, j, i;
	CRC = 0xFFFF;
	for(j = 0; j < length; j++)
	{
		CRC = CRC ^ Frame[j];
		for (i = 0; i < 8; i++)
		{
			carry = 0x0001 & CRC;
			CRC = CRC >> 1;
			if(carry)
			{
				CRC = CRC ^ 0xA001;
			}
		}
	}
	return CRC;
}
