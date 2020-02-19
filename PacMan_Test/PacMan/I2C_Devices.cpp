#include "I2C_Devices.h"

#include "Arduino.h"
#include "Wire.h"

#include "PacMan.h"



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
	Wire.beginTransmission(addr);
	Wire.write((byte)reg);
	Wire.endTransmission();
	Wire.requestFrom(addr, 1);
	return Wire.read();
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

}



// LTC4151

// Configures the LTC4151
void LTC4151_setup()
{
	i2cWriteByteToMem(I2C_ADDR_LTC4151, LTC4151_CONTROL, 0x00);
}

// Returns the measured Vin as a uint16_t in steps of 25mV with full-scale of 102.4V
uint16_t LTC4151_getVinRaw()
{
	uint8_t byte_h = i2cReadByteFromMem(I2C_ADDR_LTC4151, LTC4151_VIN_H);
	Serial.println();
	Serial.println(byte_h);
	uint8_t byte_l = i2cReadByteFromMem(I2C_ADDR_LTC4151, LTC4151_VIN_L);
	Serial.println(byte_l);
	return (uint16_t)(((((uint16_t)byte_h << 8) | byte_l) >> 4) & 0x0FFF);
}

// Returns the measured Vin as a float scaled for a full-scale of 102.4V
float LTC4151_getVin()
{
	uint8_t byte_h = i2cReadByteFromMem(I2C_ADDR_LTC4151, LTC4151_VIN_H);
	Serial.println();
	Serial.println(byte_h);
	uint8_t byte_l = i2cReadByteFromMem(I2C_ADDR_LTC4151, LTC4151_VIN_L);
	Serial.println(byte_l);
	return (float)(((((uint16_t)byte_h << 8) | byte_l) >> 4) & 0x0FFF) * 0.025;
}

// Returns the measured Vshunt as a uint16_t in steps of 20uV with full-scale of 81.92mV
uint16_t LTC4151_getVSenseRaw()
{
	uint8_t byte_h = i2cReadByteFromMem(I2C_ADDR_LTC4151, LTC4151_SENSE_H);
	uint8_t byte_l = i2cReadByteFromMem(I2C_ADDR_LTC4151, LTC4151_SENSE_L);
	return (uint16_t)(((((uint16_t)byte_h << 8) | byte_l) >> 4) & 0x0FFF);
}

// Returns a current in A generated from multiplying the sense resistance with the shunt voltage
float LTC4151_getCurrent()
{
	uint8_t byte_h = i2cReadByteFromMem(I2C_ADDR_LTC4151, LTC4151_SENSE_H);
	Serial.println();
	Serial.println(byte_h);
	uint8_t byte_l = i2cReadByteFromMem(I2C_ADDR_LTC4151, LTC4151_SENSE_L);
	Serial.println(byte_l);
	return (float)(((((uint16_t)byte_h << 8) | byte_l) >> 4) & 0x0FFF) * 0.00002 / 0.82;
}

// Returns the measured ADin as a uint16_t in steps of 500uV with full-scale of 2.048V
uint16_t LTC4151_getADinRaw()
{
	uint8_t byte_h = i2cReadByteFromMem(I2C_ADDR_LTC4151, LTC4151_ADIN_H);
	uint8_t byte_l = i2cReadByteFromMem(I2C_ADDR_LTC4151, LTC4151_ADIN_L);
	return (uint16_t)(((((uint16_t)byte_h << 8) | byte_l) >> 4) & 0x0FFF);
}

// Returns the measured ADin as a float scaled for a full-scale of 2.048V
float LTC4151_getADin()
{
	uint8_t byte_h = i2cReadByteFromMem(I2C_ADDR_LTC4151, LTC4151_ADIN_H);
	uint8_t byte_l = i2cReadByteFromMem(I2C_ADDR_LTC4151, LTC4151_ADIN_L);
	return (float)(((((uint16_t)byte_h << 8) | byte_l) >> 4) & 0x0FFF) * 0.0005;
}