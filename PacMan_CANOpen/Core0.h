/*
  Core0.h - Library Header for PacMan Core 0 Code.
  Created by Clement Hathaway & Simone Khalifa, October 30, 2019.
  Released into the public domain.
*/
#ifndef Core0_h
#define Core0_h

#include "PacMan.h"
#include "CANopen.h"

typedef struct
{
  boolean pack_id; //true = Pack 1, false = Pack 2  //change all to floats
  boolean airs_state; //true = open, false = closed (thought they were aways open but guess not)
  boolean sl_state; //true = open, false = closed
  uint16_t SOC_min;
  float max_pack_current;
  float min_pack_current;
} Misc_Configs;

typedef struct
{
  int max_temp;
  float max_voltage;
  float min_voltage;
  float max_charge_voltage;
  boolean SOH;              //true = good cell, false = bad cell--> if SOH is false, no longer allow cell to cause faults
} Cell_Configs;

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
    Core0 (CO_t *CO);

    // Define Public Functions
    void startCore0();

  private:
    // Define Private Functions
    void fsm();

    //confirmation screen
    boolean confirm();

    //main screen
    void setUpMain(boolean id, boolean charging);
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
    void defaultCellConfigs();
    void defineCellConfigs(int maxTemp, float maxV, float minV, float maxCV, boolean soh, int index);
    void cellConfigs(uint8_t cellNum);
    void updateCellConfig(uint8_t cellNum, uint8_t cellConfig, boolean direction);
    void cellChangeBack(uint8_t cellNum, uint8_t cellConfig, Cell_Configs original[1]);
    void printCellConfigs(uint8_t cellNum);
    void printCellConfigs2(uint8_t cellNum, uint8_t config_num);
    void moveCellConfig(uint8_t cellConfig);

    //cell data screen
    void cellData(uint8_t cellNum, cell current_cell);

    //misc config screen
    void miscConfigs();
    void updateMiscConfig(uint8_t miscConfig, boolean direction);
    void miscChangeBack(uint8_t miscConfig, Misc_Configs original[1]);
    void printMiscConfigs();
    void printMiscConfigs2(uint8_t config_num);
    void moveMiscConfig(uint8_t miscConfig);

    //charge screen
    void chargeScreen();
    void chargePartial(uint8_t charge_index);
};

#endif
