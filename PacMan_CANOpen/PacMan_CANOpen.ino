#include "CANopen.h"
#include "driver/timer.h"
#include <EEPROM.h>
#include "Core1.h"
#include "Core0.h"
// include library to read and write from flash memory
#define EEPROM_SIZE 100

#define TMR_TASK_INTERVAL   (1000)          /* Interval of tmrTask thread in microseconds */
#define INCREMENT_1MS(var)  (var++)         /* Increment 1ms variable in tmrTask */
#define TIMER_INTR_SEL TIMER_INTR_LEVEL  /*!< Timer level interrupt */
#define TIMER_DIVIDER   80               /*!< Hardware timer clock divider, 80 to get 1MHz clock to timer */
#define TIMER_SCALE    (TIMER_BASE_CLK / TIMER_DIVIDER)  /*!< used to calculate counter value BASE CLK is 80MHz */
#define TIMER_FINE_ADJ   (0*(TIMER_BASE_CLK / TIMER_DIVIDER)/1000000) /*!< used to compensate alarm value */
#define TIMER_INTERVAL0_SEC   (0.001)    /*!< test interval for timer 0 CANopen - 1ms */
#define TIMER_INTERVAL1_SEC   (0.1)      /*!< test interval for timer 1 I2C - 100ms */
/**
User-defined CAN base structure, passed as argument to CO_init.
*/
struct CANbase {
    uintptr_t baseAddress;  /**< Base address of the CAN module */
}; 

/* Global variables and objects */
volatile uint16_t   CO_timer1ms = 0U;   /* variable increments each millisecond */
volatile SemaphoreHandle_t* I2C_InterrupterSemaphore;
volatile SemaphoreHandle_t* chargeDetectSemaphore;

CO_NMT_reset_cmd_t reset;
uint16_t timer1msPrevious;
TaskHandle_t Task0;
TaskHandle_t Task1;
TaskHandle_t tmrTask_thread1;

static bool test_set_bits;
static bool test_clear_bits;

/* CAN interrupt function *****************************************************/
// TODO: Test if this is 1ms or not
void CO_CAN0InterruptHandler(void *para) {
    Serial.println("Interrupt Called!");
    CO_CANinterrupt(CO->CANmodule[0]);
}

volatile int cnt = 0;
void IRAM_ATTR timer_group0_isr(void *para) { // timer group 0, ISR
    int timer_idx = (int) para;
    uint32_t intr_status = TIMERG0.int_st_timers.val;
    if ((intr_status & BIT(timer_idx)) && timer_idx == TIMER_0) {
        TIMERG0.hw_timer[timer_idx].update = 1;
        TIMERG0.int_clr_timers.t0 = 1;
        TIMERG0.hw_timer[timer_idx].config.alarm_en = 1;
        cnt++;
    }
}

void IRAM_ATTR I2C_Interrupter(void *para){
    int timer_idx = (int) para;
    uint32_t intr_status = TIMERG0.int_st_timers.val;
    if ((intr_status & BIT(timer_idx)) && timer_idx == TIMER_1) {
        TIMERG0.hw_timer[timer_idx].update = 1;
        TIMERG0.int_clr_timers.t1 = 1;
        TIMERG0.hw_timer[timer_idx].config.alarm_en = 1;
        cnt++;
    }
    xSemaphoreGiveFromISR(*I2C_InterrupterSemaphore, NULL);
}

void IRAM_ATTR chargeDetectInt()
{
    xSemaphoreGiveFromISR(*chargeDetectSemaphore, NULL);
}

void setup_I2C_timer(void){
    timer_group_t timer_group = TIMER_GROUP_0;
    timer_idx_t timer_idx = TIMER_1;
    timer_config_t config;
    config.alarm_en = 1;
    config.auto_reload = 1;
    config.counter_dir = TIMER_COUNT_UP;
    config.divider = TIMER_DIVIDER;
    config.intr_type = TIMER_INTR_SEL;
    config.counter_en = TIMER_PAUSE;
    /*Configure timer*/
    timer_init(timer_group, timer_idx, &config);
    /*Stop timer counter*/
    timer_pause(timer_group, timer_idx);
    /*Load counter value */
    timer_set_counter_value(timer_group, timer_idx, 0x00000000ULL);
    /*Set alarm value*/
    timer_set_alarm_value(timer_group, timer_idx, (TIMER_INTERVAL1_SEC * TIMER_SCALE) - TIMER_FINE_ADJ);
    /*Enable timer interrupt*/
    timer_enable_intr(timer_group, timer_idx);
    /*Set ISR handler*/
    timer_isr_register(timer_group, timer_idx, I2C_Interrupter, (void*) timer_idx, ESP_INTR_FLAG_IRAM, NULL);
    /*Start timer counter*/
    timer_start(timer_group, timer_idx);
}

// 1ms
static void setup_timer(void){
    timer_group_t timer_group = TIMER_GROUP_0;
    timer_idx_t timer_idx = TIMER_0;
    timer_config_t config;
    config.alarm_en = 1;
    config.auto_reload = 1;
    config.counter_dir = TIMER_COUNT_UP;
    config.divider = TIMER_DIVIDER;
    config.intr_type = TIMER_INTR_SEL;
    config.counter_en = TIMER_PAUSE;
    /*Configure timer*/
    timer_init(timer_group, timer_idx, &config);
    /*Stop timer counter*/
    timer_pause(timer_group, timer_idx);
    /*Load counter value */
    timer_set_counter_value(timer_group, timer_idx, 0x00000000ULL);
    /*Set alarm value*/
    timer_set_alarm_value(timer_group, timer_idx, (TIMER_INTERVAL0_SEC * TIMER_SCALE) - TIMER_FINE_ADJ);
    /*Enable timer interrupt*/
    timer_enable_intr(timer_group, timer_idx);
    /*Set ISR handler*/
    timer_isr_register(timer_group, timer_idx, timer_group0_isr, (void*) timer_idx, ESP_INTR_FLAG_IRAM, NULL);
    /*Start timer counter*/
    timer_start(timer_group, timer_idx);
}

void setup() {
    Serial.begin(115200);
    reset = CO_RESET_NOT;

    pinMode(PIN_LED_GREEN,  OUTPUT);
    pinMode(PIN_LED_ORANGE, OUTPUT);
    pinMode(PIN_SLOOP_EN,   OUTPUT);
    pinMode(PIN_CHRG_EN,    OUTPUT);
    pinMode(PIN_WATCHDOG,   OUTPUT);


    // Set default values for GPIO
    digitalWrite(PIN_LED_GREEN,  LOW);
    digitalWrite(PIN_LED_ORANGE, LOW);
    digitalWrite(PIN_SLOOP_EN,   HIGH);
    OD_SLOOP_Relay = true;
    
    digitalWrite(PIN_CHRG_EN,    LOW);
    digitalWrite(PIN_WATCHDOG,   LOW);

    digitalWrite(PIN_LED_ORANGE,  HIGH);

    /* Configure microcontroller. */
    xTaskCreatePinnedToCore(&codeForTask0,"Core0Task",10000,NULL,1,&Task0,0);
    delay(500);
    xTaskCreatePinnedToCore(&codeForTask1,"Core1Task",10000,NULL,1,&Task1,0);

    /* initialize EEPROM */
    // initialize EEPROM with predefined size
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
        err = CO_init(&canBase, 10/* NodeID */, 125 /* bit rate */);
        if (err != CO_ERROR_NO) {
            while (1);
            /* CO_errorReport(CO->em, CO_EM_MEMORY_ALLOCATION_ERROR, CO_EMC_SOFTWARE_INTERNAL, err); */
        }


        /* Configure Timer interrupt function for execution every 1 millisecond */
        //setup_timer();
        setup_I2C_timer();
        Serial.println("Setup timers");

        // Setup interrupts
        attachInterrupt(digitalPinToInterrupt(PIN_CHRG_DET),  chargeDetectInt, CHANGE);

        /* Configure CAN transmit and receive interrupt */
        Serial.println("Before start can");
        /* start CAN */
        CO_CANsetNormalMode(CO->CANmodule[0]);

        reset = CO_RESET_NOT;
        timer1msPrevious = CO_timer1ms;

        xTaskCreatePinnedToCore(&tmrTask_thread,"tmrTask_thread",10000,NULL,1,&tmrTask_thread1,0);

        //Serial.println((int)(timererr==ESP_OK));
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
    }
}

void codeForTask0( void * parameter )
{
    Core0 core0;
    for(;;){
        Serial.println("task0");
        core0.startCore0();
    }
}

void codeForTask1( void * parameter ){
    Core1 core1=Core1((CO_t*)parameter);
    I2C_InterrupterSemaphore = &core1.I2C_InterrupterSemaphore;
    chargeDetectSemaphore = &core1.chargeDetectSemaphore;
    for(;;){
        Serial.println("task1");
        delay(250);
        core1.start();
    }
}




void loop() {
    delay(500);
}

/* timer thread executes in constant intervals ********************************/
static void tmrTask_thread(void * parameter ) {

    for (;;) {

        /* sleep for interval */

        INCREMENT_1MS(CO_timer1ms);

        if (CO->CANmodule[0]->CANnormal) {
            bool_t syncWas;

            CO_CANinterrupt(CO->CANmodule[0]);

            /* Process Sync */
            syncWas = CO_process_SYNC(CO, TMR_TASK_INTERVAL);

            /* Read inputs */
            CO_process_RPDO(CO, syncWas);

            /* Further I/O or nonblocking application code may go here. */

            /* Write outputs */
            CO_process_TPDO(CO, syncWas, TMR_TASK_INTERVAL);

            /* verify timer overflow */
            if (0) {
                CO_errorReport(CO->em, CO_EM_ISR_TIMER_OVERFLOW, CO_EMC_SOFTWARE_INTERNAL, 0U);
            }
        }
        delay(1);
    }
}
