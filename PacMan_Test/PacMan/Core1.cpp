/*
Core1.cpp - Library for PacMan Core 1.
Created by Clement Hathaway & Simone Khalifa, October 30, 2019.
Released into the public domain.
*/

#include "Core1.h"
#include "PacMan.h"

Core1::Core1()
{
    
}

void Core1::init()
{

}

void Core1::update()
{
    byte error, address;
    int nDevices;

    Serial.println("Scanning...");

    nDevices = 0;
    for(address = 1; address < 127; address++) {
        Wire.beginTransmission(address);
        error = Wire.endTransmission();

        if (error == 0) {
            Serial.print("I2C device found at address 0x");
            if (address<16)
            Serial.print("0");
            Serial.print(address,HEX);
            Serial.println("  !");

            nDevices++;
        }
        else if (error==4) {
            Serial.print("Unknown error at address 0x");
            if (address<16)
            Serial.print("0");
            Serial.println(address,HEX);
        }    
    }

    if (nDevices == 0)
        Serial.println("No I2C devices found\n");
    else
        Serial.println("done\n");

    delay(5000);
}
