/*
  Core0.h - Library Header for PacMan Core 1 Code.
  Created by Clement Hathaway & Simone Khalifa, October 30, 2019.
  Released into the public domain.
*/
#ifndef Core1_h
#define Core1_h

#include "Arduino.h"   // Required for use as an Arduino Library
#include <Wire.h>      // Required for I2C

struct cell {
    unsigned char address;             // Holds the I2C address of this cell
    unsigned char* cellData;           // Holds byte array from a cell got from I2C
    float cellTemp;                    // Holds converted Temp float from I2C byte stream
    float cellVoltage;                 // Holds converted Voltage float from I2C byte stream
    float balanceCurrent;              // Holds converted balanceCurrent float from I2C byte stream
};

class Core1
{
  private:
    // VARIABLES
    unsigned char* addresses;          // Hold discovered I2C Addresses
    struct cell cells[16];             // Store all 16 cells and their data
    unsigned char I2CError;            // Holds potential error when discoverying I2C devices

    // FUNCTIONS
    unsigned char* discoverCellMen(){return addresses;}
    unsigned char* requestDataFromSlave(unsigned char address){return cellData;}              // Get 12 bytes from a cellman via I2C for a certain address
    unsigned char* getCellTempData(unsigned char* cellData){return cellTempData;}             // Get the 4 bytes from the byte array containing the cell temperature float value
    unsigned char* getCellVoltageData(unsigned char* cellData){return cellVoltageData;}       // Get the 4 bytes from the byte array containing the cell voltage float value
    unsigned char* getBalanceCurrentData(unsigned char* cellData){return balanceCurrentData;} // Get the 4 bytes from the byte array containing the cell balance current float value
    float byteArrayToFloat(unsigned char* byteArray){return f;}                               // Convert 4 byte long array to a floating point value
    float getCellTemp(unsigned char* cellData){return temp;}                                  // Get temperature as a float of the cell via above functions
    float getCellVoltage(unsigned char* cellData){return voltage;}                            // Get voltage as a float of the cell via above functions
    float getBalanceCurrent(unsigned char* cellData){return balanceCurrent;}                  // Get balance current as a float of the cell via above functions
    
  public:
    // Define Constructor
    // Define Public Functions
};

#endif
