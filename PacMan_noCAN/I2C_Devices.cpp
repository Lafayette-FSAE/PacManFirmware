/**
 * I2C_Devices.cpp - The main file for all peripheral I2C devices on PacMan
 * Author: Justin Kogasaka
 * Date: 3/12/2022
 */

#include "PacMan_config.h"
#include "I2C_Devices.h"

/* Writes a byte val to the register reg on the MCP23008 */
void writeMCP23008(uint8_t reg, uint8_t val) {
  Wire.beginTransmission(I2C_ADDR_MCP23008);
  Wire.write((byte) reg);
  Wire.write((byte) val);
  Wire.endTransmission();
}

/**
 * Reads the GPIO pin output of the MCP23008
 * 
 * @return  GPIO register byte
 */
uint8_t readMCP23008() {
  Wire.beginTransmission(I2C_ADDR_MCP23008);
  Wire.write((byte) MCP23008_GPIO);
  byte error = Wire.endTransmission();
  if (error == 4) {
    Serial.println("MCP23008 read error");
  }
  Wire.requestFrom(I2C_ADDR_MCP23008, 1);
  return Wire.read();
}

/* Configure the MCP23008 registers for PacMan application */
void initMCP23008() {
  byte iodir = 0x0F;
  byte ipol = 0x0F;
  byte gppu = 0x00;
  writeMCP23008(MCP23008_IODIR, iodir);
  writeMCP23008(MCP23008_IPOL, ipol);
  writeMCP23008(MCP23008_GPPU, gppu);
//  writeMCP23008(MCP23008_GPIO, 0x40); // Arbitrary write to GPIO6 pin for testing
}

/** 
 *  Reads the value stored in the LTC4151 ADC register
 *  
 *  @param reg  The register in the LTC4151 to read
 *  @return     The byte stored in the specified register
 */
uint8_t readLTC4151(uint8_t reg) {
  Wire.beginTransmission(I2C_ADDR_LTC4151);
  byte error = Wire.endTransmission();
  if (error == 4) {
    Serial.println("LTC4151 read error");
  }
  Wire.requestFrom(I2C_ADDR_LTC4151, 1);
  return Wire.read();
}

/** 
 *  Reads the LTC4151 load current on SENSE+ and SENSE- across the shunt resistor
 *  Can detect load currents up to 100 mA for the 0.82 ohm shunt resistor
 *  
 *  @return   The current drawn by PacMan from the battery stack in mA
 */
uint16_t readLTC4151PacManCurrent() {
  uint8_t msb = readLTC4151(LTC4151_SENSE_MSB);
  uint8_t lsb = readLTC4151(LTC4151_SENSE_LSB);
  uint16_t result = (msb << 4) | (lsb >> 4);              // Last 4 bits of LSB are reserved so we only use 12 most significant bits
  result = result / (float)4096 * 81.92 / LTC4151_RSHUNT; // result/4096 is the voltage fraction of the full range voltage 81.92 mV. Convert to current by dividing by shunt resistance
  return result;
}

/** 
 *  Reads the LTC4151 input voltage (V) relative to RTN_TSV = PACK_RTN
 *  
 *  @return   The supply voltage powering the LTC4151 in V
 */
uint16_t readLTC4151SupplyVoltage() {
  uint8_t msb = readLTC4151(LTC4151_VIN_MSB);
  uint8_t lsb = readLTC4151(LTC4151_VIN_LSB);
  uint16_t result = (msb << 4) | (lsb >> 4);
  result = result / (float)4096 * 102.4;      // Full voltage range is 102.4 V
  return result;
}

/** 
 *  Reads the LTC4151 ADC input voltage (V) from BBM-01 current sensor
 *  The 0.5 gain from the INA592 is corrected to return (DCHRG_A1 - DCHARG_A2)
 *  
 *  @return   The ADC voltage from the BBM-01 current sensor in V
 */
uint16_t readLTC4151ADCVoltage() {
  uint8_t msb = readLTC4151(LTC4151_ADIN_MSB);
  uint8_t lsb = readLTC4151(LTC4151_ADIN_LSB);
  uint16_t result = (msb << 4) | (lsb >> 4);
  result = result / (float)4096 * 2.048 * 2;    // Full voltage range is 2.048 V. INA592 has a gain of 0.5 so multiply by 2 for correction
  return result;
}

/* Configures the control register of the LTC4151 for continuous polling */
void initLTC4151() {
  Wire.beginTransmission(I2C_ADDR_LTC4151);
  Wire.write(LTC4151_CONTROL);
  Wire.write(0x00);
  Wire.endTransmission();
}
