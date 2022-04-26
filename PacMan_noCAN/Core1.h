/**
 * Core1.h - The header file for the main data processing core
 * Author: Justin Kogasaka
 * Date: 2/19/2022
 */

#ifndef Core1_h
#define Core1_h

#include "PacManRegisters.h"

#define MSG_LEN 7 // I2C message length in bytes from CellMen
#define TRIGGER_TIME 59 // 59 seconds to trip safety loop for persisting problem

// Static functions
static void openSafetyLoopCallback(TimerHandle_t xTimer);

class Core1 {
  private:
    PacManRegisters* reg;   // Pointer to the PacMan registers; needs to update values so use a pointer
    uint8_t numCellMenFound;
    uint8_t numSegPlusCells;
    uint8_t numSegMinusCells;
    uint8_t cellAddr[16];      // Cell I2C addresses
    uint8_t cellAddrByPos[16]; // Cell I2C addresses indexed by pack position
    uint8_t cellFaults[16];    // Cell fault status
    uint16_t cellV[16];        // Cell voltages
    uint16_t cellT[16];        // Cell temperatures
    uint16_t cellPos[16];      // Cell positions from PACK- to PACK+

    // Safety timer handles
    TimerHandle_t lowVoltageTimer;
    TimerHandle_t highVoltageTimer;
    TimerHandle_t lowTempTimer;
    TimerHandle_t highTempTimer;

    // Private function calls
    inline void findCellMen();
    inline void writeCellManLED(unsigned char addr, bool state);
    inline void indicateCellMen();
    inline void pollCellMen();
    inline void updateRegisters();
    inline void checkSafety();
    inline void handleChargingRelays();
    
  public:
    Core1(PacManRegisters* reg);  // Constructor
    void initCore1();
    void runCore1();
};

#endif
