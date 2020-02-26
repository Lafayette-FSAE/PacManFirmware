/*
  Core1.cpp - Library for PacMan Core 1.
  Created by Clement Hathaway & Simone Khalifa, October 30, 2019.
  Released into the public domain.
*/
#include "Core1.h"


// CONSTRUCTOR
Core1::Core1(CO_t *CO) {
  Wire.begin(PIN_SDA, PIN_SCL); // Join the I2C bus (address optional for master)  -- CHANGE THIS FOR DISPLAY
  totalMAH = 0;
}

void Core1::addressVoltageQuickSort(addressVoltage* addressVoltages, int first, int last) {
  int i;
  int j;
  int pivot;
  uint16_t tempVoltage;
  unsigned char tempAddress;

  if (first < last) {
    pivot = first;
    i = first;
    j = last;

    while (i < j) {
      while (addressVoltages[i].addressMinusVoltage <= addressVoltages[pivot].addressMinusVoltage && i < last)
        i++;
      while (addressVoltages[j].addressMinusVoltage > addressVoltages[pivot].addressMinusVoltage)
        j--;
      if (i < j) {
        tempAddress = addressVoltages[i].address;
        tempVoltage = addressVoltages[i].addressMinusVoltage;

        addressVoltages[i].address = addressVoltages[j].address;
        addressVoltages[i].addressMinusVoltage = addressVoltages[j].addressMinusVoltage;

        addressVoltages[j].address = tempAddress;
        addressVoltages[j].addressMinusVoltage = tempVoltage;
      }
    }

    tempAddress = addressVoltages[pivot].address;
    tempVoltage = addressVoltages[pivot].addressMinusVoltage;

    addressVoltages[pivot].address = addressVoltages[j].address;
    addressVoltages[pivot].addressMinusVoltage = addressVoltages[j].addressMinusVoltage;

    addressVoltages[j].address = tempAddress;
    addressVoltages[j].addressMinusVoltage = tempVoltage;

    addressVoltageQuickSort(addressVoltages, first, j - 1);
    addressVoltageQuickSort(addressVoltages, j + 1, last);

  }
}

// TODO: Exclude known I2C devices e.g. GPIO Expand
uint8_t Core1::discoverCellMen() {
  unsigned char* discoveredAddresses;
  uint8_t cellMenCount = 0;

  for (int i = 0; i<16; i++){
    addresses[i] = 0x00;
  }


for (int address = 1; address < 127; address++ ) {
    // The I2C scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission(address);
    byte error = Wire.endTransmission();

    if (error == 0) {
      if (address != TEMPERATURE_SENSOR_ADDRESS
          && address != IO_EXPANDER_ADDRESS
          && address != REAL_TIME_CLOCK_ADDRESS
          && address != POWER_MONITOR_ADDRESS
          && address != POWER_MONITOR_GLOBAL_ADDRESS) {
        Serial.print("CellMan I2C device found at address 0x");
        if (address < 16) Serial.print("0");
        Serial.print(address, HEX);
        Serial.println("  !");
        
        addresses[cellMenCount] = address;
        cellMenCount++;
      }
    }
    else if (error == 4) {
      Serial.print("Unknown error at address 0x");
      if (address < 16)
        Serial.print("0");
      Serial.print(address, HEX);
      Serial.println(" skipping...");
    }
  }

  return cellMenCount;
} // USES MALLOC (Only call once at startup to prevent memory leakage)

unsigned char* Core1::requestDataFromSlave(unsigned char address) {
  Wire.requestFrom((int) address, 24); // 24 is the max data length expected in bytes
  if (DEBUG) {
    Serial.print("Requesting data from CellMan on Address: ");
    Serial.println(address);
  }
  unsigned char cd = 0;
  unsigned char* cellD = &cd;
  while (Wire.available()) {
    if (DEBUG) Serial.println("Wire Available!");
    unsigned char debugFlag = Wire.read();

    if (debugFlag == 0x00) {
      unsigned char cellData[NORMAL_I2C_LENGTH];
      cellData[0] = debugFlag;
      for (int i = 1; i < NORMAL_I2C_LENGTH; i++) {
        *(cellData + i) = Wire.read();                     // Append the read character (byte) to our cellData array
        if (DEBUG){ Serial.println(cellData[i], HEX);       // Print the character (byte) in HEX
                    //Serial.println(cellData[i], DEC);       // Print the character (byte) in DEC
        }
        }
      cellD = cellData;
    } else if (debugFlag == 0x01) {
      unsigned char cellData[DEBUG_I2C_LENGTH];
      cellData[0] = debugFlag;
      for (int i = 1; i < DEBUG_I2C_LENGTH; i++) {
        *(cellData + i) = Wire.read();                     // Append the read character (byte) to our cellData array
        if (DEBUG){ Serial.println(cellData[i], HEX);       // Print the character (byte) in HEX
                    //Serial.println(cellData[i], DEC);       // Print the character (byte) in DEC
        }      }
      cellD = cellData;
    } else {
      Serial.println("Error on the Debug byte! Don't know length to expect. Returning 24 bytes");
      Serial.print("debugFlag is: ");
      Serial.println(debugFlag, HEX);

      unsigned char cellData[DEBUG_I2C_LENGTH];
      cellData[0] = 0x01;
      for (int i = 1; i < DEBUG_I2C_LENGTH; i++) {
        *(cellData + i) = Wire.read();                     // Append the read character (byte) to our cellData array
        if (DEBUG){ Serial.println(cellData[i], HEX);       // Print the character (byte) in HEX
                    //Serial.println(cellData[i], DEC);       // Print the character (byte) in DEC
        }      }
      cellD = cellData;
    }
  }
  return cellD;
}

// uint8_t calculateCellLocation(unsigned char* cellData){
//     uint16_t minusVoltage = (uint16_t)((cellData[6]<<8)+cellData[5]);
//
//     return minusVoltage;
// }

void Core1::processCellData(unsigned char* cellData, uint8_t cellPhysicalLocation) {
  // Process the data differently depending on cellData[0] which is the debugFlag
  cellPositions[cellPhysicalLocation]          = cellPhysicalLocation;
  cellVoltages[cellPhysicalLocation]           = (uint16_t)((cellData[2] << 8) + cellData[1]); // Shift MSBs over 8 bits, then add the LSBs to the first 8 bits and cast as a uint16_t
  cellTemperatures[cellPhysicalLocation]       = (uint16_t)((cellData[4] << 8) + cellData[3]);
  minusTerminalVoltages[cellPhysicalLocation]  = (uint16_t)((cellData[6] << 8) + cellData[5]);
  cellBalanceCurrents[cellPhysicalLocation]    = (uint16_t)((cellData[8] << 8) + cellData[7]);


  // If we are in I2C Debug Mode
  if (cellData[0] == 0x01) {
    LEDStatuses[cellPhysicalLocation]          = (bool)cellData[9];
    balanceStatuses[cellPhysicalLocation]      = (bool)cellData[10];
    balanceDutyCycles[cellPhysicalLocation]    = (uint8_t)cellData[10];
    balanceFrequencies[cellPhysicalLocation]   = (uint16_t)((cellData[13] << 8) + cellData[12]);
    temperatureSlopes[cellPhysicalLocation]    = (uint16_t)((cellData[15] << 8) + cellData[14]);
    temperatureOffsets[cellPhysicalLocation]   = (uint16_t)((cellData[17] << 8) + cellData[16]);
    balanceCurrentSlopes[cellPhysicalLocation] = (uint16_t)((cellData[19] << 8) + cellData[18]);
    balanceVoltageSlopes[cellPhysicalLocation] = (uint16_t)((cellData[21] << 8) + cellData[20]);
  }
}

// Maps the arrayIndex to a physical cell location in the packs (since we can't tell between segments right now) by saying the second instance of a same voltage potential cell is in the other segment
uint8_t Core1::physicalLocationFromSortedArray(uint8_t arrayIndex) {
  uint8_t physicalAddress;
  if (arrayIndex % 2 == 0) { // If Even
    physicalAddress = arrayIndex / 2;
  } else {                  // If Odd
    physicalAddress = ((arrayIndex - 1) / 2) + 8;
  }

  return physicalAddress;
}

void Core1::calculateTotalPackSOC() {
  int SOCTotal = 0;

  for (int index = 0; index < 16; index++) {
    //SOCTotal += privateCells[index].SOC;  // Sum up all of our SOCs from all the cells to get an average for the 16 cells in a pack
  }

  packSOC = (float)(SOCTotal / 16); // Return the average SOC from the cells
}

void Core1::start() {
  // Get all CellMan Addresses
  uint8_t numberOfDiscoveredCellMen = discoverCellMen();
  unsigned char* celldata;

  // Put together addressVoltages array by requesting data from each cellman
  for (int i = 0; i < numberOfDiscoveredCellMen; i++) {
    unsigned char* tempCellData = requestDataFromSlave(addresses[i]);

    addressVoltages[i].address = addresses[i];
    addressVoltages[i].addressMinusVoltage = (uint16_t)((tempCellData[6] << 8) + tempCellData[5]);

  }
  if(DEBUG){
      Serial.print("The number of address found: ");
      Serial.println(numberOfDiscoveredCellMen);
      }
  // Sort the addressVoltages by ascending voltages
  addressVoltageQuickSort(addressVoltages, 0, numberOfDiscoveredCellMen);

  for (;;) {
    // Collect data from all the CellMen
    for (int i = 0; i < numberOfDiscoveredCellMen; i++) {
      celldata = requestDataFromSlave(addressVoltages[i].address);
        if (celldata[0] != 0){
           processCellData(celldata, physicalLocationFromSortedArray(i)); // Process data retrieved from each cellman and is inerted based off of physicalAddress
        }
    }

    // Update the Object Dictionary Here
    if (celldata[0] != 0) {
      CO_LOCK_OD();
      for (int i = 0; i < 16; i++) {
        OD_cellPosition[i]         = cellPositions[i];
        OD_cellVoltage[i]          = cellVoltages[i];
        OD_cellTemperature[i]      = cellTemperatures[i];
        OD_minusTerminalVoltage[i] = minusTerminalVoltages[i];
        OD_cellBalancingEnabled[i] = cellBalancingEnabled[i];
        OD_cellBalancingCurrent[i] = cellBalanceCurrents[i];
      }
      CO_UNLOCK_OD();
    }
    delay(100);
  }
}
