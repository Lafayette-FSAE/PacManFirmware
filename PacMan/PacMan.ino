
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
    //core1.start();
    core1.startDemo();
  }
}




/*
 *    Jon's random stuff
 */


// Define GPIO pins
#define LED_GREEN 5
#define LED_ORANGE 33
#define SLOOP_EN 27
#define IO_INT 12
#define WATCHDOG 23
#define CHRG_DETECT 17
#define CHRG_EN 2

// Set GPIO modes
pinMode(LED_GREEN, OUTPUT);
pinMode(LED_ORANGE, OUTPUT);
pinMode(SLOOP_EN, OUTPUT);
pinMode(IO_INT, INPUT);
pinMode(CHRGDETECT_Pin, INPUT);
pinMode(CHRG_EN, OUTPUT);

// Set default values for GPIO
digitalWrite(LED_GREEN, LOW);
digitalWrite(LED_ORANGE, LOW);
digitalWrite(SLOOP_EN, HIGH);
digitalWrite(CHRG_EN, LOW);

// Map interrupts
attachInterrupt(digitalPinToInterrupt(CHRG_EN), chargeDetectLowInt, FALLING);
attachInterrupt(digitalPinToInterrupt(CHRG_EN), chargeDetectHighInt, RISING);
attachInterrupt(digitalPinToInterrupt(IO_INT), ioExpanderInt, RISING);

boolean charging = false;
boolean checkIOExpander = false;

// Handles when CHRG_DETECT falls, indicating the connector is plugged in
void IRAM_ATTR chargeDetectLowInt()
{
  volatile charging = true;
  digitalWrite(CHRG_EN, HIGH)
}

// Handles when CHRG_DETECT rises, indicating the connector is unplugged
void IRAM_ATTR chargeDetectHighInt()
{
  volatile charging = false;
  digitalWrite(CHRG_EN, LOW)
}

// Handles when IO_INT rises, indicating that there was a SLOOP-related change
void IRAM_ATTR ioExpanderInt()
{
  volatile checkIOExpander = true;
}

// Sets up the MCP23008 IO Expander
MCP23008 ioExpander;
ioExpander.configure(MCP23008_IODIR, 0xEF);
ioExpander.configure(MCP23008_IPOL, 0x00);
ioExpander.configure(MCP23008_GPINTEN, 0x0E);
ioExpander.configure(MCP23008_DEFVAL, 0x00);
ioExpander.configure(MCP23008_INTCON, 0x00);
ioExpander.configure(MCP23008_IOCON, 0x30);
ioExpander.configure(MCP23008_GPPU, 0x00);
ioExpander.configure(MCP23008_GPIO, 0x00);

// Handle a change in the SLOOP readings on the IO Expander
if (checkIOExpander)
{
  uint8_t gpio = ioExpander.readGPIO();
  // Update SLOOP variables
}
