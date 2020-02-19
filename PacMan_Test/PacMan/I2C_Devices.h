#ifndef I2C_Devices_h
#define I2C_Devices_h

#include "Arduino.h"
#include "Wire.h"

#include "PacMan.h"

// Generic
void i2cWriteByteToMem(uint8_t addr, uint8_t reg, uint8_t value);
uint8_t i2cReadByteFromMem(uint8_t addr, uint8_t reg);


// MCP23008
void MCP23008_setup();
uint8_t MCP23008_readGPIO();


// LTC4151
void LTC4151_setup();

uint16_t LTC4151_getVinRaw();
float LTC4151_getVin();

uint16_t LTC4151_getVSenseRaw();
float LTC4151_getCurrent();

uint16_t LTC4151_getADinRaw();
float LTC4151_getADin();

#endif