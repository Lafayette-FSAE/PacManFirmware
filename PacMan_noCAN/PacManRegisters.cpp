/**
 * PacManRegisters.cpp - Main file for PacMan data registers
 * Author: Justin Kogasaka
 * Date: 2/27/2022
 */

#include "PacMan_config.h"
#include "PacManRegisters.h"

/* Constructor */
PacManRegisters::PacManRegisters() {
  // Set default register values
  minVoltage = 2500;
  maxVoltage = 4000;
  minTemp = 150;
  maxTemp = 650;
}
