/*
Core1.cpp - Library for PacMan Core 1.
Created by Clement Hathaway & Simone Khalifa, October 30, 2019.
Released into the public domain.
*/
#include "Core1.h"


// CONSTRUCTOR
Core1::Core1(CO_t *CO){
    Wire.begin(PIN_SDA, PIN_SCL); // Join the I2C bus (address optional for master)  -- CHANGE THIS FOR DISPLAY

    totalMAH = 0;
    addresses = (unsigned char*)malloc(16 * sizeof(unsigned char));
}

void Core1::arrayAppend(unsigned char* arr, int index, int value, int arrSize, int capacity){ // https://www.tutorialspoint.com/c_standard_library/c_function_realloc.htm
    if(arrSize > capacity){
        realloc(arr, sizeof(arr) * 2);
        capacity = sizeof(arr) * 2;
    }

    arr[index] = value;
    arrSize = arrSize + 1;
}

void addressVoltageQuickSort(struct addressVoltage* addressVoltages, int first, int last){
	int i, j, pivot;
	uint16_t tempVoltage;
	unsigned char tempAddress;

	if(first<last){
		pivot=first;
		i=first;
		j=last;

		while(i<j){
			while(addressVoltages[i].addressMinusVoltage<=addressVoltages[pivot].addressMinusVoltage&&i<last)
				i++;
			while(addressVoltages[j].addressMinusVoltage>addressVoltages[pivot].addressMinusVoltage)
				j--;
			if(i<j){
				tempAddress=addressVoltages[i].address;
				tempVoltage=addressVoltages[i].addressMinusVoltage;

				addressVoltages[i].address=addressVoltages[j].address;
				addressVoltages[i].addressMinusVoltage=addressVoltages[j].addressMinusVoltage;

				addressVoltages[j].address=tempAddress;
				addressVoltages[j].addressMinusVoltage=tempVoltage;
			}
		}

		tempAddress=addressVoltages[pivot].address;
		tempVoltage=addressVoltages[pivot].addressMinusVoltage;

		addressVoltages[pivot].address=addressVoltages[j].address;
		addressVoltages[pivot].addressMinusVoltage=addressVoltages[j].addressMinusVoltage;

		addressVoltages[j].address=tempAddress;
		addressVoltages[j].addressMinusVoltage=tempVoltage;

		addressVoltageQuickSort(addressVoltages,first,j-1);
		addressVoltageQuickSort(addressVoltages,j+1,last);

	}
}

// TODO: Exclude known I2C devices e.g. GPIO Expand
unsigned char* Core1::discoverCellMen(){
    unsigned char* discoveredAddresses;
    int nDevices = 0;
    int discoveredCapacity = 1; // Start at 1 address, increase exponentionally by 2

    discoveredAddresses = (unsigned char*)malloc(1 * sizeof(unsigned char));

    for(int address = 1; address < 127; address++ ){
        // The I2C scanner uses the return value of
        // the Write.endTransmisstion to see if
        // a device did acknowledge to the address.
        Wire.beginTransmission(address);
        byte error = Wire.endTransmission();

        if (error == 0){
            if(address != TEMPERATURE_SENSOR_ADDRESS
            && address != IO_EXPANDER_ADDRESS
            && address != REAL_TIME_CLOCK_ADDRESS
            && address != POWER_MONITOR_ADDRESS){
                Serial.print("CellMan I2C device found at address 0x");
                if (address<16) Serial.print("0");
                Serial.print(address,HEX);
                Serial.println("  !");

                arrayAppend(discoveredAddresses, nDevices, address, nDevices+1, discoveredCapacity); // Add the address to the discoveredAddresses Array using the append method to allow for expansion
                nDevices++; // Increment the number of devices by 1, strategically after the arrayAppend so we can use it as an index
            }
        }
        else if (error==4){
            Serial.print("Unknown error at address 0x");
            if (address<16)
            Serial.print("0");
            Serial.print(address,HEX);
            Serial.println(" skipping...");
        }
    }

    return discoveredAddresses;
} // USES MALLOC (Only call once at startup to prevent memory leakage)

unsigned char* Core1::requestDataFromSlave(unsigned char address){
    Wire.requestFrom((int) address, 24); // 24 is the max data length expected in bytes
    if(DEBUG){
        Serial.print("Requesting data from CellMan on Address: ");
        Serial.println(address);
    }

    while (Wire.available()) {
        if(DEBUG) Serial.println("Wire Available!");
        unsigned char debugFlag = Wire.read();

        if(debugFlag == 0x00){
            unsigned char cellData[NORMAL_I2C_LENGTH];
            cellData[0] = debugFlag;
            for(int i = 1; i<NORMAL_I2C_LENGTH; i++){
                *(cellData + i) = Wire.read();                     // Append the read character (byte) to our cellData array
                if(DEBUG) Serial.println(cellData[i], HEX);        // Print the character (byte) in HEX
            }
        }else if(debugFlag == 0x01){
            unsigned char cellData[DEBUG_I2C_LENGTH];
            cellData[0] = debugFlag;
            for(int i = 1; i<DEBUG_I2C_LENGTH; i++){
                *(cellData + i) = Wire.read();                     // Append the read character (byte) to our cellData array
                if(DEBUG) Serial.println(cellData[i], HEX);        // Print the character (byte) in HEX
            }
        }else{
            Serial.println("Error on the Debug byte! Don't know length to expect. Returning 24 bytes");
            Serial.print("debugFlag is: ");
            Serial.println(debugFlag, HEX);

            unsigned char cellData[DEBUG_I2C_LENGTH];
            cellData[0] = 0x01;
            for(int i = 1; i<DEBUG_I2C_LENGTH; i++){
                *(cellData + i) = Wire.read();                     // Append the read character (byte) to our cellData array
                if(DEBUG) Serial.println(cellData[i], HEX);        // Print the character (byte) in HEX
            }
        }
    }
    return cellData;
}

// uint8_t calculateCellLocation(unsigned char* cellData){
//     uint16_t minusVoltage = (uint16_t)((cellData[6]<<8)+cellData[5]);
//
//     return minusVoltage;
// }

void processCellData(unsigned char* cellData){
    // Process the data differently depending on cellData[0] which is the debugFlag
    uint8_t cellLocation                 = calculateCellLocation(cellData);

    cellPositions[cellLocation]          = cellLocation;
    cellVoltages[cellLocation]           = (uint16_t)((cellData[2]<<8)+cellData[1]); // Shift MSBs over 8 bits, then add the LSBs to the first 8 bits and cast as a uint16_t
    cellTemperatures[cellLocation]       = (uint16_t)((cellData[4]<<8)+cellData[3]);
    minusTerminalVoltages[cellLocation]  = (uint16_t)((cellData[6]<<8)+cellData[5]);
    cellBalanceCurrents[cellLocation]    = (uint16_t)((cellData[8]<<8)+cellData[7]);


    // If we are in I2C Debug Mode
    if(cellData[0] == 0x01){
        LEDStatuses[cellLocation]          = (bool)cellData[9];
        balanceStatuses[cellLocation]      = (bool)cellData[10];
        balanceDutyCycles[cellLocation]    = (uint8_t)cellData[10];
        balanceFrequencies[cellLocation]   = (uint16_t)((cellData[13]<<8)+cellData[12]);
        temperatureSlopes[cellLocation]    = (uint16_t)((cellData[15]<<8)+cellData[14]);
        temperatureOffsets[cellLocation]   = (uint16_t)((cellData[17]<<8)+cellData[16]);
        balanceCurrentSlopes[cellLocation] = (uint16_t)((cellData[19]<<8)+cellData[18]);
        balanceVoltageSlopes[cellLocation] = (uint16_t)((cellData[21]<<8)+cellData[20]);
    }
}


void Core1::calculateTotalPackSOC(){
    int SOCTotal = 0;

    for(int index = 0; index < 16; index++){
        SOCTotal += privateCells[index].SOC;  // Sum up all of our SOCs from all the cells to get an average for the 16 cells in a pack
    }

    packSOC = (float)(SOCTotal/16); // Return the average SOC from the cells
}

void Core1::start(){


    for(;;){


      delay(2000);
    }
}
