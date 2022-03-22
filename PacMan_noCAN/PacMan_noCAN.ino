/**
 * Project main
 * Author: Justin Kogasaka
 * Date: 2/19/2022
 */

#include "PacMan_config.h"
#include "Core0.h"
#include "Core1.h"
#include "PacManRegisters.h"

// Implement dual core operation: https://randomnerdtutorials.com/esp32-dual-core-arduino-ide/
TaskHandle_t Core0Task; // Core0 display interface handle
TaskHandle_t Core1Task; // Core1 data proccessing handle

// Global instances
PacManRegisters registers;

/** 
 *  Core 0 task
 *  Creates a Core0 object, configs pinout, initialize LCD
 *  In loop: Run FSM
 */
void core0TaskCode(void *parameter) {
  Core0 core0(&registers);
  core0.initCore0();
  for (;;) { // Infinite loop
    core0.runCore0();
    delay(500); // Ideally, use a timer
  }
}

/** 
 *  Core 1 task
 *  Creates a Core1 object, configs pinout, joins I2C bus and finds CellMen
 *  In loop: Requests CellMan data via I2C and processes it
 */
void core1TaskCode(void *parameter) {
  Core1 core1(&registers);
  core1.initCore1();
  for (;;) { // Infinite loop
    core1.runCore1();
    delay(500); // Ideally, use a timer
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("Serial begin");

  // Create Core 0 task
  xTaskCreatePinnedToCore(core0TaskCode, "Core0Task", 10000, NULL, 0, &Core0Task, 0);
  
  // Create Core 1 task
  xTaskCreatePinnedToCore(core1TaskCode, "Core1Task", 10000, NULL, 2, &Core1Task, 1);
}

void loop() {
  // Delay forever; the program runs on interrupts
  delay(1000);
}
