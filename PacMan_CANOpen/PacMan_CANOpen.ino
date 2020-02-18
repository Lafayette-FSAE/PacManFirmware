#include "CANopen.h"
#include "CO_OD.h"
#include <EEPROM.h> // include library to read and write from flash memory
#include "driver/timer.h"
#include "Core1.h"
#include "Core0.h"
//#include "PacMan.h" // Maybe not needed since within Core1/0

#define EEPROM_SIZE 100
#define TMR_TASK_INTERVAL   (1000)          /* Interval of tmrTask thread in microseconds */
#define INCREMENT_1MS(var)  (var++)         /* Increment 1ms variable in tmrTask */

/* Global variables and objects */
volatile uint16_t   CO_timer1ms = 0U;   /* variable increments each millisecond */
CO_NMT_reset_cmd_t reset;
uint16_t timer1msPrevious;
TaskHandle_t Task0, Task1;


// User-defined CAN base structure, passed as argument to CO_init.
struct CANbase {
  uintptr_t baseAddress;  /**< Base address of the CAN module */
};

/* CAN interrupt function *****************************************************/
void IRAM_ATTR CO_CAN0InterruptHandler() {
  Serial.println("Interrupt Called!");
  CO_CANinterrupt(CO->CANmodule[0]);
}

// Configure timer 0 to increment every 1us and throw an interrupt every 1ms
void setup_timer0() {
  timer0 = timerBegin(0, 80, true);
  timerAttachInterrupt(timer0, &CO_CAN0InterruptHandler, true);
  timerAlarmWrite(timer0, 1000, true);
  timerAlarmEnable(timer0);
}

// void IRAM_ATTR onTimer1() {
//     Serial.println("Timer 1 interrupt occured.");
// }

// Configure timer 1 to increment every 1us and throw an interrupt every 1s
// void setup_timer1() {
//   timer1 = timerBegin(1, 80, true);
//   timerAttachInterrupt(timer1, &onTimer1, true);
//   timerAlarmWrite(timer1, 1000000, true);
//   timerAlarmEnable(timer1);
// }

void setup() {
  /// SETUP BEGIN ///
  Serial.begin(115200); // Begin Arduino serial connection
  reset = CO_RESET_NOT;
  /* initialize EEPROM with predefined size */
  EEPROM.begin(EEPROM_SIZE);
  /* increase variable each startup. Variable is stored in EEPROM. */
  OD_powerOnCounter++;


  while (reset != CO_RESET_APP) {
    /* CANopen communication reset - initialize CANopen objects *******************/
    CO_ReturnError_t err;

    /* disable CAN and CAN interrupts */
    struct CANbase canBase = {
      .baseAddress = 0u,  /* CAN module address */
    };

    /* initialize CANopen */
    err = CO_init(&canBase, 10 /* NodeID */, 125 /* bit rate */);
    if (err != CO_ERROR_NO) {
      while (1);
      /* CO_errorReport(CO->em, CO_EM_MEMORY_ALLOCATION_ERROR, CO_EMC_SOFTWARE_INTERNAL, err); */
    }

    /* Configure Timer interrupt function for execution every 1 millisecond */
    setup_timer0();
    Serial.println("CAN Timer is now setup!"");


    /* start CAN */
    CO_CANsetNormalMode(CO->CANmodule[0]);

    reset = CO_RESET_NOT;
    timer1msPrevious = CO_timer1ms;

    xTaskCreatePinnedToCore(&codeForTask0, "Core0Task", 10000, NULL, 1, &Task0, 0);
    delay(500);
    xTaskCreatePinnedToCore(&codeForTask1, "Core1Task", 10000, NULL, 1, &Task1, 1);

    /// SETUP END ///


    /// MAIN LOOP BEGIN ///
    while (reset == CO_RESET_NOT) {
      /* loop for normal program execution ******************************************/
      uint16_t timer1msCopy, timer1msDiff;

      timer1msCopy = CO_timer1ms;
      timer1msDiff = timer1msCopy - timer1msPrevious;
      timer1msPrevious = timer1msCopy;

      /* CANopen process */
      reset = CO_process(CO, timer1msDiff, NULL);

      /* Nonblocking application code may go here. */

      /* Process EEPROM */
    }
    /// MAIN LOOP END ///
  }
}

void loop() {
  Serial.println("Yeet");
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



// /* timer thread executes in constant intervals ********************************/
// static void tmrTask_thread(void) {
//
//   for (;;) {
//
//     /* sleep for interval */
//
//     INCREMENT_1MS(CO_timer1ms);
//
//     if (CO->CANmodule[0]->CANnormal) {
//       bool_t syncWas;
//
//       /* Process Sync */
//       syncWas = CO_process_SYNC(CO, TMR_TASK_INTERVAL);
//
//       /* Read inputs */
//       CO_process_RPDO(CO, syncWas);
//
//       /* Further I/O or nonblocking application code may go here. */
//
//       /* Write outputs */
//       CO_process_TPDO(CO, syncWas, TMR_TASK_INTERVAL);
//
//       /* verify timer overflow */
//       if (0) {
//         CO_errorReport(CO->em, CO_EM_ISR_TIMER_OVERFLOW, CO_EMC_SOFTWARE_INTERNAL, 0U);
//       }
//     }
//   }
// }
