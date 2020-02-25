#ifndef PacMan_h
#define PacMan_h

// Define GPIO pins
#define PIN_CHRG_EN         	2
#define PIN_CHRG_CURRENT    	4
#define PIN_LED_GREEN       	5
#define PIN_SLOOP_EN        	12
#define PIN_DISP_DIN        	13
#define PIN_DISP_CLK        	14
#define PIN_DISP_CS         	15
#define PIN_DISP_BUSY       	16
#define PIN_CHRG_DET        	17
#define PIN_SCL             	18
#define PIN_SDA             	19
#define PIN_DISP_RST        	21
#define PIN_DISP_DC         	22
#define PIN_WATCHDOG        	23
#define PIN_CAN_TX          	25
#define PIN_CAN_RX          	26
#define PIN_IO_INT          	27
#define PIN_BTN_CENTER      	32
#define PIN_LED_ORANGE      	33
#define PIN_BTN_UP          	34
#define PIN_BTN_DOWN        	35
#define PIN_BTN_LEFT        	36
#define PIN_BTN_RIGHT       	39

// Define I2C addresses
#define I2C_ADDR_MCP23008   	32
#define I2C_ADDR_MCP9804    	24
#define I2C_ADDR_BQ32002    	104
#define I2C_ADDR_LTC4151    	111

// MCP23008 register defines
#define MCP23008_IODIR      	0x00
#define MCP23008_IPOL       	0x01
#define MCP23008_GPINTEN    	0x02
#define MCP23008_DEFVAL     	0x03
#define MCP23008_INTCON     	0x04
#define MCP23008_IOCON      	0x05
#define MCP23008_GPPU       	0x06
#define MCP23008_INTF       	0x07
#define MCP23008_INTCAP     	0x08
#define MCP23008_GPIO       	0x09
#define MCP23008_OLAT       	0x0A

// LTC4151 register defines
#define LTC4151_SENSE_H     	0x00
#define LTC4151_SENSE_L     	0x01
#define LTC4151_VIN_H       	0x02
#define LTC4151_VIN_L       	0x03
#define LTC4151_ADIN_H      	0x04
#define LTC4151_ADIN_L      	0x05
#define LTC4151_CONTROL     	0x06

// LTC4151 current shunt resistor value
#define LTC4151_RSHUNT      	0.82

// LTC4151 multipliers
#define LTC4151_VIN_FACTOR  	0.025
#define LTC4151_CURR_FACTOR_A 	0.00002439
#define LTC4151_CURR_FACTOR_mA 	0.02439
#define LTC4151_AD_FACTOR   	0.0005

#endif