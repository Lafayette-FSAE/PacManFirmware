/*
  Core1.cpp - Library for PacMan Core 1.
  Created by Clement Hathaway & Simone Khalifa, October 30, 2019.
  Released into the public domain.
*/

#include "Arduino.h"
#include "Core1.h"

// CONSTRUCTOR
Core1::Core1(int pin){
   Wire.begin();        // Join the I2C bus (address optional for master)

   addresses = (unsigned char*)malloc(16 * sizeof(unsigned char));
   cells =     (unsigned char*)malloc(16 * sizeof(cell));
}

unsigned char* Core1::discoverCellMen(){ // https://www.tutorialspoint.com/c_standard_library/c_function_realloc.htm
   unsigned char* discoveredAddresses;
   int nDevices;
   discoveredAddresses = (unsigned char*)malloc(1 * sizeof(unsigned char));

   nDevices = 0;
   for(address = 1; address < 127; address++ ){
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
 
    if (error == 0){
      Serial.print("I2C device found at address 0x");
      if (address<16)
        Serial.print("0");
      Serial.print(address,HEX);
      Serial.println("  !");
 
      nDevices++;
    }
    else if (error==4){
      Serial.print("Unknown error at address 0x");
      if (address<16)
        Serial.print("0");
      Serial.println(address,HEX);
    }    
  }
}
