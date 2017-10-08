#ifndef I2C_H_
#define I2C_H_

#define EEPROM_ADDRESS 0xA0
#define read  (uint8_t)0x01
#define write (uint8_t)0x00



void i2c_init(void);
void i2c_start(void);
void i2c_stop(void);
void i2c_TxMode_En(void);
void i2c_RxMode_En(void);
void rep_start(void);

int ack_wait(void);
int bus_busy(void);
int tx_comp(void);

void i2c_Send_Byte(uint8_t data);
uint8_t i2c_Read_Byte(void);

void WriteToEEPROM(uint16_t eeaddress, uint8_t data);
uint8_t ReadFromEEPROM(uint16_t eeaddress);




#endif
