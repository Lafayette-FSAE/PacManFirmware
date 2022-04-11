/**
 * Core1.cpp - Main file for managing data collection and processing from CellMen
 * Author: Justin Kogasaka
 * Date: 2/19/2022
 */

#include "PacMan_config.h"
#include "I2C_Devices.h"
#include "Core1.h"

// Local variables
bool lowVolt;
bool highVolt;
bool lowTemp;
bool highTemp;
bool chargeSafe;
bool chargerPresent;
uint8_t MCP23008gpio;

/**
 * Constructor
 * 
 * @param registers   A pointer to an external PacManRegisters object
 */
Core1::Core1(PacManRegisters* registers) {
  // Initialize variables
  reg = registers;
  numCellMenFound = 0;
  for (int i = 0; i < 16; i++) {
    cellFaults[i] = 0;
  }

  // Safety loop timers; when the timer expires, calls openSafetyLoopCallback
  lowVoltageTimer =  xTimerCreate("lowVoltageTimer",  pdMS_TO_TICKS(TRIGGER_TIME*1000), pdFALSE, 0, openSafetyLoopCallback);
  highVoltageTimer = xTimerCreate("highVoltageTimer", pdMS_TO_TICKS(TRIGGER_TIME*1000), pdFALSE, 0, openSafetyLoopCallback);
  lowTempTimer =     xTimerCreate("lowTempTimer",     pdMS_TO_TICKS(TRIGGER_TIME*1000), pdFALSE, 0, openSafetyLoopCallback);
  highTempTimer =    xTimerCreate("highTempTimer",    pdMS_TO_TICKS(TRIGGER_TIME*1000), pdFALSE, 0, openSafetyLoopCallback);
  
  Serial.println("Core 1 built");
}

/**
 * Searches for CellMen on I2C bus and records the I2C addresses of the CellMen that are found
 * Updates the PacManRegisters with number of cells
 */
void Core1::findCellMen() {
  // Search through all 7-bit I2C addresses
  for (int addr = 0; addr < 127; addr++) {
    // Probe address and record transmission status
    Wire.beginTransmission(addr);
    byte error = Wire.endTransmission();

    // Diagnose transmission status
    if (error == 0 && addr != I2C_ADDR_MCP9804 && addr != I2C_ADDR_MCP23008 && addr != I2C_ADDR_BQ32002 && addr != I2C_ADDR_LTC4151) {
      Serial.print("CellMan found at address 0x");
      Serial.println(addr, HEX);
      cellAddr[numCellMenFound] = addr; // Record CellMan address to array
      numCellMenFound++;
    }
    else if (error == 4) {
      Serial.print("I2C error at address 0x");
      Serial.println(addr, HEX);
    }
  }
  reg->numCells = numCellMenFound;
  Serial.print("Number of CellMen found: ");
  Serial.println(numCellMenFound);
}

/* Writes the state of the status LED on a CellMan at the specified address */
void Core1::writeCellManLED(unsigned char addr, bool state) {
  if (state) { // LED on
    Wire.beginTransmission(addr);
    Wire.write(0x23); // 0x23 is the LED register
    Wire.write(0x00); // MSB
    Wire.write(0x01); // LSB
    Wire.endTransmission();
  } else { // LED off
    Wire.beginTransmission(addr);
    Wire.write(0x23); // 0x23 is the LED register
    Wire.write(0x00); // MSB
    Wire.write(0x00); // LSB
    Wire.endTransmission();
  }
}

/* Flash the status LED on all CellMen that were found */
void Core1::indicateCellMen() {
  for (int count = 0; count < 4; count++) { // Flash four times
    for (int i = 0; i < numCellMenFound; i++) {
      writeCellManLED(cellAddr[i], true);
    }
    delay(250);
    for (int i = 0; i < numCellMenFound; i++) {
      writeCellManLED(cellAddr[i], false);
    }
    delay(250);
  }
}

/* Polls data from CellMen on the I2C bus and records voltage and temperature values */
void Core1::pollCellMen() {
  unsigned char cellData[MSG_LEN] = {0};
  // Request data from each CellMan
  for (int i = 0; i < numCellMenFound; i++) {
    Wire.requestFrom(cellAddr[i], MSG_LEN);
    for (int j = 0; j < MSG_LEN; j++) {
      cellData[j] = Wire.read();
    }

    // Process data
    reg->cellV[i] = (uint16_t)((cellData[2] << 8) + cellData[1]); // Cell voltage in mV
    reg->cellT[i] = (uint16_t)((cellData[4] << 8) + cellData[3]); // Cell temp in 1/10 oC
    Serial.print("Cell ");
    Serial.println(i);
    Serial.print("\tVoltage (V): ");
    Serial.println(cellV[i]/(float)1000); // Convert from mV to V for reading
    Serial.print("\tTemp (oC): ");
    Serial.println(cellT[i]/(float)10);   // Convert from 0.1 oC to oC for reading
  }
}

/**
 * Assign measured values to the PacManRegisters object to be read from display
 * Poll MCP23008 to record optoisolator statuses
 */
void Core1::updateRegisters() {
  uint16_t packV = 0;
  uint16_t avgTemp = 0;
  // Calculate pack voltage and average voltage
  for (int i = 0; i < numCellMenFound; i++) {
    packV += cellV[i];
    avgTemp += cellT[i];
  }
  
  // Update register values
  reg->packVoltage = packV;
  reg->avgCellVoltage = packV/numCellMenFound;
  reg->avgCellTemp = avgTemp/numCellMenFound;
  Serial.print("Pack voltage (V): ");
  Serial.println(reg->packVoltage/(float)1000);     // Convert from mV to V for reading
  Serial.print("Average cell voltage (V): ");
  Serial.println(reg->avgCellVoltage/(float)1000);  // Convert from mV to V for reading
  Serial.print("Average cell temp (oC): ");
  Serial.println(reg->avgCellTemp/(float)10);       // Convert from 0.1 oC to oC for reading

  // MCP23008 status poll
  uint8_t packStatus = readMCP23008();
  if (packStatus & 0x02) {
    reg->AIRSStatus = true;
    Serial.println("AIRs closed");
  } else {
    reg->AIRSStatus = false;
  }
  if (packStatus & 0x04) {
    reg->SLOOP2Status = true;
    Serial.println("SLOOP2 closed");
  } else {
    reg->SLOOP2Status = false;
  }
  if (packStatus & 0x08) {
    reg->SLOOP1Status = true;
    Serial.println("SLOOP1 closed");
  } else {
    reg->SLOOP1Status = false;
  }

  // ACHS-7122 status poll
  uint16_t chargingVoltage = analogRead(PIN_CHRG_CURRENT) / 4095 * 3.3;   // A2D conversion is 0V-3.3V -> 0-4095
  reg->chargingCurrent = (chargingVoltage - 2.5) * (float)10;             // See Figure 7 @ https://docs.broadcom.com/doc/ACHS-712x-DS for linear equation derivation

  // LTC4151 status poll
  reg->dischargingCurrent = readLTC4151ADCVoltage(); //******************************************************************* This is VOLTAGE! NOT CURRENT! Figure out how to translate BBM-01 voltage to current!
  reg->PacManCurrent = readLTC4151PacManCurrent();
}

/* Check measurements against trip thresholds and start timers as appropriate; checks if pack is safe to charge */
void Core1::checkSafety() {
  lowVolt = false;
  highVolt = false;
  lowTemp = false;
  highTemp = false;
  chargeSafe = true;
  
  // Update cell faults if exceeds threshold
  for (int i = 0; i < numCellMenFound; i++) {
    if (cellV[i] < reg->minVoltage) {
      cellFaults[i] = 1;
      lowVolt = true;
    } else if (cellV[i] > reg->maxVoltage) {
      cellFaults[i] = 2;
      highVolt = true;
      chargeSafe = false;
    }
    if (cellT[i] < reg->minTemp) {
      cellFaults[i] = 3;
      lowTemp = true;
    } else if (cellT[i] > reg->maxTemp) {
      cellFaults[i] = 4;
      highTemp = true;
      chargeSafe = false;
    }
  }

  // Check low voltage condition and start safety loop timer; otherwise reset timer
  if (lowVolt && (xTimerIsTimerActive(lowVoltageTimer) == pdFALSE)) {
    xTimerStart(lowVoltageTimer, 0);
  } else if (!lowVolt) {
    xTimerReset(lowVoltageTimer, 0);
    xTimerStop(lowVoltageTimer, 0);
  }
  // Check high voltage condition
  if (highVolt && (xTimerIsTimerActive(highVoltageTimer) == pdFALSE)) {
    xTimerStart(highVoltageTimer, 0);
  } else if (!highVolt) {
    xTimerReset(highVoltageTimer, 0);
    xTimerStop(highVoltageTimer, 0);
  }
  // Check low temp condition
  if (lowTemp && (xTimerIsTimerActive(lowTempTimer) == pdFALSE)) {
    xTimerStart(lowTempTimer, 0);
  } else if (!lowTemp) {
    xTimerReset(lowTempTimer, 0);
    xTimerStop(lowTempTimer, 0);
  }
  // Check high temp condition
  if (highTemp && (xTimerIsTimerActive(highTempTimer) == pdFALSE)) {
    xTimerStart(highTempTimer, 0);
  } else if (!highTemp) {
    xTimerReset(highTempTimer, 0);
    xTimerStop(highTempTimer, 0);
  }
}

/* Safety loop callback function */
void openSafetyLoopCallback(TimerHandle_t xTimer) {
  // Open safety loop
  digitalWrite(PIN_SLOOP_EN, LOW);
  Serial.println("*** SAFETY LOOP TRIPPED ***");
}

/* Interrupt function called if the state of the charging port is changed */
void detectCharger() {
  if (digitalRead(PIN_CHRG_DETECT) == LOW) {
    chargerPresent = true;
  } else {
    chargerPresent = false;
  }
}

/**
 * Closes or opens the charging relays based on the pack safety status and detecting a charging input
 * Updates PacManRegisters with charging statuses
 */
void Core1::handleChargingRelays() {
  // If pack is safe to charge and has detected a charger, close the relays
  if (chargeSafe && chargerPresent) {
    digitalWrite(PIN_CHRG_EN, HIGH);
  } else {
    digitalWrite(PIN_CHRG_EN, LOW);
  }

  // Record charging statuses
  reg->chargerDetected = chargerPresent;
  reg->chargingEnabled = chargeSafe && chargerPresent;
  Serial.print("Charger detected: ");
  Serial.println(chargerPresent);
}

/** 
 *  Initialize Core 1
 *  Configure pins and charging interrupt, setup I2C bus, and look for CellMen on I2C bus
 */
void Core1::initCore1() {
  // Setup pin modes
  pinMode(PIN_SLOOP_EN, OUTPUT);
  pinMode(PIN_CHRG_EN, OUTPUT);

  // Write default values
  digitalWrite(PIN_SLOOP_EN, HIGH);
  digitalWrite(PIN_CHRG_EN, LOW);

  // Charging interrupt
  attachInterrupt(digitalPinToInterrupt(PIN_CHRG_DETECT), detectCharger, CHANGE);

  // Join I2C bus
  Wire.begin(PIN_SDA_GLV, PIN_SCL_GLV);

  // Initialize the MCP23008 I/O expander
  initMCP23008();
  MCP23008gpio = 0x00;

  // Initialize the LTC4151 power management chip
  initLTC4151();
  
  // Search for CellMen on I2C bus and flash status LED if found
  while(numCellMenFound == 0) findCellMen();
  indicateCellMen();

  Serial.println("Core 1 ready");
}

/** 
 *  Main loop for Core 1
 *  Collects voltage and temperature data from CellMen,
 */
void Core1::runCore1() {
  pollCellMen();
  updateRegisters();
  checkSafety();
  handleChargingRelays();
}
