/*
  Core0.h - Library Header for PacMan Core 0 Code.
  Created by Clement Hathaway & Simone Khalifa, October 30, 2019.
  Released into the public domain.
*/
#ifndef Core0_h
#define Core0_h

#include "PacMan.h"
#include "CANopen.h"


class Object_Dictionary {
  public:
    int* pointer;
    char* names;
    uint16_t location;
    uint16_t attribute;
    uint16_t index;
    uint8_t subindex;
    Object_Dictionary(uint16_t index1, uint8_t sub_index) {
      CO_LOCK_OD();
      index = index1;
      subindex = sub_index;
      location = CO_OD_find((CO_SDO_t*)CO->SDO[0], index);
      pointer =  (int*)CO_OD_getDataPointer((CO_SDO_t *) CO->SDO[0], location, sub_index);
      names = CO_OD_getName((CO_SDO_t *) CO->SDO[0], location, sub_index);
      attribute = CO_OD_getAttribute((CO_SDO_t *) CO->SDO[0], location, sub_index);
      CO_UNLOCK_OD();
    }
};

typedef struct 
{
 uint8_t maxCellTemp;
 uint16_t maxCellVoltage;
 uint16_t minCellVoltage;
 uint16_t maxCellChargeVoltage;
 uint8_t cellSOH;
} Original;

void listOfConfigs();

void CButton();
void LButton();
void RButton();
void UButton();
void DButton();

class Core0
{
  public:
    // Define Constructor
    Core0();

    // Define Public Functions
    void startCore0();
    
  private:
    // Define Private Functions
    void fsm();

    //confirmation screen
    boolean confirm();

    //main screen
    void setUpMain();
    void mainPartialUpdate(float temperature, uint16_t soc, float volt, float curr, uint8_t main_index);

    //check for faults
    void checkCells(uint8_t currentCell);
    void cellPartialUpdate(int errorType, int cellNum);
    void checkForFaults(uint8_t currentCell);
    void faults(int errorType);

    //main cell and configuration screens
    void mainConfigScreen();
    void mainCellScreen(uint8_t main_index);
    void configPartial(boolean index);

    //cell configuration screen
//    void defaultCellConfigs();
//    void defineCellConfigs(int maxTemp, float maxV, float minV, float maxCV, boolean soh, int index);
    void cellConfigs(uint8_t cellNum);
    void updateCellConfig(uint8_t cellNum, uint8_t cellConfig, boolean direction);
    void cellChangeBack(uint8_t cellNum, uint8_t cellConfig, Original original);
    void printCellConfigs(uint8_t cellNum);
    void printCellConfigs2(uint8_t cellNum, uint8_t config_num);
    void moveCellConfig(uint8_t cellConfig);

    //cell data screen
    void cellData(uint8_t cellNum);

    uint8_t chooseRegister();
    void printChooseRegister(uint8_t reg);
    void updateRegister(uint8_t reg, boolean direction);
    void moveRegister(uint8_t reg);

    void editValue(uint8_t reg[], boolean state, uint8_t cellNum);
    void viewValue(uint8_t reg[], boolean state, uint8_t cellNum);
    void printEditValue(Object_Dictionary od, uint8_t reg, uint8_t lastReg);
    void printViewValue(Object_Dictionary od, uint8_t lastReg);
    int8_t convert(uint16_t index);
    Object_Dictionary updateValue(Object_Dictionary od, uint8_t place, boolean direction, uint8_t lastReg);
    void moveEdit(uint8_t reg);

    void regNotFound(uint16_t regNumb);

    //charge screen
    void chargeScreen();
    void chargePartial(uint8_t charge_index);
};

#endif