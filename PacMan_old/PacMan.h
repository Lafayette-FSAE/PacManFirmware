/*
  Core0.h - Library Header for PacMan Core 0 Code.
  Created by Clement Hathaway & Simone Khalifa, October 30, 2019.
  Released into the public domain.
*/
#ifndef PacMan_h
#define PacMan_h

#include "Arduino.h"

// GLOBALS
typedef struct {
  unsigned char address;             // Holds the I2C address of this cell
  unsigned char cellData[12];           // Holds byte array from a cell got from I2C
  float cellTemp;                    // Holds converted Temp float from I2C byte stream
  float cellVoltage;                 // Holds converted Voltage float from I2C byte stream
  float balanceCurrent;              // Holds converted balanceCurrent float from I2C byte stream
  int SOC;                           // Holds calculated SOC value for the cell
} cell;

//extern cell cells[16];               // Written to by Core1, read by Core0
//extern float externalFault;                 // Written to by Core1 from CAN Bus, read by Core0
//extern boolean AIRSOpen;                    // Written to by Core0, read by Core1 for CAN Bus

//struct cellConfig cellConfigs[16]; // Written to by Core0, read by Core1 for CAN Bus    --to be added as a later feature
//float miscConfigs[6];              // Written to by Core0, read by Core1 for CAN Bus    --to be added as a later feature

// SEMAPHORES -- MOVED TO PACMAN
//extern TaskHandle_t Task1, Task2;
//extern SemaphoreHandle_t cellArraySem;
//extern SemaphoreHandle_t externalFaultSem;
//extern SemaphoreHandle_t AIRSOpenSem;
//extern SemaphoreHandle_t sampleSem;
#endif
