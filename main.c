#include "main.h"

int main()
{
	uint8_t address = 0x0000;
	Clk_Init();
	i2c_init();
	UART2_Init();
	WriteToEEPROM(address,'A');
	Send_Char(ReadFromEEPROM(address));
 return 0;
}

void delay(uint16_t tms)
{
	uint16_t i,j;
	for(i=0;i<tms;i++)
	for(j=0;j<=1000;j++);
}
