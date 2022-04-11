/**
 * Core0.h - Main file for managing the LCD display
 * Author: Justin Kogasaka
 * Date: 3/12/2022
 */

// External library files from Adafruit
// Download in Sketch -> Include Library -> Manage Libraries... and install Adafruit_ILI9341 library and all associated files
#include <Adafruit_ILI9341.h>

#include "PacMan_config.h"
#include "Core0.h"

// Local variables
Adafruit_ILI9341 disp(PIN_DISP_CS, PIN_DISP_DC, PIN_DISP_MOSI, PIN_DISP_CLK, PIN_DISP_RST, -1);
uint8_t dispSOC;
uint16_t dispVoltage;
uint16_t dispTemp;
uint16_t dispCurrent;
uint8_t cellNumTens;
uint8_t cellNumOnes;

boolean upPress = false;
boolean downPress = false;
boolean centerPress = false;
boolean rightPress = false;
boolean leftPress = false;
uint8_t dbDelay = 10;

/** 
 *  Constructor
 *  
 *  @param registers    A pointer to an external PacManRegisters object
 */
Core0::Core0(PacManRegisters* registers) {
  // Initialize variables
  reg = registers;
}

/**
 * Helper function that draws the cursor in the tens position
 * 
 * @param color   Color to draw graphics. Use ILI9341_BLACK to draw and ILI9341_WHITE to erase.
 */
void Core0::drawTensCursor(uint16_t color) {
  disp.setTextColor(color);
  disp.setTextSize(3);
  disp.setCursor(135, 202);
  disp.print("_");
}

/**
 * Helper function that draws the cursor in the ones position
 * 
 * @param color   Color to draw graphics. Use ILI9341_BLACK to draw and ILI9341_WHITE to erase.
 */
void Core0::drawOnesCursor(uint16_t color) {
  disp.setTextColor(color);
  disp.setTextSize(3);
  disp.setCursor(155, 202);
  disp.print("_");
}

/**
 * Draws the main screen used to display pack data
 * 
 * @param color   Color to draw graphics. Use ILI9341_BLACK to draw and ILI9341_WHITE to erase.
 */
void Core0::drawPackScreen(uint16_t color) {
  disp.setTextColor(color);

  // Draw Cells
  for (int i = 0; i < 8; i++) {
    disp.drawRect(20*(i+1),  65, 20, 30, color);
    disp.drawRect(20*(i+1), 140, 20, 30, color);
  }
  // Print cell numbers
  disp.setTextSize(1);
  for (int i = 1; i <=8; i++) {
    disp.setCursor(20*i+7, 100);
    disp.print(i);
  }
  disp.setCursor(27, 175);
  disp.print("9");
  for (int i = 10; i <=16; i++) {
    disp.setCursor(20*(i-8)+5, 175);
    disp.print(i);
  }

  // Print all data labels and segment labels
  disp.setTextSize(2);
  disp.setCursor(70, 45);
  disp.print("Seg 1");
  disp.setCursor(70, 120);
  disp.print("Seg 2");
  
  disp.setCursor(210, 10);
  disp.print("SOC(%)");
  disp.setCursor(210, 70);
  disp.print("Volt(V)");
  disp.setCursor(210, 130);
  disp.print("Temp(oC)");
  disp.setCursor(210, 190);
  disp.print("Curr(A)");
  
  disp.setCursor(20, 200);
  disp.print("Cell Num:");
  
  // Print pack number and cell select
  disp.setTextSize(3);
  disp.setCursor(10, 10);
  disp.print("Pack 1");

  disp.setCursor(135, 197);
  disp.print(cellNumTens);
  disp.setCursor(155, 197);
  disp.print(cellNumOnes);
  drawTensCursor(color);
}

/**
 * Draws the main screen used to display individual cell data
 * Can only be reached from the PackScreen
 * 
 * @param color     Color to draw graphics. Use ILI9341_BLACK to draw and ILI9341_WHITE to erase.
 * @param cellNum   Position number in the pack to observe
 */
void Core0::drawCellScreen(uint16_t color, uint8_t cellNum) {
  // Erase unnecessary pack screen items
  disp.setTextColor(ILI9341_WHITE);
  disp.setTextSize(2);
  disp.setCursor(210, 10);
  disp.print("SOC(%)");
  disp.setCursor(210, 190);
  disp.print("Curr(A)");
  
  // Draw new label
  disp.setTextColor(color);
  disp.setTextSize(3);
  disp.setCursor(6*3*7+10, 10);
  disp.print("Cell");
  disp.setCursor(6*3*12+10, 10);
  disp.print(cellNum);
}

/**
 * Draws the Threshold screen used to change the threshold limits
 * Assumes the screen is blank (white)
 * 
 * @param color   Color to draw graphics. Use ILI9341_BLACK to draw and ILI9341_WHITE to erase.
 */
void Core0::drawThresholdScreen(uint16_t color) {
  // Print header
  disp.setTextColor(color);
  disp.setTextSize(3);
  disp.setCursor(10, 10);
  disp.print("Pack 1 Thresholds");

  // Print data headers
  disp.setTextSize(2);
  disp.setCursor(20, 70);
  disp.print("minVolt(V)");
  disp.setCursor(180, 70);
  disp.print("maxVolt(V)");
  disp.setCursor(20, 130);
  disp.print("minTemp(oC)");
  disp.setCursor(180, 130);
  disp.print("maxTemp(oC)");
  disp.setCursor(20, 220);
  disp.print("HOME");
}

/**
 * Draws the Confirmation screen shown when changing thresholds of the pack
 * 
 * @param color   Color to draw graphics. Use ILI9341_BLACK to draw and ILI9341_WHITE to erase.
 */
void Core0::drawConfirmScreen(uint16_t color) {
  
}


/**
 * Updates displayed data values and writes pack data to the display
 */
void Core0::writePackData() {
  // Pull data from PacManRegisters
  dispSOC = reg->SOC;
  dispVoltage = reg->packVoltage;
  dispTemp = reg->avgCellTemp;
  dispCurrent = reg->dischargingCurrent;

  // Display data
  disp.setTextColor(ILI9341_BLACK);
  disp.setTextSize(3);
  disp.setCursor(210, 30);
  disp.print(dispSOC);
  disp.setCursor(210, 90);
  disp.print(dispVoltage);
  disp.setCursor(210, 150);
  disp.print(dispTemp);
  disp.setCursor(210, 210);
  disp.print(dispCurrent);
}

/**
 * Updates displayed data values and writes the data to the display
 * 
 * @param cellNum   Position number in the pack to observe
 */
void Core0::writeCellData(uint8_t cellNum) {
  // Pull data from PacManRegisters
  dispVoltage = reg->cellV[cellNum];
  dispTemp = reg->cellT[cellNum];

  // Display cell data
  disp.setTextColor(ILI9341_BLACK);
  disp.setTextSize(3);
  disp.setCursor(210, 90);
  disp.print(dispVoltage);
  disp.setCursor(210, 150);
  disp.print(dispTemp);
}

/**
 * Erases the old data on the display
 */
void Core0::eraseOldData() {
  disp.setTextColor(ILI9341_WHITE);
  disp.setTextSize(3);
  disp.setCursor(210, 30);
  disp.print(dispSOC);
  disp.setCursor(210, 90);
  disp.print(dispVoltage);
  disp.setCursor(210, 150);
  disp.print(dispTemp);
  disp.setCursor(210, 210);
  disp.print(dispCurrent);
}

/**
 * Up button pressed interrupt function with debounce
 */
void UButton() {
  volatile static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  if (interrupt_time - last_interrupt_time > dbDelay) upPress = true;
  else upPress = false;
  last_interrupt_time = interrupt_time;
}

/**
 * Down button pressed interrupt function with debounce
 */
void DButton() {
  volatile static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  if (interrupt_time - last_interrupt_time > dbDelay) downPress = true;
  else downPress = false;
  last_interrupt_time = interrupt_time;
}

/**
 * Center button pressed interrupt function with debounce
 */
void CButton() {
  volatile static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  if (interrupt_time - last_interrupt_time > dbDelay) centerPress = true;
  else centerPress = false;
  last_interrupt_time = interrupt_time;
}

/**
 * Right button pressed interrupt function with debounce
 */
void RButton() {
  volatile static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  if (interrupt_time - last_interrupt_time > dbDelay) rightPress = true;
  else rightPress = false;
  last_interrupt_time = interrupt_time;
}

/**
 * Left button pressed interrupt function with debounce
 */
void LButton() {
  volatile static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  if (interrupt_time - last_interrupt_time > dbDelay) leftPress = true;
  else leftPress = false;
  last_interrupt_time = interrupt_time;
}

/** 
 *  Initialize Core 0
 *  Configure pins and display, button interrupts, and initialize FSM
 */
void Core0::initCore0() {
  // Setup buttons
  pinMode(PIN_BTN_LEFT, INPUT);
  pinMode(PIN_BTN_RIGHT, INPUT);
  pinMode(PIN_BTN_UP, INPUT);
  pinMode(PIN_BTN_DOWN, INPUT);
  pinMode(PIN_BTN_CENTER, INPUT);
  attachInterrupt(digitalPinToInterrupt(PIN_BTN_LEFT), LButton, FALLING);
  attachInterrupt(digitalPinToInterrupt(PIN_BTN_RIGHT), RButton, FALLING);
  attachInterrupt(digitalPinToInterrupt(PIN_BTN_UP), UButton, FALLING);
  attachInterrupt(digitalPinToInterrupt(PIN_BTN_DOWN), DButton, FALLING);
  attachInterrupt(digitalPinToInterrupt(PIN_BTN_CENTER), CButton, FALLING);

  // Initialize display
  disp.begin();
  disp.setRotation(3); // 2=(0,0) in lower left math std, 3=(0,0) in upper left cs std
  disp.fillScreen(ILI9341_WHITE);
  drawPackScreen(ILI9341_BLACK);
//  drawCellScreen(ILI9341_BLACK, 1);
//  drawThresholdScreen(ILI9341_BLACK);
  
  Serial.println("Core 0 ready");
}

typedef enum {
  PackScreen, CellScreen, ThresholdScreen
} Screen_State;

/** 
 *  Main loop for Core 0
 *  Runs FSM for display
 */
void Core0::runCore0() {
  // Variables
  Screen_State dispState = PackScreen;
  uint8_t cellNum;
  uint8_t dispCursor = 0;
  upPress = false;
  downPress = false;
  centerPress = false;
  rightPress = false;
  leftPress = false;

  // FSM
  switch(dispState) { //**************************************************************************** Transitions
    case(PackScreen): //**************************************************************************** PackScreen
      if(upPress) {
        upPress = false;
        if(dispCursor = 0) { // Increase tens count
          if(cellNumTens == 9) cellNumTens = 0;
          else cellNumTens++;
        } else { // Increase ones count
          if(cellNumOnes == 9) cellNumTens = 0;
          else cellNumOnes++;
        }
      } else if (downPress) {
        downPress = false;
        if(dispCursor = 0) { // Decrease tens count
          if(cellNumTens == 0) cellNumTens = 9;
          else cellNumTens--;
        } else { // Decrease ones count
          if(cellNumOnes == 0) cellNumOnes = 9;
          else cellNumOnes--;
        }
      } else if(rightPress) {
        rightPress = false;
        if(dispCursor == 0) { // Move cursor to the right
          drawTensCursor(ILI9341_WHITE);
          drawOnesCursor(ILI9341_BLACK);
          dispCursor = 1;
        }
      } else if(leftPress) {
        leftPress = false;
        if(dispCursor == 1) { // Move cursor to the left
          drawOnesCursor(ILI9341_WHITE);
          drawTensCursor(ILI9341_BLACK);
          dispCursor = 0;
        }
      } else if (centerPress) {
        centerPress = false;
        cellNum = cellNumTens*10+cellNumOnes;
        if(cellNum >= 1 && cellNum <=16) { // For valid cell numbers, update to CellScreen
          dispState = CellScreen;
          drawCellScreen(ILI9341_BLACK, cellNum);
        } else if(cellNum == 99) {
          dispState = ThresholdScreen;
          drawPackScreen(ILI9341_WHITE);
          drawThresholdScreen(ILI9341_BLACK);
        }
      }
    break;
    case(CellScreen): //**************************************************************************** CellScreen
      if(upPress) {
        upPress = false;
        if(dispCursor = 0) { // Increase tens count
          if(cellNumTens == 9) cellNumTens = 0;
          else cellNumTens++;
        } else { // Increase ones count
          if(cellNumOnes == 9) cellNumTens = 0;
          else cellNumOnes++;
        }
      } else if (downPress) {
        downPress = false;
        if(dispCursor = 0) { // Decrease tens count
          if(cellNumTens == 0) cellNumTens = 9;
          else cellNumTens--;
        } else { // Decrease ones count
          if(cellNumOnes == 0) cellNumOnes = 9;
          else cellNumOnes--;
        }
      } else if(rightPress) {
        rightPress = false;
        if(dispCursor == 0) { // Move cursor to the right
          drawTensCursor(ILI9341_WHITE);
          drawOnesCursor(ILI9341_BLACK);
          dispCursor = 1;
        }
      } else if(leftPress) {
        leftPress = false;
        if(dispCursor == 1) { // Move cursor to the left
          drawOnesCursor(ILI9341_WHITE);
          drawTensCursor(ILI9341_BLACK);
          dispCursor = 0;
        }
      } else if (centerPress) {
        centerPress = false;
        cellNum = cellNumTens*10+cellNumOnes;
        if(cellNum == 0) {
          dispState = PackScreen;
          drawCellScreen(ILI9341_WHITE, cellNum);
          drawPackScreen(ILI9341_BLACK);
        }
        if(cellNum >= 1 && cellNum <=16) { // For valid cell numbers, update to CellScreen
          dispState = CellScreen;
          drawCellScreen(ILI9341_BLACK, cellNum);
        } else if(cellNum == 99) {
          dispState = ThresholdScreen;
          drawPackScreen(ILI9341_WHITE);
          drawThresholdScreen(ILI9341_BLACK);
        }
      }
    break;
    case(ThresholdScreen):
    break;
    default: dispState = PackScreen;
    break;
  }
  switch(dispState) { // State actions
    case(PackScreen):
      eraseOldData();
      writePackData();
    break;
    case(CellScreen):
      eraseOldData();
      writeCellData(cellNum);
    break;
    case(ThresholdScreen):
    break;
    default:
      dispCursor = 0;
      cellNumTens = 0;
      cellNumOnes = 0;
      cellNum = 0;
    break;
  }
  delay(100);
}
