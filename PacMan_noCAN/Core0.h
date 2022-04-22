/**
 * Core0.h - The header file for the main display interface core
 * Author: Justin Kogasaka
 * Date: 3/12/2022
 */

#ifndef Core0_h
#define Core0_h

#include "PacManRegisters.h"

class Core0 {
  private:
    PacManRegisters* reg;  // Pointer to the PacMan registers; needs to update values so use a pointer

    // Private function calls
    void drawCursor(uint16_t color, uint8_t index);
    void drawCellNumTens(uint16_t color);
    void drawCellNumOnes(uint16_t color);
    void drawCellsOnSeg(uint16_t color);
    
    void drawPackScreen(uint16_t color);
    void drawCellScreen(uint16_t color, uint8_t cellNum);
    void drawNoCellScreen(uint16_t color);
    void drawThresholdScreen(uint16_t color);
    void drawEditScreen(uint16_t color, uint8_t cursor);
    void drawConfirmScreen(uint16_t color, uint8_t cursor);
    
    void writePackData();
    void writeCellData(uint8_t cellNum);
    void eraseOldData();
    void writeThreshold(uint16_t color, uint8_t cursor);
  public:
    Core0(PacManRegisters* reg);  // Constructor
    void initCore0();
    void runCore0();
};

#endif
