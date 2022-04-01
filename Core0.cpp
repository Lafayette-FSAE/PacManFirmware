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
int boardWidth = 320;
int boardHeight = 240;

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
  disp.setRotation(3); // 2=(0,0) in lower left math std, 3=(0,0) in upper left cs std
  disp.fillScreen(ILI9341_WHITE);
  //Pack #1 Text
  disp.drawChar(20, 10, 'P', ILI9341_BLACK,ILI9341_WHITE, 3);
  disp.drawChar(40, 10, 'a', ILI9341_BLACK,ILI9341_WHITE, 3);
  disp.drawChar(60, 10, 'c', ILI9341_BLACK,ILI9341_WHITE, 3);
  disp.drawChar(80, 10, 'k', ILI9341_BLACK,ILI9341_WHITE, 3);
  disp.drawChar(110, 10, '#', ILI9341_BLACK,ILI9341_WHITE, 3);
  disp.drawChar(130, 10, '1', ILI9341_BLACK,ILI9341_WHITE, 3);
  //Seg 1
  disp.drawChar(70, 43, 'S', ILI9341_BLACK,ILI9341_WHITE, 2);
  disp.drawChar(85, 43, 'e', ILI9341_BLACK,ILI9341_WHITE, 2);
  disp.drawChar(100, 43, 'g', ILI9341_BLACK,ILI9341_WHITE, 2);
  disp.drawChar(120, 43, '1', ILI9341_BLACK,ILI9341_WHITE, 2);
  disp.drawRect(21.4,60,21,40,ILI9341_BLACK);
  disp.drawRect(42.4,60,21,40,ILI9341_BLACK);
  disp.drawRect(63.4,60,21,40,ILI9341_BLACK);
  disp.drawRect(84.4,60,21,40,ILI9341_BLACK);
  disp.drawRect(105.4,60,21,40,ILI9341_BLACK);
  disp.drawRect(126.4,60,21,40,ILI9341_BLACK);
  disp.drawRect(147.4,60,21,40,ILI9341_BLACK);
  disp.drawRect(168.4,60,21,40,ILI9341_BLACK);
  disp.drawChar(28.4, 102, '1', ILI9341_BLACK,ILI9341_WHITE, 1);
  disp.drawChar(49.4, 102, '2', ILI9341_BLACK,ILI9341_WHITE, 1);
  disp.drawChar(70.4, 102, '3', ILI9341_BLACK,ILI9341_WHITE, 1);
  disp.drawChar(91.4, 102, '4', ILI9341_BLACK,ILI9341_WHITE, 1);
  disp.drawChar(111.4, 102, '5', ILI9341_BLACK,ILI9341_WHITE, 1);
  disp.drawChar(132.4, 102, '6', ILI9341_BLACK,ILI9341_WHITE, 1);
  disp.drawChar(153.4, 102, '7', ILI9341_BLACK,ILI9341_WHITE, 1);
  disp.drawChar(174.4, 102, '8', ILI9341_BLACK,ILI9341_WHITE, 1);
  //Seg 2
  disp.drawChar(70, 120, 'S', ILI9341_BLACK,ILI9341_WHITE, 2);
  disp.drawChar(85, 120, 'e', ILI9341_BLACK,ILI9341_WHITE, 2);
  disp.drawChar(100, 120, 'g', ILI9341_BLACK,ILI9341_WHITE, 2);
  disp.drawChar(120, 120, '2', ILI9341_BLACK,ILI9341_WHITE, 2);
  disp.drawRect(21.4,139,21,40,ILI9341_BLACK);
  disp.drawRect(42.4,139,21,40,ILI9341_BLACK);
  disp.drawRect(63.4,139,21,40,ILI9341_BLACK);
  disp.drawRect(84.4,139,21,40,ILI9341_BLACK);
  disp.drawRect(105.4,139,21,40,ILI9341_BLACK);
  disp.drawRect(126.4,139,21,40,ILI9341_BLACK);
  disp.drawRect(147.4,139,21,40,ILI9341_BLACK);
  disp.drawRect(168.4,139,21,40,ILI9341_BLACK);
  disp.drawChar(28.4, 182, '9', ILI9341_BLACK,ILI9341_WHITE, 1);
  disp.drawChar(49.4, 182, '1', ILI9341_BLACK,ILI9341_WHITE, 1);
  disp.drawChar(53.4, 182, '0', ILI9341_BLACK,ILI9341_WHITE, 1);
  disp.drawChar(70.4, 182, '1', ILI9341_BLACK,ILI9341_WHITE, 1);
  disp.drawChar(74.4, 182, '1', ILI9341_BLACK,ILI9341_WHITE, 1);
  disp.drawChar(91.4, 182, '1', ILI9341_BLACK,ILI9341_WHITE, 1);
  disp.drawChar(95.4, 182, '2', ILI9341_BLACK,ILI9341_WHITE, 1);
  disp.drawChar(111.4, 182, '1', ILI9341_BLACK,ILI9341_WHITE, 1);
  disp.drawChar(115.4, 182, '3', ILI9341_BLACK,ILI9341_WHITE, 1);
  disp.drawChar(132.4, 182, '1', ILI9341_BLACK,ILI9341_WHITE, 1);
  disp.drawChar(136.4, 182, '4', ILI9341_BLACK,ILI9341_WHITE, 1);
  disp.drawChar(153.4, 182, '1', ILI9341_BLACK,ILI9341_WHITE, 1);
  disp.drawChar(157.4, 182, '5', ILI9341_BLACK,ILI9341_WHITE, 1);
  disp.drawChar(174.4, 182, '1', ILI9341_BLACK,ILI9341_WHITE, 1);
  disp.drawChar(178.4, 182, '6', ILI9341_BLACK,ILI9341_WHITE, 1);
  //Cell Select
  disp.drawChar(20, 205, 'C', ILI9341_BLACK,ILI9341_WHITE, 2);
  disp.drawChar(32, 205, 'e', ILI9341_BLACK,ILI9341_WHITE, 2);
  disp.drawChar(44, 205, 'l', ILI9341_BLACK,ILI9341_WHITE, 2);
  disp.drawChar(56, 205, 'l', ILI9341_BLACK,ILI9341_WHITE, 2);
  disp.drawChar(75, 205, 'n', ILI9341_BLACK,ILI9341_WHITE, 2);
  disp.drawChar(87, 205, 'u', ILI9341_BLACK,ILI9341_WHITE, 2);
  disp.drawChar(99, 205, 'm', ILI9341_BLACK,ILI9341_WHITE, 2);
  disp.drawChar(111, 205, ':', ILI9341_BLACK,ILI9341_WHITE, 2);
  disp.drawChar(135, 205, '0', ILI9341_BLACK,ILI9341_WHITE, 3);
  disp.drawChar(160, 205, '0', ILI9341_BLACK,ILI9341_WHITE, 3);
  disp.drawChar(160, 235, '_', ILI9341_BLACK,ILI9341_WHITE, 3);
  //Data
  disp.drawChar(205, 20, 'S', ILI9341_BLACK,ILI9341_WHITE, 3);
  disp.drawChar(225, 20, 'O', ILI9341_BLACK,ILI9341_WHITE, 3);
  disp.drawChar(245, 20, 'C', ILI9341_BLACK,ILI9341_WHITE, 3);
  disp.drawChar(260, 20, ':', ILI9341_BLACK,ILI9341_WHITE, 3);
  disp.drawChar(225, 43, '8', ILI9341_BLACK,ILI9341_WHITE, 3);
  disp.drawChar(245, 43, '4', ILI9341_BLACK,ILI9341_WHITE, 3);
  disp.drawChar(265, 43, '%', ILI9341_BLACK,ILI9341_WHITE, 3);
  disp.drawChar(205, 75, 'V', ILI9341_BLACK,ILI9341_WHITE, 3);
  disp.drawChar(225, 75, 'o', ILI9341_BLACK,ILI9341_WHITE, 3);
  disp.drawChar(240, 75, 'l', ILI9341_BLACK,ILI9341_WHITE, 3);
  disp.drawChar(255, 75, 't', ILI9341_BLACK,ILI9341_WHITE, 3);
  disp.drawChar(270, 75, ':', ILI9341_BLACK,ILI9341_WHITE, 3);
  disp.drawChar(225, 97, '4', ILI9341_BLACK,ILI9341_WHITE, 3);
  disp.drawChar(245, 97, '4', ILI9341_BLACK,ILI9341_WHITE, 3);
  disp.drawChar(260, 97, '.', ILI9341_BLACK,ILI9341_WHITE, 3);
  disp.drawChar(275, 97, '4', ILI9341_BLACK,ILI9341_WHITE, 3);
  disp.drawChar(295, 97, 'V', ILI9341_BLACK,ILI9341_WHITE, 3);
  disp.drawChar(205, 130, 'C', ILI9341_BLACK,ILI9341_WHITE, 3);
  disp.drawChar(225, 130, 'u', ILI9341_BLACK,ILI9341_WHITE, 3);
  disp.drawChar(245, 130, 'r', ILI9341_BLACK,ILI9341_WHITE, 3);
  disp.drawChar(262, 130, 'r', ILI9341_BLACK,ILI9341_WHITE, 3);
  disp.drawChar(275, 130, ':', ILI9341_BLACK,ILI9341_WHITE, 3);
  disp.drawChar(225, 157, '1', ILI9341_BLACK,ILI9341_WHITE, 3);
  disp.drawChar(245, 157, '5', ILI9341_BLACK,ILI9341_WHITE, 3);
  disp.drawChar(260, 157, '.', ILI9341_BLACK,ILI9341_WHITE, 3);
  disp.drawChar(275, 157, '2', ILI9341_BLACK,ILI9341_WHITE, 3);
  disp.drawChar(295, 157, 'A', ILI9341_BLACK,ILI9341_WHITE, 3);
  disp.drawChar(205, 185, 'T', ILI9341_BLACK,ILI9341_WHITE, 3);
  disp.drawChar(225, 185, 'e', ILI9341_BLACK,ILI9341_WHITE, 3);
  disp.drawChar(245, 185, 'm', ILI9341_BLACK,ILI9341_WHITE, 3);
  disp.drawChar(262, 185, 'p', ILI9341_BLACK,ILI9341_WHITE, 3);
  disp.drawChar(277, 185, ':', ILI9341_BLACK,ILI9341_WHITE, 3);
  disp.drawChar(225, 215, '2', ILI9341_BLACK,ILI9341_WHITE, 3);
  disp.drawChar(245, 215, '2', ILI9341_BLACK,ILI9341_WHITE, 3);
  disp.drawChar(260, 215, '.', ILI9341_BLACK,ILI9341_WHITE, 3);
  disp.drawChar(275, 215, '0', ILI9341_BLACK,ILI9341_WHITE, 3);
  disp.drawChar(295, 215, 'C', ILI9341_BLACK,ILI9341_WHITE, 3);
  Serial.println("Core 0 ready");
}

/** 
 *  Main loop for Core 0
 *  Runs FSM for display
 */
void Core0::runCore0() {
  delay(500);
}
