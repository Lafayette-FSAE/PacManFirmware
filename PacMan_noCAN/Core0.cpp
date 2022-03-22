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
//  attachInterrupt(digitalPinToInterrupt(PIN_BTN_LEFT), LButton, FALLING);
//  attachInterrupt(digitalPinToInterrupt(PIN_BTN_RIGHT), RButton, FALLING);
//  attachInterrupt(digitalPinToInterrupt(PIN_BTN_UP), UButton, FALLING);
//  attachInterrupt(digitalPinToInterrupt(PIN_BTN_DOWN), DButton, FALLING);
//  attachInterrupt(digitalPinToInterrupt(PIN_BTN_CENTER), CButton, FALLING);
  
  disp.begin();
  disp.setRotation(2); // 2=(0,0) in lower left math std, 3=(0,0) in upper left cs std
  disp.fillScreen(ILI9341_WHITE);
  disp.drawLine(0, 0, 50, 50, ILI9341_BLACK);
  Serial.println("Core 0 ready");
}

/** 
 *  Main loop for Core 0
 *  Runs FSM for display
 */
void Core0::runCore0() {
  delay(500);
}
