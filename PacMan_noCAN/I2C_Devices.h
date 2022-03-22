/**
 * I2C_Devices.h - The configuration file for all peripheral I2C devices on PacMan
 * Author: Justin Kogasaka
 * Date: 3/12/2022
 */

#ifndef I2C_Devices_h
#define I2C_Devices_h

// Define MCP23008 addresses
#define MCP23008_IODIR          0x00
#define MCP23008_IPOL           0x01
#define MCP23008_GPINTEN        0x02
#define MCP23008_DEFVAL         0x03
#define MCP23008_INTCON         0x04
#define MCP23008_IOCON          0x05
#define MCP23008_GPPU           0x06
#define MCP23008_INTF           0x07
#define MCP23008_INTCAP         0x08
#define MCP23008_GPIO           0x09
#define MCP23008_OLAT           0x0A

void writeMCP23008(uint8_t reg, uint8_t val);
uint8_t readMCP23008();
void initMCP23008();

// Define LTC4151 addresses
#define LTC4151_SENSE_MSB       0x00
#define LTC4151_SENSE_LSB       0x01
#define LTC4151_VIN_MSB         0x02
#define LTC4151_VIN_LSB         0x03
#define LTC4151_ADIN_MSB        0x04
#define LTC4151_ADIN_LSB        0x05
#define LTC4151_CONTROL         0x06

// Define LTC4151 current measuring resistance
#define LTC4151_RSHUNT          0.82

uint8_t readLTC4151(uint8_t reg);
uint16_t readLTC4151PacManCurrent();
uint16_t readLTC4151SupplyVoltage();
uint16_t readLTC4151ADCVoltage();
void initLTC4151();

#endif
