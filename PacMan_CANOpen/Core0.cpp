/*
  Core0.cpp - Library for PacMan Core 0.
  Created by Clement Hathaway & Simone Khalifa, October 30, 2019.
  Released into the public domain.
*/

//Display Firmware
//Simone Khalifa

#include "References/GxGDEH029A1/GxGDEH029A1.cpp"
#include "References/GxIO/GxIO_SPI/GxIO_SPI.cpp"
#include "References/GxIO/GxIO.cpp"
#include "References/BitmapGraphics.h"

#include "References/Fonts/FreeSansBold24pt7b.h"
#include "References/Fonts/FreeSansBold12pt7b.h"
#include "References/Fonts/FreeSansBold9pt7b.h"

#include "References/Adafruit_GFX.cpp"
#include "References/Adafruit_SPITFT.cpp"
#include "References/glcdfont.c"
#include "References/GxEPD.cpp"
#include "References/GxFont_GFX.cpp"

#include "Core0.h"

GxIO_Class io(SPI, PIN_DISP_CS, PIN_DISP_DC, PIN_DISP_RST);
GxEPD_Class display(io, PIN_DISP_RST, PIN_DISP_BUSY);

#define NUM_CELLS 16

void setupCore0() {

  display.init();

  const GFXfont* f = &FreeSansBold9pt7b;
  display.setFont(f);
  display.setTextColor(GxEPD_BLACK);

  pinMode(PIN_BTN_CENTER, INPUT); //button
  pinMode(PIN_BTN_UP,     INPUT); //button
  pinMode(PIN_BTN_DOWN,   INPUT); //button
  pinMode(PIN_BTN_LEFT,   INPUT); //button
  pinMode(PIN_BTN_RIGHT,  INPUT); //button
  attachInterrupt(digitalPinToInterrupt(PIN_BTN_CENTER), CButton, FALLING);
  attachInterrupt(digitalPinToInterrupt(PIN_BTN_UP), UButton, FALLING);
  attachInterrupt(digitalPinToInterrupt(PIN_BTN_DOWN), DButton, FALLING);
  attachInterrupt(digitalPinToInterrupt(PIN_BTN_LEFT), LButton, FALLING);
  attachInterrupt(digitalPinToInterrupt(PIN_BTN_RIGHT), RButton, FALLING);
}

//misc configs
//boolean id = 0; boolean sl = 0; int soc = 50; float max_pc = 250; float min_pc = 0; boolean airs = 0;//defaults
//Cell_Configs configs[NUM_CELLS];
//Misc_Configs misc_configs[1] = {{id, airs, sl, soc, max_pc, min_pc}}; //here is where the pack parameters are stored--can add as many as we want and send them to can with one line of semaphore

uint8_t regista[3] = {0, 0, 0};

typedef struct
{
  String names;
  float value;
} Configurations;

//Configurations configurations[] = {};

/*void listOfConfigs() {
  configurations[0] = {"id", 0};
  configurations[1] = {"sl", 0};
  configurations[2] = {"airs", 0};
  configurations[3] = {"max pack current", 250};
  configurations[4] = {"min pack current", 0};
  configurations[5] = {"soc", 50};
  }*/

//cell currentCellData[NUM_CELLS];
//cell currentCellDataInt[NUM_CELLS];
//boolean airs_state;

//constructor
Core0::Core0() {
}

//# define getName(var, str)  sprintf(str, "%s", #var)
float voltage1; float current1; float temp1; uint16_t soc_test;

//class Object_Dictionary {
//  public:
//    void* pointer;
//    char* names;
//    uint16_t location;
//    Object_Dictionary(uint16_t index, uint8_t sub_index) {
//      Serial.println("hellos1");
//      CO_LOCK_OD();
//      Serial.println(index);
//      location = CO_OD_find((CO_SDO_t*)CO->SDO[0], index);
//      Serial.println("hellos3");
//      pointer =  (void*)CO_OD_getDataPointer((CO_SDO_t *) CO->SDO[0], location, sub_index);
//      Serial.println("hellos4");
//      names = (char*)CO_OD_getName((CO_SDO_t *) CO->SDO[0], location, sub_index);
//      Serial.println("hellos5");
//      CO_UNLOCK_OD();
//    }
//};

void Core0::startCore0() {
  for (;;) {
    setupCore0();
    fsm();

  }
}

boolean centerPress = false;
boolean leftPress = false;
boolean rightPress = false;
boolean upPress = false;
boolean downPress = false;
uint8_t dbDelay = 1000;

void CButton() //interrupt with debounce
{
  volatile static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = micros();
  if (interrupt_time - last_interrupt_time > dbDelay) {
    centerPress = true;
    //    Serial.println("c");
  }
  else {
    centerPress = false;
  }
  last_interrupt_time = interrupt_time;
}

void LButton() //interrupt with debounce
{
  volatile static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = micros();
  if (interrupt_time - last_interrupt_time > dbDelay) {
    leftPress = true;
    //    Serial.println("l");
  }
  else {
    leftPress = false;
  }
  last_interrupt_time = interrupt_time;
}

void RButton() //interrupt with debounce
{
  volatile static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = micros();
  if (interrupt_time - last_interrupt_time > dbDelay) {
    rightPress = true;
    //    Serial.println("r");
  }
  else {
    rightPress = false;
  }
  last_interrupt_time = interrupt_time;
}

void UButton() //interrupt with debounce
{
  volatile static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = micros();
  if (interrupt_time - last_interrupt_time > dbDelay) {
    upPress = true;
    //    Serial.println("u");
  }
  else {
    upPress = false;
  }
  last_interrupt_time = interrupt_time;
}

void DButton() //interrupt with debounce
{
  volatile static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = micros();
  if (interrupt_time - last_interrupt_time > dbDelay) {
    downPress = true;
    //    Serial.println("d");
  }
  else {
    downPress = false;
  }
  last_interrupt_time = interrupt_time;
}

//FSM for display screens
typedef enum {
  Main,
  Config_State,
  Cell_State,
  Choose_Register,
  Choose_Cell_Register,
  Charging,
  Cell_Data,
//  Cell_Configurs,
  Edit_Value,
  View_Value,
  Edit_Cell_Value,
  View_Cell_Value,
  Reg_Not_Found
} State;

void Core0::fsm() {

  boolean config_index = true;
  boolean cell_index = true;
  uint8_t main_index = 0;
  uint8_t charge_index = 0;
  boolean charging = false;
  uint16_t regNumb = 0;

  State nextState = Main;
  State state = Main;
  setUpMain();
  //defaultCellConfigs();

  centerPress = false; upPress = false; downPress = false; leftPress = false; rightPress = false;

  while (1) {
    switch (nextState) {
      case Main: {
          if (state != Main) {
            Serial.println("hello hello over here yes yes yes");
            setUpMain();
            main_index = 0;
            state = Main;
          }
          else if (leftPress || upPress) {
            leftPress = false; upPress = false;
            Serial.println("left/up");
            if (main_index == 0) main_index = 8;
            else if (main_index == 1) main_index = 16;
            else if (main_index == 9) main_index = 0;
            else main_index -= 1;
          }
          else if (rightPress || downPress) {
            rightPress = false; downPress = false;
            Serial.println("right/down");
            if (main_index == 0) main_index = 9;
            else if (main_index == 16) main_index = 1;
            else if (main_index == 8) main_index = 0;
            else main_index += 1;
          }
          else if (centerPress && main_index == 0) {
            centerPress = false;
            Serial.println("center");  //testing
            nextState = Config_State;
          }
          else if (centerPress && main_index != 0) {
            centerPress = false;
            Serial.println("center");  //testing
            nextState = Cell_State;
          }

          voltage1 = 0; current1 = 0; temp1 = 0; soc_test = 0;
          for (int i = 0; i < NUM_CELLS; i++) {
            voltage1 += OD_cellVoltage[i];
            current1 += OD_cellBalancingCurrent[i];
            temp1 = max(temp1, (float)OD_cellTemperature[i]);
            soc_test += OD_cellSOC[i];
          }
          current1 = current1 / NUM_CELLS;
          soc_test = soc_test / NUM_CELLS;
          mainPartialUpdate(temp1, soc_test, voltage1, current1, main_index);
        }
        break;

      case Config_State: {
          if (centerPress && config_index == true) {
            centerPress = false;
            Serial.println("center");  //testing
            nextState = Choose_Register;
          }
          else if (centerPress && config_index == false) {
            centerPress = false;
            Serial.println("center");  //testing
            nextState = Charging;
          }
          else if (upPress || downPress || leftPress || rightPress) {
            upPress = false; downPress = false; leftPress = false; rightPress = false;
            Serial.println("up/down/left/right");
            config_index = !config_index;
            configPartial(config_index);
          }
          else if (state != Config_State) {
            config_index = true;
            mainConfigScreen();
            state = Config_State;
          }
          delay(20);
        }
        break;

      case Cell_State: {
          if (centerPress && cell_index == true) {
            centerPress = false;
            Serial.println("center");    //testing
            nextState = Choose_Cell_Register;
          }
          else if (centerPress && cell_index == false) {
            centerPress = false;
            Serial.println("center");    //testing
            nextState = Cell_Data;
          }
          else if (upPress || downPress || leftPress || rightPress) {
            upPress = false; downPress = false; leftPress = false; rightPress = false;
            Serial.println("up/down/left/right");
            cell_index = !cell_index;
            configPartial(cell_index);
          }
          else if (state != Cell_State) {
            cell_index = true;
            mainCellScreen(main_index);
            state = Cell_State;
          }
          delay(20);
        }
        break;

      case Choose_Register: {
          uint8_t regnum = chooseRegister();
          if (regnum == 3) {
            regNumb = 0x2000 + (regista[0] * 100) + (regista[1] * 10) + regista[2];
            Object_Dictionary od(regNumb, 0);
            if (od.location == 0xFFFF){
              regNumb-= 0x2000;
              nextState = Reg_Not_Found;
            }
            else if (od.attribute == 0x0e) nextState = Edit_Value;
            else nextState = View_Value;
          }
          else if (regnum == 4) nextState = Main; //exits function if on home button
        }
        break;

      case Choose_Cell_Register: {
          uint8_t regnum = chooseRegister();
          if (regnum == 3) {
            regNumb = 0x3000 + (regista[0] * 100) + (regista[1] * 10) + regista[2];
            Object_Dictionary od1(regNumb, 0);
            if (od1.location == 0xFFFF) {
              regNumb-= 0x3000;
              nextState = Reg_Not_Found;
            }
            else if (od1.attribute == 0x8e) nextState = Edit_Cell_Value;
            else nextState = View_Cell_Value;
          }
          else if (regnum == 4) nextState = Main; //exits function if on home button
        }
        break;
        
      case Edit_Value: {
          editValue(regista, true, 0);
          nextState = Main; //exits function if on home button
        }
        break;

      case View_Value: {
          viewValue(regista, true, 0);
          nextState = Main; //exits function if on home button
        }
        break;

      case View_Cell_Value: {
          viewValue(regista, false, main_index - 1);
          nextState = Main; //exits function if on home button
        }
        break;

      case Edit_Cell_Value: {
          editValue(regista, false, main_index - 1);
          nextState = Main; //exits function if on home button
        }
        break;

      case Reg_Not_Found: {
          regNotFound(regNumb);
          nextState = Main;
          delay(20);
        }
        break;

      case Charging: {
          if (centerPress && charge_index == 0) {
            centerPress = false;
            Serial.println("center");    //testing
            if (confirm()) {
              charging = 1;
            }
            chargeScreen();
          }
          else if (centerPress && charge_index == 1) {
            centerPress = false;
            Serial.println("center");    //testing
            if (confirm()) {
              charging = 0;
            }
            charge_index = 0;
            chargeScreen();
          }
          else if (centerPress && charge_index == 2) {
            centerPress = false;
            Serial.println("center");    //testing
            Serial.println("charging1");
            Serial.print(charging);
            nextState = Main;
          }
          else if (upPress || leftPress) {
            upPress = false; leftPress = false;
            Serial.println("up/left");
            if (charge_index != 0) charge_index -= 1;
            else charge_index = 2;
            chargePartial(charge_index);
          }
          else if (downPress || rightPress) {
            downPress = false; rightPress = false;
            Serial.println("downright");
            if (charge_index != 2) charge_index += 1;
            else charge_index = 0;
            chargePartial(charge_index);
          }
          else if (state != Charging) {
            charge_index = 0;
            chargeScreen();
            state = Charging;
          }
          delay(20);
        }
        break;

      /*case Cell_Configurs: {
          cellConfigs(main_index - 1); //exits function if on home button
          nextState = Main;
        }
        break;*/

      case Cell_Data: {  //display cell data
          cellData(main_index - 1);
          if (centerPress) {
            centerPress = false;
            nextState = Main;
          }
        }
        break;
    }
  }
}

//confirmation screen
boolean Core0::confirm() {
  boolean confirm_index = false;
  const GFXfont* font = &FreeSansBold9pt7b;
  display.setFont(font);

  display.fillRect(88, 18, 120, 76, GxEPD_BLACK);
  display.fillRect(90, 20, 116, 72, GxEPD_WHITE);
  display.setTextColor(GxEPD_BLACK);
  display.setCursor(110, 40);
  display.print("Confirm?");
  display.setCursor(110, 80);
  display.print("No");
  display.setCursor(160, 80);
  display.print("Yes");
  display.fillRect(110, 82, 30, 1, GxEPD_BLACK);
  display.updateWindow(5, 5, 118, 286, false);
  while (!centerPress) {
    if (upPress || downPress || leftPress || rightPress) {
      upPress = false; downPress = false; leftPress = false; rightPress = false;
      Serial.println("up/down/left/right");
      confirm_index = !confirm_index;
      if (!confirm_index) {
        display.fillRect(160, 82, 25, 1, GxEPD_WHITE);
        display.fillRect(110, 82, 30, 1, GxEPD_BLACK); //no
      }
      else {
        display.fillRect(110, 82, 30, 1, GxEPD_WHITE);
        display.fillRect(160, 82, 25, 1, GxEPD_BLACK); //yes
      }
      display.updateWindow(5, 5, 118, 286, false);
    }
    delay(20);
  }
  centerPress = false;
  return confirm_index;
}

void Core0::setUpMain() {
  display.setRotation(0);
  display.drawExampleBitmap(gImage_new_main, 0, 0, 128, 296, GxEPD_BLACK);

  display.setRotation(45);
  const GFXfont* f = &FreeSansBold9pt7b;  //set font
  display.setFont(f);

  display.setCursor(175, 16);
  if (OD_packNodeID == 4) display.print("1");
  else display.print("2");

  if (OD_chargeCableDetected || OD_chargingEnabled) {
    display.setCursor(265, 15);
    display.print("Ch");
  }
  Serial.println("ya need somthin");
  display.update();
  display.update();
  display.setRotation(45);
  Serial.println("hello hello over here yes yes yess");
}

void Core0::mainPartialUpdate(float temperature, uint16_t soc, float volt, float curr, uint8_t main_index)
{
  const GFXfont* f = &FreeSansBold9pt7b;  //set font
  display.setFont(f);
  display.setTextColor(GxEPD_BLACK);
  display.setRotation(45);

  String temp = String(String(temperature, 1) + " C"); //convert to strings
  String voltage = String(String(volt, 1) + " V");
  String current = String(String(curr, 1) + " A");
  String SOC = String(String(soc, DEC) + " %");

  uint16_t box_x = 15;  //set update window
  uint16_t box_y = 110;
  uint16_t box_w = 296 - box_x * 2;
  uint16_t box_h = 20;
  uint16_t indent = box_w / 4;

  uint16_t w = 14;
  uint16_t h = 1;
  uint16_t y = 65;
  uint16_t x = 11;

  display.fillRect(108, 19 - h, 76, h, GxEPD_WHITE);
  display.fillRect(x, y - h, 296 - x * 2, h, GxEPD_WHITE);

  if (main_index == 0) {
    x = 108;
    y = 19;
    w = 76;
  }
  else if (main_index <= 8) {
    x = 11 + (w - 1) * (main_index - 1);
  }
  else if (main_index > 8) {
    x = 180 + (w - 1) * (main_index - 9);
  }

  display.fillRect(x, y - h, w, h, GxEPD_BLACK);
  display.fillRect(box_x, box_y - box_h + 1, box_w, box_h, GxEPD_WHITE);

  display.setCursor(box_x, box_y);  //print SOC
  display.print(SOC);

  display.setCursor(box_x + indent, box_y); //print V
  display.print(voltage);

  display.setCursor(box_x + 2 * indent, box_y); //print current
  display.print(current);

  display.setCursor(box_x + 3 * indent, box_y); //print temp
  display.print(temp);

  checkCells(0); //calls cell partial update if need be

  display.updateWindow(5, 5, 118, 286, false);

  //checkForFaults(0);//calls faults();
}

void Core0::checkCells(uint8_t currentCell) {
  for (uint8_t cell = currentCell; cell < NUM_CELLS; cell++) {
    if (OD_cellSOH[cell] == 1) cellPartialUpdate(1, cell);
    /*   else if(currentCellData[cell].cellTemp>=configs[cell].max_temp-(0.2*configs[cell].max_temp)) cellPartialUpdate(2, cell); //within 80%
       else if(currentCellData[cell].cellVoltage>configs[cell].max_voltage -(0.1*configs[cell].max_voltage)
               ||currentCellData[cell].cellVoltage<configs[cell].min_voltage + (0.1*configs[cell].min_voltage)) cellPartialUpdate(3, cell);*/
  }
}

void Core0::checkForFaults(uint8_t currentCell) {
  for (uint8_t cell = currentCell; cell < NUM_CELLS; cell++) {
    if (OD_SLOOP_Relay == 1) faults(1); //sl open
    else if (OD_AIRS == 1) faults(2); //airs open
    else if (OD_cellTemperature[cell] >= OD_maxCellTemp[cell] + (0.1 * OD_maxCellTemp[cell])
             || OD_cellTemperature[cell] < OD_minCellTemp[cell] - (0.1 * OD_minCellTemp[cell])) faults(3); //temp too high

    else if (!OD_chargingEnabled && (OD_cellVoltage[cell] > OD_maxCellVoltage[cell] + (0.1 * OD_maxCellVoltage[cell]) //voltage too high
                                     || OD_cellVoltage[cell] < OD_minCellVoltage[cell] - (0.1 * OD_minCellVoltage[cell]))) faults(4); //voltage too low

    else if (OD_chargingEnabled && (OD_cellVoltage[cell] > OD_maxCellChargeVoltage[cell] + (0.1 * OD_maxCellChargeVoltage[cell]) //voltage too high
                                    || OD_cellVoltage[cell] < OD_minCellVoltage[cell] - (0.1 * OD_minCellVoltage[cell]))) faults(4); //voltage too low

    else if (OD_cellBalancingCurrent[cell] > OD_maxCellCurrent[cell] + (0.1 * OD_maxCellCurrent[cell]) //current too high
             || OD_cellBalancingCurrent[cell] < OD_minCellCurrent[cell] - (0.1 * OD_minCellCurrent[cell])) faults(5); //current too low

    else if (OD_cellSOC[cell] >= ((OD_cellSOC_Min[cell] * 52) / 100)) faults(6); //soc below min
  }
}

void Core0::cellPartialUpdate(int errorType, int cellNum)
{
  uint16_t box_w = 14;
  uint16_t box_h = 23;
  uint16_t box_y = 63;
  uint16_t box_x = 0;

  if (cellNum < 8) { //seg1
    box_x = 11 + (box_w - 1) * cellNum;
  }
  else { //seg2
    box_x = 180 + (box_w - 1) * (cellNum - 8);
  }

  //seg variables
  if (errorType == 1) { //soh bad
    display.fillRect(box_x, box_y - box_h, box_w, box_h, GxEPD_BLACK);
  }
  else if (errorType == 2) { //temp
    display.fillRect(box_x, box_y - box_h, box_w, box_h, GxEPD_BLACK);
    display.setTextColor(GxEPD_WHITE);
    display.setCursor(box_x + 1, box_y - 6);
    display.print("T");
  }
  else if (errorType == 3) { //high voltage
    display.fillRect(box_x, box_y - box_h, box_w, box_h, GxEPD_BLACK);
    display.setTextColor(GxEPD_WHITE);
    display.setCursor(box_x + 1, box_y - 6);
    display.print("V");
  }
  if (cellNum < NUM_CELLS - 1) {
    checkCells(cellNum + 1);
  }
  //  display.updateWindow(128 - box_y, box_x, box_h, box_w, false);
}

void Core0::faults(int errorType)
{
  const GFXfont* font = &FreeSansBold24pt7b;
  display.setFont(font);

  display.fillScreen(GxEPD_BLACK);
  display.setTextColor(GxEPD_WHITE);
  display.setCursor(30, 75);

  if (errorType == 1) { //SL Open
    display.print(" SL Open ");
  }
  else if (errorType == 2) { //Airs Open
    display.print("AIRS Open");
  }
  else if (errorType == 3) { //Dangerous temp
    display.print("High Temp");
  }
  else if (errorType == 4) { //Dangerous voltage
    display.print("H/L Volt");
  }
  else if (errorType == 5) { //Current
    display.print("H/L Curr");
  }
  else if (errorType == 6) { //Low SOC
    display.print("Low SOC");
  }
  display.update();
  while (!centerPress) {
    delay(20);
  }
  centerPress = false;
}

void Core0::mainConfigScreen()
{
  const GFXfont* font = &FreeSansBold12pt7b;
  display.setFont(font);

  display.fillScreen(GxEPD_WHITE);
  display.setTextColor(GxEPD_BLACK);
  display.setCursor(30, 50);
  display.print("Configurations");
  display.setCursor(30, 80);
  display.print("Fault Resolved");
  display.fillRect(20, 41, 4, 4, GxEPD_BLACK);
  display.updateWindow(5, 5, 118, 286, false);
}

void Core0::mainCellScreen(uint8_t main_index)
{
  const GFXfont* font = &FreeSansBold9pt7b;
  display.setFont(font);

  display.fillScreen(GxEPD_WHITE);
  display.setTextColor(GxEPD_BLACK);
  display.setCursor(100, 20);
  display.print("Cell #" + String(main_index, DEC));
  display.setCursor(30, 50);
  display.print("Cell Configs");
  display.setCursor(30, 80);
  display.print("Cell Data");
  display.fillRect(20, 41, 4, 4, GxEPD_BLACK);
  display.updateWindow(5, 5, 118, 286, false);
}

void Core0::configPartial(boolean index) {
  int top = 45;
  int bottom = 75;
  int older;
  int newer;
  if (index) {
    newer = top;  //choose cell
    older = bottom;
  }
  else {
    newer = bottom;  //choose misc
    older = top;
  }
  //draw underline
  display.fillRect(20, newer - 4, 4, 4, GxEPD_BLACK);
  display.fillRect(20, older - 4, 4, 4, GxEPD_WHITE);
  display.updateWindow(128 - bottom, 20, bottom - top + 4, 4, false);
}

#define NUM_CELL_CONFIGS 5

/*void Core0::defaultCellConfigs() {
  for (int i = 0; i < NUM_CELLS; i++) {
    defineCellConfigs(65, 3.6, 2.5, 3.6, 1, i);
  }
  }

  void Core0::defineCellConfigs(int maxTemp, float maxV, float minV, float maxCV, boolean soh, int index) //pretty much set I think
  {
  configs[index].max_temp = maxTemp; //=input[]
  configs[index].max_voltage = maxV;
  configs[index].min_voltage = minV;
  configs[index].max_charge_voltage = maxCV;
  configs[index].SOH = soh;
  }*/

void Core0::cellConfigs(uint8_t cellNum)
{
  //update screen to print all options for cell
  printCellConfigs(cellNum);
  boolean confirmed = false;
  uint8_t cell_config = 0;
  centerPress = false; upPress = false; downPress = false; leftPress = false; rightPress = false;

  while (1) {
    if (cell_config == NUM_CELL_CONFIGS && centerPress)  { //exit
      centerPress = false;
      Serial.println("center");    //testing
      delay(50);
      break;
    }
    else if (leftPress || upPress) {
      leftPress = false; upPress = false;
      delay(50);
      Serial.println("left");
      if (cell_config == 0) {
        cell_config = NUM_CELL_CONFIGS;
      }
      else {
        cell_config--;
      }
      moveCellConfig(cell_config);
    }
    else if (rightPress || downPress) {
      rightPress = false; downPress = false;
      delay(50);
      Serial.println("right");
      if (cell_config == NUM_CELL_CONFIGS) {
        cell_config = 0;
      }
      else {
        cell_config++;
      }
      moveCellConfig(cell_config);
    }
    else if (centerPress) {
      centerPress = false;
      Serial.println("center");
      delay(50);
      const GFXfont* f = &FreeSansBold9pt7b;  //set font
      display.setFont(f);
      display.setTextColor(GxEPD_BLACK);
      display.setRotation(45);
      display.setCursor(270, 15);
      display.print("*");
      display.updateWindow(128 - 15, 270, 12, 20, false);
      Original original = {OD_maxCellTemp[cellNum], OD_maxCellVoltage[cellNum], OD_minCellVoltage[cellNum],
                           OD_maxCellChargeVoltage[cellNum], OD_cellSOH[cellNum]
                          };
      while (!centerPress) {
        if (upPress || rightPress) {
          upPress = false; downPress = false;
          Serial.println("up");
          delay(50);
          updateCellConfig(cellNum, cell_config, true);
        }
        else if (downPress || leftPress) {
          downPress = false; leftPress = false;
          Serial.println("down");
          delay(50);
          updateCellConfig(cellNum, cell_config, false);
        }
        delay(20);
      }
      centerPress = false;
      confirmed = confirm();
      if (!confirmed) {
        cellChangeBack(cellNum, cell_config, original);
      }
      else {
        printCellConfigs2(cellNum, cell_config);
      }
      moveCellConfig(cell_config);
    }
    delay(30);
  }
}

void Core0::updateCellConfig(uint8_t cellNum, uint8_t cellConfig, boolean direction)
{
  //change value of config
  if (cellConfig == 0) { //temp
    if (direction) {
      OD_maxCellTemp[cellNum] += 1;
    }
    else {
      OD_maxCellTemp[cellNum] -= 1;
    }
  }
  else if (cellConfig == 1) { //max_voltage
    if (direction) {
      OD_maxCellVoltage[cellNum] += 1;
    }
    else {
      OD_maxCellVoltage[cellNum] -= 1;
    }
  }
  else if (cellConfig == 2) { //min voltage
    if (direction) {
      OD_minCellVoltage[cellNum] += 1;
    }
    else {
      OD_minCellVoltage[cellNum] -= 1;
    }
  }
  else if (cellConfig == 3) { //max charge voltage
    if (direction) {
      OD_maxCellChargeVoltage[cellNum] += 1;
    }
    else {
      OD_maxCellChargeVoltage[cellNum] -= 1;
    }
  }
  else if (cellConfig == 4) { //soh
    OD_cellSOH[cellNum] = !OD_cellSOH[cellNum];
  }
  printCellConfigs2(cellNum, cellConfig);
}

void Core0::cellChangeBack(uint8_t cellNum, uint8_t cellConfig, Original original) {
  if (cellConfig == 0) {
    OD_maxCellTemp[cellNum] = original.maxCellTemp;
  }
  else if (cellConfig == 1) {
    OD_maxCellVoltage[cellNum] = original.maxCellVoltage;
  }
  else if (cellConfig == 2) {
    OD_minCellVoltage[cellNum] = original.minCellVoltage;
  }
  else if (cellConfig == 3) {
    OD_maxCellChargeVoltage[cellNum] = original.maxCellChargeVoltage;
  }
  else if (cellConfig == 4) {
    OD_cellSOH[cellNum] = original.cellSOH;
  }
  printCellConfigs2(cellNum, cellConfig);
}

void Core0::printCellConfigs(uint8_t cellNum)
{
  //print each
  const GFXfont* font = &FreeSansBold9pt7b;
  display.setFont(font);

  uint8_t left = 10;
  uint8_t right = (296 / 2);
  uint8_t top = 60;
  uint8_t line = 20;
  uint8_t y_point = top;

  String num = String("Cell #" + String(cellNum + 1, DEC));
  String mtemp = String("Max T " + String(OD_maxCellTemp[cellNum], DEC));
  String maxv = String("Max V " + String(OD_maxCellVoltage[cellNum], 1));
  String minv = String("Min V " + String(OD_minCellVoltage[cellNum], 1));
  String maxcv = String("Max ChV " + String(OD_maxCellChargeVoltage[cellNum], 1));
  String soh = String("SOH " + String(OD_cellSOH[cellNum], DEC));

  display.fillScreen(GxEPD_WHITE);
  display.setTextColor(GxEPD_BLACK);
  display.setCursor(110, 30);
  display.print(num);
  display.setCursor(235, 120);
  display.print("HOME");
  display.setCursor(left, y_point);
  display.fillRect(left - 5, y_point - 8, 4, 4, GxEPD_BLACK);
  y_point += line;
  display.print(mtemp);
  display.setCursor(left, y_point);
  y_point += line;
  display.print(maxv);
  display.setCursor(left, y_point);
  y_point = top;
  display.print(minv);
  display.setCursor(right, y_point);
  y_point += line;
  display.print(maxcv);
  display.setCursor(right, y_point);
  y_point += line;
  display.print(soh);
  display.updateWindow(5, 5, 118, 286, false);
}

void Core0::printCellConfigs2(uint8_t cellNum, uint8_t config_num)
{
  //print each
  const GFXfont* font = &FreeSansBold9pt7b;
  display.setFont(font);

  uint8_t left = 10;
  uint8_t right = (296 / 2);
  uint8_t top = 60;
  uint8_t line = 20;
  uint8_t y_point = top;

  uint8_t side = left;
  if (config_num >= 3) {
    side = right;
  }

  String num = String("Cell #" + String(cellNum + 1, DEC));
  String mtemp = String("Max T " + String(OD_maxCellTemp[cellNum], DEC));
  String maxv = String("Max V " + String(OD_maxCellVoltage[cellNum], 1));
  String minv = String("Min V " + String(OD_minCellVoltage[cellNum], 1));
  String maxcv = String("Max ChV " + String(OD_maxCellChargeVoltage[cellNum], 1));
  String soh = String("SOH " + String(OD_cellSOH[cellNum], DEC));

  display.fillScreen(GxEPD_WHITE);
  display.setTextColor(GxEPD_BLACK);
  display.setCursor(110, 30);
  display.print(num);
  display.setCursor(235, 120);
  display.print("HOME");

  display.setCursor(left, y_point);
  // display.fillRect(left - 5, y_point - 8, 4, 4, GxEPD_BLACK);
  y_point += line;
  display.print(mtemp);
  display.setCursor(left, y_point);
  y_point += line;
  display.print(maxv);
  display.setCursor(left, y_point);
  y_point = top;
  display.print(minv);

  display.setCursor(right, y_point);
  y_point += line;
  display.print(maxcv);
  display.setCursor(right, y_point);
  y_point += line;
  display.print(soh);
  //display.updateWindow(15, side, 100, 140, false);
  display.updateWindow(5, 5, 103, 286, false);
}

void Core0::moveCellConfig(uint8_t cellConfig)
{
  //change position of bullet point
  uint8_t left = 5;
  uint8_t right = (296 / 2) - 5;
  uint8_t side = left;
  uint8_t top = 60;
  uint8_t line = 20;
  uint8_t y_point = top - 8;
  if (cellConfig <= 2) {
    y_point = top - 8 + 20 * cellConfig;
    side = left;
  }
  else if (cellConfig > 2 && cellConfig < NUM_CELL_CONFIGS) {
    y_point = top - 8 + 20 * (cellConfig - 3);
    side = right;
  }
  else {
    y_point = 112;
    side = 230;
  }
  display.fillRect(right, top - 8, 4, 90, GxEPD_WHITE);
  display.fillRect(left, top - 8, 4, 90, GxEPD_WHITE);
  display.fillRect(230, 112, 4, 4, GxEPD_WHITE);

  display.fillRect(side, y_point, 4, 4, GxEPD_BLACK);
  display.updateWindow(5, 5, 118, 286, false);
}

void Core0::cellData(uint8_t cellNum)
{
  //print each
  const GFXfont* font = &FreeSansBold9pt7b;
  display.setFont(font);

  uint8_t left = 10;
  uint8_t right = (296 / 2);
  uint8_t top = 60;
  uint8_t line = 20;
  uint8_t y_point = top;

  String num = String("Cell #" + String(cellNum + 1, DEC));
  String temp = String("Temp " + String(OD_cellTemperature[cellNum], 1));
  String volt = String("Volt " + String(OD_cellVoltage[cellNum], 1));
  String curr = String("Curr " + String(OD_cellBalancingCurrent[cellNum], 1));
  String soc = String("SOH " + String(OD_cellSOC[cellNum], DEC));

  display.fillScreen(GxEPD_WHITE);
  display.setTextColor(GxEPD_BLACK);
  display.setCursor(110, 30);
  display.print(num);
  display.setCursor(235, 120);
  display.print("HOME");
  display.setCursor(left, y_point);
  display.fillRect(230, 112, 4, 4, GxEPD_BLACK);
  y_point += line;
  display.print(temp);
  display.setCursor(left, y_point);
  y_point = top;
  display.print(volt);
  display.setCursor(right, y_point);
  y_point += line;
  display.print(curr);
  display.setCursor(right, y_point);
  y_point += line;
  display.print(soc);
  display.updateWindow(5, 5, 118, 286, false);
  //  while (!centerPress) {
  //    delay(20);
  //  }
  //  centerPress = false;
}


uint8_t Core0::chooseRegister()
{
  regista[0] = 0; regista[1] = 0; regista[2] = 0;
  printChooseRegister(0);
  centerPress = false; upPress = false; downPress = false; leftPress = false; rightPress = false;
  uint8_t reg = 0;
  boolean confirmed = false;
  while (1) {
    if (reg == 4 && centerPress)  {//exit
      centerPress = false;
      Serial.println("center");
      delay(50);
      return reg;
    }
    if (reg == 3 && centerPress)  {//exit
      centerPress = false;
      Serial.println("center");
      delay(50);
      return reg;
    }
    else if (leftPress) {
      leftPress = false;
      Serial.println("left");
      delay(50);
      if (reg == 0) {
        reg = 4;
      }
      else {
        reg--;
      }
      moveRegister(reg);
    }
    else if (rightPress) {
      rightPress = false;
      Serial.println("right");
      delay(50);
      if (reg == 4) {
        reg = 0;
      }
      else {
        reg++;
      }
      moveRegister(reg);
    }
    else if (upPress && reg <= 2) {
      upPress = false;
      Serial.println("up");
      delay(50);
      updateRegister(reg, true);
    }
    else if (downPress && reg <= 2) {
      downPress = false;
      Serial.println("down");
      delay(50);
      updateRegister(reg, false);
    }
    delay(20);
  }
}

void Core0::printChooseRegister(uint8_t reg) {

  display.fillScreen(GxEPD_WHITE);
  const GFXfont* f = &FreeSansBold9pt7b;  //set font
  display.setFont(f);
  display.setTextColor(GxEPD_BLACK);
  display.setCursor(90, 20);
  display.print("Choose Register");
  display.setCursor(235, 120);
  display.print("HOME");
  display.setCursor(10, 120);
  display.print("ENTER");

  f = &FreeSansBold24pt7b;  //set font
  display.setFont(f);
  display.setCursor(110, 80);
  display.print(regista[0]);
  display.setCursor(139, 80);
  display.print(regista[1]);
  display.setCursor(167, 80);
  display.print(regista[2]);

  if (reg == 0) display.fillRect(113, 84, 20, 2, GxEPD_BLACK);
  if (reg == 1) display.fillRect(140, 84, 20, 2, GxEPD_BLACK);
  if (reg == 2) display.fillRect(167, 84, 20, 2, GxEPD_BLACK);

  display.updateWindow(5, 5, 108, 286, false);
}


void Core0::updateRegister(uint8_t reg, boolean direction) {
  if (direction & regista[reg] != 9) regista[reg] += 1;
  else if (direction & regista[reg] == 9) regista[reg] = 0;
  else if (!direction & regista[reg] != 0) regista[reg] -= 1;
  else if (!direction & regista[reg] == 0) regista[reg] = 9;
  printChooseRegister(reg);
}

void Core0::moveRegister(uint8_t reg) {
  display.fillRect(113, 84, 75, 2, GxEPD_WHITE);
  display.fillRect(230, 112, 4, 4, GxEPD_WHITE);
  display.fillRect(5, 112, 4, 4, GxEPD_WHITE);

  if (reg == 0) display.fillRect(113, 84, 20, 2, GxEPD_BLACK);
  else if (reg == 1) display.fillRect(140, 84, 20, 2, GxEPD_BLACK);
  else if (reg == 2) display.fillRect(167, 84, 20, 2, GxEPD_BLACK);
  else if (reg == 3) display.fillRect(5, 112, 4, 4, GxEPD_BLACK);
  else if (reg == 4) display.fillRect(230, 112, 4, 4, GxEPD_BLACK);

  display.updateWindow(5, 5, 108, 286, false);
}

void Core0::regNotFound(uint16_t regNumb) {
  const GFXfont* font = &FreeSansBold9pt7b;
  display.setFont(font);
  display.fillScreen(GxEPD_WHITE);
  display.setCursor(40, 75);
  Serial.print("regNumb ");
  Serial.println(regNumb);
  String str = String("Register " + String(regNumb, DEC) + " not found");
  display.print(str);

  display.setCursor(235, 120);
  display.print("HOME");
  display.fillRect(230, 112, 4, 4, GxEPD_BLACK);

  display.updateWindow(5, 5, 108, 286, false);
  while(!centerPress){ delay(10);}
  centerPress = false;
}

void Core0::printEditValue(Object_Dictionary od, uint8_t reg) {
  display.fillScreen(GxEPD_WHITE);
  const GFXfont* f = &FreeSansBold9pt7b;  //set font
  display.setFont(f);
  display.setTextColor(GxEPD_BLACK);
  display.setCursor(20, 25);
  String names1 = (String)od.names;
  if (od.index >=0x3000) names1 = "Cell #" + String(od.subindex+1) + "- " + od.names;
  display.print(names1);
  display.setCursor(235, 120);
  display.print("HOME");
  display.setCursor(10, 120);
  display.print("ENTER");

  f = &FreeSansBold24pt7b;  //set font
  display.setFont(f);
  display.setCursor(65, 80); //hundreds
  Serial.print("value: ");
  Serial.println(*od.pointer);
  //value in mV so place times 1000
  display.print((int)((*od.pointer / 100000) % 10));
  display.setCursor(95, 80);
  display.print((int)((*od.pointer / 10000) % 10)); //tens
  display.setCursor(125, 80);
  display.print((int)((*od.pointer / 1000) % 10)); //ones
  display.setCursor(155, 80);
  display.print("."); //decimal
  display.setCursor(170, 80);
  display.print((int)((*od.pointer / 100) % 10)); //tenths
  display.setCursor(200, 80);
  display.print((int)((*od.pointer / 10) % 10)); //hundredths

  if (reg == 0) display.fillRect(65, 84, 20, 2, GxEPD_BLACK);
  if (reg == 1) display.fillRect(95, 84, 20, 2, GxEPD_BLACK);
  if (reg == 2) display.fillRect(125, 84, 20, 2, GxEPD_BLACK);
  if (reg == 3) display.fillRect(170, 84, 20, 2, GxEPD_BLACK);
  if (reg == 4) display.fillRect(200, 84, 20, 2, GxEPD_BLACK);
  display.updateWindow(5, 5, 108, 286, false);
}



void Core0::editValue(uint8_t regista[], boolean state, uint8_t cellNum) {
  uint8_t regNum = regista[0] * 100 + regista[1] * 10 + regista[2];
  uint16_t index;
  if (state) index = regNum + 0x2000;
  else       index = regNum + 0x3000;
  Serial.print("cellNum ");
  Serial.println(cellNum);
  Object_Dictionary od(index, cellNum);
  if (!state) od.names = 'Cell #' + cellNum + '- ' + od.names;
  Serial.println(od.names);
  int original = *od.pointer;
    boolean confirmed = false;
    printEditValue(od, 0);
    centerPress = false; upPress = false; downPress = false; leftPress = false; rightPress = false;
    uint8_t reg = 0;
    while (1) {
      if (reg == 6 && centerPress)  {//exit
        centerPress = false;
        Serial.println("center");    //testing
        *od.pointer = original;
        delay(50);
        break;
      }
      if (reg == 5 && centerPress)  {//enter
        centerPress = false;
        Serial.println("center");    //testing
        delay(50);
        confirmed = confirm();
        Serial.println(original);
        if (!confirmed) *od.pointer = original;
        original = *od.pointer;
        printEditValue(od, reg);
      }
      else if (leftPress) {
        leftPress = false;
        Serial.println("left");
        delay(50);
        if (reg == 0) {
          reg = 6;
        }
        else {
          reg--;
        }
        moveEdit(reg);
      }
      else if (rightPress) {
        rightPress = false;
        Serial.println("right");
        delay(50);
        if (reg == 6) {
          reg = 0;
        }
        else {
          reg++;
        }
        moveEdit(reg);
      }
      else if (upPress) {
        upPress = false;
        Serial.println("up");
        delay(50);
        od = updateValue(od, reg, true);
        Serial.print("pointer val: ");
        Serial.println(*od.pointer);
        
      }
      else if (downPress) {
        downPress = false;
        Serial.println("down");
        delay(50);
        od = updateValue(od, reg, false);
        Serial.print("pointer val: ");
        Serial.println(*od.pointer);
}
      delay(50);
    
  }
}

void Core0::viewValue(uint8_t regista[], boolean state, uint8_t cellNum) {
  uint8_t regNum = regista[0] * 100 + regista[1] * 10 + regista[2];
  uint16_t index;
  if (state) index = regNum + 0x2000;
  else       index = regNum + 0x3000;
  Object_Dictionary od(index, cellNum);
  //if (!state) String names = od.names = 'Cell #' + cellNum + '- ' + od.names;
  centerPress = false; upPress = false; downPress = false; leftPress = false; rightPress = false;
    
    while (!centerPress) {
          printViewValue(od);
          delay(1000);
    }    
    centerPress = false;
}

void Core0::printViewValue(Object_Dictionary od) {
  display.fillScreen(GxEPD_WHITE);
  const GFXfont* f = &FreeSansBold9pt7b;  //set font
  display.setFont(f);
  display.setTextColor(GxEPD_BLACK);
  display.setCursor(20, 25);
  Serial.println(od.names);
  String names1 = (String)od.names;
  if (od.index >=0x3000) names1 = "Cell #" + String(od.subindex+1) + "- " + od.names;
  display.print(names1);
  display.setCursor(235, 120);
  display.print("HOME");


  f = &FreeSansBold24pt7b;  //set font
  display.setFont(f);
  display.setCursor(65, 80); //hundreds
  Serial.print("value: ");
  Serial.println(*od.pointer);
  //value in mV so place times 1000
  display.print((int)((*od.pointer / 100000) % 10));
  display.setCursor(95, 80);
  display.print((int)((*od.pointer / 10000) % 10)); //tens
  display.setCursor(125, 80);
  display.print((int)((*od.pointer / 1000) % 10)); //ones
  display.setCursor(155, 80);
  display.print("."); //decimal
  display.setCursor(170, 80);
  display.print((int)((*od.pointer / 100) % 10)); //tenths
  display.setCursor(200, 80);
  display.print((int)((*od.pointer / 10) % 10)); //hundredths
  
  display.fillRect(230, 112, 4, 4, GxEPD_BLACK);

  display.updateWindow(5, 5, 108, 286, false);
}

Object_Dictionary Core0::updateValue(Object_Dictionary od, uint8_t place, boolean direction) {
  //if place = 0--100000, 1--10000, 2--1000, 3--100, 4--10
  int temp =pow(10,(5-place));
//  Serial.println(temp2);
    if      (direction &  (int)(*od.pointer / temp) % 10 != 9)  od.pointer += temp;
    else if (direction &  (int)(*od.pointer / temp) % 10 == 9)  od.pointer -= 9 * temp;
    else if (!direction & (int)(*od.pointer / temp) % 10 != 0)  od.pointer -= temp;
    else if (!direction & (int)(*od.pointer / temp) % 10 == 0)  od.pointer += 9 * temp;
  
  printEditValue(od, place);
  return od;
}

void Core0::moveEdit(uint8_t reg) {
  display.fillRect(65, 84, 180, 2, GxEPD_WHITE);
  display.fillRect(230, 112, 4, 4, GxEPD_WHITE);
  display.fillRect(5, 112, 4, 4, GxEPD_WHITE);

  if      (reg == 0) display.fillRect(65,  84, 20, 2, GxEPD_BLACK);
  else if (reg == 1) display.fillRect(95, 84, 20, 2, GxEPD_BLACK);
  else if (reg == 2) display.fillRect(125, 84, 20, 2, GxEPD_BLACK);
  else if (reg == 3) display.fillRect(170, 84, 20, 2, GxEPD_BLACK);
  else if (reg == 4) display.fillRect(200, 84, 20, 2, GxEPD_BLACK);
  else if (reg == 5) display.fillRect(5,  112,  4, 4, GxEPD_BLACK);
  else if (reg == 6) display.fillRect(230, 112, 4, 4, GxEPD_BLACK);

  display.updateWindow(5, 5, 108, 286, false);
}

void Core0::chargeScreen() {
  const GFXfont* font = &FreeSansBold9pt7b;
  display.setFont(font);

  display.fillScreen(GxEPD_WHITE);
  display.setTextColor(GxEPD_BLACK);
  display.setCursor(80, 25);
  display.print("Fault Resolved?");
  display.setCursor(30, 50);
  display.print("Yes");
  display.setCursor(30, 80);
  display.print("No");
  display.setCursor(235, 120);
  display.print("HOME");
  display.fillRect(20, 45, 4, 4, GxEPD_BLACK);
  display.updateWindow(5, 5, 118, 286, false);
}

void Core0::chargePartial(uint8_t charge_index) {
  //change position of bullet point
  uint8_t x_point = 20;
  uint8_t y_point = 75;

  if (charge_index == 0) {
    y_point = 45;
    x_point = 20;
  }
  else if (charge_index == 1) {
    y_point = 75;
    x_point = 20;
  }
  else {
    y_point = 112;
    x_point = 230;
  }
  display.fillRect(20, 45, 4, 35, GxEPD_WHITE);
  display.fillRect(230, 112, 4, 4, GxEPD_WHITE);
  display.fillRect(x_point, y_point, 4, 4, GxEPD_BLACK);
  display.updateWindow(5, 5, 118, 286, false);
}