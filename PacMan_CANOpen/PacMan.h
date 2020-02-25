#ifndef PacMan_h
#define PacMan_h

// Define GPIO pins
#define PIN_CHRG_EN       2
#define PIN_CHRG_CURRENT  4
#define PIN_LED_GREEN     5
#define PIN_IO_INT        12
#define PIN_DISP_DIN      13
#define PIN_DISP_CLK      14
#define PIN_DISP_CS       15
#define PIN_DISP_BUSY     33
#define PIN_CHRG_DET      17
#define PIN_SCL           18
#define PIN_SDA           19
#define PIN_DISP_RST      21
#define PIN_DISP_DC       22
#define PIN_WATCHDOG      23
#define PIN_CAN_TX        25
#define PIN_CAN_RX        26
#define PIN_SLOOP_EN      27
#define PIN_BTN_CENTER    32
//#define PIN_LED_ORANGE    33
#define PIN_BTN_UP        34
#define PIN_BTN_DOWN      35
#define PIN_BTN_LEFT      36
#define PIN_BTN_RIGHT     39

// Known I2C Addresses
#define TEMPERATURE_SENSOR_ADDRESS 0x18
#define IO_EXPANDER_ADDRESS 0x20
#define REAL_TIME_CLOCK_ADDRESS 0x68
#define POWER_MONITOR_ADDRESS 0x6F







#endif
