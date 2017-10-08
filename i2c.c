#include "main.h"

uint8_t ack1,ack2,ack3,ack4,ack5,ack6,ack7,ack8,ack9;


void i2c_init(void)
{
	SIM_PINSEL0 |= !SIM_PINSEL_I2C0PS_MASK;  /* I2C0_SDA PTA2* & I2C0_SCL PTA3 */
	SIM->SCGC |= SIM_SCGC_I2C0_MASK;         /* Enable bus clock to the I2C0 i.e 20MHz*/
   
	I2C0->A1 |= 0xA0;                        /* EEPROM Address */
	
	I2C0->C2 |= !I2C_C2_GCAEN_MASK;          /* Genrel Call Disaable */
	I2C0->C2 |= !I2C_C2_ADEXT_MASK;          /* 7 bit slave address device */
	I2C0->C2 |= !I2C_C2_SBRC_MASK;           /* Slave follow master baude rate */
	I2C0->C2 |= !I2C_C2_RMEN_MASK;           /* Range address matching is Disabled */
	
	I2C0->F |= 0x8D;                       /*SCL Divider = 48  Mult = 4, ICR  = 0x0D So that BR = 100 Kb/s*/
	
	I2C0->C1 |= I2C_C1_IICEN_MASK;          /* I2C0 Enable */
	I2C0->C1 |= !I2C_C1_IICIE_MASK;         /* I2C0 Interrupt Disabled */
	I2C0->C1 |= I2C_C1_TXAK_MASK;           /* NACK Is sent following byte reception */
	I2C0->C1 |= !I2C_C1_WUEN_MASK;          /* Normal Operation*/
	
	I2C0->FLT |= 0x00;                     /* filter glitch resister Stop Hold Enable*/
	I2C0->RA  |= 0;
	I2C0->SMB |= 0;                     /*if FACK = 0 then ACK or NACK Is sent following byte reception */  
	I2C0->A2  |= 0;
	I2C0->SLTH = 0;
	I2C0->SLTL = 0;
	
}

void i2c_start(void)
{
	I2C0->C1 |= !I2C_C1_MST_MASK; /* MST = 0 */
	I2C0->C1 |= I2C_C1_MST_MASK;  /* MST = 1 */
}

void i2c_stop(void)
{
	I2C0->C1 |= I2C_C1_MST_MASK;  /* MST = 1 */
	I2C0->C1 &= !I2C_C1_MST_MASK; /* MST = 0 */
}

void rep_start(void)
{
	I2C0->C1 = I2C_C1_RSTA_MASK;    /* Repeat start to indicate that i am current master  */
}

void i2c_TxMode_En(void)
{
	I2C0->C1 |= I2C_C1_TX_MASK; /* Transmitt mode enable */
}

void i2c_RxMode_En(void)
{
	I2C0->C1 |= !I2C_C1_TX_MASK; /* Recieve  mode enable */	
}

int ack_wait(void)
{
	if((I2C0->S & I2C_S_RXAK_MASK) == I2C_S_RXAK_MASK)
	{
		return 1; // NACK is detected 
	}
	else
	{
		return 0; //  ACK is detected
	}
}

int bus_busy(void)
{
	if((I2C0->S & I2C_S_BUSY_MASK) == I2C_S_BUSY_MASK)
	{
		return 1; // bus is busy
	}
	else
	{
		return 0; // bus is ideal
	}
	
}

int tx_comp(void)
{
  if ((I2C0->S & I2C_S_TCF_MASK) == I2C_S_TCF_MASK)
	{
		 return 1; //  transmmision is complete
	}
	else 
	{
		 return 0; // transmission is in progress 
	}
}

void i2c_Send_Byte(uint8_t data)
{	
	  while(!tx_comp()); //wait transfer is in progress
	  i2c_TxMode_En();
		I2C0->D = data; //Master Tx and  Slave Rx 
}

uint8_t i2c_Read_Byte(void)
{
	  while((tx_comp()) == 0); //wait transfer is in progress
	  i2c_RxMode_En();
	  return  I2C0->D;   //Slave Tx and Master Rx
}

void WriteToEEPROM( uint16_t eeaddress, uint8_t data)
{
	uint8_t add_h;
	uint8_t add_l;
	add_h = (uint8_t)eeaddress >> 8;       // msb
	add_l = (uint8_t)eeaddress & 0x00FF;   // lsb
	puts("Byte write to EEPROM \n");
	
	i2c_start();
	i2c_Send_Byte(EEPROM_ADDRESS);         // Slave Address
	i2c_Send_Byte(write);                  // master tx and slave rx
	ack1 = ack_wait();                     // ack recieved 
	puts("ack1 : ");
	Send_Char(ack1);
	puts("\n");
	
	i2c_Send_Byte(add_h);                 // MSB
	ack2 = ack_wait();                    // ack recieved 
	puts("ack2 : ");
	Send_Char(ack2);
	puts("\n");
	
	i2c_Send_Byte(add_l);                 // LSB
	ack3 = ack_wait();                    // ack recieved 
	puts("ack3 : ");
	Send_Char(ack3);
	puts("\n");
	
	i2c_Send_Byte(data);                 // Data  transmitted
	ack4 = ack_wait();                   // ack recieved
	puts("ack4 : ");
	Send_Char(ack4);
	puts("\n");
	
	puts("TxData: ");
	Send_Char(data);
	puts("\n");
	i2c_stop();
	delay(100);
}

uint8_t ReadFromEEPROM( uint16_t eeaddress)
{
	puts("Byte Read from EEPROM \n");
	char Rx_Data = 0xFF;
	uint8_t add_h;
	uint8_t add_l;
	add_h = (uint8_t)eeaddress >> 8;       // msb
	add_l = (uint8_t)eeaddress & 0x00FF;   // lsb
	
	i2c_start();                           // to communicate with slave in tx mode
	i2c_Send_Byte(EEPROM_ADDRESS);         // Slave Address
	i2c_Send_Byte(write);                  // master tx and slave rx
	ack5 = ack_wait();                     // ack recieved 
	puts("ack5 : ");
	Send_Char(ack5);
	puts("\n");
	
	i2c_Send_Byte(add_h);                 // MSB
	ack6 = ack_wait();                    // ack recieved 
	puts("ack6 : ");
	Send_Char(ack6);
	puts("\n");
	
	i2c_Send_Byte(add_l);                 // LSB
	ack7 = ack_wait();                    // ack recieved 
	puts("ack7 : ");
	Send_Char(ack7);
	puts("\n");
	
	i2c_start();
	i2c_Send_Byte(EEPROM_ADDRESS);       // Slave Address
	i2c_Send_Byte(read);                 // master rx and slave tx
	ack8 = ack_wait();                   // ack recieved
	puts("ack8 : ");
	Send_Char(ack8);
	puts("\n");
	if(!bus_busy())
	{
			Rx_Data = i2c_Read_Byte();           // Recived data
	}
	ack9 = ack_wait();                   // ack recieved
	puts("ack9 : ");
	Send_Char(ack9);
	puts("\n");
	puts("Rx_Data : ");
	i2c_stop();
	return Rx_Data;
}

	
