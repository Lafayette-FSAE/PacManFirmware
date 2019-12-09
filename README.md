# PacMan Firmware

This repository contains the major code for Lafayette College's 2019/2020 PacMan Board

## Description

This project provides the firmware for the ESP32 processor used on the [PacMan board](https://github.com/Lafayette-FSAE/PacMan). It provides the essential functions for the board, including but not limited to, CAN Bus communication, SOC Calculation, TSV Pack e-ink Display operation, and I2C CellMen communication.

## Getting Started

1. Install the [Arduino IDE](https://www.arduino.cc/en/main/software). Tested on version 1.8.10
2. Add ESP32 board to the Arduino IDE by adding this Additional Board Manager URL: https://dl.espressif.com/dl/package_esp32_index.json. A tutorial is located [here](https://randomnerdtutorials.com/installing-the-esp32-board-in-arduino-ide-windows-instructions/)
3. Select the ESP32 Wrover Module in the boards list - You can probably use other boards here
4. Connect the ESP32 TTGO T8 v1.7 Board to the computer via the microUSB port
5. Select the proper serial port (dependent on host OS) with a baud rate of 115200
6. Click on the compile and upload button

## Useful Documents
- [Usable ESP32 Pins](https://randomnerdtutorials.com/esp32-pinout-reference-gpios/)
- [GitHub Page for the ESP32 T8 v1.7](https://github.com/LilyGO/TTGO-T8-ESP32)

## Notes
- You can use almost any ESP32 board or reference if you strictly use the GPIO numbers and not board specific numbers, this is also how it is written in the code (using GPIO numbers).

## Authors

- [Clement Hathaway](mailto:cwbh10@gmail.com)
- [Simone Khalifa](mailto:khalifas@lafayette.edu)

## Version History

* 0.0.1
    * Initial Code Outlines

## Useful Links

- [A tutorial for writing and using Libraries (similar to classes) in Arduino](https://www.arduino.cc/en/Hacking/libraryTutorial)
- [Custom building in the Atom IDE](https://atom.io/packages/build)
- [Arduino Command Line Interface (CLI) to use with the custom building](https://github.com/arduino/arduino-cli)
- [Tips on using Git and Github within Atom](https://flight-manual.atom.io/using-atom/sections/version-control-in-atom/)
- [Understanding Branches](https://git-scm.com/book/en/v2/Git-Branching-Branches-in-a-Nutshell)
