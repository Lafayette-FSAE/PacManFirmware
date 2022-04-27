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

uint16_t dispMinV;
uint16_t dispMaxV;
uint16_t dispMinT;
uint16_t dispMaxT;

uint8_t cellNumTens;
uint8_t cellNumOnes;
uint8_t cellNum;
uint8_t dispCellNum;
uint8_t dispNumSegPlusCells;
uint8_t dispNumSegMinusCells;

uint8_t dispCursor;
uint8_t confirmCursor;

boolean upPress;
boolean downPress;
boolean centerPress;
boolean rightPress;
boolean leftPress;

typedef enum {
  PackScreen, CellScreen, NoCellScreen, ThresholdScreen, EditScreen, ConfirmScreen
} Screen_State;

Screen_State dispState;

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
 * Helper function that draws the cursor for the various states
 *
 * @param color   Color to draw graphics. Use ILI9341_BLUE to draw and ILI9341_WHITE to erase.
 */
void Core0::drawCursor(uint16_t color, uint8_t index) {
  disp.setTextColor(color);
  disp.setTextSize(3);
  if (index == 0) { // Tens position on Pack/Cell screen
    disp.setCursor(135, 202);
    disp.print('_');
  } else if (index == 1) { // Ones position on Pack/Cell screen
    disp.setCursor(155, 202);
    disp.print('_');
  } else if (index == 2) {
    disp.fillRect(10, 75, 5, 5, color);
  } else if (index == 3) {
    disp.fillRect(170, 75, 5, 5, color);
  } else if (index == 4) {
    disp.fillRect(10, 140, 5, 5, color);
  } else if (index == 5) {
    disp.fillRect(170, 140, 5, 5, color);
  } else if (index == 6) {
    disp.fillRect(10, 225, 5, 5, color);
  } else if (index == 7) {
    disp.setCursor(20, 100);
    disp.print('_');
  } else if (index == 8) {
    disp.setCursor(2*3*6+20, 100);
    disp.print('_');
  } else if (index == 9) {
    disp.setCursor(3*3*6+20, 100);
    disp.print('_');
  } else if (index == 10) {
    disp.setCursor(180, 100);
    disp.print('_');
  } else if (index == 11) {
    disp.setCursor(2*3*6+180, 100);
    disp.print('_');
  } else if (index == 12) {
    disp.setCursor(3*3*6+180, 100);
    disp.print('_');
  } else if (index == 13) {
    disp.setCursor(20, 165);
    disp.print('_');
  } else if (index == 14) {
    disp.setCursor(3*6+20, 165);
    disp.print('_');
  } else if (index == 15) {
    disp.setCursor(3*3*6+20, 165);
    disp.print('_');
  } else if (index == 16) {
    disp.setCursor(180, 165);
    disp.print('_');
  } else if (index == 17) {
    disp.setCursor(3*6+180, 165);
    disp.print('_');
  } else if (index == 18) {
    disp.setCursor(3*3*6+180, 165);
    disp.print('_');
  } else if (index == 19) {
    disp.fillRect(37, 177, 7, 7, color);
  } else if (index == 20) {
    disp.fillRect(187, 177, 7, 7, color);
  }
}

/**
 * Helper function that draws the tens place cell number in the specified color on the display
 *
 * @param color   Color to draw graphics. Use ILI9341_BLACK to draw and ILI9341_WHITE to erase.
 */
void Core0::drawCellNumTens(uint16_t color) {
  disp.setTextColor(color);
  disp.setTextSize(3);
  disp.setCursor(135, 197);
  disp.print(cellNumTens);
}

/**
 * Helper function that draws the ones place cell number in the specified color on the display
 *
 * @param color   Color to draw graphics. Use ILI9341_BLACK to draw and ILI9341_WHITE to erase.
 */
void Core0::drawCellNumOnes(uint16_t color) {
  disp.setTextColor(color);
  disp.setTextSize(3);
  disp.setCursor(155, 197);
  disp.print(cellNumOnes);
}

/**
 * Helper function that colors the seg chart for populated positions in the specified color on the display
 *
 * @param color   Color to draw graphics
 */
void Core0::drawCellsOnSeg(uint16_t color) {
  // Fill in cells on Seg- chart
  if (dispNumSegMinusCells != reg->numSegMinusCells) {
    dispNumSegMinusCells = reg->numSegMinusCells;
    // Erase old color
    for (int i = 0; i < 8; i++) {
      disp.fillRect(20*(i+1), 65, 20, 30, ILI9341_WHITE);
    }
    // Print new color and outline
    for (int i = 0; i < dispNumSegMinusCells; i++) {
      disp.fillRect(20*(i+1), 65, 20, 30, color);
    }
    for (int i = 0; i < 8; i++) {
      disp.drawRect(20*(i+1), 65, 20, 30, ILI9341_BLACK);
    }
  }

  // Fill in cells on Seg+ chart
  if (dispNumSegPlusCells != reg->numSegPlusCells) {
    dispNumSegPlusCells = reg->numSegPlusCells;
    // Erase old color
    for (int i = 0; i < 8; i++) {
      disp.fillRect(20*(i+1), 140, 20, 30, ILI9341_WHITE);
    }
    // Print new color
    for (int i = 0; i < dispNumSegPlusCells; i++) {
      disp.fillRect(20*(i+1), 140, 20, 30, color);
    }
    for (int i = 0; i < 8; i++) {
      disp.drawRect(20*(i+1), 140, 20, 30, ILI9341_BLACK);
    }
  }
}

/**
 * Erase the cells on seg fill color
 */
void Core0::eraseCellsOnSeg() {
  for (int i = 0; i < 8; i++) {
    disp.fillRect(20*(i+1), 65, 20, 30, ILI9341_WHITE);
    disp.fillRect(20*(i+1), 140, 20, 30, ILI9341_WHITE);
    
  }
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
  for (int i = 1; i <= 8; i++) {
    disp.setCursor(20*i+7, 100);
    disp.print(i);
  }
  disp.setCursor(27, 175);
  disp.print("9");
  for (int i = 10; i <= 16; i++) {
    disp.setCursor(20*(i-8)+5, 175);
    disp.print(i);
  }

  // Print all data labels and segment labels
  disp.setTextSize(2);
  disp.setCursor(70, 45);
  disp.print("Seg -");
  disp.setCursor(70, 120);
  disp.print("Seg +");

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

  drawCellNumTens(ILI9341_BLACK);
  drawCellNumOnes(ILI9341_BLACK);
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
 * Draws the NoCell screen used for out of bound cell numbers
 * Can be reached from the PackScreen or the CellScreen
 * 
 * @param color   Color to draw graphics. Use ILI9341_BLACK to draw and ILI9341_WHITE to erase.
 */
void Core0::drawNoCellScreen(uint16_t color) {
  // Erase unnecessary pack/cell screen items
  disp.setTextColor(ILI9341_WHITE);
  disp.setTextSize(2);
  disp.setCursor(210, 10);
  disp.print("SOC(%)");
  disp.setCursor(210, 70);
  disp.print("Volt(V)");
  disp.setCursor(210, 130);
  disp.print("Temp(oC)");
  disp.setCursor(210, 190);
  disp.print("Curr(A)");

  // Write new headers
  disp.setTextColor(color);
  disp.setCursor(210, 70);
  disp.print("Pack Data");
  disp.setCursor(210, 130);
  disp.print("Cell Data");
  disp.setCursor(210, 190);
  disp.print("Pack Menu");

  disp.setTextSize(3);
  disp.setCursor(210, 90);
  disp.print("0");
  disp.setCursor(210, 150);
  disp.print("1-16");
  disp.setCursor(210, 210);
  disp.print("99");
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
  disp.setCursor(20, 135);
  disp.print("minTemp(oC)");
  disp.setCursor(180, 135);
  disp.print("maxTemp(oC)");
  disp.setCursor(20, 220);
  disp.print("HOME");
}

/**
 * Draws the Edit screen shown to change thresholds of the pack
 *
 * @param color   Color to draw graphics. Use ILI9341_BLACK to draw and ILI9341_WHITE to erase.
 * @param cursor  Case indentifier for which threshold is being changed
 */
void Core0::drawEditScreen(uint16_t color, uint8_t cursor) {
  disp.setTextColor(color);
  disp.setTextSize(3);
  if (cursor == 2) {
    disp.setCursor(20, 95);
    disp.print(dispMinV/(float)1000);
  } else if (cursor == 3) {
    disp.setCursor(180, 95);
    disp.print(dispMaxV/(float)1000);
  } else if (cursor == 4) {
    disp.setCursor(20, 160);
    disp.print(dispMinT/(float)10);
    disp.setTextColor(ILI9341_WHITE); // Block hundredths place on temp
    disp.setCursor(4*3*6+20, 160);
    disp.print('0');
  } else if (cursor == 5) {
    disp.setCursor(180, 160);
    disp.print(dispMaxT/(float)10);
    disp.setTextColor(ILI9341_WHITE); // Block hundredths place on temp
    disp.setCursor(4*3*6+180, 160);
    disp.print('0');
  }
}

/**
 * Draws the Confirmation screen shown when changing thresholds of the pack
 *
 * @param color   Color to draw graphics. Use ILI9341_BLACK to draw and ILI9341_WHITE to erase.
 */
void Core0::drawConfirmScreen(uint16_t color, uint8_t cursor) {
  // Print header
  disp.setTextColor(color);
  disp.setTextSize(3);
  disp.setCursor(10, 10);
  disp.print("Pack 1 Thresholds");

  // Print param
  disp.setCursor(30, 75);
  disp.print("Change ");
  if      (cursor >= 7  && cursor <= 9)  disp.print("minVolt");
  else if (cursor >= 10 && cursor <= 12) disp.print("maxVolt");
  else if (cursor >= 13 && cursor <= 15) disp.print("minTemp");
  else if (cursor >= 16 && cursor <= 18) disp.print("maxTemp");
  
  // Voltage case
  if (cursor >= 7 && cursor <= 12) {
    disp.setCursor(2*3*6+37, 120);
    disp.print("to ");
    if (cursor <= 9) disp.print(dispMinV/(float)1000);
    else             disp.print(dispMaxV/(float)1000);
    disp.print("V?");
  } 
  // Temp case
  else if (cursor >= 13 && cursor <= 18) {
    disp.setCursor(2*3*6+30, 120);
    disp.print("to ");
    if (cursor <= 15) disp.print(dispMinT/(float)10);
    else              disp.print(dispMaxT/(float)10);
    disp.setTextColor(ILI9341_WHITE);
    disp.setCursor(9*3*6+30, 120);
    disp.print('0');
    disp.setTextColor(color);
    disp.setCursor(9*3*6+30, 120);
    disp.print("oC?");
  }
  disp.setCursor(50, 170);
  disp.print("NO");
  disp.setCursor(200, 170);
  disp.print("YES");
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
  disp.print(dispVoltage / (float)1000);
  disp.setCursor(210, 150);
  disp.print(dispTemp / (float)10);
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
  dispVoltage = reg->cellV[cellNum - 1];
  dispTemp = reg->cellT[cellNum - 1];

  // Display cell data
  disp.setTextColor(ILI9341_BLACK);
  disp.setTextSize(3);
  disp.setCursor(210, 90);
  disp.print(dispVoltage / (float)1000);
  disp.setCursor(210, 150);
  disp.print(dispTemp / (float)10);
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
  disp.print(dispVoltage / (float)1000);
  disp.setCursor(210, 150);
  disp.print(dispTemp / (float)10);
  disp.setCursor(210, 210);
  disp.print(dispCurrent);
}

/**
 * Writes threshold value to the display
 * 
 * @param color   Color to write the data. Use ILI9341_BLACK to write and ILI9341_WHITE to erase.
 * @param cursor  Cursor value
 */
void Core0::writeThreshold(uint16_t color, uint8_t cursor) {
  disp.setTextColor(color);
  disp.setTextSize(3);
  if (cursor == 7 || cursor == 8 || cursor == 9) {
    disp.setCursor(20, 95);
    disp.print(dispMinV/(float)1000);
  } else if (cursor == 10 || cursor == 11 || cursor == 12) {
    disp.setCursor(180, 95);
    disp.print(dispMaxV/(float)1000);
  } else if (cursor == 13 || cursor == 14 || cursor == 15) {
    disp.setCursor(20, 160);
    disp.print(dispMinT/(float)10);
    disp.setTextColor(ILI9341_WHITE); // Block hundredths place
    disp.setCursor(4*3*6+20, 160);
    disp.print('0');
  } else if (cursor == 16 || cursor == 17 || cursor == 18) {
    disp.setCursor(180, 160);
    disp.print(dispMaxT/(float)10);
    disp.setTextColor(ILI9341_WHITE); // Block hundredths place
    disp.setCursor(4*3*6+180, 160);
    disp.print('0');
  }
}

/**
 * Up button pressed interrupt function with debounce
 */
void UButton() {
  upPress = true;
}

/**
 * Down button pressed interrupt function with debounce
 */
void DButton() {
  downPress = true;
}

/**
 * Center button pressed interrupt function with debounce
 */
void CButton() {
  centerPress = true;
}

/**
 * Right button pressed interrupt function with debounce
 */
void RButton() {
  rightPress = true;
}

/**
 * Left button pressed interrupt function with debounce
 */
void LButton() {
  leftPress = true;
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
  pinMode(PIN_TS_ACTIVE, OUTPUT);

  attachInterrupt(digitalPinToInterrupt(PIN_BTN_LEFT), LButton, FALLING);
  attachInterrupt(digitalPinToInterrupt(PIN_BTN_RIGHT), RButton, FALLING);
  attachInterrupt(digitalPinToInterrupt(PIN_BTN_UP), UButton, FALLING);
  attachInterrupt(digitalPinToInterrupt(PIN_BTN_DOWN), DButton, FALLING);
  attachInterrupt(digitalPinToInterrupt(PIN_BTN_CENTER), CButton, FALLING);
  
  // Initialize FSM variables
  dispState = PackScreen;
  dispCursor = 0;
  dispMinV = reg->minVoltage;
  dispMaxV = reg->maxVoltage;
  dispMinT = reg->minTemp;
  dispMaxT = reg->maxTemp;
  dispNumSegPlusCells = 0;
  dispNumSegMinusCells = 0;
  upPress = false;
  downPress = false;
  centerPress = false;
  rightPress = false;
  leftPress = false;
  
  // Initialize display
  disp.begin();
  disp.setRotation(3); // 2=(0,0) in lower left math std, 3=(0,0) in upper left cs std
  disp.fillScreen(ILI9341_WHITE);
  drawPackScreen(ILI9341_BLACK);
  drawCursor(ILI9341_BLUE, 0);
  
  Serial.println("Core 0 ready");
}

/**
 *  Main loop for Core 0
 *  Runs FSM for display
 */
void Core0::runCore0() {
  // FSM
  switch (dispState) { //**************************************************************************** Transitions
    case (PackScreen): //**************************************************************************** PackScreen
      if (upPress) {
        upPress = false;
        if (dispCursor == 0) { // Increase tens count
          drawCellNumTens(ILI9341_WHITE); // Erase old tens place
          if (cellNumTens == 9) cellNumTens = 0;
          else cellNumTens++;
          drawCellNumTens(ILI9341_BLACK); // Draw new tens place
        } else { // Increase ones count
          drawCellNumOnes(ILI9341_WHITE); // Erase old ones place
          if (cellNumOnes == 9) cellNumOnes = 0;
          else cellNumOnes++;
          drawCellNumOnes(ILI9341_BLACK); // Draw new ones place
        }
      } else if (downPress) {
        downPress = false;
        if (dispCursor == 0) { // Decrease tens count
          drawCellNumTens(ILI9341_WHITE); // Erase old tens place
          if (cellNumTens == 0) cellNumTens = 9;
          else cellNumTens--;
          drawCellNumTens(ILI9341_BLACK); // Draw new tens place
        } else { // Decrease ones count
          drawCellNumOnes(ILI9341_WHITE); // Erase old ones place
          if (cellNumOnes == 0) cellNumOnes = 9;
          else cellNumOnes--;
          drawCellNumOnes(ILI9341_BLACK); // Draw new ones place
        }
      } else if (rightPress) {
        rightPress = false;
        if (dispCursor == 0) { // Move cursor to the right
          drawCursor(ILI9341_WHITE, dispCursor);
          dispCursor = 1;
          drawCursor(ILI9341_BLUE, dispCursor);
        }
      } else if (leftPress) {
        leftPress = false;
        if (dispCursor == 1) { // Move cursor to the left
          drawCursor(ILI9341_WHITE, dispCursor);
          dispCursor = 0;
          drawCursor(ILI9341_BLUE, dispCursor);
        }
      } else if (centerPress) {
        centerPress = false;
        cellNum = cellNumTens*10+cellNumOnes;
        if (cellNum >= 1 && cellNum <= 16) { // For valid cell numbers, update to CellScreen
          dispState = CellScreen;
          drawCellScreen(ILI9341_BLACK, cellNum);
        } else if (cellNum > 16 && cellNum < 99) {
          dispState = NoCellScreen;
          eraseOldData();
          drawNoCellScreen(ILI9341_BLACK);
        } else if (cellNum == 99) {
          dispState = ThresholdScreen;
          drawPackScreen(ILI9341_WHITE);
          eraseOldData();
          drawCellNumTens(ILI9341_WHITE);
          drawCellNumOnes(ILI9341_WHITE);
          eraseCellsOnSeg();
          drawCursor(ILI9341_WHITE, dispCursor);
          drawThresholdScreen(ILI9341_BLACK);
          dispCursor = 2;
          drawCursor(ILI9341_BLUE, dispCursor);
        }
      }
      break;
    case (CellScreen): //**************************************************************************** CellScreen
      if (upPress) {
        upPress = false;
        if (dispCursor == 0) { // Increase tens count
          drawCellNumTens(ILI9341_WHITE); // Erase old tens place
          if (cellNumTens == 9) cellNumTens = 0;
          else cellNumTens++;
          drawCellNumTens(ILI9341_BLACK); // Draw new tens place
        } else { // Increase ones count
          drawCellNumOnes(ILI9341_WHITE); // Erase old ones place
          if (cellNumOnes == 9) cellNumOnes = 0;
          else cellNumOnes++;
          drawCellNumOnes(ILI9341_BLACK); // Draw new ones place
        }
      } else if (downPress) {
        downPress = false;
        if (dispCursor == 0) { // Decrease tens count
          drawCellNumTens(ILI9341_WHITE); // Erase old tens place
          if (cellNumTens == 0) cellNumTens = 9;
          else cellNumTens--;
          drawCellNumTens(ILI9341_BLACK); // Draw new tens place
        } else { // Decrease ones count
          drawCellNumOnes(ILI9341_WHITE); // Erase old ones place
          if (cellNumOnes == 0) cellNumOnes = 9;
          else cellNumOnes--;
          drawCellNumOnes(ILI9341_BLACK); // Draw new ones place
        }
      } else if (rightPress) {
        rightPress = false;
        if (dispCursor == 0) { // Move cursor to the right
          drawCursor(ILI9341_WHITE, dispCursor);
          dispCursor = 1;
          drawCursor(ILI9341_BLUE, dispCursor);
        }
      } else if (leftPress) {
        leftPress = false;
        if (dispCursor == 1) { // Move cursor to the left
          drawCursor(ILI9341_WHITE, dispCursor);
          dispCursor = 0;
          drawCursor(ILI9341_BLUE, dispCursor);
        }
      } else if (centerPress) {
        centerPress = false;
        drawCellScreen(ILI9341_WHITE, cellNum);
        cellNum = cellNumTens*10+cellNumOnes;
        if (cellNum == 0) {
          dispState = PackScreen;
          drawPackScreen(ILI9341_BLACK);
        } else if (cellNum >= 1 && cellNum <= 16) { // For valid cell numbers, update to CellScreen
          drawCellScreen(ILI9341_BLACK, cellNum);
        } else if (cellNum > 16 && cellNum < 99) { // For invalid cell numbers
          dispState = NoCellScreen;
          eraseOldData();
          drawNoCellScreen(ILI9341_BLACK);
        } else if (cellNum == 99) {
          dispState = ThresholdScreen;
          drawPackScreen(ILI9341_WHITE);
          eraseOldData();
          drawCellNumTens(ILI9341_WHITE);
          drawCellNumOnes(ILI9341_WHITE);
          eraseCellsOnSeg();
          drawCursor(ILI9341_WHITE, dispCursor);
          drawThresholdScreen(ILI9341_BLACK);
          dispCursor = 2;
          drawCursor(ILI9341_BLUE, dispCursor);
        } 
      }
      break;
    case (NoCellScreen): //************************************************************************** NoCellScreen
      if (upPress) {
        upPress = false;
        if (dispCursor == 0) { // Increase tens count
          drawCellNumTens(ILI9341_WHITE); // Erase old tens place
          if (cellNumTens == 9) cellNumTens = 0;
          else cellNumTens++;
          drawCellNumTens(ILI9341_BLACK); // Draw new tens place
        } else { // Increase ones count
          drawCellNumOnes(ILI9341_WHITE); // Erase old ones place
          if (cellNumOnes == 9) cellNumOnes = 0;
          else cellNumOnes++;
          drawCellNumOnes(ILI9341_BLACK); // Draw new ones place
        }
      } else if (downPress) {
        downPress = false;
        if (dispCursor == 0) { // Decrease tens count
          drawCellNumTens(ILI9341_WHITE); // Erase old tens place
          if (cellNumTens == 0) cellNumTens = 9;
          else cellNumTens--;
          drawCellNumTens(ILI9341_BLACK); // Draw new tens place
        } else { // Decrease ones count
          drawCellNumOnes(ILI9341_WHITE); // Erase old ones place
          if (cellNumOnes == 0) cellNumOnes = 9;
          else cellNumOnes--;
          drawCellNumOnes(ILI9341_BLACK); // Draw new ones place
        }
      } else if (rightPress) {
        rightPress = false;
        if (dispCursor == 0) { // Move cursor to the right
          drawCursor(ILI9341_WHITE, dispCursor);
          dispCursor = 1;
          drawCursor(ILI9341_BLUE, dispCursor);
        }
      } else if (leftPress) {
        leftPress = false;
        if (dispCursor == 1) { // Move cursor to the left
          drawCursor(ILI9341_WHITE, dispCursor);
          dispCursor = 0;
          drawCursor(ILI9341_BLUE, dispCursor);
        }
      } else if (centerPress) {
        centerPress = false;
        drawNoCellScreen(ILI9341_WHITE); // Erase
        cellNum = cellNumTens*10+cellNumOnes;
        if (cellNum == 0) {
          dispState = PackScreen;
          drawPackScreen(ILI9341_BLACK);
        } else if (cellNum >= 1 && cellNum <= 16) { // For valid cell numbers, update to CellScreen
          dispState = CellScreen;
          drawPackScreen(ILI9341_BLACK);
          drawCellScreen(ILI9341_BLACK, cellNum);
        } else if (cellNum == 99) {
          dispState = ThresholdScreen;
          drawPackScreen(ILI9341_WHITE);
          eraseOldData();
          drawCellNumTens(ILI9341_WHITE);
          drawCellNumOnes(ILI9341_WHITE);
          eraseCellsOnSeg();
          drawCursor(ILI9341_WHITE, dispCursor);
          drawThresholdScreen(ILI9341_BLACK);
          dispCursor = 2;
          drawCursor(ILI9341_BLUE, dispCursor);
        } else {
          drawNoCellScreen(ILI9341_BLACK);
        }
      }
      break;
    case (ThresholdScreen): //************************************************************************** ThresholdScreen
      if (upPress) {
        upPress = false;
        if (dispCursor >= 4 && dispCursor <= 6) {
          drawCursor(ILI9341_WHITE, dispCursor);
          dispCursor-=2;
          drawCursor(ILI9341_BLUE, dispCursor);
        }
      } else if (downPress) {
        downPress = false;
        if (dispCursor >= 2 && dispCursor <= 4) {
          drawCursor(ILI9341_WHITE, dispCursor);
          dispCursor+=2;
          drawCursor(ILI9341_BLUE, dispCursor);
        }
      } else if (rightPress) {
        rightPress = false;
        if (dispCursor == 2 || dispCursor == 4) {
          drawCursor(ILI9341_WHITE, dispCursor);
          dispCursor++;
          drawCursor(ILI9341_BLUE, dispCursor);
        }
      } else if (leftPress) {
        leftPress = false;
        if (dispCursor == 3 || dispCursor == 5) {
          drawCursor(ILI9341_WHITE, dispCursor);
          dispCursor--;
          drawCursor(ILI9341_BLUE, dispCursor);
        }
      } else if (centerPress) {
        centerPress = false;
        if (dispCursor >= 2 && dispCursor <= 5) {
          dispState = EditScreen;
          drawEditScreen(ILI9341_BLACK, dispCursor);
        } else if (dispCursor == 6) {
          dispState = PackScreen;
          drawThresholdScreen(ILI9341_WHITE);
          drawCursor(ILI9341_WHITE, dispCursor);
          cellNumTens = 0;
          cellNumOnes = 0;
          drawPackScreen(ILI9341_BLACK);
          dispCursor = 0;
          drawCursor(ILI9341_BLUE, dispCursor);
          dispNumSegMinusCells = 0;
          dispNumSegPlusCells = 0;
        }
      }
      break;
    case (EditScreen): //**************************************************************************** EditScreen
      if (upPress) {
        upPress = false;
        writeThreshold(ILI9341_WHITE, dispCursor);
        if      (dispCursor == 7  && dispMinV <= 8999) dispMinV+=1000;
        else if (dispCursor == 8  && dispMinV <= 9899) dispMinV+=100;
        else if (dispCursor == 9  && dispMinV <= 9989) dispMinV+=10;
        else if (dispCursor == 10 && dispMaxV <= 8999) dispMaxV+=1000;
        else if (dispCursor == 11 && dispMaxV <= 9899) dispMaxV+=100;
        else if (dispCursor == 12 && dispMaxV <= 9989) dispMaxV+=10;
        else if (dispCursor == 13 && dispMinT <= 899)  dispMinT+=100;
        else if (dispCursor == 14 && dispMinT <= 989)  dispMinT+=10;
        else if (dispCursor == 15 && dispMinT <= 998)  dispMinT++;
        else if (dispCursor == 16 && dispMaxT <= 899)  dispMaxT+=100;
        else if (dispCursor == 17 && dispMaxT <= 989)  dispMaxT+=10;
        else if (dispCursor == 18 && dispMaxT <= 998)  dispMaxT++;
        writeThreshold(ILI9341_BLACK, dispCursor);
      } else if (downPress) {
        downPress = false;
        writeThreshold(ILI9341_WHITE, dispCursor);
        if      (dispCursor == 7  && dispMinV >= 1000) dispMinV-=1000;
        else if (dispCursor == 8  && dispMinV >= 100 ) dispMinV-=100;
        else if (dispCursor == 9  && dispMinV >= 10  ) dispMinV-=10;
        else if (dispCursor == 10 && dispMaxV >= 1000) dispMaxV-=1000;
        else if (dispCursor == 11 && dispMaxV >= 100 ) dispMaxV-=100;
        else if (dispCursor == 12 && dispMaxV >= 10  ) dispMaxV-=10;
        else if (dispCursor == 13 && dispMinT >= 100 ) dispMinT-=100;
        else if (dispCursor == 14 && dispMinT >= 10  ) dispMinT-=10;
        else if (dispCursor == 15 && dispMinT >= 1   ) dispMinT--;
        else if (dispCursor == 16 && dispMaxT >= 100 ) dispMaxT-=100;
        else if (dispCursor == 17 && dispMaxT >= 10  ) dispMaxT-=10;
        else if (dispCursor == 18 && dispMaxT >= 1   ) dispMaxT--;
        writeThreshold(ILI9341_BLACK, dispCursor);
      } else if (rightPress) {
        rightPress = false;
        drawCursor(ILI9341_WHITE, dispCursor);
        if      (dispCursor == 2)  dispCursor = 7;
        else if (dispCursor == 3)  dispCursor = 10;
        else if (dispCursor == 4)  dispCursor = 13;
        else if (dispCursor == 5)  dispCursor = 16;
        else if (dispCursor == 7  || dispCursor == 8  || dispCursor == 10 || dispCursor == 11 ||
                 dispCursor == 13 || dispCursor == 14 || dispCursor == 16 || dispCursor == 17)  dispCursor++;
        drawCursor(ILI9341_BLUE, dispCursor);
      } else if (leftPress) {
        leftPress = false;
        drawCursor(ILI9341_WHITE, dispCursor);
        if      (dispCursor == 7)   dispCursor = 2;
        else if (dispCursor == 10)  dispCursor = 3;
        else if (dispCursor == 13)  dispCursor = 4;
        else if (dispCursor == 16)  dispCursor = 5;
        else if (dispCursor == 8  || dispCursor == 9  || dispCursor == 11 || dispCursor == 12 ||
                 dispCursor == 14 || dispCursor == 15 || dispCursor == 17 || dispCursor == 18)  dispCursor--;
        drawCursor(ILI9341_BLUE, dispCursor);                     
      } else if (centerPress) {
        centerPress = false;
        drawEditScreen(ILI9341_WHITE, dispCursor);
        if (dispCursor == 2 || dispCursor == 3 || dispCursor == 4 || dispCursor == 5) {
          dispState = ThresholdScreen;
          dispMinV = reg->minVoltage;
          dispMaxV = reg->maxVoltage;
          dispMinT = reg->minTemp;
          dispMaxT = reg->maxTemp;
        } else if (dispCursor >=7 && dispCursor <= 18) {
          dispState = ConfirmScreen;
          drawThresholdScreen(ILI9341_WHITE);
          writeThreshold(ILI9341_WHITE, dispCursor);
          drawCursor(ILI9341_WHITE, dispCursor);
          confirmCursor = dispCursor;
          drawConfirmScreen(ILI9341_BLACK, confirmCursor);
          dispCursor = 19;
          drawCursor(ILI9341_BLUE, dispCursor);
        }
      }
      break;
    case (ConfirmScreen): //************************************************************************* State actions
      if (upPress) upPress = false;
      else if (downPress) downPress = false;
      else if (rightPress) {
        rightPress = false;
        if (dispCursor == 19) { // Redraw cursor in right pos
          drawCursor(ILI9341_WHITE, dispCursor);
          dispCursor = 20;
          drawCursor(ILI9341_BLUE, dispCursor);
        }
      } else if (leftPress) {
        leftPress = false;
        if (dispCursor == 20) { // Redraw cursor in left pos
          drawCursor(ILI9341_WHITE, dispCursor);
          dispCursor = 19;
          drawCursor(ILI9341_BLUE, dispCursor);
        }
      } else if (centerPress) {
        centerPress = false;
        drawConfirmScreen(ILI9341_WHITE, confirmCursor);
        drawCursor(ILI9341_WHITE, dispCursor);
        drawThresholdScreen(ILI9341_BLACK);
        if (dispCursor == 19) {
          dispState = EditScreen;
          // Reset register values
          dispMinV = reg->minVoltage;
          dispMaxV = reg->maxVoltage;
          dispMinT = reg->minTemp;
          dispMaxT = reg->maxTemp;
          dispCursor = confirmCursor;
          drawEditScreen(ILI9341_BLACK, dispCursor);
          writeThreshold(ILI9341_BLACK, dispCursor);
        } else if (dispCursor = 20) {
          dispState = ThresholdScreen;
          // Update register values
          reg->minVoltage = dispMinV;
          reg->maxVoltage = dispMaxV;
          reg->minTemp = dispMinT;
          reg->maxTemp = dispMaxT;
          dispCursor = 2;
        }
        drawCursor(ILI9341_BLUE, dispCursor);
        confirmCursor = 0;
      }
      break;
    default: dispState = PackScreen;
      break;
  }
  switch (dispState) { //**************************************************************************** State actions
    case (PackScreen):
      eraseOldData();
      writePackData();
      drawCellsOnSeg(ILI9341_GREEN);
      break;
    case (CellScreen):
      eraseOldData();
      writeCellData(cellNum);
      drawCellsOnSeg(ILI9341_GREEN);
      break;
    case (NoCellScreen): // Do nothing
      break;
    case (ThresholdScreen): // Do nothing
      break;
    case (EditScreen): // Do nothing
      break;
    case (ConfirmScreen): // Do nothing
      break;
    default:
      dispCursor = 0;
      cellNumTens = 0;
      cellNumOnes = 0;
      cellNum = 0;
      break;
  }
}
