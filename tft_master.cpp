/***************************************************
  This is our library for the Adafruit ILI9341 Breakout and Shield
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
#define _SUPPRESS_PLIB_WARNING
#include <plib.h>
#include <xc.h>
#include "tft_master.h"

inline void Mode16(void){  // configure SPI1 for 16-bit mode
    SPI1CONSET = 0x400;
}

inline void Mode8(void){  // configure SPI1 for 8-bit mode
    SPI1CONCLR = 0x400;
}

void tft_init_hw(void) {
  _width = ILI9341_TFTWIDTH;
  _height = ILI9341_TFTHEIGHT;
  RPB11R = 3;               // SDO pin for SPI - goes to MOSI on TFT
  //SDI1R = 0; // RA1       // I won't be reading from TFT
}

void tft_spiwrite(unsigned char c) {

    while (TxBufFullSPI1());
    WriteSPI1(c);
    while (SPI1STATbits.SPIBUSY); // wait for it to end of transaction

}

void tft_spiwrite8(unsigned char c) {   // Transfer one byte c to SPI
 /* The default mode for me is to transfer 16-bits at once
  * However, it is necessary sometimes to transfer only 8-bits at a time
  * But this is required less often than 16-bits at once
  * So, the default mode is 16-bit mode and is switched to 8-bit mode when
  *     required, and then switched back at the end of the function
  */
    Mode8(); // switch to 8-bit mode
    while (TxBufFullSPI1());
    WriteSPI1(c);
    while (SPI1STATbits.SPIBUSY); // wait for it to end of transaction
    Mode16(); // switch back to 16-bit mode
}

void tft_spiwrite16(unsigned short c){  // Transfer two bytes "c" to SPI
    while (TxBufFullSPI1());
    WriteSPI1(c);
    while (SPI1STATbits.SPIBUSY); // wait for it to end of transaction
}


void tft_writecommand(unsigned char c) {
    _dc_low();
    _cs_low();

    tft_spiwrite8(c);

    _cs_high();

}

void tft_writecommand16(unsigned short c) {
    _dc_low();
    _cs_low();

    tft_spiwrite16(c);

    _cs_high();

}


void tft_writedata(unsigned char c) {
    _dc_high();
    _cs_low();

    tft_spiwrite8(c);

    _cs_high();

}

void tft_writedata16(unsigned short c) {
    _dc_high();
    _cs_low();

    tft_spiwrite16(c);

    _cs_high();

}

// Rather than a bazillion writecommand() and writedata() calls, screen
// initialization commands and arguments are organized in these tables
// stored in PROGMEM.  The table may look bulky, but that's mostly the
// formatting -- storage-wise this is hundreds of bytes more compact
// than the equivalent code.  Companion function follows.
#define DELAY 0x80

void tft_begin(void) {

  TRIS_rst = 0;
  _rst_low();
  TRIS_dc = 0;
  TRIS_cs = 0;

  _dc_low();
  _cs_high();

    SpiChnOpen(1, SPI_OPEN_MSTEN | SPI_OPEN_MODE8 | SPI_OPEN_ON |
        SPI_OPEN_DISSDI | SPI_OPEN_CKE_REV , PBCLK/SPI_freq);

    // Start with 8-bit mode for initialization - move to 16-bit mode once
    // that's done

  _rst_high();
  delay_ms(5);
  _rst_low();
  delay_ms(20);
  _rst_high();
  delay_ms(150);

  tft_writecommand(0xEF);
  tft_writedata(0x03);
  tft_writedata(0x80);
  tft_writedata(0x02);

  tft_writecommand(0xCF);  
  tft_writedata(0x00); 
  tft_writedata(0XC1); 
  tft_writedata(0X30); 

  tft_writecommand(0xED);  
  tft_writedata(0x64); 
  tft_writedata(0x03); 
  tft_writedata(0X12); 
  tft_writedata(0X81); 
 
  tft_writecommand(0xE8);  
  tft_writedata(0x85); 
  tft_writedata(0x00); 
  tft_writedata(0x78); 

  tft_writecommand(0xCB);  
  tft_writedata(0x39); 
  tft_writedata(0x2C); 
  tft_writedata(0x00); 
  tft_writedata(0x34); 
  tft_writedata(0x02); 
 
  tft_writecommand(0xF7);  
  tft_writedata(0x20); 

  tft_writecommand(0xEA);  
  tft_writedata(0x00); 
  tft_writedata(0x00); 
 
  tft_writecommand(ILI9341_PWCTR1);    //Power control 
  tft_writedata(0x23);   //VRH[5:0] 
 
  tft_writecommand(ILI9341_PWCTR2);    //Power control 
  tft_writedata(0x10);   //SAP[2:0];BT[3:0] 
 
  tft_writecommand(ILI9341_VMCTR1);    //VCM control 
  tft_writedata(0x3e); 
  tft_writedata(0x28); 
  
  tft_writecommand(ILI9341_VMCTR2);    //VCM control2 
  tft_writedata(0x86);  //--
 
  tft_writecommand(ILI9341_MADCTL);    // Memory Access Control 
  tft_writedata(0x48);

  tft_writecommand(ILI9341_PIXFMT);    
  tft_writedata(0x55); 
  
  tft_writecommand(ILI9341_FRMCTR1);    
  tft_writedata(0x00);  
  tft_writedata(0x18); 
 
  tft_writecommand(ILI9341_DFUNCTR);    // Display Function Control 
  tft_writedata(0x08); 
  tft_writedata(0x82);
  tft_writedata(0x27);  
 
  tft_writecommand(0xF2);    // 3Gamma Function Disable 
  tft_writedata(0x00); 
 
  tft_writecommand(ILI9341_GAMMASET);    //Gamma curve selected 
  tft_writedata(0x01); 
 
  tft_writecommand(ILI9341_GMCTRP1);    //Set Gamma 
  tft_writedata(0x0F); 
  tft_writedata(0x31); 
  tft_writedata(0x2B); 
  tft_writedata(0x0C); 
  tft_writedata(0x0E); 
  tft_writedata(0x08); 
  tft_writedata(0x4E); 
  tft_writedata(0xF1); 
  tft_writedata(0x37); 
  tft_writedata(0x07); 
  tft_writedata(0x10); 
  tft_writedata(0x03); 
  tft_writedata(0x0E); 
  tft_writedata(0x09); 
  tft_writedata(0x00); 
  
  tft_writecommand(ILI9341_GMCTRN1);    //Set Gamma 
  tft_writedata(0x00); 
  tft_writedata(0x0E); 
  tft_writedata(0x14); 
  tft_writedata(0x03); 
  tft_writedata(0x11); 
  tft_writedata(0x07); 
  tft_writedata(0x31); 
  tft_writedata(0xC1); 
  tft_writedata(0x48); 
  tft_writedata(0x08); 
  tft_writedata(0x0F); 
  tft_writedata(0x0C); 
  tft_writedata(0x31); 
  tft_writedata(0x36); 
  tft_writedata(0x0F); 

  tft_writecommand(ILI9341_SLPOUT);    //Exit Sleep
  delay_ms(120);
  tft_writecommand(ILI9341_DISPON);    //Display on

  // Now move to 16-bit mode to speed things up for display
  Mode16();

}


void tft_setAddrWindow(unsigned short x0, unsigned short y0, unsigned short x1,
 unsigned short y1) {

  tft_writecommand(ILI9341_CASET); // Column addr set
  tft_writedata16(x0);
  tft_writedata16(x1);

  tft_writecommand(ILI9341_PASET); // Row addr set
  tft_writedata16(y0);
  tft_writedata16(y1);

  tft_writecommand(ILI9341_RAMWR); // write to RAM
}


void tft_pushColor(unsigned short color) {
  _dc_high();
  _cs_low();

  tft_spiwrite16(color);

  _cs_high();
}

#define NOP asm("nop");
#define wait16 NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;
#define wait8 NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;
#define wait12 NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;
#define wait15 NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;

/* Draw a pixel at location (x,y) with given color
 * Parameters:
 *      x:  x-coordinate of pixel to draw; top left of screen is x=0
 *              and x increases to the right
 *      y:  y-coordinate of pixel to draw; top left of screen is y=0
 *              and y increases to the bottom
 *      color:  16-bit color value
 * Returns:     Nothing
 */

void tft_drawPixel(short x, short y, unsigned short color) {

  if((x < 0) ||(x >= _width) || (y < 0) || (y >= _height)) return;
/*
  tft_setAddrWindow(x,y,x+1,y+1);
  _dc_high();
  _cs_low();
  tft_spiwrite16(color);
  _cs_high();
*/
  //tft_writecommand(ILI9341_CASET); // Column addr set
   _dc_low();
    _cs_low();
    //tft_spiwrite8(ILI9341_CASET);
    Mode8(); // switch to 8-bit mode
    while (TxBufFullSPI1());
    WriteSPI1(ILI9341_CASET);
    //while (SPI1STATbits.SPIBUSY); // wait for it to end of transaction
    wait16;
    Mode16(); // switch back to 16-bit mode
    _cs_high();

  //tft_writedata16(x);
   _dc_high();
    _cs_low();
    //tft_spiwrite16(c);
   // while (TxBufFullSPI1());
    WriteSPI1(x);
    //while (SPI1STATbits.SPIBUSY);
    wait16;wait16;wait8;
    _cs_high();

    
  //tft_writedata16(x+1);
  // _dc_high();
    _cs_low();
    //tft_spiwrite16(c);
    //while (TxBufFullSPI1());
    WriteSPI1(x+1);
    //while (SPI1STATbits.SPIBUSY);
    wait16;wait16;wait8;
    _cs_high();

  //t_writecommand(ILI9341_PASET); // Row addr set
  _dc_low();
    _cs_low();
    //tft_spiwrite8(ILI9341_PASET);
    Mode8(); // switch to 8-bit mode
    //while (TxBufFullSPI1());
    WriteSPI1(ILI9341_PASET);
    //while (SPI1STATbits.SPIBUSY); // wait for it to end of transaction
    wait16;wait8;
    Mode16(); // switch back to 16-bit mode
    _cs_high();

  //tft_writedata16(y);
   _dc_high();
    _cs_low();
    //tft_spiwrite16(c);
   // while (TxBufFullSPI1());
    WriteSPI1(y);
   // while (SPI1STATbits.SPIBUSY);
    wait16;wait16;wait8;
    _cs_high();

    
  //tft_writedata16(y+1);
   //_dc_high();
    _cs_low();
    //tft_spiwrite16(c);
    //while (TxBufFullSPI1());
    WriteSPI1(y+1);
    //while (SPI1STATbits.SPIBUSY);
    wait16;wait16;wait8;
    _cs_high();

  //tft_writecommand(ILI9341_RAMWR); // write to RAM
    _dc_low();
    _cs_low();
    //tft_spiwrite8(ILI9341_RAMWR);
    Mode8(); // switch to 8-bit mode
   // while (TxBufFullSPI1());
    WriteSPI1(ILI9341_RAMWR);
   // while (SPI1STATbits.SPIBUSY); // wait for it to end of transaction
    wait16;wait8;
    Mode16(); // switch back to 16-bit mode
    _cs_high();

  _dc_high();
  _cs_low();
  //tft_spiwrite16(color);
 // while (TxBufFullSPI1());
    WriteSPI1(color);
    //while (SPI1STATbits.SPIBUSY);
    wait16;wait16;wait8;
  _cs_high();

}

/* Draw a vertical line at location from (x,y) to (x,y+h-1) with color
 * Parameters:
 *      x:  x-coordinate line to draw; top left of screen is x=0
 *              and x increases to the right
 *      y:  y-coordinate of starting point of line; top left of screen is y=0
 *              and y increases to the bottom
 *      h:  height of line to draw
 *      color:  16-bit color value
 * Returns:     Nothing
 */
void tft_drawFastVLine(int16_t x, int16_t y, int16_t h,
 uint16_t color) {

  // Rudimentary clipping
  if((x >= _width) || (y >= _height)) return;

  if((y+h-1) >= _height) 
    h = _height-y;

  tft_setAddrWindow(x, y, x, y+h-1);

  _dc_high();
  _cs_low();

  while (h--) {
    tft_spiwrite16(color);
  }
  _cs_high();
}

/* Draw a horizontal line at location from (x,y) to (x+w-1,y) with color
 * Parameters:
 *      x:  x-coordinate starting point of line; top left of screen is x=0
 *              and x increases to the right
 *      y:  y-coordinate of starting point of line; top left of screen is y=0
 *              and y increases to the bottom
 *      w:  width of line to draw
 *      color:  16-bit color value
 * Returns:     Nothing
 */

void tft_drawFastHLine(int16_t x, int16_t y, int16_t w,
  uint16_t color) {

  // Rudimentary clipping
  if((x >= _width) || (y >= _height)) return;
  if((x+w-1) >= _width)  w = _width-x;
  tft_setAddrWindow(x, y, x+w-1, y);

  _dc_high();
  _cs_low();

  while (w--) {
    tft_spiwrite16(color);
  }
  _cs_high();
}

/* Fill entire screen with given color
 * Parameters:
 *      color: 16-bit color value
 * Returs:  Nothing
 */
void tft_fillScreen(uint16_t color) {
    tft_fillRect(0, 0,  _width, _height, color);
}

// fill a rectangle
void tft_fillRect(int16_t x, int16_t y, int16_t w, int16_t h,
  uint16_t color) {

  // rudimentary clipping (drawChar w/big text requires this)
  if((x >= _width) || (y >= _height)) return;
  if((x + w - 1) >= _width)  w = _width  - x;
  if((y + h - 1) >= _height) h = _height - y;

  tft_setAddrWindow(x, y, x+w-1, y+h-1);

  _dc_high();
  _cs_low();

  for(y=h; y>0; y--) {
    for(x=w; x>0; x--) {
      tft_spiwrite16(color);
    }
  }
  _cs_high();
}

/* Pass 8-bit (each) R,G,B, get back 16-bit packed color
 * Parameters:
 *      r:  8-bit R/red value from RGB
 *      g:  8-bit g/green value from RGB
 *      b:  8-bit b/blue value from RGB
 * Returns:
 *      16-bit packed color value for color info
 */
// Pass 8-bit (each) R,G,B, get back 16-bit packed color
inline uint16_t tft_color565(uint8_t r, uint8_t g, uint8_t b) {
  return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}


#define MADCTL_MY  0x80
#define MADCTL_MX  0x40
#define MADCTL_MV  0x20
#define MADCTL_ML  0x10
#define MADCTL_RGB 0x00
#define MADCTL_BGR 0x08
#define MADCTL_MH  0x04

void tft_setRotation(uint8_t m) {
  unsigned char rotation;
  tft_writecommand(ILI9341_MADCTL);
  rotation = m % 4; // can't be higher than 3
  switch (rotation) {
   case 0:
     tft_writedata(MADCTL_MX | MADCTL_BGR);
     _width  = ILI9341_TFTWIDTH;
     _height = ILI9341_TFTHEIGHT;
     break;
   case 1:
     tft_writedata(MADCTL_MV | MADCTL_BGR);
     _width  = ILI9341_TFTHEIGHT;
     _height = ILI9341_TFTWIDTH;
     break;
  case 2:
    tft_writedata(MADCTL_MY | MADCTL_BGR);
     _width  = ILI9341_TFTWIDTH;
     _height = ILI9341_TFTHEIGHT;
    break;
   case 3:
     tft_writedata(MADCTL_MX | MADCTL_MY | MADCTL_MV | MADCTL_BGR);
     _width  = ILI9341_TFTHEIGHT;
     _height = ILI9341_TFTWIDTH;
     break;
  }
}


//void Adafruit_ILI9341::invertDisplay(boolean i) {
//  if (hwSPI) spi_begin();
//  writecommand(i ? ILI9341_INVON : ILI9341_INVOFF);
//  if (hwSPI) spi_end();
//}


void delay_ms(unsigned long i){
/* Create a software delay about i ms long
 * Parameters:
 *      i:  equal to number of milliseconds for delay
 * Returns: Nothing
 * Note: Uses Core Timer. Core Timer is cleared at the initialiazion of
 *      this function. So, applications sensitive to the Core Timer are going
 *      to be affected
 */
    unsigned int j;
    j = dTime_ms * i;
    WriteCoreTimer(0);
    while (ReadCoreTimer() < j);
}

void delay_us(unsigned long i){
/* Create a software delay about i us long
 * Parameters:
 *      i:  equal to number of microseconds for delay
 * Returns: Nothing
 * Note: Uses Core Timer. Core Timer is cleared at the initialiazion of
 *      this function. So, applications sensitive to the Core Timer are going
 *      to be affected
 */
    unsigned int j;
    j = dTime_us * i;
    WriteCoreTimer(0);
    while (ReadCoreTimer() < j);
}


////////// stuff not actively being used, but kept for posterity

/*
uint8_t Adafruit_ILI9341::spiread(void) {
  uint8_t r = 0;
  if (hwSPI) {
#if defined (__AVR__)
    uint8_t backupSPCR = SPCR;
    SPCR = mySPCR;
    SPDR = 0x00;
    while(!(SPSR & _BV(SPIF)));
    r = SPDR;
    SPCR = backupSPCR;
#elif defined(TEENSYDUINO)
    r = SPI.transfer(0x00);
#elif defined (__arm__)
    SPI.setClockDivider(11); // 8-ish MHz (full! speed!)
    SPI.setBitOrder(MSBFIRST);
    SPI.setDataMode(SPI_MODE0);
    r = SPI.transfer(0x00);
#endif
  } else {
    for (uint8_t i=0; i<8; i++) {
      digitalWrite(_sclk, LOW);
      digitalWrite(_sclk, HIGH);
      r <<= 1;
      if (digitalRead(_miso))
  r |= 0x1;
    }
  }
  //Serial.print("read: 0x"); Serial.print(r, HEX);
  
  return r;
}
 uint8_t Adafruit_ILI9341::readdata(void) {
   digitalWrite(_dc, HIGH);
   digitalWrite(_cs, LOW);
   uint8_t r = spiread();
   digitalWrite(_cs, HIGH);
   
   return r;
}
 
uint8_t Adafruit_ILI9341::readcommand8(uint8_t c, uint8_t index) {
   if (hwSPI) spi_begin();
   digitalWrite(_dc, LOW); // command
   digitalWrite(_cs, LOW);
   spiwrite(0xD9);  // woo sekret command?
   digitalWrite(_dc, HIGH); // data
   spiwrite(0x10 + index);
   digitalWrite(_cs, HIGH);
   digitalWrite(_dc, LOW);
   digitalWrite(_sclk, LOW);
   digitalWrite(_cs, LOW);
   spiwrite(c);
 
   digitalWrite(_dc, HIGH);
   uint8_t r = spiread();
   digitalWrite(_cs, HIGH);
   if (hwSPI) spi_end();
   return r;
}
*/

 
/*
 uint16_t Adafruit_ILI9341::readcommand16(uint8_t c) {
 digitalWrite(_dc, LOW);
 if (_cs)
 digitalWrite(_cs, LOW);
 
 spiwrite(c);
 pinMode(_sid, INPUT); // input!
 uint16_t r = spiread();
 r <<= 8;
 r |= spiread();
 if (_cs)
 digitalWrite(_cs, HIGH);
 
 pinMode(_sid, OUTPUT); // back to output
 return r;
 }
 
 uint32_t Adafruit_ILI9341::readcommand32(uint8_t c) {
 digitalWrite(_dc, LOW);
 if (_cs)
 digitalWrite(_cs, LOW);
 spiwrite(c);
 pinMode(_sid, INPUT); // input!
 
 dummyclock();
 dummyclock();
 
 uint32_t r = spiread();
 r <<= 8;
 r |= spiread();
 r <<= 8;
 r |= spiread();
 r <<= 8;
 r |= spiread();
 if (_cs)
 digitalWrite(_cs, HIGH);
 
 pinMode(_sid, OUTPUT); // back to output
 return r;
 }
 
 */
