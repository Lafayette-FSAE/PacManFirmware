#ifndef _MCP23008_H
#define _MCP23008_H

#include <Wire.h>

// Device I2C address
#define MCP23008_ADDRESS 0x20

// Register addresses
#define MCP23008_IODIR 0x00
#define MCP23008_IPOL 0x01
#define MCP23008_GPINTEN 0x02
#define MCP23008_DEFVAL 0x03
#define MCP23008_INTCON 0x04
#define MCP23008_IOCON 0x05
#define MCP23008_GPPU 0x06
#define MCP23008_INTF 0x07
#define MCP23008_INTCAP 0x08
#define MCP23008_GPIO 0x09
#define MCP23008_OLAT 0x0A

class MCP23008
{
public:
  void init();
  uint8_t readGPIO();
  void digitalWrite(uint8_t pin, uint8_t value);
  uint8_t digitalRead(uint8_t pin);

private:
  uint8_t read8(uint8_t addr);
  void write8(uint8_t addr, uint8_t data);
};

#endif
