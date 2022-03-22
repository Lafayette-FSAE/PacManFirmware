/**
 * PacManRegisters.h - The header file for PacMan registers which hold threshold values
 * Author: Justin Kogasaka
 * Date: 2/27/2022
 */

#ifndef PacManRegisters_h
#define PacManRegisters_h

class PacManRegisters {
  public:
    // Register variables
    uint8_t numCells;
    uint16_t minVoltage;
    uint16_t maxVoltage;
    uint16_t minTemp;
    uint16_t maxTemp;
    uint16_t packVoltage;
    uint16_t minCellVoltage;
    uint16_t avgCellVoltage;
    uint16_t maxCellVoltage;
    uint16_t minCellTemp;
    uint16_t avgCellTemp;
    uint16_t maxCellTemp;
    bool GLVStatus;
    bool SLOOP1Status;
    bool SLOOP2Status;
    bool AIRSStatus;
    bool chargerDetected;
    bool chargingEnabled;
    uint16_t chargingCurrent;
    uint16_t dischargingCurrent;
    uint16_t PacManCurrent;
    uint8_t SOC;

    // Functions
    PacManRegisters(); // Constructor
};

#endif
