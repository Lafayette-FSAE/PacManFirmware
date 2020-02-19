#include "Arduino.h"
#include "Wire.h"

#include "PacMan.h"
#include "I2C_Devices.h"

// Semaphores to allow detection of interrupts from the main loop
volatile SemaphoreHandle_t timerCellManSemaphore;
volatile SemaphoreHandle_t chargeDetectSemaphore;
volatile SemaphoreHandle_t ioExpanderSemaphore;

TaskHandle_t Task0, Task1;
hw_timer_t * timer0 = NULL;


// Flags that the CellMen should be polled
void IRAM_ATTR isrTimerCellMan() {
  xSemaphoreGiveFromISR(timerCellManSemaphore, NULL);
  // Serial.println("CellMan timer interrupt occured.");
}


// Flags that CHRG_DET has changed
void IRAM_ATTR chargeDetectInt()
{
  xSemaphoreGiveFromISR(chargeDetectSemaphore, NULL);
  // Serial.println("Charge detect interrupt occured.");
}


// Flags that there was an IO expander interrupt
void IRAM_ATTR ioExpanderInt()
{
  xSemaphoreGiveFromISR(ioExpanderSemaphore, NULL);
  // Serial.println("IO expander interrupt occured.");
}


// Configures the ESP32
void setup() {
  delay(1);
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

  // Create the interrupt semaphores
  timerCellManSemaphore = xSemaphoreCreateBinary();
  chargeDetectSemaphore = xSemaphoreCreateBinary();
  ioExpanderSemaphore   = xSemaphoreCreateBinary();

  // Map interrupts
  attachInterrupt(digitalPinToInterrupt(PIN_CHRG_DET), chargeDetectInt, CHANGE);
  attachInterrupt(digitalPinToInterrupt(PIN_IO_INT), ioExpanderInt, RISING);

  // Configure timer for polling of the CellMen
  timer0 = timerBegin(0, 80, true);
  timerAttachInterrupt(timer0, &isrTimerCellMan, true);
  timerAlarmWrite(timer0, 1000000, true);

  Wire.begin(PIN_SDA, PIN_SCL);
  MCP23008_setup();
  // LTC4151_setup();

  // xTaskCreatePinnedToCore(&codeForTask0, "Core0Task", 10000, NULL, 1, &Task0, 0);
  // xTaskCreatePinnedToCore(&codeForTask1, "Core1Task", 10000, NULL, 1, &Task1, 1);

  // Enable timers
  timerAlarmEnable(timer0);
}

void loop() {
  if (xSemaphoreTake(timerCellManSemaphore, 0) == pdTRUE) {
  }

  if (xSemaphoreTake(chargeDetectSemaphore, 0) == pdTRUE) {
  }

  if (xSemaphoreTake(ioExpanderSemaphore, 0) == pdTRUE) {
  }
  Serial.print("Voltage: ");
  Serial.print(LTC4151_getVin(), 3);
  Serial.println(" V");
  Serial.print("Current: ");
  Serial.print(LTC4151_getCurrent(), 3);
  Serial.println(" A");
  delay(2000);
}

// void codeForTask0(void * parameter) {
//   Core0 core0;
//   core0.init();
//   while (true) {
//     core0.update();
//   }
// }

// void codeForTask1(void * parameter) {
//   Core1 core1;
//   core1.init();
//   while (true) {
//     core1.update();
//   }
// }
