//#include "Core0.h"
#include "Core1.h"

// GLOBALS
struct cell cells[16];             // Written to by Core1, read by Core0
struct cellConfig cellConfigs[16]; // Written to by Core0, read by Core1 for CAN Bus
float miscConfigs[6];              // Written to by Core0, read by Core1 for CAN Bus
boolean externalFault;             // Written to by Core1 from CAN Bus, read by Core0
boolean AIRSOpen;                  // Written to by Core0, read by Core1 for CAN Bus

// SEMAPHORES
//SemaphoreHandle_t cellArraySem;
//SemaphoreHandle_t cellConfigSem;
//SemaphoreHandle_t miscConfigSem;
//SemaphoreHandle_t externalFaultSem;
//SemaphoreHandle_t AIRSOpenSem;

void setup() {
  // put your setup code here, to run once:
  //Core0.run(); //add task stuff
  //Core1.run(); //add task stuff
}

void loop() {
  delay(100); //delay is so that loop does not eat up CPU time
}
