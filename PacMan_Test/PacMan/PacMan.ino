#include "I2C_Devices.h"

// Semaphores to allow detection of interrupts from the main loop
volatile SemaphoreHandle_t timerCellManSemaphore;
volatile SemaphoreHandle_t chargeDetectSemaphore;
volatile SemaphoreHandle_t ioExpanderSemaphore;
volatile SemaphoreHandle_t CButtonSemaphore;
volatile SemaphoreHandle_t LButtonSemaphore;
volatile SemaphoreHandle_t RButtonSemaphore;
volatile SemaphoreHandle_t UButtonSemaphore;
volatile SemaphoreHandle_t DButtonSemaphore;

TaskHandle_t Task0, Task1;
hw_timer_t * timer0 = NULL;


void IRAM_ATTR isrTimerCellMan()
{
	xSemaphoreGiveFromISR(timerCellManSemaphore, NULL);
}

void IRAM_ATTR chargeDetectInt()
{
	xSemaphoreGiveFromISR(chargeDetectSemaphore, NULL);
}

void IRAM_ATTR ioExpanderInt()
{
	xSemaphoreGiveFromISR(ioExpanderSemaphore, NULL);
}

void IRAM_ATTR CBUTTON()
{
	xSemaphoreGiveFromISR(CButtonSemaphore, NULL);
}

void IRAM_ATTR LBUTTON()
{
	xSemaphoreGiveFromISR(LButtonSemaphore, NULL);
}

void IRAM_ATTR RBUTTON()
{
	xSemaphoreGiveFromISR(RButtonSemaphore, NULL);
}

void IRAM_ATTR UBUTTON()
{
	xSemaphoreGiveFromISR(UButtonSemaphore, NULL);
}

void IRAM_ATTR DBUTTON()
{
	xSemaphoreGiveFromISR(DButtonSemaphore, NULL);
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
	CButtonSemaphore      = xSemaphoreCreateBinary();
	LButtonSemaphore      = xSemaphoreCreateBinary();
	RButtonSemaphore      = xSemaphoreCreateBinary();
	UButtonSemaphore      = xSemaphoreCreateBinary();
	DButtonSemaphore      = xSemaphoreCreateBinary();

	// Map interrupts
	attachInterrupt(digitalPinToInterrupt(PIN_CHRG_DET),  chargeDetectInt, CHANGE);
	attachInterrupt(digitalPinToInterrupt(PIN_IO_INT),    ioExpanderInt,   RISING);
	attachInterrupt(digitalPinToInterrupt(PIN_BTN_UP),    CBUTTON,         FALLING);
	attachInterrupt(digitalPinToInterrupt(PIN_BTN_LEFT),  CBUTTON,         FALLING);
	attachInterrupt(digitalPinToInterrupt(PIN_BTN_RIGHT), CBUTTON,         FALLING);
	attachInterrupt(digitalPinToInterrupt(PIN_BTN_UP),    CBUTTON,         FALLING);
	attachInterrupt(digitalPinToInterrupt(PIN_BTN_DOWN),  CBUTTON,         FALLING);

	// Configure timer for polling of the CellMen
	timer0 = timerBegin(0, 80, true);
	timerAttachInterrupt(timer0, &isrTimerCellMan, true);
	timerAlarmWrite(timer0, 1000000, true);

	Wire.begin(PIN_SDA, PIN_SCL);
	MCP23008_setup();
	LTC4151_setup();

	// xTaskCreatePinnedToCore(&codeForTask0, "Core0Task", 10000, NULL, 1, &Task0, 0);
	// xTaskCreatePinnedToCore(&codeForTask1, "Core1Task", 10000, NULL, 1, &Task1, 1);

	// Enable timers
	timerAlarmEnable(timer0);
}

void loop() {
	if (xSemaphoreTake(timerCellManSemaphore, 0) == pdTRUE) {
		LTC4151_update();
		Serial.print("Voltage: ");
		Serial.print(LTC4151_getVoltage(), 1);
		Serial.println(" V");
		Serial.print("Current: ");
		Serial.print(LTC4151_getCurrentmA(), 3);
		Serial.println(" mA");
		Serial.print("Diff voltage: ");
		Serial.print(LTC4151_getDiff(), 3);
		Serial.println(" V");
	}

	if (xSemaphoreTake(chargeDetectSemaphore, 0) == pdTRUE) {
	}

	if (xSemaphoreTake(ioExpanderSemaphore, 0) == pdTRUE) {
		Serial.print("IO: ");
		Serial.println(MCP23008_readGPIO());
	}

	if (xSemaphoreTake(CButtonSemaphore, 0) == pdTRUE) {
		if (digitalRead(PIN_LED_GREEN)) {
			digitalWrite(PIN_LED_GREEN, LOW);
		}
		else {
			digitalWrite(PIN_LED_GREEN, HIGH);
		}
	}

	if (xSemaphoreTake(LButtonSemaphore, 0) == pdTRUE) {
		if (digitalRead(PIN_LED_GREEN)) {
			digitalWrite(PIN_LED_GREEN, LOW);
		}
		else {
			digitalWrite(PIN_LED_GREEN, HIGH);
		}
	}

	if (xSemaphoreTake(RButtonSemaphore, 0) == pdTRUE) {
		if (digitalRead(PIN_LED_GREEN)) {
			digitalWrite(PIN_LED_GREEN, LOW);
		}
		else {
			digitalWrite(PIN_LED_GREEN, HIGH);
		}
	}

	if (xSemaphoreTake(UButtonSemaphore, 0) == pdTRUE) {
		if (digitalRead(PIN_LED_GREEN)) {
			digitalWrite(PIN_LED_GREEN, LOW);
		}
		else {
			digitalWrite(PIN_LED_GREEN, HIGH);
		}
	}

	if (xSemaphoreTake(DButtonSemaphore, 0) == pdTRUE) {
		if (digitalRead(PIN_LED_GREEN)) {
			digitalWrite(PIN_LED_GREEN, LOW);
		}
		else {
			digitalWrite(PIN_LED_GREEN, HIGH);
		}
	}
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
