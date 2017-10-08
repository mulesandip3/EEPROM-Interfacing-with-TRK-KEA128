#include <SKEAZ1284.h>
#include "uart2.h"

#define TDRE 1<<7
#define RDRF 1<<5

char buf[100],c;
unsigned int i;

void UART2_Init(void)
{
	 SIM_PINSEL1 |= !SIM_PINSEL1_UART2PS_MASK;  /*0(Rx = PTD6  and Tx = PTD7) 1(Rx = PTI0 and Tx = PTI1)*/
	 SIM_SCGC |=  SIM_SCGC_UART2_MASK;		     /* Enable bus clock in UART2*/
	 UART2_BDH = 0;							               /* One stop bit*/
	 UART2_BDL = 128;						               /* Baud rate at 9600*/
	 UART2_C1  = 0;							               /* No parity enable,8 bit format*/
	 UART2_C2 |= UART_C2_TE_MASK;			         /* Enable Transmitter*/
	 UART2_C2 |= UART_C2_RE_MASK;			         /* Enable Receiver*/
	 UART2_C2 |= UART_C2_RIE_MASK;	
}

char isEmpty(void)  // buffer is empty to write
{
   if(UART2_S1 & TDRE)
			return 1;
		else
			return 0;
}

void Send_Char(char ch)
{
		while((isEmpty()) == 0);
		UART2_D = ch;
} 
				
unsigned int puts(char *str)
{
		i=0;
		while( str[i] != '\0')
			Send_Char(str[i++]);
		return i;
}
	
