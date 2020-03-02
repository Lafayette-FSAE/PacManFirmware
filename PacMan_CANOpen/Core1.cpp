/*
  Core1.cpp - Library for PacMan Core 1.
  Created by Clement Hathaway & Simone Khalifa, October 30, 2019.
  Released into the public domain.
*/
#include "Core1.h"


// CONSTRUCTOR
Core1::Core1(CO_t *CO) {
  // TODO: Move this to the top-level
  Wire.begin(PIN_SDA, PIN_SCL); // Join the I2C bus (address optional for master)  -- CHANGE THIS FOR DISPLAY
  totalMAH = 0;
}

// Quick sorts out struct of addressVoltage based off of the .addressMinusVoltage property
void Core1::addressVoltageQuickSort(addressVoltage* addressVoltages, int first, int last) {
  int i, j, pivot;
  uint16_t tempVoltage;
  uint8_t tempAddress;

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

bool Core1::addressVoltageSorter(addressVoltage const lhs, addressVoltage const rhs) {
  return (lhs.addressMinusVoltage < rhs.addressMinusVoltage);
}

// Discovery I2C devices, excluding I2C devices on PacMan
uint8_t Core1::discoverCellMen() {
  uint8_t cellMenCount = 0;

  // Since we are using a set array size, let's initialise all values in the array to 0
  for (int i = 0; i < 16; i++) {
    addresses[i] = 0x00;
  }

  // Start the scan for I2C devices between 1 and 127
  for (int address = 1; address < 127; address++ ) {
    // The I2C scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission(address);
    byte error = Wire.endTransmission();

    if (error == 0) {
      // Exclude PacMan ICs
      if (address != TEMPERATURE_SENSOR_ADDRESS
          && address != IO_EXPANDER_ADDRESS
          && address != REAL_TIME_CLOCK_ADDRESS
          && address != POWER_MONITOR_ADDRESS
          && address != POWER_MONITOR_GLOBAL_ADDRESS) {
        Serial.print("CellMan I2C device found at address 0x");
        if (address < 16) Serial.print("0");
        Serial.print(address, HEX);
        Serial.println("  !");

        // Add the new address to our address array and increment the number we have found
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
}

// Request byte array from specified CellMan I2C address
unsigned char* Core1::requestDataFromSlave(unsigned char address) {
  Wire.requestFrom((int) address, REQUEST_LENGTH); // 24 is the max data length expected in bytes
  if (DEBUG) {
    Serial.print("Requesting data from CellMan on Address: ");
    Serial.println(address);
  }

  if (Wire.available()) {
    if (DEBUG) Serial.println("Wire Available!");
      for (int i = 0; i < REQUEST_LENGTH; i++) {
        *(cellDs + i) = Wire.read();                     // Append the read character (byte) to our cellData array
        if (DEBUG) {
            Serial.println(cellDs[i], HEX);              // Print the character (byte) in HEX
            Serial.println(cellDs[i], DEC);              // Print the character (byte) in DEC
        }
      }
  }

  return cellDs;
}


void Core1::processCellData(unsigned char* cellData, uint8_t cellPhysicalLocation) {
  cellPositions[cellPhysicalLocation]          = cellPhysicalLocation;
  cellVoltages[cellPhysicalLocation]           = (uint16_t)((cellData[2] << 8) + cellData[1]); // Shift MSBs over 8 bits, then add the LSBs to the first 8 bits and cast as a uint16_t
  cellTemperatures[cellPhysicalLocation]       = (uint16_t)((cellData[4] << 8) + cellData[3]);
  minusTerminalVoltages[cellPhysicalLocation]  = (uint16_t)((cellData[6] << 8) + cellData[5]);
  cellBalanceCurrents[cellPhysicalLocation]    = (uint16_t)((cellData[8] << 8) + cellData[7]);


  // If we are in I2C Debug Mode
  if (DEBUG) {
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

// Simple voltage-based SOC - Very inaccurate
void Core1::calculateTotalPackSOC() {
  int SOCTotal = 0;

  for (int index = 0; index < 16; index++) {
    //SOCTotal += privateCells[index].SOC;  // Sum up all of our SOCs from all the cells to get an average for the 16 cells in a pack
  }

  packSOC = (float)(SOCTotal / 16); // Return the average SOC from the cells
}

// Start main loop for thread
void Core1::start() {
  /// Initial Functions

  unsigned char* tempCellData;

  // Not really needed to zero the array of structures but why not
  for (int i = 0; i < 16; i++) {
    addressVoltages[i].address              = 0x00;
    addressVoltages[i].addressMinusVoltage  = 0;
    cellPositions[i]=i;
    minusTerminalVoltages[i]=0;
  }


  // Get all CellMan Addresses
  uint8_t numberOfDiscoveredCellMen = discoverCellMen();

  // Loop where trying to discovery until we get some devices to prevent crashing of the CPU
  while (numberOfDiscoveredCellMen == 0) {
    Serial.println("In the while loop, looking for CellMen");
    numberOfDiscoveredCellMen = discoverCellMen();
  }

  if (DEBUG) {
    Serial.print("The number of address found: ");
    Serial.println(numberOfDiscoveredCellMen);
  }


  // Put together addressVoltages array by requesting data from each cellman
  for (int i = 0; i < numberOfDiscoveredCellMen; i++) {
    tempCellData = requestDataFromSlave(addresses[i]);

    addressVoltages[i].address = addresses[i];
    addressVoltages[i].addressMinusVoltage = (uint16_t)((tempCellData[6] << 8) + tempCellData[5]);
  }

  // Sort the addressVoltages by ascending voltages - Wow this bug fix took FOREVER, forgot the -1 (haha jouny) after the numberOfDiscoveredCellMen oof
  addressVoltageQuickSort(addressVoltages, 0, numberOfDiscoveredCellMen - 1);

  /// Main Loop
  for (;;) {
    // Collect data from all the CellMen
    for (int i = 0; i < numberOfDiscoveredCellMen; i++) {
      unsigned char* celldata = requestDataFromSlave(addressVoltages[i].address);
      processCellData(celldata, physicalLocationFromSortedArray(i)); // Process data retrieved from each cellman and is inerted based off of physicalAddress
    }

    // Update the Object Dictionary Here
    CO_LOCK_OD();
    for (int i = 0; i < 16; i++) {
      if( minusTerminalVoltages[i]!=0){
      OD_cellPosition[i]         = cellPositions[i];
      //Serial.print("The number of address found: ");
      //Serial.println(OD_cellPosition[i]);
      OD_cellVoltage[i]          = cellVoltages[i];
      //Serial.print("The number of OD_cellVoltage found: ");
      //Serial.println(OD_cellVoltage[i]);
      OD_cellTemperature[i]      = cellTemperatures[i];
      //Serial.print("The number of OD_cellTemperature found: ");
      //Serial.println(OD_cellTemperature[i]);
      OD_minusTerminalVoltage[i] = minusTerminalVoltages[i];
      //Serial.print("The number of OD_minusTerminalVoltage found: ");
      //Serial.println(OD_minusTerminalVoltage[i]);
      OD_cellBalancingEnabled[i] = cellBalancingEnabled[i];
      //Serial.print("The number of OD_cellBalancingEnabled found: ");
      //Serial.println(OD_cellBalancingEnabled[i]);
      OD_cellBalancingCurrent[i] = cellBalanceCurrents[i];
      //Serial.print("The number of OD_cellBalancingCurrent found: ");
      //Serial.println(OD_cellBalancingCurrent[i]);
    }
    }
    CO_UNLOCK_OD();

    delay(100);
  }
}
