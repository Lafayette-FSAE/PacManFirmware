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

#include "References/Adafruit_GFX.h"
#include "References/Adafruit_GFX.cpp"
#include "References/Adafruit_SPITFT.h"
#include "References/Adafruit_SPITFT.cpp"
#include "References/Adafruit_SPITFT_Macros.h"
#include "References/gfxfont.h"
#include "References/glcdfont.c"
#include "References/GxEPD.h"
#include "References/GxEPD.cpp"
#include "References/GxFont_GFX.h"
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

  //  Serial.begin(115200);
  display.init();

  //setUpMain(true); //id

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
boolean id = 0; boolean sl = 0; int soc = 50; float max_pc = 250; float min_pc = 0; boolean airs = 0;//need to set the equal to inputs but give defaults

//typedef struct
//{
//  float curr_voltage;
//  float curr_curr;
//  float curr_temp;
//  float curr_SOC;
//} cell;



cell currentCellData[NUM_CELLS];
cell currentCellDataInt[NUM_CELLS];
//cell* currentCellDataInt;

//define all variables for constructor
//cell* currentCellData; //current cell data array (for sems)
//float extFaultData; //external fault data float (for sems)
//float extFaultDataInt;
//SemaphoreHandle_t cellArraySem;
//SemaphoreHandle_t externalFaultSem;
//SemaphoreHandle_t AIRSOpenSem;
//SemaphoreHandle_t sampleSem;
//int* samplePointer=0;
//SemaphoreHandle_t* sampleSemPointer=0;

//constructor
Core0::Core0(cell* cells1, float* extFault, boolean* AIRSOp, SemaphoreHandle_t* cellSem, SemaphoreHandle_t* extFaultSem, SemaphoreHandle_t* AIRSOpSem, SemaphoreHandle_t* smpleSem, int* smple)
{
  //do this for all
//  for (int i = 0; i < sizeof(cells1); i++) {
//    currentCellDataInt[i] = cells1[i];
//  }
  cellArrayPointer = cells1;
  externalFaultPointer = extFault;
  AIRSOpenPointer = AIRSOp;
  samplePointer = smple;
  
  cellArraySemPointer = cellSem;
  externalFaultSemPointer = extFaultSem;
  AIRSOpenSemPointer = AIRSOpSem;
  sampleSemPointer = smpleSem;

}

float voltage1 = 0;  //to be removed
float current1 = 0;   //to be removed
float temp1 = 0;      //to be removed
uint16_t soc_test = 100;  //to be removed


void Core0::startCore0() {
  for (;;) {
    //copy memory block for cellArraySem and externalFaultSem
    //    xSemaphoreTake(cellArraySem, portMAX_DELAY );
    //    memcpy(&currentCellData, &currentCellDataInt, sizeof(currentCellDataInt));
    //    xSemaphoreGive(cellArraySem);
    //
    //    xSemaphoreTake(externalFaultSem, portMAX_DELAY );
    //    memcpy(&extFaultData, &extFaultDataInt, sizeof(extFaultDataInt));
    //    xSemaphoreGive(externalFaultSem);

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

typedef struct
{
  int max_temp;
  float max_voltage;
  float min_voltage;
  float max_charge_voltage;
  boolean SOH; //true = good cell, false = bad cell--> if SOH is false, no longer allow cell to cause faults
} Configs;


#define NUM_CELLS 16
Configs configs[NUM_CELLS];

void Core0::defineCellConfigs(int maxTemp, float maxV, float minV, float maxCV, boolean soh, int index) //pretty much set I think
{
  configs[index].max_temp = maxTemp; //=input[]
  configs[index].max_voltage = maxV;
  configs[index].min_voltage = minV;
  configs[index].max_charge_voltage = maxCV;
  configs[index].SOH = soh;
}

typedef struct
{
  boolean pack_id; //true = Pack 1, false = Pack 2  //change all to floats
  boolean airs_state; //true = closed, false = open
  boolean sl_state; //true = closed, false = open
  uint16_t SOC_min;
  float max_pack_current;
  float min_pack_current;
} Misc_Configs;

Misc_Configs misc_configs[1] = {{id, airs, sl, soc, max_pc, min_pc}};

void Core0::defaultCellConfigs(){
  for(int i = 0; i<NUM_CELLS; i++){
    defineCellConfigs(55, 3.3, 2.5, 3.5, 1, i);
  }
}

void Core0::setUpMain(boolean id) {
  display.setRotation(0);
  if (id) {
    display.drawExampleBitmap(gImage_main1, 0, 0, 128, 296, GxEPD_BLACK); //true = Pack 1
  }
  else {
    display.drawExampleBitmap(gImage_main2, 0, 0, 128, 296, GxEPD_BLACK); //false = Pack 2
  }
  display.update();
  display.update();
  display.setRotation(45);
}

void Core0::mainPartialUpdate(float temperature, uint16_t soc, float volt, float curr)
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

  display.fillRect(box_x, box_y - box_h, box_w, box_h, GxEPD_WHITE);

  display.setCursor(box_x, box_y);  //print SOC
  display.print(SOC);

  display.setCursor(box_x + indent, box_y); //print V
  display.print(voltage);

  display.setCursor(box_x + 2 * indent, box_y); //print current
  display.print(current);

  display.setCursor(box_x + 3 * indent, box_y); //print temp
  display.print(temp);

  display.updateWindow(128 - box_y, box_x, box_h, box_w, false);
  checkCells(0); //calls cell partial update if need be
  //checkForFaults()--calls faults();
}

void Core0::checkCells(uint8_t currentCell){
  for (uint8_t cell = currentCell; cell< NUM_CELLS; cell++){
    if(configs[cell].SOH == 0) cellPartialUpdate(2, cell);
    //else if(temp>) cellPartialUpdate(1, cell);
    //else if(voltage>3) cellPartialUpdate(3, cell);
  }
}
void Core0::checkCells2(uint8_t currentCell){
  for (uint8_t cell = currentCell; cell< NUM_CELLS; cell++){
    if(configs[cell].SOH == 0) cellPartialUpdate2(2, cell);
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

  //cellNum = cellNum - 1; //start index at 1

  if (cellNum < 8) { //seg1
    box_x = 11 + (box_w - 1) * cellNum;
  }
  else { //seg2
    box_x = 180 + (box_w - 1) * (cellNum-8);
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
  if(cellNum<NUM_CELLS-1){  checkCells(cellNum+1);}
}

void Core0::cellPartialUpdate2(int errorType, int cellNum)
{
  uint16_t box_w = 14;
  uint16_t box_h = 23;
  uint16_t box_y = 78;
  uint16_t box_x = 0;

  //cellNum = cellNum - 1; //start index at 1

  if (cellNum < 8) { //seg1
    box_x = 25 + (box_w - 1) * cellNum;
  }
  else { //seg2
    box_x = 166 + (box_w - 1) * (cellNum-8);
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
  if(cellNum<NUM_CELLS-1){  checkCells(cellNum+1);}
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
  display.print("Cell Configs");
  display.setCursor(30, 80);
  display.print("Misc Configs");
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

void Core0::chooseCellScreen(uint8_t cellNum)
{
  display.setRotation(0);
  display.drawExampleBitmap(gImage_chooseCell, 0, 0, 128, 296, GxEPD_BLACK);

  display.setRotation(45);

  uint16_t box_w = 14;
  uint16_t box_h = 1;
  uint16_t box_y = 81;
  uint16_t box_x = 25;

  display.fillRect(box_x, box_y - box_h, box_w, box_h, GxEPD_BLACK);
  display.updateWindow(5, 5, 118, 286, false);
  checkCells2(0);
}

void Core0::partialChooseCell(uint8_t cellNum) {
  display.setRotation(0);
  display.drawExampleBitmap(gImage_chooseCell, 0, 0, 128, 296, GxEPD_BLACK, GxEPD::bm_default | GxEPD::bm_partial_update);
  display.setRotation(45);

  uint16_t box_w = 14;
  uint16_t box_h = 1;
  uint16_t box_y = 81;
  uint16_t box_x = 25;

  if (cellNum < 8)  {
    box_x = 25 + (box_w - 1) * cellNum;
  }
  else {
    box_x = 166 + (box_w - 1) * (cellNum - 8);
  }

  display.fillRect(box_x, box_y - box_h, box_w, box_h, GxEPD_BLACK);
  display.updateWindow(128 - box_y, 25, box_h, 246, false);
}

struct button
{
  const uint8_t   pin;
  uint8_t         buttonState;
  uint8_t         lastButtonState;
  unsigned long   lastDebounceTime;
};

struct button buttons[] =
{
  { CENTER_BUTTON, LOW, LOW, 0},
  { UP_BUTTON, LOW, LOW, 0},
  { DOWN_BUTTON, LOW, LOW, 0},
  { LEFT_BUTTON, LOW, LOW, 0},
  { RIGHT_BUTTON, LOW, LOW, 0}
};

void Core0::updateCellConfig(uint8_t cellNum, uint8_t cellConfig, boolean direction)
{
  //change value of config
  if (cellConfig == 0) { //temp
    if (direction) {
      configs[cellNum].max_temp += 1;
    }
    else {
      configs[cellNum].max_temp -= 1;
    }
  }
  else if (cellConfig == 1) { //temp
    if (direction) {
      configs[cellNum].max_voltage += 0.1;
    }
    else {
      configs[cellNum].max_voltage -= 0.1;
    }
  }
  else if (cellConfig == 2) { //temp
    if (direction) {
      configs[cellNum].min_voltage += 0.1;
    }
    else {
      configs[cellNum].min_voltage -= 0.1;
    }
  }
  else if (cellConfig == 3) { //temp
    if (direction) {
      configs[cellNum].max_charge_voltage += 0.1;
    }
    else {
      configs[cellNum].max_charge_voltage -= 0.1;
    }
  }
  else if (cellConfig == 4) { //temp
    configs[cellNum].SOH = !configs[cellNum].SOH;
  }
  printCellConfigs2(cellNum, cellConfig);
}

#define NUM_CELL_CONFIGS 5

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
  display.updateWindow(15, side, 100, 140, false);
}

uint8_t last_s = 5;
uint8_t last_y = 52;

void Core0::cellConfigs(uint8_t cellNum)
{
  //update screen to print all options for cell

  last_s = 5;
  last_y = 52;
  printCellConfigs(cellNum);

  uint8_t cell_config = 0;
  centerPress = false; upPress = false; downPress = false; leftPress = false; rightPress = false;

  while (1) {
    //
    //    boolean centerPress = buttonPress(&buttons[0]);
    //    boolean upPress = buttonPress(&buttons[1]);
    //    boolean downPress = buttonPress(&buttons[2]);
    //    boolean leftPress = buttonPress(&buttons[3]);
    //    boolean rightPress = buttonPress(&buttons[4]);

    if (cell_config == NUM_CELL_CONFIGS && centerPress)  { //exit
      Serial.println("center");
      centerPress = false;
      delay(50);
      break;
    }
    else if (leftPress) {
      leftPress = false;
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
    else if (rightPress) {
      rightPress = false;
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
    else if (upPress) {
      Serial.println("up");
      upPress = false;
      delay(50);
      updateCellConfig(cellNum, cell_config, true);
    }
    else if (downPress) {
      Serial.println("down");
      downPress = false;
      delay(50);
      updateCellConfig(cellNum, cell_config, false);
    }
    delay(30);
  }
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
  display.fillRect(last_s, last_y, 4, 4, GxEPD_WHITE);
  display.fillRect(side, y_point, 4, 4, GxEPD_BLACK);
  display.updateWindow(5, 5, 118, 286, false);

  last_s = side;
  last_y = y_point;
}


uint8_t last_sm = 5;
uint8_t last_ym = 42;

#define NUM_MISC_CONFIGS 6
void Core0::miscConfigs()
{
  last_sm = 5;
  last_ym = 42;
  printMiscConfigs();
  centerPress = false; upPress = false; downPress = false; leftPress = false; rightPress = false;
  uint8_t misc_config = 0;

  while (1) {
    //
    //    boolean centerPress = buttonPress(&buttons[0]);
    //    boolean upPress = buttonPress(&buttons[1]);
    //    boolean downPress = buttonPress(&buttons[2]);
    //    boolean leftPress = buttonPress(&buttons[3]);
    //    boolean rightPress = buttonPress(&buttons[4]);

    if (misc_config == NUM_MISC_CONFIGS && centerPress)  {//exit
      Serial.println("center");
      centerPress = false;
      delay(50);
      break;
    }
    else if (leftPress) {
      Serial.println("left");
      leftPress = false;
      delay(50);
      if (misc_config == 0) {
        misc_config = NUM_MISC_CONFIGS;
      }
      else {
        misc_config--;
      }
      moveMiscConfig(misc_config);
    }
    else if (rightPress) {
      Serial.println("right");
      rightPress = false;
      delay(50);
      if (misc_config == NUM_MISC_CONFIGS) {
        misc_config = 0;
      }
      else {
        misc_config++;
      }
      moveMiscConfig(misc_config);
    }
    else if (upPress) {
      Serial.println("up");
      upPress = false;
      delay(50);
      updateMiscConfig(misc_config, true);
    }
    else if (downPress) {
      Serial.println("down");
      downPress = false;
      delay(50);
      updateMiscConfig(misc_config, false);
    }
    delay(30);
  }
}

void Core0::updateMiscConfig(uint8_t miscConfig, boolean direction)  //finish this
{
  //change value of config
  if (miscConfig == 0) { //pack id
    misc_configs[0].pack_id = !misc_configs[0].pack_id;
  }
  else if (miscConfig == 1) { //airs
    misc_configs[0].airs_state = !misc_configs[0].airs_state;
  }
  else if (miscConfig == 2) { //sl
    misc_configs[0].sl_state = !misc_configs[0].sl_state;
  }
  else if (miscConfig == 3) { //soc min
    if (direction) {
      misc_configs[0].SOC_min += 1;
    }
    else {
      misc_configs[0].SOC_min -= 1;
    }
  }
  else if (miscConfig == 4) { //max current
    if (direction) {
      misc_configs[0].max_pack_current += 0.1;
    }
    else {
      misc_configs[0].max_pack_current -= 0.1;
    }
  }
  else if (miscConfig == 5) { //min current
    if (direction) {
      misc_configs[0].min_pack_current += 0.1;
    }
    else {
      misc_configs[0].min_pack_current -= 0.1;
    }
  }
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
  display.updateWindow(5, 5, 118, 286, false);
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
  
  display.updateWindow(15, side, 100, 140, false);
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
  display.fillRect(last_sm, last_ym, 4, 4, GxEPD_WHITE);
  display.fillRect(side, y_point, 4, 4, GxEPD_BLACK);
  display.updateWindow(5, 5, 118, 286, false);

  last_sm = side;
  last_ym = y_point;
}

unsigned long debounceDelay = 40;    // the debounce time; increase if the output flickers

boolean Core0::buttonPress(struct button* button) {

  int reading = digitalRead((*button).pin);
  if (reading != (*button).lastButtonState) {
    (*button).lastDebounceTime = millis();
  }
  if ((millis() - (*button).lastDebounceTime) > debounceDelay) {
    if (reading != (*button).buttonState) {
      (*button).buttonState = reading;
      if ((*button).buttonState == HIGH) {
        (*button).lastButtonState = reading;
        return true;
      }
    }
  }
  (*button).lastButtonState = reading;
  return false;
}

typedef enum {
  Main,
  Config_State,
  Misc_Configurations,
  Choose_Cell,
  Cell_Configs
} State;

void Core0::fsm() {

  boolean config_index = true;
  uint8_t cell_index = 0;
  uint8_t misc_index = 0;

  State nextState = Main;
  State state = Main;
  setUpMain(id);
  defaultCellConfigs();
  
  centerPress = false; upPress = false; downPress = false; leftPress = false; rightPress = false;

  while (1) {
    //    //centerPress = digitalRead(CENTER_BUTTON);
    //    boolean upPress = buttonPress(&buttons[1]);
    //    boolean downPress = buttonPress(&buttons[2]);
    //    boolean leftPress = buttonPress(&buttons[3]);
    //    boolean rightPress = buttonPress(&buttons[4]);
    switch (nextState) {
      case Main: {  //done
          if (centerPress) {
            Serial.println("center");  //testing
            centerPress = false;
            nextState = Config_State;
          }
          else if (state != Main) {
            setUpMain(id);
            state = Main;
          }
          else if (state == Main){
//          xSemaphoreTake(*sampleSemPointer, portMAX_DELAY );
//          voltage1 = (float) *samplePointer; //to be removed
//          current1 = (float) *samplePointer;  //to be removed
//          temp1 = (float) *samplePointer;     //to be removed
//          soc_test = (uint16_t) *samplePointer; //to be removed
//          Serial.println(*samplePointer);
//          xSemaphoreGive(*sampleSemPointer);
//          Serial.println(temp1);
            
            xSemaphoreTake(*cellArraySemPointer, portMAX_DELAY );
            for(int index = 0; index<NUM_CELLS; index++){
              currentCellData[index] = cellArrayPointer[index];
            }
            //memcpy(currentCellData, currentCellDataInt, sizeof(currentCellDataInt));
            xSemaphoreGive(*cellArraySemPointer);
            voltage1 = 0; current1 = 0; temp1 = 0; soc_test = 0;
            for (int i = 0; i<NUM_CELLS; i++){
              voltage1 += currentCellData[i].cellVoltage;
              current1 += currentCellData[i].balanceCurrent;
              temp1 = max(temp1, currentCellData[i].cellTemp);
              soc_test += currentCellData[i].SOC;
            }
            current1 = current1/NUM_CELLS;
            soc_test = soc_test/NUM_CELLS;
          
          mainPartialUpdate(temp1, soc_test, voltage1, current1);
          }
        }
        break;

      case Config_State: {   //done
          if (centerPress && config_index == true) {
            Serial.println("center");    //testing
            centerPress = false;
            nextState = Choose_Cell;
          }
          else if (centerPress && config_index == false) {
            Serial.println("center");    //testing
            centerPress = false;
            nextState = Misc_Configurations;
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

      case Misc_Configurations: {  
          miscConfigs(); //exits function if on home button
          nextState = Main;
        }
        break;

      case Choose_Cell: {  //done
          if (centerPress) {
            Serial.println("center");
            centerPress = false;
            nextState = Cell_Configs;
          }
          else if (leftPress) {
            Serial.println("left");
            leftPress = false;
            if (cell_index == 0) {
              cell_index = 15;
            }
            else {
              cell_index--;
            }
            partialChooseCell(cell_index);
          }
          else if (rightPress) {
            Serial.println("right");
            rightPress = false;
            if (cell_index == 15) {
              cell_index = 0;
            }
            else {
              cell_index++;
            }
            partialChooseCell(cell_index);
          }
          else if (state != Choose_Cell) {
            cell_index = 0;
            chooseCellScreen(cell_index);
            state = Choose_Cell;
          }
          delay(20);
        }
        break;

      case Cell_Configs: {  
          cellConfigs(cell_index); //exits function if on home button
          nextState = Main;
        }
        break;
    }
  }
}
