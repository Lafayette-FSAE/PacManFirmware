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
#include "can.h"
#include "gpio.h"
#include "Arduino.h"
#include "CANopen.h"

#define DEBUG true
#define NORMAL_I2C_LENGTH 9
#define DEBUG_I2C_LENGTH 22


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
    uint8_t addresses[16];           // Hold discovered I2C Addresses

    unsigned char I2CError;             // Holds potential error when discoverying I2C devices

    float dischargeCurrent;             // Holds currently read discharge current from I2C sensor
    unsigned long oldTime;              // Holds previous time in microseconds for integration
    unsigned long newTime;              // Holds current time in microseconds for integration
    int totalMAH;                       // Holds totalMAH of the pack
    int consumedMAH;                    // Keeps track of the current mAh's consumed, calculated from integration of current sensor
    float packSOC;                      // Keeps track of the Pack's total SOC

    // Normal Cells Data
    uint8_t cellPositions[16];         // Cell Position calculated from the voltages
    uint16_t cellVoltages[16];          // Cell Voltage data for all cells in Pack
    uint16_t cellTemperatures[16];      // Cell Temperature data for all the cells
    uint16_t minusTerminalVoltages[16]; // Cell voltages at each cellman's minus terminal with respect to ground (which ground...?)
    uint8_t  cellBalancingEnabled[16];  // Is this cellman balancing or not?
    uint16_t cellBalanceCurrents[16];   // Balancing Currents for each cellman
    uint8_t  cellSOCs[16];              // SOCs for each cell

    // Debug Cells Data
    bool LEDStatuses[16];               // Debug LED status on each CellMan
    bool balanceStatuses[16];           // Balance status for each CellMan
    uint8_t balanceDutyCycles[16];      // Balance duty cycles for each CellMan
    uint16_t balanceFrequencies[16];    // balanceFrequencies for each CellMan
    int16_t temperatureSlopes[16];      // temperatureSlopes for each CellMan
    int16_t temperatureOffsets[16];     // temepratureOffsets for each CellMan
    int16_t balanceCurrentSlopes[16];   // balanceCurrentSlopes for each CellMan
    int16_t balanceVoltageSlopes[16];   // balanceVoltageSlopes for each CellMan

    struct addressVoltage {
        uint8_t address;
        uint16_t addressMinusVoltage;
    };

    addressVoltage addressVoltages[16];

    // FUNCTIONS
    void arrayAppend(uint8_t* arr, int index, int value, int arrSize, int capacity);
    void addressVoltageQuickSort(addressVoltage* addressVoltages, int first, int last);

    uint8_t* discoverCellMen();                                       // Discover CellMen (I2C slaves) on the I2C bus
    unsigned char* requestDataFromSlave(uint8_t address);             // Get 12 bytes from a cellman via I2C for a certain address

    void processCellData(unsigned char* cellData, uint8_t cellLocation);    // Insert cellData values into the appropriate arrays depending on i2c debug flag and calculates the proper position inside based off of voltage -- Will need to be adjusted with the new hardware
    uint8_t physicalLocationFromSortedArray(uint8_t arrayIndex);
    void calculateTotalPackSOC();


  public:
    Core1(CO_t *CO);
    void start();
};

#endif
