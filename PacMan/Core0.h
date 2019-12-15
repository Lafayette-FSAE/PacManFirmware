/*
  Core0.h - Library Header for PacMan Core 0 Code.
  Created by Clement Hathaway & Simone Khalifa, October 30, 2019.
  Released into the public domain.
*/
#ifndef Core0_h
#define Core0_h

#include "PacMan.h"
//#include "Arduino.h"
//    typedef struct {
//    unsigned char address;             // Holds the I2C address of this cell
//    unsigned char* cellData;           // Holds byte array from a cell got from I2C
//    float cellTemp;                    // Holds converted Temp float from I2C byte stream
//    float cellVoltage;                 // Holds converted Voltage float from I2C byte stream
//    float balanceCurrent;              // Holds converted balanceCurrent float from I2C byte stream
//    int SOC;                           // Holds calculated SOC value for the cell
//} cell;

    void CButton();
    void LButton();
    void RButton();
    void UButton();
    void DButton();
    void defineCellConfigs(int maxTemp, long maxV, long minV, long maxCV, boolean soh, int index);
    void setUpMain(boolean id);
    void mainPartialUpdate(float temperature, uint16_t soc, float volt, float curr);
    void cellPartialUpdate(int errorType, int cellNum);
    void faults(int errorType);
    void mainConfigScreen();
    void configPartial(boolean index);
    void chooseCellScreen(uint8_t cellNum);
    void partialChooseCell(uint8_t cellNum);
    void updateCellConfig(uint8_t cellNum, uint8_t cellConfig, boolean direction);
    void printCellConfigs(uint8_t cellNum);
    void cellConfigs(uint8_t cellNum);
    void moveCellConfig(uint8_t cellConfig);
    void miscConfigs();
    void updateMiscConfig(uint8_t miscConfig, boolean direction);
    void printMiscConfigs();
    void printMiscConfigs2(uint8_t config_num);
    void moveMiscConfig(uint8_t miscConfig);
    boolean buttonPress(struct button* button);
    void fsm();
    
  extern cell* cellArrayPointer;
  extern float* externalFaultPointer;
  extern boolean* AIRSOpenPointer;
  extern int* samplePointer;
    
  extern SemaphoreHandle_t* cellArraySemPointer;
  extern SemaphoreHandle_t* externalFaultSemPointer;
  extern SemaphoreHandle_t* AIRSOpenSemPointer;
  extern SemaphoreHandle_t* sampleSemPointer;
    
class Core0
{
  public:
    // Define Constructor
        Core0 (cell* cells, float* externalFault, boolean* AIRSOpen, SemaphoreHandle_t* cellArraySem, SemaphoreHandle_t* externalFaultSem, SemaphoreHandle_t* AIRSOpenSem, SemaphoreHandle_t* sampleSem, int* sample);
    // Define Public Functions
    void startCore0();
  private:
    // Define Private Functions
    //all the functionss
      
    // Define Private variables

  
};

#endif
