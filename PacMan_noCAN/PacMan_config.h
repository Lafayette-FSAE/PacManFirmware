/**
 * PacMan_config.h - The configuration file for PacMan which defines the ESP32 MCU pinout and threshold values
 * Author: Justin Kogasaka
 * Date: 2/19/2022
 */

#ifndef PacMan_config_h
#define PacMan_config_h

// Main files and libraries used by the ESP32
#include "Arduino.h"
#include <stdint.h>
#include <Wire.h>
#include "driver/timer.h"

// Define ESP32 pinout names
#define PIN_BTN_UP              34
#define PIN_BTN_DOWN            35
#define PIN_BTN_CENTER          32
#define PIN_BTN_RIGHT           33
#define PIN_CANTX               25
#define PIN_CANRX               26
#define PIN_IO_INT              27
#define PIN_DISP_CLK            14
#define PIN_SLOOP_EN            12
#define PIN_DISP_MOSI           13
#define PIN_WATCHDOG            23
#define PIN_DISP_DC             22
#define PIN_DISP_RST            21
#define PIN_SDA_GLV             19
#define PIN_SCL_GLV             18
#define PIN_BTN_LEFT            51
#define PIN_CHRG_DETECT         17
#define PIN_DISP_BUSY           16
#define PIN_CHRG_CURRENT        4
#define PIN_CHRG_EN             2
#define PIN_DISP_CS             15

// Define I2C addresses
#define I2C_ADDR_MCP9804        24
#define I2C_ADDR_MCP23008       32
#define I2C_ADDR_BQ32002        104
#define I2C_ADDR_LTC4151        111

#endif
