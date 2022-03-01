/***************************************************
  This is our library for the Adafruit  ILI9341 Breakout and Shield
  ----> http://www.adafruit.com/products/1651
  Check out the links above for our tutorials and wiring diagrams
  These displays use SPI to communicate, 4 or 5 pins are required to
  interface (RST is optional)
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!
  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/

#ifndef _ADAFRUIT_ILI9341H_
#define _ADAFRUIT_ILI9341H_

//typedef short int16_t;
//typedef unsigned short uint16_t;
//typedef char int8_t;
//typedef unsigned char uint8_t;

#include <stdio.h>
#include <plib.h>
//#include <Adafruit_GFX.h>

#define _dc         LATBbits.LATB0
#define TRIS_dc     TRISBbits.TRISB0
#define _dc_high()  {LATBSET = 1;}
#define _dc_low()   {LATBCLR = 1;}

#define _cs         LATBbits.LATB1
#define TRIS_cs     TRISBbits.TRISB1
#define _cs_high()  {LATBSET = 2;}
#define _cs_low()   {LATBCLR = 2;}

#define _rst        LATBbits.LATB2
#define TRIS_rst    TRISBbits.TRISB2
#define _rst_high() {LATBSET = 4;}
#define _rst_low()  {LATBCLR = 4;}

#define ILI9341_TFTWIDTH  240
#define ILI9341_TFTHEIGHT 320

#define ILI9341_NOP     0x00
#define ILI9341_SWRESET 0x01
#define ILI9341_RDDID   0x04
#define ILI9341_RDDST   0x09

#define ILI9341_SLPIN   0x10
#define ILI9341_SLPOUT  0x11
#define ILI9341_PTLON   0x12
#define ILI9341_NORON   0x13

#define ILI9341_RDMODE  0x0A
#define ILI9341_RDMADCTL  0x0B
#define ILI9341_RDPIXFMT  0x0C
#define ILI9341_RDIMGFMT  0x0D
#define ILI9341_RDSELFDIAG  0x0F

#define ILI9341_INVOFF  0x20
#define ILI9341_INVON   0x21
#define ILI9341_GAMMASET 0x26
#define ILI9341_DISPOFF 0x28
#define ILI9341_DISPON  0x29

#define ILI9341_CASET   0x2A
#define ILI9341_PASET   0x2B
#define ILI9341_RAMWR   0x2C
#define ILI9341_RAMRD   0x2E

#define ILI9341_PTLAR   0x30
#define ILI9341_MADCTL  0x36
#define ILI9341_PIXFMT  0x3A

#define ILI9341_FRMCTR1 0xB1
#define ILI9341_FRMCTR2 0xB2
#define ILI9341_FRMCTR3 0xB3
#define ILI9341_INVCTR  0xB4
#define ILI9341_DFUNCTR 0xB6

#define ILI9341_PWCTR1  0xC0
#define ILI9341_PWCTR2  0xC1
#define ILI9341_PWCTR3  0xC2
#define ILI9341_PWCTR4  0xC3
#define ILI9341_PWCTR5  0xC4
#define ILI9341_VMCTR1  0xC5
#define ILI9341_VMCTR2  0xC7

#define ILI9341_RDID1   0xDA
#define ILI9341_RDID2   0xDB
#define ILI9341_RDID3   0xDC
#define ILI9341_RDID4   0xDD

#define ILI9341_GMCTRP1 0xE0
#define ILI9341_GMCTRN1 0xE1
/*
#define ILI9341_PWCTR6  0xFC
*/

// Color definitions
#define ILI9341_BLACK       0x0000      /*   0,   0,   0 */
#define ILI9341_NAVY        0x000F      /*   0,   0, 128 */
#define ILI9341_DARKGREEN   0x03E0      /*   0, 128,   0 */
#define ILI9341_DARKCYAN    0x03EF      /*   0, 128, 128 */
#define ILI9341_MAROON      0x7800      /* 128,   0,   0 */
#define ILI9341_PURPLE      0x780F      /* 128,   0, 128 */
#define ILI9341_OLIVE       0x7BE0      /* 128, 128,   0 */
#define ILI9341_LIGHTGREY   0xC618      /* 192, 192, 192 */
#define ILI9341_DARKGREY    0x7BEF      /* 128, 128, 128 */
#define ILI9341_BLUE        0x001F      /*   0,   0, 255 */
#define ILI9341_GREEN       0x07E0      /*   0, 255,   0 */
#define ILI9341_CYAN        0x07FF      /*   0, 255, 255 */
#define ILI9341_RED         0xF800      /* 255,   0,   0 */
#define ILI9341_MAGENTA     0xF81F      /* 255,   0, 255 */
#define ILI9341_YELLOW      0xFFE0      /* 255, 255,   0 */
#define ILI9341_WHITE       0xFFFF      /* 255, 255, 255 */
#define ILI9341_ORANGE      0xFD20      /* 255, 165,   0 */
#define ILI9341_GREENYELLOW 0xAFE5      /* 173, 255,  47 */
#define ILI9341_PINK        0xF81F

#define PBCLK 40000000 // peripheral bus clock
#define SPI_freq    20000000

#define tabspace 4 // number of spaces for a tab

#define dTime_ms PBCLK/2000
#define dTime_us PBCLK/2000000

void tft_init_hw(void);
void tft_spiwrite(unsigned char c);
void tft_spiwrite8(unsigned char c);
void tft_spiwrite16(unsigned short c);
void tft_writecommand(unsigned char c);
void tft_writecommand16(unsigned short c);
void tft_writedata(unsigned char c);
void tft_writedata16(unsigned short c);
void tft_commandList(unsigned char *addr);
void tft_begin(void);
void tft_setAddrWindow(unsigned short x0, unsigned short y0, unsigned short x1, unsigned short y1);
void tft_pushColor(unsigned short color);
void tft_drawPixel(short x, short y, unsigned short color);
void tft_drawFastVLine(short x, short y, short h, unsigned short color);
void tft_drawFastHLine(short x, short y, short w, unsigned short color);
void tft_fillScreen(unsigned short color);
void tft_fillRect(short x, short y, short w, short h, unsigned short color);
unsigned short tft_Color565(unsigned char r, unsigned char g, unsigned char b);
void tft_setRotation(unsigned char m);
unsigned char tft_spiread(void);
unsigned char tft_readdata(void);
unsigned char tft_readcommand8(unsigned char c);

unsigned short _width, _height;

void delay_ms(unsigned long);
void delay_us(unsigned long);

#endif
