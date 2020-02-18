/*
Core1.cpp - Library for PacMan Core 1.
Created by Clement Hathaway & Simone Khalifa, October 30, 2019.
Released into the public domain.
*/
#include "Core1.h"


// CONSTRUCTOR
Core1::Core1(CO_t *CO){
    Wire.begin(PIN_SDA, PIN_SCL); // Join the I2C bus (address optional for master)  -- CHANGE THIS FOR DISPLAY

    totalMAH = 0;
    addresses = (unsigned char*)malloc(16 * sizeof(unsigned char));
}

void Core1::arrayAppend(unsigned char* arr, int index, int value, int arrSize, int capacity){ // https://www.tutorialspoint.com/c_standard_library/c_function_realloc.htm
    if(arrSize > capacity){
        realloc(arr, sizeof(arr) * 2);
        capacity = sizeof(arr) * 2;
    }

    arr[index] = value;
    arrSize = arrSize + 1;
}

uint16_t byteArrayToUInt16(unsigned char* byteArray){
    uint16_t uint;
    memcpy(&uint, byteArray, sizeof(uint));
    return uint;
}

// TODO: Exclude known I2C devices e.g. GPIO Expand
unsigned char* Core1::discoverCellMen(){
    unsigned char* discoveredAddresses;
    int nDevices = 0;
    int discoveredCapacity = 1; // Start at 1 address, increase exponentionally by 2

    discoveredAddresses = (unsigned char*)malloc(1 * sizeof(unsigned char));

    for(int address = 1; address < 127; address++ ){
        // The I2C scanner uses the return value of
        // the Write.endTransmisstion to see if
        // a device did acknowledge to the address.
        Wire.beginTransmission(address);
        byte error = Wire.endTransmission();

        if (error == 0){
            Serial.print("CellMan I2C device found at address 0x");
            if (address<16) Serial.print("0");
            Serial.print(address,HEX);
            Serial.println("  !");

            arrayAppend(discoveredAddresses, nDevices, address, nDevices+1, discoveredCapacity); // Add the address to the discoveredAddresses Array using the append method to allow for expansion
            nDevices++; // Increment the number of devices by 1, strategically after the arrayAppend so we can use it as an index
        }
        else if (error==4){
            Serial.print("Unknown error at address 0x");
            if (address<16)
            Serial.print("0");
            Serial.print(address,HEX);
            Serial.println(" skipping...");
        }
    }

    return discoveredAddresses;
} // USES MALLOC (Only call once at startup to prevent memory leakage)

unsigned char* Core1::requestDataFromSlave(unsigned char address){
    Wire.requestFrom((int) address, 24); // 24 is the max data length expected in bytes
    if(DEBUG){
        Serial.print("Requesting data from CellMan on Address: ");
        Serial.println(address);
    }

    while (Wire.available()) {
        if(DEBUG) Serial.println("Wire Available!");
        unsigned char debugFlag = Wire.read();

        if(debugFlag == 0x00){
            unsigned char cellData[NORMAL_I2C_LENGTH];
            cellData[0] = debugFlag;
            for(int i = 1; i<NORMAL_I2C_LENGTH; i++){
                *(cellData + i) = Wire.read();                     // Append the read character (byte) to our cellData array
                if(DEBUG) Serial.println(cellData[i], HEX);        // Print the character (byte) in HEX
            }
        }else if(debugFlag == 0x01){
            unsigned char cellData[DEBUG_I2C_LENGTH];
            cellData[0] = debugFlag;
            for(int i = 1; i<DEBUG_I2C_LENGTH; i++){
                *(cellData + i) = Wire.read();                     // Append the read character (byte) to our cellData array
                if(DEBUG) Serial.println(cellData[i], HEX);        // Print the character (byte) in HEX
            }
        }else{
            Serial.println("Error on the Debug byte! Don't know length to expect. Returning 24 bytes");
            Serial.print("debugFlag is: ");
            Serial.println(debugFlag, HEX);

            unsigned char cellData[DEBUG_I2C_LENGTH];
            cellData[0] = 0x01;
            for(int i = 1; i<DEBUG_I2C_LENGTH; i++){
                *(cellData + i) = Wire.read();                     // Append the read character (byte) to our cellData array
                if(DEBUG) Serial.println(cellData[i], HEX);        // Print the character (byte) in HEX
            }
        }
    }
    return cellData;
}

void processCellData(unsigned char* cellData, uint8_t cellLocation){
    // Process the data differently depending on cellData[0] which is the debugFlag
    cellPositions[cellLocation]          = cellLocation;
    cellVoltages[cellLocation]           = (uint16_t)((cellData[2]<<8)+cellData[1]); // Shift MSBs over 8 bits, then add the LSBs to the first 8 bits and cast as a uint16_t
    cellTemperatures[cellLocation]       = (uint16_t)((cellData[4]<<8)+cellData[3]);
    minusTerminalVoltages[cellLocation]  = (uint16_t)((cellData[6]<<8)+cellData[5]);
    cellBalanceCurrents[cellLocation]    = (uint16_t)((cellData[8]<<8)+cellData[7]);

    // If we are in I2C Debug Mode
    if(cellData[0] == 0x01){
        LEDStatuses[cellLocation]
    }
}


void Core1::calculateTotalPackSOC(){
    int SOCTotal = 0;

    for(int index = 0; index < 16; index++){
        SOCTotal += privateCells[index].SOC;  // Sum up all of our SOCs from all the cells to get an average for the 16 cells in a pack
    }

    packSOC = (float)(SOCTotal/16); // Return the average SOC from the cells
}

void Core1::start(){
  for(;;){
      // MAIN Loop - Calls all private functions to operate

      delay(2000);
  }
}
