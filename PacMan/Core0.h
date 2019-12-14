/*
  Core0.h - Library Header for PacMan Core 0 Code.
  Created by Clement Hathaway & Simone Khalifa, October 30, 2019.
  Released into the public domain.
*/
#ifndef Core0_h
#define Core0_h

#include "Arduino.h"

class Core0
{
  public:
    // Define Constructor
        Core0 (struct cell, float, boolean, SemaphoreHandle_t, SemaphoreHandle_t, SemaphoreHandle_t);
    // Define Public Functions
    void start();
  private:
    // Define Private Functions
    //all the functionss
    // Define Private variables
};

#endif
