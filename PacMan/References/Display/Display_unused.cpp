/*
  Core0.cpp - Library for PacMan Core 0.
  Created by Clement Hathaway & Simone Khalifa, October 30, 2019.
  Released into the public domain.
*/

//Display functions that are no longer used
//Simone Khalifa

//void Core0::checkCells2(uint8_t currentCell) {
//  for (uint8_t cell = currentCell; cell < NUM_CELLS; cell++) {
//    if (configs[cell].SOH == 0) cellPartialUpdate2(2, cell);
//    //else if(temp>) cellPartialUpdate(1, cell);
//    //else if(voltage>3) cellPartialUpdate(3, cell);
//  }
//}


//void Core0::cellPartialUpdate2(int errorType, int cellNum)
//{
//  uint16_t box_w = 14;
//  uint16_t box_h = 23;
//  uint16_t box_y = 78;
//  uint16_t box_x = 0;
//
//  //cellNum = cellNum - 1; //start index at 1
//
//  if (cellNum < 8) { //seg1
//    box_x = 25 + (box_w - 1) * cellNum;
//  }
//  else { //seg2
//    box_x = 166 + (box_w - 1) * (cellNum - 8);
//  }
//
//  //seg variables
//  if (errorType == 1) { //temp
//    display.fillRect(box_x, box_y - box_h, box_w, box_h, GxEPD_BLACK);
//    display.setTextColor(GxEPD_WHITE);
//    display.setCursor(box_x + 1, box_y - 6);
//    display.print("V");
//  }
//  else if (errorType == 2) { //soh bad
//    display.fillRect(box_x, box_y - box_h, box_w, box_h, GxEPD_BLACK);
//  }
//  else if (errorType == 3) { //high voltage
//    display.fillRect(box_x, box_y - box_h, box_w, box_h, GxEPD_BLACK);
//    display.setTextColor(GxEPD_WHITE);
//    display.setCursor(box_x + 1, box_y - 6);
//    display.print("T");
//  }
//
//  display.updateWindow(128 - box_y, box_x, box_h, box_w, false);
//  if (cellNum < NUM_CELLS - 1) {
//    
//(cellNum + 1);
//  }
//}

//void Core0::chooseCellScreen(uint8_t cellNum)
//{
//  display.setRotation(0);
//  display.drawExampleBitmap(gImage_chooseCell, 0, 0, 128, 296, GxEPD_BLACK);
//
//  display.setRotation(45);
//
//  uint16_t box_w = 14;
//  uint16_t box_h = 1;
//  uint16_t box_y = 81;
//  uint16_t box_x = 25;
//
//  display.fillRect(box_x, box_y - box_h, box_w, box_h, GxEPD_BLACK);
//  display.updateWindow(5, 5, 118, 286, false);
//  checkCells2(0);
//}
//
//void Core0::partialChooseCell(uint8_t cellNum) {
//  display.setRotation(0);
//  display.drawExampleBitmap(gImage_chooseCell, 0, 0, 128, 296, GxEPD_BLACK, GxEPD::bm_default | GxEPD::bm_partial_update);
//  display.setRotation(45);
//
//  uint16_t box_w = 14;
//  uint16_t box_h = 1;
//  uint16_t box_y = 81;
//  uint16_t box_x = 25;
//
//  if (cellNum < 8)  {
//    box_x = 25 + (box_w - 1) * cellNum;
//  }
//  else {
//    box_x = 166 + (box_w - 1) * (cellNum - 8);
//  }
//
//  display.fillRect(box_x, box_y - box_h, box_w, box_h, GxEPD_BLACK);
//  display.updateWindow(128 - box_y, 25, box_h, 246, false);
//}

//boolean Core0::buttonPress(struct button* button) {
//
//  int reading = digitalRead((*button).pin);
//  if (reading != (*button).lastButtonState) {
//    (*button).lastDebounceTime = millis();
//  }
//  if ((millis() - (*button).lastDebounceTime) > debounceDelay) {
//    if (reading != (*button).buttonState) {
//      (*button).buttonState = reading;
//      if ((*button).buttonState == HIGH) {
//        (*button).lastButtonState = reading;
//        return true;
//      }
//    }
//  }
//  (*button).lastButtonState = reading;
//  return false;
//}
