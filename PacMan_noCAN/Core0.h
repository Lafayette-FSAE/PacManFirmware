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
    void drawPackScreen(uint16_t color);
    void drawCellScreen(uint16_t color, uint8_t cellNum);
    void drawThresholdScreen(uint16_t color);
    void drawConfirmScreen(uint16_t color);
    
    void writePackData();
    void writeCellData(uint8_t cellNum);
    void eraseOldData();
  public:
    Core0(PacManRegisters* reg);  // Constructor
    void initCore0();
    void runCore0();
};

#endif
