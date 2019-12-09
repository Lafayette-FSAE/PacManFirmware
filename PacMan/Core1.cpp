/*
Core1.cpp - Library for PacMan Core 1.
Created by Clement Hathaway & Simone Khalifa, October 30, 2019.
Released into the public domain.
*/

#include "Arduino.h"
#include "Core1.h"
#define DEBUG true

// CONSTRUCTOR
Core1::Core1(int pin){
    Wire.begin();        // Join the I2C bus (address optional for master)
    if(DEBUG){
        Serial.begin(115200);
    }

    addresses = (unsigned char*)malloc(16 * sizeof(unsigned char));
    cells =     (unsigned char*)malloc(16 * sizeof(cell));
}

void Core1::arrayAppend(int *arr, int index, int value, int *size, int *capacity){ // https://www.tutorialspoint.com/c_standard_library/c_function_realloc.htm
    if(*size > *capacity){
        realloc(arr, sizeof(arr) * 2);
        *capacity = sizeof(arr) * 2;
    }

    arr[index] = value;
    *size = *size + 1;
}

unsigned char* Core1::discoverCellMen(){
    unsigned char* discoveredAddresses;
    int nDevices = 0;
    int discoveredCapacity = 1; // Start at 1 address, increase exponentionally by 2

    discoveredAddresses = (unsigned char*)malloc(1 * sizeof(unsigned char));

    for(address = 1; address < 127; address++ ){
        // The I2C scanner uses the return value of
        // the Write.endTransmisstion to see if
        // a device did acknowledge to the address.
        Wire.beginTransmission(address);
        error = Wire.endTransmission();

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
    unsigned char* cellData[12];   //  IMPORTANT NOTE: WILL THIS NEED TO BE FREED LATER?
    Wire.requestFrom(address, 12); // 12 is the data length expected in bytes
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

    return cellData;
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
