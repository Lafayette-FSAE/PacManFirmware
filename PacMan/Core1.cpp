/*
Core1.cpp - Library for PacMan Core 1.
Created by Clement Hathaway & Simone Khalifa, October 30, 2019.
Released into the public domain.
*/

#include "Arduino.h"
#include "Core1.h"
#define DEBUG false

#include "References/CAN_Files/can.h"
#include "References/CAN_Files/gpio.h"

// CONSTRUCTOR
Core1::Core1(cell* cells, float* externalFault, boolean* AIRSOpen, SemaphoreHandle_t* cellArraySem, SemaphoreHandle_t* externalFaultSem, SemaphoreHandle_t* AIRSOpenSem, SemaphoreHandle_t* sampleSem, int* sample){
    cellArrayPointer = cells;
    externalFaultPointer = externalFault;
    AIRSOpenPointer = AIRSOpen;
    samplePointer = sample;

    cellArraySemPointer = cellArraySem;
    externalFaultSemPointer = externalFaultSem;
    AIRSOpenSemPointer = AIRSOpenSem;
    sampleSemPointer = sampleSem;

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

void Core1::updateGlobalCells(cell* globalCellsPointer, cell* internalCellsPointer, SemaphoreHandle_t* cellsSem){
    // Set the global cells array to our private one
    xSemaphoreTake(*cellsSem, portMAX_DELAY);
    memcpy(globalCellsPointer, globalCellsPointer, sizeof(globalCellsPointer)); // Functions like: *cells = privateCells; but required like this in C
    xSemaphoreGive(*cellsSem);
}

void Core1::updateGlobalFaults(float* globalFaultsPointer, float internalFault, SemaphoreHandle_t* faultsSem){
    xSemaphoreTake(*faultsSem, portMAX_DELAY);
    memcpy(globalFaultsPointer, &internalFault, sizeof(&internalFault));
    xSemaphoreGive(*faultsSem);
}

void Core1::updateGlobalAIRSOpen(boolean* globalAIRSOpenPointer, boolean internalAIRSOpen, SemaphoreHandle_t* AIRSOpenSem){
    xSemaphoreTake(*AIRSOpenSem, portMAX_DELAY);
    memcpy(globalAIRSOpenPointer, &internalAIRSOpen, sizeof(&internalAIRSOpen));
    xSemaphoreGive(*AIRSOpenSem);
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

esp_err_t Core1::queueCANMessage(uint32_t flags, uint32_t identifier, uint8_t data_length_code, unsigned char* CANdata){
    can_message_t sendMessage;
    sendMessage.identifier = identifier;
    sendMessage.flags = flags;
    sendMessage.data_length_code = data_length_code;

    for (int i = 0; i < data_length_code; i++){
        sendMessage.data[i] = CANdata[i];
    }

    return can_transmit(&sendMessage, pdMS_TO_TICKS(1000) == ESP_OK);
}

void Core1::updateInternalCellsData(){
    for(int index = 0; index < 16; index++){
        privateCells[index].cellData = requestDataFromSlave(privateCells[index].address);         // Get the cellData by requesting it from the given cell with it's given I2C address, I2C addresses will be automatically populated in the future
        privateCells[index].cellTemp = getCellTemp(privateCells[index].cellData);                 // Get the cell temperature from the cellData from I2C
        privateCells[index].cellVoltage = getCellVoltage(privateCells[index].cellData);           // Get the cell voltage from the cellData from I2C
        privateCells[index].balanceCurrent = getBalanceCurrent(privateCells[index].cellData);     // Get the balance current from the cellData from I2C
        privateCells[index].SOC = (int)round(((privateCells[index].cellVoltage - 2.0)/1.65)*100); // Calculate a SOC from the voltage (BAD but quick)
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
      xSemaphoreTake(*sampleSemPointer, portMAX_DELAY);
      *samplePointer = *samplePointer + 1;
      Serial.print("The Sample is: ");
      Serial.println(*samplePointer);
      xSemaphoreGive(*sampleSemPointer);
      delay(1000);
  }
}

void Core1::startDemo(){
    // SETUP STUFF
    unsigned char* cellData;
    uint32_t CANFlag = CAN_MSG_FLAG_NONE; // Sets us up for a standard CAN frame transmission
    uint32_t CANIdentifier = 0x00000001;  // ID of 1, shouldn't be more than 11 bits for standard frame transmission
    uint8_t CANDataLength = 4;            // Sending a float for SOC which is a size of 4 bytes

    // Template for making our identical data (e.g. all get from same cellMan) for the demo
    cell privateCellExample = {0x01,        // I2C Address
                               0x00000000,  // cellData Array Pointer
                               0.00,        // Cell Temp
                               0.00,        // Cell Voltage
                               0.00,        // Cell balance current
                               0};          // Cell SOC

    // Initialise our privateCells arrays
    for(int index = 0; index < 16; index++){
        cellData = (unsigned char*)malloc(12 * sizeof(unsigned char));
        privateCells[index].address = privateCellExample.address;
        privateCells[index].cellData = cellData; // Allocate a new block of memory for each cellData for each cell
        privateCells[index].cellTemp = privateCellExample.cellTemp;
        privateCells[index].cellVoltage = privateCellExample.cellVoltage;
        privateCells[index].balanceCurrent = privateCellExample.balanceCurrent;
        privateCells[index].SOC = privateCellExample.SOC;
    }

    // Make the update to the globals that we just did above
    updateGlobalCells(cellArrayPointer, privateCells, cellArraySemPointer); // Update our globals safely with Semaphores
    updateGlobalFaults(externalFaultPointer, 0.00, externalFaultSemPointer);
    updateGlobalAIRSOpen(AIRSOpenPointer, true, AIRSOpenSemPointer);

    for(;;){
        updateInternalCellsData(); // Update our internal cellsData from the I2C bus

        // Update our globals safely with Semaphores
        updateGlobalCells(cellArrayPointer, privateCells, cellArraySemPointer);
        updateGlobalFaults(externalFaultPointer, 0.00, externalFaultSemPointer);
        updateGlobalAIRSOpen(AIRSOpenPointer, true, AIRSOpenSemPointer);

        calculateTotalPackSOC(); // Update our internal pack SOC
        queueCANMessage(CANFlag, CANIdentifier, CANDataLength, (unsigned char*)&packSOC); // Send our SOC out on the CAN bus - Also cast our float as a byte array

        delay(1000); // Wait a second
    }
}
