/*
Core1.cpp - Library for PacMan Core 1.
Created by Clement Hathaway & Simone Khalifa, October 30, 2019.
Released into the public domain.
*/

#include "Arduino.h"
#include "Core1.h"
#define DEBUG false


// CONSTRUCTOR
Core1::Core1(cell* cells, float* externalFault, boolean* AIRSOpen, SemaphoreHandle_t* cellArraySem, SemaphoreHandle_t* externalFaultSem, SemaphoreHandle_t* AIRSOpenSem, SemaphoreHandle_t* sampleSem, int* sample){
    cell* cellArrayPointer = cells;
    float* externalFaultPointer = externalFault;
    boolean* AIRSOpenPointer = AIRSOpen;
    int* samplePointer = sample;
    
    SemaphoreHandle_t* cellArraySemPointer = cellArraySem;
    SemaphoreHandle_t* externalFaultSemPointer = externalFaultSem;
    SemaphoreHandle_t* AIRSOpenSemPointer = AIRSOpenSem;
    SemaphoreHandle_t* sampleSemPointer = sampleSem;
    
    xSemaphoreTake(*sampleSemPointer, portMAX_DELAY );
    *samplePointer = 0;
    xSemaphoreGive(*sampleSemPointer);
    Serial.print("The Sample is: ");
    Serial.println(*samplePointer);

    //Wire.begin();        // Join the I2C bus (address optional for master)  -- CHANGE THIS FOR DISPLAY

    totalMAH = 0;
    addresses = (unsigned char*)malloc(16 * sizeof(unsigned char));
  Serial.println("help");
}

void Core1::arrayAppend(unsigned char* arr, int index, int value, int arrSize, int capacity){ // https://www.tutorialspoint.com/c_standard_library/c_function_realloc.htm
    if(arrSize > capacity){
        realloc(arr, sizeof(arr) * 2);
        capacity = sizeof(arr) * 2;
    }

    arr[index] = value;
    arrSize = arrSize + 1;
}

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

//float getDischargeCurrent(){
//    // Insert I2C Code for Current Sensor
//}

//esp_err_t Core1::queueCANMessage(uint32_t flags, uint32_t identifier, uint8_t data_length_code, unsigned byte* CANdata){
//    can_message_t sendMessage;
//    sendMessage.identifier = identifier;
//    sendMessage.flags = flags;
//    sendMessage.data_length_code = data_length_code;
//
//    for (int i = 0; i < data_length_code; i++){
//        sendMessage.data[i] = CANdata[i];
//    }
//
//    return can_transmit(&sendMessage, pdMS_TO_TICKS(1000) == ESP_OK);
//}

void Core1::start(){
  for(;;){
      // MAIN Loop - Calls all private functions to operate
      xSemaphoreTake(*sampleSemPointer, portMAX_DELAY );
      *samplePointer = *samplePointer + 1;
      Serial.print("The Sample is: ");
      Serial.println(*samplePointer);
      xSemaphoreGive(*sampleSemPointer);
      delay(1000);
  }
}
