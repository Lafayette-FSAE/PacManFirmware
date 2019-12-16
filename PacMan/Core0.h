/*
  Core0.h - Library Header for PacMan Core 0 Code.
  Created by Clement Hathaway & Simone Khalifa, October 30, 2019.
  Released into the public domain.
*/
#ifndef Core0_h
#define Core0_h

#include "PacMan.h"

void CButton();
void LButton();
void RButton();
void UButton();
void DButton();

class Core0
{
  public:
    // Define Constructor
    Core0 (cell* cells, float* externalFault, boolean* AIRSOpen, SemaphoreHandle_t* cellArraySem, SemaphoreHandle_t* externalFaultSem, SemaphoreHandle_t* AIRSOpenSem, SemaphoreHandle_t* sampleSem, int* sample);

    // Define Public Functions
    void startCore0();
    
  private:
    // Define Private Functions
    void fsm();
    void defineCellConfigs(int maxTemp, float maxV, float minV, float maxCV, boolean soh, int index);
    void setUpMain(boolean id);
    void mainPartialUpdate(float temperature, uint16_t soc, float volt, float curr);
    void cellPartialUpdate(int errorType, int cellNum);
    void cellPartialUpdate2(int errorType, int cellNum);
    void faults(int errorType);
    void mainConfigScreen();
    void configPartial(boolean index);
    void chooseCellScreen(uint8_t cellNum);
    void partialChooseCell(uint8_t cellNum);
    void updateCellConfig(uint8_t cellNum, uint8_t cellConfig, boolean direction);
    void printCellConfigs(uint8_t cellNum);
    void printCellConfigs2(uint8_t cellNum, uint8_t config_num);
    void cellConfigs(uint8_t cellNum);
    void moveCellConfig(uint8_t cellConfig);
    void miscConfigs();
    void updateMiscConfig(uint8_t miscConfig, boolean direction);
    void printMiscConfigs();
    void printMiscConfigs2(uint8_t config_num);
    void moveMiscConfig(uint8_t miscConfig);
    boolean buttonPress(struct button* button);
    void defaultCellConfigs();
    void checkCells(uint8_t currentCell);
    void checkCells2(uint8_t currentCell);
    
    // Define Private variables
    int* samplePointer;
    cell* cellArrayPointer;
    float* externalFaultPointer;
    boolean* AIRSOpenPointer;

    SemaphoreHandle_t* sampleSemPointer;
    SemaphoreHandle_t* cellArraySemPointer;
    SemaphoreHandle_t* externalFaultSemPointer;
    SemaphoreHandle_t* AIRSOpenSemPointer;
};

#endif
