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


static BaseType_t updateTask;
static TaskHandle_t updateTaskHandle;
static bool updating = false;
static bool updatingWindow = false;

// Task to update the screen
void updateTaskMethod(void* pvParameters){
    display.update();
    display.update();

    updating = false;
    vTaskDelete(NULL); // Safety kill this task since they should not return normally
}

static BaseType_t updateWindowTask;
static TaskHandle_t updateWindowTaskHandle;

// Task to update the screen
void updateWindowTaskMethod(void* pvParameters){
    // void* updateWindowParams = (void*) pvParameters;
    // // Cast before ya dereferences!
    // uint16_t x = *((uint16_t*) (updateWindowParams));
    // uint16_t y = *((uint16_t*) (updateWindowParams + 1*sizeof(uint16_t)));
    // uint16_t w = *((uint16_t*) (updateWindowParams + 2*sizeof(uint16_t)));
    // uint16_t h = *((uint16_t*) (updateWindowParams + 3*sizeof(uint16_t)));
    // bool rotation = *((bool*)  (updateWindowParams + 3*sizeof(uint16_t)+sizeof(bool)));

    display.updateWindow(5, 5, 118, 286, false);
    updatingWindow = false;

    vTaskDelete(NULL); // Safety kill this task since they should not return normally
}

uint8_t rotation = 45;
void setupCore0() {

  display.init();
  CO_LOCK_OD();
  if(OD_displayOrientation) rotation = 0;
  CO_UNLOCK_OD();
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

uint8_t regista[3] = {0, 0, 0};
uint8_t faultNum=0;
Fault faults1[] = {
  {"SL Open", true, false},
  {"Airs Open", true, false},
  {"High Voltage", true, false},
  {"Low Voltage", true, false},
  {"High Temperature", true, false},
  {"Low Temperature", true, false},
  {"High Current", true, false},
  {"Low Current", true, false},
  {"Low SOC", true, false}
  };

//constructor
Core0::Core0() {}

float voltage1; float current1; float temp1; uint16_t soc_test;
uint8_t triggered = 0;

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
  Edit_Value,
  View_Value,
  Edit_Cell_Value,
  View_Cell_Value,
  Reg_Not_Found
} State;

boolean backToHome = 0;

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

  centerPress = false; upPress = false; downPress = false; leftPress = false; rightPress = false;

  while (1) {
    delay(10);
    switch (nextState) {
      case Main: {
          if (state != Main || backToHome == 1) {
            backToHome = 0;
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
            nextState = Choose_Register;
          }
          else if (centerPress && main_index != 0) {
            centerPress = false;
            Serial.println("center");  //testing
            nextState = Cell_State;
          }
          if(nextState == Main){
          voltage1 = 0; current1 = 0; temp1 = 0; soc_test = 0;
          CO_LOCK_OD();
          for (int i = 0; i < NUM_CELLS; i++) {
            voltage1 += OD_cellVoltage[i];
            current1 += OD_cellBalancingCurrent[i];
            temp1 = max(temp1, (float)OD_cellTemperature[i]);
            soc_test += OD_cellSOC[i];
          }
          CO_UNLOCK_OD();
          voltage1 = voltage1 / 1000.0;
          current1 = current1 / (1000.0 * NUM_CELLS);
          soc_test = soc_test / NUM_CELLS;
          temp1 = temp1 / 10.0;
          mainPartialUpdate(temp1, soc_test, voltage1, current1, main_index);
          }
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
            nextState = Cell_Data;
          }
          else if (centerPress && cell_index == false) {
            centerPress = false;
            Serial.println("center");    //testing
            nextState = Choose_Cell_Register;
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
          if (state != Choose_Register) state = Choose_Register;
          uint8_t regnum = chooseRegister();
          if (regnum == 3) {
            regNumb = 0x2000 + (regista[0] * 100) + (regista[1] * 10) + regista[2];
            Object_Dictionary od(regNumb, 0);
            if (od.location == 0xFFFF || regNumb < 0x2000 || regNumb > 0x2020){
              regNumb-= 0x2000;
              nextState = Reg_Not_Found;
            }
            else if (od.attribute == 0x0f || od.attribute == 0x8f) nextState = Edit_Value;
            else nextState = View_Value;
          }
          else if (regnum == 4) nextState = Main; //exits function if on home button
        }
        break;

      case Choose_Cell_Register: {
        if (state!= Choose_Cell_Register) state = Choose_Cell_Register;
          uint8_t regnum = chooseRegister();
          if (regnum == 3) {
            regNumb = 0x3000 + (regista[0] * 100) + (regista[1] * 10) + regista[2];
            Object_Dictionary od1(regNumb, main_index-1);
            Serial.print("attr: ");
            Serial.println(od1.attribute);
            if (od1.location == 0xFFFF || regNumb < 0x3000 || regNumb > 0x3020) {
              regNumb-= 0x3000;
              nextState = Reg_Not_Found;
            }
            else if (od1.attribute == 0x0f || od1.attribute == 0x8f) nextState = Edit_Cell_Value;
            else nextState = View_Cell_Value;
          }
          else if (regnum == 4) nextState = Main; //exits function if on home button
        }
        break;
        
      case Edit_Value: {
          if (state!= Edit_Value) state = Edit_Value;
          editValue(regista, true, 0);
          nextState = Main; //exits function if on home button
        }
        break;

      case View_Value: {
          if (state!= View_Value) state = View_Value;
          viewValue(regista, true, 0);
          nextState = Main; //exits function if on home button
        }
        break;

      case View_Cell_Value: {
          if (state!= View_Cell_Value) state = View_Cell_Value;
          viewValue(regista, false, main_index - 1);
          nextState = Main; //exits function if on home button
        }
        break;

      case Edit_Cell_Value: {
          if (state!= Edit_Cell_Value) state = Edit_Cell_Value;
          editValue(regista, false, main_index - 1);
          nextState = Main; //exits function if on home button
        }
        break;

      case Reg_Not_Found: {
          if (state!= Reg_Not_Found) state = Reg_Not_Found;
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
              faults1[faultNum].enabled = false;
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
            faultDisablePartial(charge_index);
          }
          else if (downPress || rightPress) {
            downPress = false; rightPress = false;
            Serial.println("downright");
            if (charge_index != 2) charge_index += 1;
            else charge_index = 0;
            faultDisablePartial(charge_index);
          }
          else if (state != Charging) {
            charge_index = 0;
            chargeScreen();
            state = Charging;
          }
          delay(20);
        }
        break;

      case Cell_Data: {  //display cell data
          if (state!= Cell_Data) state = Cell_Data;
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

  CO_LOCK_OD();
  if(OD_displayOrientation) rotation = 3;
  else if (!OD_displayOrientation) rotation = 1;
  
  display.setRotation(rotation-1);

  display.drawExampleBitmap(gImage_new_main, 0, 0, 128, 296, GxEPD_BLACK);

  display.setRotation(rotation);
  const GFXfont* f = &FreeSansBold9pt7b;  //set font
  display.setFont(f);

  display.setCursor(175, 16);

  if (OD_packNodeID == 4) display.print("1");
  else display.print("2");

    CO_UNLOCK_OD();
 
/*  String fault_string;
  for (uint8_t i =0; i< NUM_CELLS; i++){
    if (OD_fault[i]>0) fault_string = "Fault #" + String(OD_fault[i], DEC);
  }*/

  //if (triggered > 0) fault_string = "Fault #" + String(triggered, DEC);
  


//  display.setCursor(5, 15);
//  display.print(fault_string);
  
  // if(updating == false){
  //     updateTask = xTaskCreate(
  //     updateTaskMethod,       /* Function that implements the task. */
  //     "updateTask",          /* Text name for the task. */
  //     1000,      /* Stack size in words, not bytes. */
  //     ( void * ) NULL,    /* Parameter passed into the task. */
  //     tskIDLE_PRIORITY,/* Priority at which the task is created. */
  //     &updateTaskHandle );      /* Used to pass out the created task's handle. */
  //     updating = true;
  //     Serial.println("Update Task Created!");
  // }

  display.update();
  display.update();

  display.setRotation(rotation);
}

void Core0::mainPartialUpdate(float temperature, uint16_t soc, float volt, float curr, uint8_t main_index)
{
  const GFXfont* f = &FreeSansBold9pt7b;  //set font
  display.setFont(f);
  display.setTextColor(GxEPD_BLACK);
  display.setRotation(rotation);

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

  CO_LOCK_OD();
  if (!(OD_chargeCableDetected || OD_chargingEnabled)) display.setTextColor(GxEPD_WHITE);
  CO_UNLOCK_OD();

    display.setCursor(250, 15);
    display.print("Chrg");
    display.setTextColor(GxEPD_BLACK);
    
  String fault_string;
  if (triggered > 0) fault_string = "Fault #" + String(triggered, DEC);
  display.setCursor(5, 15);
  display.print(fault_string);

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
  //display.updateWindow(5, 5, 118, 286, false);

  if(updatingWindow == false){
      updateWindowTask = xTaskCreate(
      updateWindowTaskMethod,       /* Function that implements the task. */
      "updateWindowTask",          /* Text name for the task. */
      2000,      /* Stack size in words, not bytes. */
      ( void * ) NULL,    /* Parameter passed into the task. */
      tskIDLE_PRIORITY,/* Priority at which the task is created. */
      &updateWindowTaskHandle);      /* Used to pass out the created task's handle. */

      //display.updateWindow(5, 5, 118, 286, false);
      updatingWindow = true;
      Serial.println("UpdateWindow Task Created!");
  }
  checkForFaults(0);//calls faults();
}

void Core0::checkCells(uint8_t currentCell) {
  CO_LOCK_OD();
  for (uint8_t cell = currentCell; cell < NUM_CELLS; cell++) {
    if (OD_warning[cell] < 1) cellPartialUpdate(0, cell);
    else if (OD_cellSOH[cell] == 1) cellPartialUpdate(1, cell);
    else if (OD_warning[cell] == 1 || OD_warning[cell] == 2) cellPartialUpdate(2, cell); //voltage
    else if (OD_warning[cell] == 3 || OD_warning[cell] == 4) cellPartialUpdate(3, cell); //temp
    else if (OD_warning[cell] == 5 || OD_warning[cell] == 6) cellPartialUpdate(4, cell); //current
    else if (OD_warning[cell] == 9) cellPartialUpdate(5, cell); //misc
  }
  CO_UNLOCK_OD();
}

void Core0::checkForFaults(uint8_t currentCell) {
    CO_LOCK_OD();
    //if (OD_SLOOP_Relay == 0) faults(0, 0); //sl open
    if (OD_SLOOP_Relay == 1) triggered = 0;
    //else if (OD_AIRS == 1) faults(1, 0); //airs open
  
  for (uint8_t cell = currentCell; cell < NUM_CELLS; cell++) {   //ADD SOH AS A MEASURE
    if (OD_fault[cell] == 1 && triggered!=1 && !OD_faultHV_Disable[cell]){            //can do a nested for loop
      faults(2, cell+1); //high voltage
      triggered = 1;
    }
    if (OD_fault[cell] == 2 && triggered!=2 && !OD_faultLV_Disable[cell]){
      faults(3, cell+1); //low voltage
      triggered = 2;
    }
    if (OD_fault[cell] == 3 && triggered!=3 && !OD_faultHT_Disable[cell]) {
      faults(4, cell+1); //high temp
      triggered = 3;
    }
//    if (OD_fault[cell] == 4 && triggered!=4 && !OD_faultLT_Disable[cell]) faults(5, cell+1); //low temp
//    if (OD_fault[cell] == 5 && triggered!=5 && !OD_faultHC_Disable[cell]) faults(6, cell+1); //high current
//    if (OD_fault[cell] == 6 && triggered!=6 && !OD_faultLC_Disable[cell]) faults(7, cell+1); //low current
//    if (OD_fault[cell] == 7 && triggered!=7 && !OD_faultLSOC_Disable[cell]) faults(8, cell+1); //low soc


  }
  CO_UNLOCK_OD();
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
  if (errorType == 0) { //soh bad
    display.fillRect(box_x+1, box_y - box_h+1, box_w-2, box_h-2, GxEPD_WHITE);
  }
  else if (errorType == 1) { //soh bad
    display.fillRect(box_x, box_y - box_h, box_w, box_h, GxEPD_BLACK);
  }
  else if (errorType == 2) { //voltage
    display.fillRect(box_x, box_y - box_h, box_w, box_h, GxEPD_BLACK);
    display.setTextColor(GxEPD_WHITE);
    display.setCursor(box_x + 1, box_y - 6);
    display.print("V");
  }
  else if (errorType == 3) { //temp
    display.fillRect(box_x, box_y - box_h, box_w, box_h, GxEPD_BLACK);
    display.setTextColor(GxEPD_WHITE);
    display.setCursor(box_x + 1, box_y - 6);
    display.print("T");
  }
  else if (errorType == 4) { //current
    display.fillRect(box_x, box_y - box_h, box_w, box_h, GxEPD_BLACK);
    display.setTextColor(GxEPD_WHITE);
    display.setCursor(box_x + 1, box_y - 6);
    display.print("C");
  }
  else if (errorType == 5) { //current
    display.fillRect(box_x, box_y - box_h, box_w, box_h, GxEPD_BLACK);
    display.setTextColor(GxEPD_WHITE);
    display.setCursor(box_x + 1, box_y - 6);
    display.print("!");
  }
//  if (cellNum < NUM_CELLS - 1) {
//    checkCells(cellNum + 1);
//  }
  //  display.updateWindow(128 - box_y, box_x, box_h, box_w, false);
}

void Core0::faults(uint8_t errorType, uint8_t cellNum)
{
  /*if (faults1[errorType].enabled==true){
  faults1[errorType].triggered = true;
  faultNum = errorType;*/
  const GFXfont* font = &FreeSansBold12pt7b;
  display.setFont(font);

  display.fillScreen(GxEPD_BLACK);
  display.setTextColor(GxEPD_WHITE);
  display.setCursor(30, 75);
  String message = faults1[errorType].message;
  if (cellNum>0) message += " - Cell # " + String(cellNum, DEC);
  display.print(message);

  display.update();
  while (!centerPress) {
    delay(20);
  }
  centerPress = false;
  Serial.println("center");
  backToHome = 1;
 // }
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
  if(updatingWindow == false){
      updateWindowTask = xTaskCreate(updateWindowTaskMethod, "updateWindowTask", 2000, ( void * ) NULL, tskIDLE_PRIORITY, &updateWindowTaskHandle);
      updatingWindow = true;
      Serial.println("UpdateWindow Task Created!");
  }
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
  display.print("Cell Data");
  display.setCursor(30, 80);
  display.print("Cell Configs");
  display.fillRect(20, 41, 4, 4, GxEPD_BLACK);
  // if(updatingWindow == false){
  //     updateWindowTask = xTaskCreate(updateWindowTaskMethod, "updateWindowTask", 2000, ( void * ) NULL, tskIDLE_PRIORITY, &updateWindowTaskHandle);
  //     updatingWindow = true;
  //     Serial.println("UpdateWindow Task Created!");
  // }
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
  CO_LOCK_OD();
  float temperature = OD_cellTemperature[cellNum]/10.0;
  float voltage = OD_cellVoltage[cellNum]/1000.0;
  float current = OD_cellBalancingCurrent[cellNum]/1000.0;
  uint16_t cellSOC = OD_cellSOC[cellNum];
  CO_UNLOCK_OD();
  
  String num = String("Cell #" + String(cellNum + 1, DEC));
  String temp = String("Temp " + String(temperature, 1));
  String volt = String("Volt " + String(voltage, 1));
  String curr = String("Curr " + String(current, 1));
  String soc = String("SOH " + String(cellSOC, DEC));

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
  if(updatingWindow == false){
      updateWindowTask = xTaskCreate(updateWindowTaskMethod, "updateWindowTask", 2000, ( void * ) NULL, tskIDLE_PRIORITY, &updateWindowTaskHandle);
      updatingWindow = true;
      Serial.println("UpdateWindow Task Created!");
  }
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

  // if(updatingWindow == false){
  //     updateWindowTask = xTaskCreate(updateWindowTaskMethod, "updateWindowTask", 2000, ( void * ) NULL, tskIDLE_PRIORITY, &updateWindowTaskHandle);
  //     updatingWindow = true;
  //     Serial.println("UpdateWindow Task Created!");
  // }
  display.updateWindow(5, 5, 118, 286, false);
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

  if(updatingWindow == false){
      updateWindowTask = xTaskCreate(updateWindowTaskMethod, "updateWindowTask", 2000, ( void * ) NULL, tskIDLE_PRIORITY, &updateWindowTaskHandle);
      updatingWindow = true;
      Serial.println("UpdateWindow Task Created!");
  }
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

  if(updatingWindow == false){
      updateWindowTask = xTaskCreate(updateWindowTaskMethod, "updateWindowTask", 2000, ( void * ) NULL, tskIDLE_PRIORITY, &updateWindowTaskHandle);
      updatingWindow = true;
      Serial.println("UpdateWindow Task Created!");
  }
  while(!centerPress){ delay(10);}
  centerPress = false;
}

void Core0::printEditValue(Object_Dictionary od, uint8_t reg, int8_t conversion) {
  uint8_t lastReg = conversion + 4;
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
  Serial.println(od.value);
  if (od.lengths == 1)od.value = *od.pointer & 0x000000FF;
  else if (od.lengths == 2) od.value = *od.pointer & 0x0000FFFF;
  //value in mV so place times 1000
  display.print((int)((od.value / (int)pow(10,conversion+4)) % 10));
  display.setCursor(95, 80);
  display.print((int)((od.value / (int)pow(10,conversion+3)) % 10)); //tens
  display.setCursor(125, 80);
  display.print((int)((od.value / (int)pow(10,conversion+2)) % 10)); //ones
  if (lastReg>=3){
  display.setCursor(155, 80);
  display.print("."); //decimal
  display.setCursor(170, 80);
  display.print((int)((od.value / (int)pow(10,conversion+1)) % 10)); //tenths
  }
  if (lastReg>=4){
  display.setCursor(200, 80);
  display.print((int)((od.value / (int)pow(10,conversion)) % 10)); //hundredths
  }

  if (reg == 0) display.fillRect(65, 84, 20, 2, GxEPD_BLACK);
  if (reg == 1) display.fillRect(95, 84, 20, 2, GxEPD_BLACK);
  if (reg == 2) display.fillRect(125, 84, 20, 2, GxEPD_BLACK);
  if (reg == 3) display.fillRect(170, 84, 20, 2, GxEPD_BLACK);
  if (reg == 4) display.fillRect(200, 84, 20, 2, GxEPD_BLACK);
  if (reg == 5) display.fillRect(5, 112, 4, 4, GxEPD_BLACK);
  if(updatingWindow == false){
      updateWindowTask = xTaskCreate(updateWindowTaskMethod, "updateWindowTask", 2000, ( void * ) NULL, tskIDLE_PRIORITY, &updateWindowTaskHandle);
      updatingWindow = true;
      Serial.println("UpdateWindow Task Created!");
  }
}

int8_t Core0::convert(uint16_t index){
  int8_t conversion = -2;
  if (index == 0x2005 || index == 0x200f || index == 0x2010        //voltage
      || index == 0x2011 || index == 0x3002 || index == 0x3004 
      || index == 0x3006 || index == 0x3009 || index == 0x300a 
      || index == 0x300f) conversion = 1; 
      
  if (index == 0x2006 || index == 0x200d || index == 0x3007        //current
      || index == 0x300d || index == 0x300e) conversion = 0; 
      
  if (index == 0x200e || index == 0x2012 || index == 0x2013        //temp
      || index == 0x2014 || index == 0x3003 || index == 0x300b
      || index == 0x300c) conversion = -1; 
  //if (index == ) conversion = ;
  //if (index == ) conversion = ;
  //if (index == ) conversion = ;
  //if (index == ) conversion = ;
  //if (index == ) conversion = ;
  //if (index == ) conversion = ;
  //if (index == ) conversion = ;
  return conversion;
}

void Core0::editValue(uint8_t regista[], boolean state, uint8_t cellNum) {
  uint8_t regNum = regista[0] * 100 + regista[1] * 10 + regista[2];
  uint16_t index;
  if (state) index = regNum + 0x2000;
  else       index = regNum + 0x3000;

  int8_t conversion = convert(index);
  uint8_t lastReg = 4 + conversion;
  if (lastReg > 4) lastReg = 4;
  
  Object_Dictionary od(index, cellNum);
    int original = *od.pointer;
    boolean confirmed = false;
    printEditValue(od, 0, conversion);
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
        printEditValue(od, reg, conversion);
      }
      else if (leftPress) {
        leftPress = false;
        Serial.println("left");
        delay(50);
        if (reg == 0) {
          reg = 6;
        }
        else if (reg == 5) {
          reg = lastReg;
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
        else if (reg == lastReg) {
          reg = 5;
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
        od = updateValue(od, reg, true, conversion);
        Serial.print("pointer val: ");
        Serial.println(*od.pointer);
        
      }
      else if (downPress) {
        downPress = false;
        Serial.println("down");
        delay(50);
        od = updateValue(od, reg, false, conversion);
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
  
  int8_t conversion = convert(index);
  uint8_t lastReg = 4 + conversion;
  if (lastReg > 4) lastReg = 4;
  
  Object_Dictionary od(index, cellNum);
  //if (!state) String names = od.names = 'Cell #' + cellNum + '- ' + od.names;
  centerPress = false; upPress = false; downPress = false; leftPress = false; rightPress = false;
    
    while (!centerPress) {
          printViewValue(od, lastReg);
          delay(1000);
    }    
    centerPress = false;
}

void Core0::printViewValue(Object_Dictionary od, int8_t conversion) {
  uint8_t lastReg = conversion + 4;
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

  if (od.lengths == 1)od.value = *od.pointer & 0x000000FF;
  else if (od.lengths == 2) od.value = *od.pointer & 0x0000FFFF;
  f = &FreeSansBold24pt7b;  //set font
  display.setFont(f);
  display.setCursor(65, 80); //hundreds
  Serial.print("view value: ");
  Serial.println(od.value);
  //value in mV so place times 1000
  display.print((int)((od.value / (int)pow(10,conversion+4)) % 10));
  display.setCursor(95, 80);
  display.print((int)((od.value / (int)pow(10,conversion+3)) % 10)); //tens
  display.setCursor(125, 80);
  display.print((int)((od.value / (int)pow(10,conversion+2)) % 10)); //ones
  if(lastReg >=3){
  display.setCursor(155, 80);
  display.print("."); //decimal
  display.setCursor(170, 80);
  display.print((int)((od.value / (int)pow(10,conversion+1)) % 10)); //tenths
  }
  if (lastReg >= 4){
  display.setCursor(200, 80);
  display.print((int)((od.value / (int)pow(10,conversion)) % 10)); //hundredths
  }
  
  display.fillRect(230, 112, 4, 4, GxEPD_BLACK);

  if(updatingWindow == false){
      updateWindowTask = xTaskCreate(updateWindowTaskMethod, "updateWindowTask", 2000, ( void * ) NULL, tskIDLE_PRIORITY, &updateWindowTaskHandle);
      updatingWindow = true;
      Serial.println("UpdateWindow Task Created!");
  }
}

Object_Dictionary Core0::updateValue(Object_Dictionary od, uint8_t place, boolean direction, int8_t conversion) {
  //if place = 0--10^(t+4), 1--10^(t+3), 2--10^(t+2), 3--10^(t+1), 4--10^t
  int temp2 = 4-place;
  int temp =pow(10,conversion+temp2);
  if (od.lengths == 1)od.value = *od.pointer & 0x000000FF;
  else if (od.lengths == 2)od.value = *od.pointer & 0x0000FFFF;
//  Serial.println(temp2);
    if      (direction &  (int)(od.value / temp) % 10 != 9)  *od.pointer += temp;
    else if (direction &  (int)(od.value / temp) % 10 == 9)  *od.pointer -= 9 * temp;
    else if (!direction & (int)(od.value / temp) % 10 != 0)  *od.pointer -= temp;
    else if (!direction & (int)(od.value / temp) % 10 == 0)  *od.pointer += 9 * temp;
  
  printEditValue(od, place, conversion);
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

  if(updatingWindow == false){
      updateWindowTask = xTaskCreate(updateWindowTaskMethod, "updateWindowTask", 2000, ( void * ) NULL, tskIDLE_PRIORITY, &updateWindowTaskHandle);
      updatingWindow = true;
      Serial.println("UpdateWindow Task Created!");
  }
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
  if(updatingWindow == false){
      updateWindowTask = xTaskCreate(updateWindowTaskMethod, "updateWindowTask", 2000, ( void * ) NULL, tskIDLE_PRIORITY, &updateWindowTaskHandle);
      updatingWindow = true;
      Serial.println("UpdateWindow Task Created!");
  }
}


void Core0::faultDisablePartial(uint8_t charge_index) {
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
  if(updatingWindow == false){
      updateWindowTask = xTaskCreate(updateWindowTaskMethod, "updateWindowTask", 2000, ( void * ) NULL, tskIDLE_PRIORITY, &updateWindowTaskHandle);
      updatingWindow = true;
      Serial.println("UpdateWindow Task Created!");
  }
}
