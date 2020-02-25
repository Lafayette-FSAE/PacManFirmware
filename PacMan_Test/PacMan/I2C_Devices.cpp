#include "I2C_Devices.h"



// Generic

// Transmits a byte to the specified memory location of an I2C device
void i2cWriteByteToMem(uint8_t addr, uint8_t reg, uint8_t value)
{
	Wire.beginTransmission(addr);
	Wire.write((byte)reg);
	Wire.write((byte)value);
	Wire.endTransmission();
}

// Reads a byte from the specified memory location of an I2C device
uint8_t i2cReadByteFromMem(uint8_t addr, uint8_t reg)
{
  Serial.print("Reg: ");
  Serial.println(reg);
	Wire.beginTransmission(addr);
	Wire.write((byte)reg);
	byte error = Wire.endTransmission();
	Serial.print("Error code: ");
	Serial.println(error);
	Wire.requestFrom(addr, 1);
//	return Wire.read();
  while (Wire.available()) {
    Serial.println(Wire.read());
  }
  return -1;
}



// MCP23008

// Configures the MCP23008 I/O expander
void MCP23008_setup()
{
	i2cWriteByteToMem(I2C_ADDR_MCP23008, MCP23008_IODIR,   0x0E);
	i2cWriteByteToMem(I2C_ADDR_MCP23008, MCP23008_IPOL,    0x0E);
	i2cWriteByteToMem(I2C_ADDR_MCP23008, MCP23008_GPINTEN, 0x0E);
	i2cWriteByteToMem(I2C_ADDR_MCP23008, MCP23008_DEFVAL,  0x00);
	i2cWriteByteToMem(I2C_ADDR_MCP23008, MCP23008_INTCON,  0x00);
	i2cWriteByteToMem(I2C_ADDR_MCP23008, MCP23008_IOCON,   0x32);
	i2cWriteByteToMem(I2C_ADDR_MCP23008, MCP23008_GPPU,    0x00);
}

// Reads the GPIO port
uint8_t MCP23008_readGPIO()
{
	return i2cReadByteFromMem(I2C_ADDR_MCP23008, MCP23008_GPIO);
}


// LTC4151
uint16_t LTC4151_Vsense  = 0;
uint16_t LTC4151_Vin	 = 0;
uint16_t LTC4151_ADin	 = 0;

void LTC4151_setup()
{

}

void LTC4151_update()
{
	Wire.requestFrom(I2C_ADDR_LTC4151, 6);
	
	uint16_t senseH = Wire.read();
	uint16_t senseL = Wire.read();
	uint16_t vinH   = Wire.read();
	uint16_t vinL   = Wire.read();
	uint16_t ADinH  = Wire.read();
	uint16_t ADinL  = Wire.read();
	
	LTC4151_Vsense = senseH << 4 | senseL >> 4;
	LTC4151_Vin    = vinH   << 4 | vinL   >> 4;
	LTC4151_ADin   = ADinH  << 4 | ADinL  >> 4;
}

// Returns the measured input voltage
float LTC4151_getVoltage()
{
	return LTC4151_Vin * LTC4151_VIN_FACTOR;
}

// Returns the calculated current drawn in A through the shunt
float LTC4151_getCurrentA()
{
	return LTC4151_Vsense * LTC4151_CURR_FACTOR_A;
}

// Returns the calculated current drawn in mA through the shunt
float LTC4151_getCurrentmA()
{
	return LTC4151_Vsense * LTC4151_CURR_FACTOR_mA;
}

// Returns the calculated current drawn through the shunt
float LTC4151_getDiff()
{
	return LTC4151_ADin * LTC4151_AD_FACTOR;
}