#include "Arduino.h"
#include "Wire.h"
#include "MCP23008.h"

MCP23008::MCP23008()
{
  
}

void MCP23008::configure(uint8_t reg, uint8_t value)
{
  write8(reg, value);
}

uint8_t MCP23008::readGPIO()
{
  return read8(MCP23008_GPIO);
}

void MCP23008::writeGPIO(uint8_t value)
{
  write8(MCP23008_GPIO, value);
}

void MCP23008::digitalWrite(uint8_t pin, uint8_t value) {
  uint8_t gpio = readGPIO();

  if (value == HIGH)
  {
    gpio |= 1 << pin; 
  }
  else
  {
    gpio &= ~(1 << pin);
  }

  writeGPIO(gpio);
}

uint8_t MCP23008::digitalRead(uint8_t pin) {
  return (readGPIO() >> pin) & 0x1;
}

uint8_t MCP23008::read8(uint8_t addr)
{
  Wire.beginTransmission(MCP23008_ADDRESS);
  Wire.write((byte)addr);
  Wire.endTransmission();
  Wire.requestFrom(MCP23008_ADDRESS, 1);
  return Wire.read();
}


void MCP23008::write8(uint8_t addr, uint8_t data)
{
  Wire.beginTransmission(MCP23008_ADDRESS);
  Wire.write((byte)addr);
  Wire.write((byte)data);
  Wire.endTransmission();
}
