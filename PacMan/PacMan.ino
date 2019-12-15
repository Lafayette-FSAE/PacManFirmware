
//#include <Core0.h>
//#include "Core0.h"
#include "Core1.h"
#include "Core0.h"
#include "PacMan.h"

cell cells[16];               // Written to by Core1, read by Core0
float externalFault;                 // Written to by Core1 from CAN Bus, read by Core0
boolean AIRSOpen;                    // Written to by Core0, read by Core1 for CAN Bus
int sample;

// SEMAPHORES
TaskHandle_t Task0, Task1;
SemaphoreHandle_t sampleSem;
SemaphoreHandle_t cellArraySem;
SemaphoreHandle_t externalFaultSem;
SemaphoreHandle_t AIRSOpenSem;


#include "Core0.h"
#include "Core1.h"

// GLOBALS
struct cell cells[16];               // Written to by Core1, read by Core0
float externalFault;                 // Written to by Core1 from CAN Bus, read by Core0
boolean AIRSOpen;                    // Written to by Core0, read by Core1 for CAN Bus
//struct cellConfig cellConfigs[16]; // Written to by Core0, read by Core1 for CAN Bus    --to be added as a later feature
//float miscConfigs[6];              // Written to by Core0, read by Core1 for CAN Bus    --to be added as a later feature

// SEMAPHORES
TaskHandle_t Task1, Task2;
SemaphoreHandle_t cellArraySem;
SemaphoreHandle_t externalFaultSem;
SemaphoreHandle_t AIRSOpenSem;

void setup() {
  cellArraySem = xSemaphoreCreateMutex();
  externalFaultSem = xSemaphoreCreateMutex();
  AIRSOpenSem = xSemaphoreCreateMutex();

  xTaskCreatePinnedToCore(&codeForTask1, "Core0Task", 1000, NULL, 1, &Task1, 0);
  delay(500);
  xTaskCreatePinnedToCore(&codeForTask2, "Core1Task", 1000, NULL, 1, &Task2, 1);
void setup() {
 Serial.begin(115200);
 
 cellArraySem = xSemaphoreCreateMutex();
 externalFaultSem = xSemaphoreCreateMutex();
 AIRSOpenSem = xSemaphoreCreateMutex();
 sampleSem = xSemaphoreCreateMutex();

  xTaskCreatePinnedToCore(&codeForTask0, "Core0Task", 10000, NULL, 1, &Task0, 0);
  delay(500);
  xTaskCreatePinnedToCore(&codeForTask1, "Core1Task", 10000, NULL, 1, &Task1, 1);
}

void loop() {
// put your main code here, to run repeatedly:
    delay(100);
}

void codeForTask0( void * parameter )
{
  Core0 core0(cells, &externalFault, &AIRSOpen, &cellArraySem, &externalFaultSem, &AIRSOpenSem, &sampleSem, &sample);
  for(;;){
    Serial.println("task0");
    core0.startCore0();
  }
}

void codeForTask1( void * parameter ){
  Core1 core1(cells, &externalFault, &AIRSOpen, &cellArraySem, &externalFaultSem, &AIRSOpenSem, &sampleSem, &sample);
  for(;;){
    Serial.println("task1");
    delay(250);
    core1.start();
  }
}

void codeForTask1( void * parameter )
{
    Core0 core0 (cells, externalFault, AIRSOpen, cellArraySem, externalFaultSem, AIRSOpenSem);
    core0.start();
}

void codeForTask2( void * parameter )
{
    Core1 core1 (cells, externalFault, AIRSOpen, cellArraySem, externalFaultSem, AIRSOpenSem);
    core1.start();
}
