/*
  Core0.h - Library Header for PacMan Core 1 Code.
  Created by Clement Hathaway & Simone Khalifa, October 30, 2019.
  Released into the public domain.
*/
#ifndef Core1_h
#define Core1_h

//#include "Arduino.h"   // Required for use as an Arduino Library
#include "PacMan.h"
#include <Wire.h>      // Required for I2C


//typedef struct {
//    unsigned char address;             // Holds the I2C address of this cell
//    unsigned char* cellData;           // Holds byte array from a cell got from I2C
//    float cellTemp;                    // Holds converted Temp float from I2C byte stream
//    float cellVoltage;                 // Holds converted Voltage float from I2C byte stream
//    float balanceCurrent;              // Holds converted balanceCurrent float from I2C byte stream
//    int SOC;                           // Holds calculated SOC value for the cell
//} cell;

class Core1
{
  private:
    // VARIABLES
    unsigned char* addresses;           // Hold discovered I2C Addresses
    unsigned char currentSensorAddress; // Hold I2C Address for the Discharge Current Sensor
    cell privateCells[16];              // Store all 16 cells and their data
    unsigned char I2CError;             // Holds potential error when discoverying I2C devices

    float dischargeCurrent;             // Holds currently read discharge current from I2C sensor
    unsigned long oldTime;              // Holds previous time in microseconds for integration
    unsigned long newTime;              // Holds current time in microseconds for integration
    int totalMAH;                       // Holds totalMAH of the pack
    int consumedMAH;                    // Keeps track of the current mAh's consumed, calculated from integration of current sensor
    float packSOC;                        // Keeps track of the Pack's total SOC


    // SHARED VARIABLES PASSED IN VIA CONSTRUCTOR
    cell* cellArrayPointer;
    float* externalFaultPointer;
    boolean* AIRSOpenPointer;
    int* samplePointer;

    SemaphoreHandle_t* cellArraySemPointer;
    SemaphoreHandle_t* externalFaultSemPointer;
    SemaphoreHandle_t* AIRSOpenSemPointer;
    SemaphoreHandle_t* sampleSemPointer;

    // FUNCTIONS
    void arrayAppend(unsigned char* arr, int index, int value, int arrSize, int capacity);
    void updateGlobalCells(cell*, cell*, SemaphoreHandle_t*);
    void updateGlobalFaults(float*, float, SemaphoreHandle_t*);
    void updateGlobalAIRSOpen(boolean*, boolean, SemaphoreHandle_t*);

    unsigned char* discoverCellMen();                                       // Discover CellMen (I2C slaves) on the I2C bus
    unsigned char* requestDataFromSlave(unsigned char address);             // Get 12 bytes from a cellman via I2C for a certain address
    unsigned char* getCellTempData(unsigned char* cellData);                // Get the 4 bytes from the byte array containing the cell temperature float value
    unsigned char* getCellVoltageData(unsigned char* cellData);             // Get the 4 bytes from the byte array containing the cell voltage float value
    unsigned char* getBalanceCurrentData(unsigned char* cellData);          // Get the 4 bytes from the byte array containing the cell balance current float value
    float byteArrayToFloat(unsigned char* byteArray);                       // Convert 4 byte long array to a floating point value
    float getCellTemp(unsigned char* cellData);                             // Get temperature as a float of the cell via above functions
    float getCellVoltage(unsigned char* cellData);                          // Get voltage as a float of the cell via above functions
    float getBalanceCurrent(unsigned char* cellData);                       // Get balance current as a float of the cell via above functions
//    float getDischargeCurrent();                                            // Get current discharge current from sensor over I2C
    esp_err_t queueCANMessage(uint32_t flags, uint32_t identifier, uint8_t data_length_code, unsigned char* CANdata);

    void updateInternalCellsData();
    float calculateTotalPackSOC(cell*);


  public:
    Core1(cell* cells, float* externalFault, boolean* AIRSOpen, SemaphoreHandle_t* cellArraySem, SemaphoreHandle_t* externalFaultSem, SemaphoreHandle_t* AIRSOpenSem, SemaphoreHandle_t* sampleSem, int* sample);
    void start();
    void startDemo();
};

#endif
