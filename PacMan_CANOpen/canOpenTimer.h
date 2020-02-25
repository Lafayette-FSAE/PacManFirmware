/*  Wave Generator Example

    This example code is in the Public Domain (or CC0 licensed, at your option.)

    DAC output channel, waveform, wave frequency can be customized in menuconfig.
    If any questions about this example or more information is needed, please read README.md before your start.
*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/timer.h"

/*  The timer ISR has an execution time of 5.5 micro-seconds(us).
    Therefore, a timer period less than 5.5 us will cause trigger the interrupt watchdog.
    7 us is a safe interval that will not trigger the watchdog. No need to customize it.
*/

#define WITH_RELOAD            1
#define TIMER_INTR_US          7                                    // Execution time of each ISR interval in micro-seconds
#define TIMER_DIVIDER          16
#define CONST_PERIOD_2_PI      6.2832
#define SEC_TO_MICRO_SEC(x)    ((x) / 1000 / 1000)                  // Convert second to micro-second
#define UNUSED_PARAM           __attribute__((unused))              // A const period parameter which equals 2 * pai, used to calculate raw dac output value.
#define TIMER_TICKS            (TIMER_BASE_CLK / TIMER_DIVIDER)     // TIMER_BASE_CLK = APB_CLK = 80MHz
#define ALARM_VAL_US           SEC_TO_MICRO_SEC(TIMER_INTR_US * TIMER_TICKS)     // Alarm value in micro-seconds

/* Timer interrupt service routine */
static void IRAM_ATTR timer0_ISR(void *ptr)
{
    timer_group_clr_intr_status_in_isr(TIMER_GROUP_0, TIMER_0);
    timer_group_enable_alarm_in_isr(TIMER_GROUP_0, TIMER_0);
	(*ptr)(); // Call our CANInterrupt function that we passed in
}

/* Timer group0 TIMER_0 initialization */
static void example_timer_init(int timer_idx, bool auto_reload, void* CANInterruptFunctionPointer)
{
    esp_err_t ret;
    timer_config_t config = {
        .divider = TIMER_DIVIDER,
        .counter_dir = TIMER_COUNT_UP,
        .counter_en = TIMER_PAUSE,
        .alarm_en = TIMER_ALARM_EN,
        .intr_type = TIMER_INTR_LEVEL,
        .auto_reload = auto_reload,
    };

    ret = timer_init(TIMER_GROUP_0, timer_idx, &config);
    ESP_ERROR_CHECK(ret);
    ret = timer_set_counter_value(TIMER_GROUP_0, timer_idx, 0x00000000ULL);
    ESP_ERROR_CHECK(ret);
    ret = timer_set_alarm_value(TIMER_GROUP_0, timer_idx, ALARM_VAL_US);
    ESP_ERROR_CHECK(ret);
    ret = timer_enable_intr(TIMER_GROUP_0, TIMER_0);
    ESP_ERROR_CHECK(ret);
    /* Register an ISR handler */
    timer_isr_register(TIMER_GROUP_0, timer_idx, timer0_ISR, (void *)CANInterruptFunctionPointer, ESP_INTR_FLAG_IRAM, NULL);
}
