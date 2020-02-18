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
    unsigned char cellData[12];   //  IMPORTANT NOTE: WILL THIS NEED TO BE FREED LATER?
    Wire.requestFrom((int) address, 12); // 12 is the data length expected in bytes
    if(DEBUG){
        Serial.print("Requesting data from CellMan on Address: ");
        Serial.println(address);
    }

    while (Wire.available()) { // NOTE: Slave may send less than requested
        if(DEBUG) Serial.println("Wire Available!");
        for(int i = 0; i<12; i++){
            *(cellData + i) = Wire.read();                     // Append the read character (byte) to our cellData array
            if(DEBUG) Serial.println(cellData[i], HEX);        // Print the character (byte) in HEX
        }
    }

    unsigned char cellData2[12] = {0xA4,0x70,0x9D,0x3F,0xCD,0xCC,0x5C,0x40,0xC3,0xF5,0xD8,0x40};

    return cellData2;
}

unsigned char* Core1::getCellTempData(unsigned char* cellData){
    unsigned char *cellTempData = (unsigned char*)malloc(4 * sizeof(unsigned char));
    memcpy(cellTempData, cellData, 4*sizeof(unsigned char));
    return cellTempData;
}

unsigned char* Core1::getCellVoltageData(unsigned char* cellData){
    unsigned char *cellVoltageData = (unsigned char*)malloc(4 * sizeof(unsigned char));
    memcpy(cellVoltageData, cellData + 4, 4*sizeof(unsigned char));
    return cellVoltageData;
}

unsigned char* Core1::getBalanceCurrentData(unsigned char* cellData){
    unsigned char *balanceCurrentData = (unsigned char*)malloc(4 * sizeof(unsigned char));
    memcpy(balanceCurrentData, cellData + 8, 4*sizeof(unsigned char));
    return balanceCurrentData;
}

float Core1::byteArrayToFloat(unsigned char* byteArray){
    float f;
    memcpy(&f, byteArray, sizeof(f));
    return f;
}

float Core1::getCellTemp(unsigned char* cellData){
    unsigned char* cellTempData = getCellTempData(cellData);
    float cellTemp = byteArrayToFloat(cellTempData);
    //free(cellTempData); // IS THIS REQUIRED?
    return cellTemp;
}

float Core1::getCellVoltage(unsigned char* cellData){
    unsigned char* cellVoltageData = getCellVoltageData(cellData);
    float cellVoltage = byteArrayToFloat(cellVoltageData);
    //free(cellVoltageData);
    return cellVoltage;
}

float Core1::getBalanceCurrent(unsigned char* cellData){
    unsigned char* cellBalanceCurrentData = getBalanceCurrentData(cellData);
    float cellBalanceCurrent = byteArrayToFloat(cellBalanceCurrentData);
    return cellBalanceCurrent;
}

//float getDischargeCurrent(){
//    // Insert I2C Code for Current Sensor
//}

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
