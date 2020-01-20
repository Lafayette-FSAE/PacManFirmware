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

GxIO_Class io(SPI, 15, 22, 21);
GxEPD_Class display(io, 21, 23);

#define CENTER_BUTTON 27
#define UP_BUTTON 19
#define DOWN_BUTTON 18
#define LEFT_BUTTON 36
#define RIGHT_BUTTON 39
#define NUM_CELLS 16

void setupCore0() {

  display.init();

  const GFXfont* f = &FreeSansBold9pt7b;
  display.setFont(f);
  display.setTextColor(GxEPD_BLACK);

  pinMode(CENTER_BUTTON, INPUT); //button
  pinMode(UP_BUTTON, INPUT); //button
  pinMode(DOWN_BUTTON, INPUT); //button
  pinMode(LEFT_BUTTON, INPUT); //button
  pinMode(RIGHT_BUTTON, INPUT); //button
  attachInterrupt(digitalPinToInterrupt(CENTER_BUTTON), CButton, RISING);
  attachInterrupt(digitalPinToInterrupt(UP_BUTTON), UButton, RISING);
  attachInterrupt(digitalPinToInterrupt(DOWN_BUTTON), DButton, RISING);
  attachInterrupt(digitalPinToInterrupt(LEFT_BUTTON), LButton, RISING);
  attachInterrupt(digitalPinToInterrupt(RIGHT_BUTTON), RButton, RISING);
}

//misc configs
boolean id = 0; boolean sl = 0; int soc = 50; float max_pc = 250; float min_pc = 0; boolean airs = 0;//defaults
Configs configs[NUM_CELLS];
Misc_Configs misc_configs[1] = {{id, airs, sl, soc, max_pc, min_pc}};

cell currentCellData[NUM_CELLS];
cell currentCellDataInt[NUM_CELLS];

//constructor
Core0::Core0(cell* cells1, float* extFault, boolean* AIRSOp, SemaphoreHandle_t* cellSem, SemaphoreHandle_t* extFaultSem, 
              SemaphoreHandle_t* AIRSOpSem, SemaphoreHandle_t* smpleSem, int* smple)
{
  cellArrayPointer = cells1;
  externalFaultPointer = extFault;
  AIRSOpenPointer = AIRSOp;
  samplePointer = smple;

  cellArraySemPointer = cellSem;
  externalFaultSemPointer = extFaultSem;
  AIRSOpenSemPointer = AIRSOpSem;
  sampleSemPointer = smpleSem;
}

float voltage1; float current1; float temp1; uint16_t soc_test; 

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
  unsigned long interrupt_time = millis();
  if (interrupt_time - last_interrupt_time > dbDelay) {
    centerPress = true;
    Serial.println("c");
  }
  else {
    centerPress = false;
  }
  last_interrupt_time = interrupt_time;
}

void LButton() //interrupt with debounce
{
  volatile static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  if (interrupt_time - last_interrupt_time > dbDelay) {
    leftPress = true;
    Serial.println("l");
  }
  else {
    leftPress = false;
  }
  last_interrupt_time = interrupt_time;
}

void RButton() //interrupt with debounce
{
  volatile static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  if (interrupt_time - last_interrupt_time > dbDelay) {
    rightPress = true;
    Serial.println("r");
  }
  else {
    rightPress = false;
  }
  last_interrupt_time = interrupt_time;
}

void UButton() //interrupt with debounce
{
  volatile static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  if (interrupt_time - last_interrupt_time > dbDelay) {
    upPress = true;
    Serial.println("u");
  }
  else {
    upPress = false;
  }
  last_interrupt_time = interrupt_time;
}

void DButton() //interrupt with debounce
{
  volatile static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  if (interrupt_time - last_interrupt_time > dbDelay) {
    downPress = true;
    Serial.println("d");
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
  Misc_Configurations,
  Charging,
  Cell_Data,
  Cell_Configs
} State;

void Core0::fsm() {

  boolean config_index = true;
  boolean cell_index = true;
  uint8_t main_index = 0;
  uint8_t charge_index = 0;
  boolean charging = false;

  State nextState = Main;
  State state = Main;
  setUpMain(id, charging);
  defaultCellConfigs();

  centerPress = false; upPress = false; downPress = false; leftPress = false; rightPress = false;

  while (1) {
    switch (nextState) {
      case Main: {
          if (leftPress || upPress) {
            leftPress = false; upPress = false;
            if (main_index == 0) main_index = 8;
            else if (main_index == 1) main_index = 16;
            else if (main_index == 9) main_index = 0;
            else main_index -= 1;
          }
          else if (rightPress || downPress) {
            rightPress = false; downPress = false;
            if (main_index == 0) main_index = 9;
            else if (main_index == 16) main_index = 1;
            else if (main_index == 8) main_index = 0;
            else main_index += 1;
          }
          else if (centerPress && main_index == 0) {
            Serial.println("center");  //testing
            centerPress = false;
            nextState = Config_State;
          }
          else if (centerPress && main_index != 0) {
            Serial.println("center");  //testing
            centerPress = false;
            nextState = Cell_State;
          }
          else if (state != Main) {
            setUpMain(id, charging);
            main_index = 0;
            state = Main;
          }
          xSemaphoreTake(*cellArraySemPointer, portMAX_DELAY );
          for (int index = 0; index < NUM_CELLS; index++) {
            currentCellData[index] = cellArrayPointer[index];
          }
          //memcpy(currentCellData, currentCellDataInt, sizeof(currentCellDataInt));
          xSemaphoreGive(*cellArraySemPointer);
          voltage1 = 0; current1 = 0; temp1 = 0; soc_test = 0;
          for (int i = 0; i < NUM_CELLS; i++) {
            voltage1 += currentCellData[i].cellVoltage;
            current1 += currentCellData[i].balanceCurrent;
            temp1 = max(temp1, currentCellData[i].cellTemp);
            soc_test += currentCellData[i].SOC;
          }
          current1 = current1 / NUM_CELLS;
          soc_test = soc_test / NUM_CELLS;
          Serial.println("main_index");
          Serial.println(main_index);
          mainPartialUpdate(temp1, soc_test, voltage1, current1, main_index);
        }
        break;

      case Config_State: {   
          if (centerPress && config_index == true) {
            Serial.println("center");    //testing
            centerPress = false;
            nextState = Misc_Configurations;
          }
          else if (centerPress && config_index == false) {
            Serial.println("center");    //testing
            centerPress = false;
            nextState = Charging;
          }
          else if (upPress || downPress || leftPress || rightPress) {
            Serial.println("up/down/left/right");
            upPress = false; downPress = false; leftPress = false; rightPress = false;
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
            Serial.println("center");    //testing
            centerPress = false;
            nextState = Cell_Configs;
          }
          else if (centerPress && cell_index == false) {
            Serial.println("center");    //testing
            centerPress = false;
            nextState = Cell_Data;
          }
          else if (upPress || downPress || leftPress || rightPress) {
            Serial.println("up/down/left/right");
            upPress = false; downPress = false; leftPress = false; rightPress = false;
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

      case Misc_Configurations: {
          miscConfigs(); //exits function if on home button
          nextState = Main;
        }
        break;

      case Charging: {  
          if (centerPress && charge_index == 0) {
            Serial.println("center");    //testing
            centerPress = false;
            if (confirm()) {charging = 1;}
            Serial.println("charging");
            Serial.print(charging);
            chargeScreen();
          }
          else if (centerPress && charge_index == 1) {
            Serial.println("center");    //testing
            centerPress = false;
            if (confirm()) {charging = 0;}
            charge_index = 0;
            chargeScreen();
          }
          else if (centerPress && charge_index == 2) {
            Serial.println("center");    //testing
            centerPress = false;
            Serial.println("charging1");
            Serial.print(charging);
            nextState = Main;
          }
          else if (upPress || leftPress) {
            Serial.println("up/left");
            upPress = false; leftPress = false;
            if (charge_index != 0) charge_index -= 1;
            else charge_index = 2;
            chargePartial(charge_index);
          }
          else if (downPress || rightPress) {
            Serial.println("downright");
            downPress = false; rightPress = false;
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

      case Cell_Configs: {
          cellConfigs(main_index-1); //exits function if on home button
          nextState = Main;
        }
        break;

      case Cell_Data: {   //done
          cellData(main_index, currentCellData[main_index]);
          nextState = Main;
        }
        break;
    }
  }
}

//confirmation screen
boolean Core0::confirm() {
  boolean confirm_index = true;
  const GFXfont* font = &FreeSansBold9pt7b;
  display.setFont(font);

  display.fillRect(88, 18, 120, 76, GxEPD_BLACK);
  display.fillRect(90, 20, 116, 72, GxEPD_WHITE);
  display.setTextColor(GxEPD_BLACK);
  display.setCursor(110, 40);
  display.print("Confirm?");
  display.setCursor(110, 80);
  display.print("Yes");
  display.setCursor(160, 80);
  display.print("No");
  display.fillRect(110, 82, 30, 1, GxEPD_BLACK);
  display.updateWindow(5, 5, 118, 286, false);
  while (!centerPress) {
    if (upPress || downPress || leftPress || rightPress) {
      Serial.println("up/down/left/right");
      upPress = false; downPress = false; leftPress = false; rightPress = false;
      confirm_index = !confirm_index;
      if (confirm_index) {
        display.fillRect(160, 82, 25, 1, GxEPD_WHITE);
        display.fillRect(110, 82, 30, 1, GxEPD_BLACK);
      }
      else {
        display.fillRect(110, 82, 30, 1, GxEPD_WHITE);
        display.fillRect(160, 82, 25, 1, GxEPD_BLACK);
      }
      display.updateWindow(5, 5, 118, 286, false);
    }
    delay(20);
  }
  centerPress = false;
  return confirm_index;
}

void Core0::setUpMain(boolean id, boolean charging) {
  display.setRotation(0);
  if (id) {
    display.drawExampleBitmap(gImage_main1, 0, 0, 128, 296, GxEPD_BLACK); //true = Pack 1
  }
  else {
    display.drawExampleBitmap(gImage_main2, 0, 0, 128, 296, GxEPD_BLACK); //false = Pack 2
  }

  display.setRotation(45);
  const GFXfont* f = &FreeSansBold9pt7b;  //set font
  display.setFont(f);
  if (charging) {
    display.setCursor(265, 15);
    display.print("Ch");
  }

  display.update();
  display.update();
  display.setRotation(45);
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
  display.fillRect(box_x, box_y - box_h, box_w, box_h, GxEPD_WHITE);

  display.setCursor(box_x, box_y);  //print SOC
  display.print(SOC);

  display.setCursor(box_x + indent, box_y); //print V
  display.print(voltage);

  display.setCursor(box_x + 2 * indent, box_y); //print current
  display.print(current);

  display.setCursor(box_x + 3 * indent, box_y); //print temp
  display.print(temp);
  display.updateWindow(5, 5, 118, 286, false);

  checkCells(0); //calls cell partial update if need be
  //checkForFaults()--calls faults();
}

void Core0::checkCells(uint8_t currentCell) {
  for (uint8_t cell = currentCell; cell < NUM_CELLS; cell++) {
    if (configs[cell].SOH == 0) cellPartialUpdate(2, cell);
    //else if(temp>) cellPartialUpdate(1, cell);
    //else if(voltage>3) cellPartialUpdate(3, cell);
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
  if (errorType == 1) { //temp
    display.fillRect(box_x, box_y - box_h, box_w, box_h, GxEPD_BLACK);
    display.setTextColor(GxEPD_WHITE);
    display.setCursor(box_x + 1, box_y - 6);
    display.print("V");
  }
  else if (errorType == 2) { //soh bad
    display.fillRect(box_x, box_y - box_h, box_w, box_h, GxEPD_BLACK);
  }
  else if (errorType == 3) { //high voltage
    display.fillRect(box_x, box_y - box_h, box_w, box_h, GxEPD_BLACK);
    display.setTextColor(GxEPD_WHITE);
    display.setCursor(box_x + 1, box_y - 6);
    display.print("T");
  }

  display.updateWindow(128 - box_y, box_x, box_h, box_w, false);
  if (cellNum < NUM_CELLS - 1) {
    checkCells(cellNum + 1);
  }
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
    display.print("High Volt");
  }
  else if (errorType == 5) { //Low SOC
    display.print("Low SOC");
  }
  display.update();
}

void Core0::mainConfigScreen()
{
  const GFXfont* font = &FreeSansBold12pt7b;
  display.setFont(font);

  display.fillScreen(GxEPD_WHITE);
  display.setTextColor(GxEPD_BLACK);
  display.setCursor(30, 50);
  display.print("Misc Configs");
  display.setCursor(30, 80);
  display.print("Charging");
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

void Core0::defaultCellConfigs() {
  for (int i = 0; i < NUM_CELLS; i++) {
    defineCellConfigs(55, 3.3, 2.5, 3.5, 1, i);
  }
}

void Core0::defineCellConfigs(int maxTemp, float maxV, float minV, float maxCV, boolean soh, int index) //pretty much set I think
{
  configs[index].max_temp = maxTemp; //=input[]
  configs[index].max_voltage = maxV;
  configs[index].min_voltage = minV;
  configs[index].max_charge_voltage = maxCV;
  configs[index].SOH = soh;
}

void Core0::cellConfigs(uint8_t cellNum)
{
  //update screen to print all options for cell
  printCellConfigs(cellNum);
  boolean confirmed = false;
  uint8_t cell_config = 0;
  centerPress = false; upPress = false; downPress = false; leftPress = false; rightPress = false;

  while (1) {
    if (cell_config == NUM_CELL_CONFIGS && centerPress)  { //exit
      Serial.println("center");
      centerPress = false;
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
    else if (centerPress){
      centerPress = false;
      Serial.println("center");
      delay(50);
      const GFXfont* f = &FreeSansBold9pt7b;  //set font
      display.setFont(f);
      display.setTextColor(GxEPD_BLACK);
      display.setRotation(45);
      display.setCursor(270, 15);
      display.print("*");
      display.updateWindow(128-15, 270, 12, 20, false);
      Configs original[1] = {{configs[cellNum].max_temp, configs[cellNum].max_voltage, configs[cellNum].min_voltage, 
                              configs[cellNum].max_charge_voltage, configs[cellNum].SOH}};
    while(!centerPress){
    if (upPress || rightPress) {
      Serial.println("up");
      upPress = false; downPress = false;
      delay(50);
      updateCellConfig(cellNum, cell_config, true);
    }
    else if (downPress || leftPress) {
      Serial.println("down");
      downPress = false; leftPress = false;
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
      else{
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
    if (direction) { configs[cellNum].max_temp += 1; }
    else { configs[cellNum].max_temp -= 1; }
  }
  else if (cellConfig == 1) { //max_voltage
    if (direction) { configs[cellNum].max_voltage += 0.1; }
    else { configs[cellNum].max_voltage -= 0.1; }
  }
  else if (cellConfig == 2) { //min voltage
    if (direction) { configs[cellNum].min_voltage += 0.1;  }
    else { configs[cellNum].min_voltage -= 0.1; }
  }
  else if (cellConfig == 3) { //max charge voltage
    if (direction) { configs[cellNum].max_charge_voltage += 0.1; }
    else { configs[cellNum].max_charge_voltage -= 0.1; }
  }
  else if (cellConfig == 4) { //soh
    configs[cellNum].SOH = !configs[cellNum].SOH;
  }
  printCellConfigs2(cellNum, cellConfig);
}

void Core0::cellChangeBack(uint8_t cellNum, uint8_t cellConfig, Configs original[1]){
  if (cellConfig == 0){configs[cellNum].max_temp = original[0].max_temp;}
  else if (cellConfig == 1){configs[cellNum].max_voltage = original[0].max_voltage;}
  else if (cellConfig == 2){configs[cellNum].min_voltage = original[0].min_voltage;}
  else if (cellConfig == 3){configs[cellNum].max_charge_voltage = original[0].max_charge_voltage;}
  else if (cellConfig == 4){configs[cellNum].SOH = original[0].SOH;}
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
  String mtemp = String("Max T " + String(configs[cellNum].max_temp, DEC));
  String maxv = String("Max V " + String(configs[cellNum].max_voltage, 1));
  String minv = String("Min V " + String(configs[cellNum].min_voltage, 1));
  String maxcv = String("Max ChV " + String(configs[cellNum].max_charge_voltage, 1));
  String soh = String("SOH " + String(configs[cellNum].SOH, DEC));

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
  String mtemp = String("Max T " + String(configs[cellNum].max_temp, DEC));
  String maxv = String("Max V " + String(configs[cellNum].max_voltage, 1));
  String minv = String("Min V " + String(configs[cellNum].min_voltage, 1));
  String maxcv = String("Max ChV " + String(configs[cellNum].max_charge_voltage, 1));
  String soh = String("SOH " + String(configs[cellNum].SOH, DEC));

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
  display.fillRect(right, top-8, 4, 90, GxEPD_WHITE);
  display.fillRect(left, top-8, 4, 90, GxEPD_WHITE);
  display.fillRect(230, 112, 4, 4, GxEPD_WHITE);
  
  display.fillRect(side, y_point, 4, 4, GxEPD_BLACK);
  display.updateWindow(5, 5, 118, 286, false);
}

void Core0::cellData(uint8_t cellNum, cell current_cell)
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
  String temp = String("Temp " + String(current_cell.cellTemp, 1));
  String volt = String("Volt " + String(current_cell.cellVoltage, 1));
  String curr = String("Curr " + String(current_cell.balanceCurrent, 1));
  String soc = String("SOH " + String(current_cell.SOC, DEC));

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
  while (!centerPress) {
    delay(20);
  }
  centerPress = false;
}

#define NUM_MISC_CONFIGS 6

void Core0::miscConfigs()
{
  printMiscConfigs();
  centerPress = false; upPress = false; downPress = false; leftPress = false; rightPress = false;
  uint8_t misc_config = 0;
  boolean confirmed = false;
  while (1) {
    if (misc_config == NUM_MISC_CONFIGS && centerPress)  {//exit
      Serial.println("center");
      centerPress = false;
      delay(50);
      break;
    }
    else if (leftPress || upPress) {
      Serial.println("left/up");
      leftPress = false; upPress = false;
      delay(50);
      if (misc_config == 0) {
        misc_config = NUM_MISC_CONFIGS;
      }
      else {
        misc_config--;
      }
      moveMiscConfig(misc_config);
    }
    else if (rightPress || downPress) {
      Serial.println("right");
      rightPress = false; downPress = false;
      delay(50);
      if (misc_config == NUM_MISC_CONFIGS) {
        misc_config = 0;
      }
      else {
        misc_config++;
      }
      moveMiscConfig(misc_config);
    }
    else if (centerPress) {
      Serial.println("center");
      centerPress = false;
      delay(50);
      const GFXfont* f = &FreeSansBold9pt7b;  //set font
      display.setFont(f);
      display.setTextColor(GxEPD_BLACK);
      display.setRotation(45);
      display.setCursor(270, 15);
      display.print("*");
      display.updateWindow(128-15, 270, 12, 20, false);
      Misc_Configs original[1] = {{misc_configs[0].pack_id, misc_configs[0].airs_state, misc_configs[0].sl_state, 
                                  misc_configs[0].SOC_min, misc_configs[0].max_pack_current, misc_configs[0].min_pack_current}};
      while (!centerPress) {
        if (upPress || rightPress) {
          Serial.println("up");
          upPress = false; rightPress = false;
          delay(50);
          updateMiscConfig(misc_config, true);
        }
        else if (downPress || leftPress) {
          Serial.println("down");
          downPress = false; leftPress = false;
          delay(50);
          updateMiscConfig(misc_config, false);
        }
        delay(20);
      }
      centerPress = false;
      confirmed = confirm();
      
      //misc_config = 0;
      if (!confirmed) {
          miscChangeBack(misc_config, original);
      }
      else{
        printMiscConfigs2(misc_config);
      }
      moveMiscConfig(misc_config);
    }
    delay(30);
  }
}

void Core0::updateMiscConfig(uint8_t miscConfig, boolean direction)  //finish this
{
  //change value of config
  if (miscConfig == 0) { misc_configs[0].pack_id = !misc_configs[0].pack_id;  } //pack id
  else if (miscConfig == 1) { misc_configs[0].airs_state = !misc_configs[0].airs_state;  } //airs
  else if (miscConfig == 2) { misc_configs[0].sl_state = !misc_configs[0].sl_state;  } //sl
  else if (miscConfig == 3) { //soc min
    if (direction) { misc_configs[0].SOC_min += 1; }
    else { misc_configs[0].SOC_min -= 1; }
  }
  else if (miscConfig == 4) { //max current
    if (direction) { misc_configs[0].max_pack_current += 0.1; }
    else { misc_configs[0].max_pack_current -= 0.1; }
  }
  else if (miscConfig == 5) { //min current
    if (direction) { misc_configs[0].min_pack_current += 0.1; }
    else { misc_configs[0].min_pack_current -= 0.1; }
  }
  printMiscConfigs2(miscConfig);
}

void Core0::miscChangeBack(uint8_t miscConfig, Misc_Configs original[1])  //finish this
{
  if (miscConfig == 0) { misc_configs[0].pack_id = !misc_configs[0].pack_id; }
  else if (miscConfig == 1) { misc_configs[0].airs_state = !misc_configs[0].airs_state; }
  else if (miscConfig == 2) { misc_configs[0].sl_state = !misc_configs[0].sl_state; }
  else if (miscConfig == 3) { misc_configs[0].SOC_min =original[0].SOC_min;  }
  else if (miscConfig == 4) { misc_configs[0].max_pack_current = original[0].max_pack_current; }
  else if (miscConfig == 5) { misc_configs[0].min_pack_current = original[0].min_pack_current;  }
  printMiscConfigs2(miscConfig);
}

void Core0::printMiscConfigs()
{
  //print each
  const GFXfont* font = &FreeSansBold9pt7b;
  display.setFont(font);

  uint8_t left = 10;
  uint8_t right = (296 / 2);
  uint8_t top = 50;
  uint8_t line = 20;
  uint8_t y_point = top;

  String pack = String("PackID " + String(misc_configs[0].pack_id, DEC));
  String air = String("AIRS " + String(misc_configs[0].airs_state, DEC));
  String sl = String("SL " + String(misc_configs[0].sl_state, DEC));
  String minSOC = String("SOCmin " + String(misc_configs[0].SOC_min, DEC));
  String maxC = String("Max C " + String(misc_configs[0].max_pack_current, 1));
  String minC = String("Min C " + String(misc_configs[0].min_pack_current, 1));

  display.fillScreen(GxEPD_WHITE);
  display.setTextColor(GxEPD_BLACK);
  display.setCursor(235, 120);
  display.print("HOME");

  display.setCursor(left, y_point);
  display.fillRect(left - 5, y_point - 8, 4, 4, GxEPD_BLACK);
  y_point += line;
  display.print(pack);
  display.setCursor(left, y_point);
  y_point += line;
  display.print(air);
  display.setCursor(left, y_point);
  y_point = top;
  display.print(sl);

  display.setCursor(right, y_point);
  y_point += line;
  display.print(minSOC);
  display.setCursor(right, y_point);
  y_point += line;
  display.print(maxC);
  display.setCursor(right, y_point);
  y_point += line;
  display.print(minC);
  display.updateWindow(5, 5, 108, 286, false);
}

void Core0::printMiscConfigs2(uint8_t config_num)
{
  //print each
  const GFXfont* font = &FreeSansBold9pt7b;
  display.setFont(font);

  uint8_t left = 10;
  uint8_t right = (296 / 2);
  uint8_t top = 50;
  uint8_t line = 20;
  uint8_t y_point = top;

  uint8_t side = left;
  if (config_num >= 3) {
    side = right;
  }

  String pack = String("PackID " + String(misc_configs[0].pack_id, DEC));
  String air = String("AIRS " + String(misc_configs[0].airs_state, DEC));
  String sl = String("SL " + String(misc_configs[0].sl_state, DEC));
  String minSOC = String("SOCmin " + String(misc_configs[0].SOC_min, DEC));
  String maxC = String("Max C " + String(misc_configs[0].max_pack_current, 1));
  String minC = String("Min C " + String(misc_configs[0].min_pack_current, 1));

  display.fillScreen(GxEPD_WHITE);
  display.setTextColor(GxEPD_BLACK);
  display.setCursor(235, 120);
  display.print("HOME");

  display.setCursor(left, y_point);
  y_point += line;
  display.print(pack);
  display.setCursor(left, y_point);
  y_point += line;
  display.print(air);
  display.setCursor(left, y_point);
  y_point = top;
  display.print(sl);

  display.setCursor(right, y_point);
  y_point += line;
  display.print(minSOC);
  display.setCursor(right, y_point);
  y_point += line;
  display.print(maxC);
  display.setCursor(right, y_point);
  y_point += line;
  display.print(minC);
  display.updateWindow(5, 5, 103, 286, false);
}

void Core0::moveMiscConfig(uint8_t miscConfig)
{
  //change position of bullet point
  uint8_t left = 5;
  uint8_t right = (296 / 2) - 5;
  uint8_t side = left;
  uint8_t top = 50;
  uint8_t line = 20;
  uint8_t y_point = top - 8;
  if (miscConfig <= 2) {
    y_point = top - 8 + 20 * miscConfig;
    side = left;
  }
  else if (miscConfig > 2 && miscConfig < NUM_MISC_CONFIGS) {
    y_point = top - 8 + 20 * (miscConfig - 3);
    side = right;
  }
  else {
    y_point = 112;
    side = 230;
  }
  display.fillRect(right, top-8, 4, 90, GxEPD_WHITE);
  display.fillRect(left, top-8, 4, 90, GxEPD_WHITE);
  display.fillRect(230, 112, 4, 4, GxEPD_WHITE);
  display.fillRect(side, y_point, 4, 4, GxEPD_BLACK);
  display.updateWindow(5, 5, 118, 286, false);
}

void Core0::chargeScreen() {
  const GFXfont* font = &FreeSansBold9pt7b;
  display.setFont(font);

  display.fillScreen(GxEPD_WHITE);
  display.setTextColor(GxEPD_BLACK);
  display.setCursor(95, 25);
  display.print("Charging?");
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
