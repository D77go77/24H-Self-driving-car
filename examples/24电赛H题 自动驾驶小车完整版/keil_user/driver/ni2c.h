#ifndef __NI2C_H
#define __NI2C_H




void single_writei2c(unsigned char SlaveAddress,unsigned char REG_Address,unsigned char REG_data);
unsigned char single_readi2c(unsigned char SlaveAddress,unsigned char REG_Address);
void i2creadnbyte(uint8_t addr, uint8_t regAddr, uint8_t *data, uint8_t length);


#endif



