#include "Arduino.h"
#include "Wire.h"

#include "PacMan.h"
#include "Core1.h"

volatile boolean charging = false;
volatile boolean checkIOExpander = false;

TaskHandle_t Task0, Task1;
hw_timer_t * timer0 = NULL;

// Handles when CHRG_DETECT changes
void IRAM_ATTR chargeDetectInt()
{
  if (digitalRead(PIN_CHRG_DET)) {
    charging = false;
    digitalWrite(PIN_CHRG_EN, LOW);
  }
  else {
    charging = true;
    digitalWrite(PIN_CHRG_EN, HIGH);
  }
}

// Handles when IO_INT rises, indicating that there was a SLOOP-related change
void IRAM_ATTR ioExpanderInt()
{
  checkIOExpander = true;
}

void IRAM_ATTR onTimer() {
    Serial.println("Timer interrupt occured.");
}

void setup() {
  Serial.begin(115200);
  
  // Set GPIO modes
  pinMode(PIN_BTN_CENTER, INPUT);
  pinMode(PIN_BTN_UP,     INPUT);
  pinMode(PIN_BTN_DOWN,   INPUT);
  pinMode(PIN_BTN_LEFT,   INPUT);
  pinMode(PIN_BTN_RIGHT,  INPUT);
  pinMode(PIN_IO_INT,     INPUT);
  pinMode(PIN_CHRG_DET,   INPUT);
  
  pinMode(PIN_LED_GREEN,  OUTPUT);
  pinMode(PIN_LED_ORANGE, OUTPUT);
  pinMode(PIN_SLOOP_EN,   OUTPUT);
  pinMode(PIN_CHRG_EN,    OUTPUT);
  pinMode(PIN_WATCHDOG,   OUTPUT);

  // Set default values for GPIO
  digitalWrite(PIN_LED_GREEN,  LOW);
  digitalWrite(PIN_LED_ORANGE, LOW);
  digitalWrite(PIN_SLOOP_EN,   HIGH);
  digitalWrite(PIN_CHRG_EN,    LOW);
  digitalWrite(PIN_WATCHDOG,   LOW);

  // Map interrupts
  attachInterrupt(digitalPinToInterrupt(PIN_CHRG_DET), chargeDetectInt, CHANGE);
  attachInterrupt(digitalPinToInterrupt(PIN_IO_INT), ioExpanderInt, RISING);

  timer0 = timerBegin(0, 80, true);
  timerAttachInterrupt(timer0, &onTimer, true);
  timerAlarmWrite(timer0, 1000000, true);
  timerAlarmEnable(timer0);

  // xTaskCreatePinnedToCore(&codeForTask0, "Core0Task", 10000, NULL, 1, &Task0, 0);
  xTaskCreatePinnedToCore(&codeForTask1, "Core1Task", 10000, NULL, 1, &Task1, 1);
}

void loop() {
  delay(1000);
}

// void codeForTask0(void * parameter) {
//   Core0 core0;
//   core0.init();
//   while (true) {
//     core0.update();
//   }
// }

void codeForTask1(void * parameter) {
  Core1 core1;
  core1.init();
  while (true) {
    core1.update();
  }
}
