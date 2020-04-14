/**
 * @file Core1.cpp
 * @author Clement Hathaway (cwbh10@gmail.com)
 * @brief The Code running on Core1 controlling much of the data processing
 * @version 1.0
 * @date 2020-04-13
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#include "Core1.h"

uint8_t cellFaults[16];

/**
 * @brief A Callback triggered by the warning timer
 * Locks Object Dictionary
 * Updates the Object Dictionary with the cellFaults warnings that have been set prior to their appropriate values
 * Unlocks Object Dictionary
 * @param pxTimer parameter used in the callback in the timers - given by ESP IDF guidelines
 */
void warningCallBack(TimerHandle_t pxTimer){
    CO_LOCK_OD();
    //Serial.println("15 seconds has past setting warnings!");
    for(int i = 0; i < 16; i++){
        OD_warning[i] = cellFaults[i];
    }
    CO_UNLOCK_OD();
}
/**
 * @brief A Callback triggered by the safetyloop timer
 * Opens Safety loop
 * Locks Object Dictionary
 * Updates the Object Dictionary with the cellFaults faults that have been set prior to their appropriate values
 * Unlocks Object Dictionary+
 * @param pxTimer parameter used in the callback in the timers - given by ESP IDF guideline
 */
void openSafetyLoopCallBack(TimerHandle_t pxTimer){
    digitalWrite(PIN_SLOOP_EN, LOW);
    OD_SLOOP_Relay = false;
    Serial.println("SAFETY LOOP OPENED!");
    
    CO_LOCK_OD();
    for(int i = 0; i < 16; i++){
        OD_fault[i] = cellFaults[i];
    }
    CO_UNLOCK_OD();
}

// CONSTRUCTOR
/**
 * @brief Constructs a new Core 1::Core 1 object
 * Created interrupt Semaphores
 * Starts I2C driver and joins the bus on specified pins in PacMan.h
 * Creates all the warning and fault timers
 * @param CO The CANopen object which is passed in via the toplevel PacMan file
 */
Core1::Core1(CO_t *CO) {
    // Interrupt Semaphores
    if (DEBUG) printf("Core 1: Initialising");

    I2C_InterrupterSemaphore = xSemaphoreCreateBinary();
    chargeDetectSemaphore = xSemaphoreCreateBinary();
    if (DEBUG) printf("Core 1: I2C and Charge Detect Interrupt Semaphores Created");

    Wire.begin(PIN_SDA, PIN_SCL); // Join the I2C bus (address optional for master)  -- CHANGE THIS FOR DISPLAY
    if (DEBUG) printf("Core 1: Joined I2C Bus");
    totalMAH = 0;

    // Create safetyloop timers
    underVoltageTimer = xTimerCreate(
                                      /* Just a text name, not used by the RTOS kernel. */
                                      "underVoltageTimer",
                                      /* The timer period in ticks. From Time To Trigger *1000 for ms */
                                      pdMS_TO_TICKS(TTT*1000),
                                      /* The timer is a one-shot timer. */
                                      pdFALSE,
                                      /* The id is not used by the callback so can take any
                                      value. */
                                      0,
                                      /* The callback function that switches the LCD back-light
                                      off. */
                                      openSafetyLoopCallBack);
    
    underVoltageWarningTimer = xTimerCreate("underVoltageWarningTimer", pdMS_TO_TICKS(TTT*1000/6), pdFALSE, 0, warningCallBack);
    overVoltageTimer = xTimerCreate("overVoltageTimer", pdMS_TO_TICKS(TTT*1000), pdFALSE, 0, openSafetyLoopCallBack);
    overVoltageWarningTimer = xTimerCreate("overVoltageWarningTimer", pdMS_TO_TICKS(TTT*1000/6), pdFALSE, 0, warningCallBack);
    overTemperatureTimer = xTimerCreate("overTemperatureTimer", pdMS_TO_TICKS(TTT*1000), pdFALSE, 0, openSafetyLoopCallBack);
    overTemperatureWarningTimer = xTimerCreate("overTemperatureWarningTimer", pdMS_TO_TICKS(TTT*1000/6), pdFALSE, 0, warningCallBack);
    if (DEBUG) printf("Core 1: Created Software Safety Timers!");                           
}

// Quick sorts out struct of addressVoltage based off of the .addressMinusVoltage property
/**
 * @brief Sorts an array of AddressVoltage structs by ascending voltage reference level
 * Uses quicksort as the main sorting method
 * @param addressVoltages The array of structs of type addressVoltage
 * @param first Part of the recursion in Quicksort
 * @param last Part of the recursion in Quicksort
 */
void Core1::addressVoltageQuickSort(addressVoltage* addressVoltages, int first, int last) {
    int i, j, pivot;
    uint16_t tempVoltage;
    uint8_t tempAddress;

    if (first < last) {
        pivot = first;
        i = first;
        j = last;

        while (i < j) {
            while (addressVoltages[i].addressMinusVoltage <= addressVoltages[pivot].addressMinusVoltage && i < last)
            i++;
            while (addressVoltages[j].addressMinusVoltage > addressVoltages[pivot].addressMinusVoltage)
            j--;
            if (i < j) {
                tempAddress = addressVoltages[i].address;
                tempVoltage = addressVoltages[i].addressMinusVoltage;

                addressVoltages[i].address = addressVoltages[j].address;
                addressVoltages[i].addressMinusVoltage = addressVoltages[j].addressMinusVoltage;

                addressVoltages[j].address = tempAddress;
                addressVoltages[j].addressMinusVoltage = tempVoltage;
            }
        }

        tempAddress = addressVoltages[pivot].address;
        tempVoltage = addressVoltages[pivot].addressMinusVoltage;

        addressVoltages[pivot].address = addressVoltages[j].address;
        addressVoltages[pivot].addressMinusVoltage = addressVoltages[j].addressMinusVoltage;

        addressVoltages[j].address = tempAddress;
        addressVoltages[j].addressMinusVoltage = tempVoltage;

        addressVoltageQuickSort(addressVoltages, first, j - 1);
        addressVoltageQuickSort(addressVoltages, j + 1, last);
    }
}

bool Core1::addressVoltageSorter(addressVoltage const lhs, addressVoltage const rhs) {
    return (lhs.addressMinusVoltage < rhs.addressMinusVoltage);
}

// Discovery I2C devices, excluding I2C devices on PacMan
/**
 * @brief Discover the CellMen that are connected to the I2C bus
 * Scans for devices between all available I2C addresses (1-127)
 * Excludes known non-cellman devices defined in PacMan.h
 * @return uint8_t number of CellMen found
 */
uint8_t Core1::discoverCellMen() {
    uint8_t cellMenCount = 0;

    // Since we are using a set array size, let's initialise all values in the array to 0
    for (int i = 0; i < 16; i++) {
        addresses[i] = 0x00;
    }

    // Start the scan for I2C devices between 1 and 127
    for (int address = 1; address < 127; address++ )
    {
        // The I2C scanner uses the return value of
        // the Write.endTransmisstion to see if
        // a device did acknowledge to the address.
        Wire.beginTransmission(address);
        byte error = Wire.endTransmission();

        if (error == 0) {
            // Exclude PacMan I2C devices
            if (address != I2C_ADDR_MCP9804
                && address != I2C_ADDR_MCP23008
                && address != I2C_ADDR_BQ32002
                && address != I2C_ADDR_LTC4151
                && address != I2C_ADDR_LTC4151_GLOB)
            {
                Serial.print("CellMan I2C device found at address 0x");
                if (address < 16) Serial.print("0");
                Serial.print(address, HEX);
                Serial.println("  !");

                // Add the new address to our address array and increment the number we have found
                addresses[cellMenCount] = address;
                cellMenCount++;
            }
        }
        else if (error == 4) {
            Serial.print("Unknown error at address 0x");
            if (address < 16)
            Serial.print("0");
            Serial.print(address, HEX);
            Serial.println(" skipping...");
        }
    }
    return cellMenCount;
}

// Request byte array from specified CellMan I2C address, using index and preCollect we know when to check for I2C faults
/**
 * @brief Requests data from an I2C slave
 * Requests data from I2C slaves, used for collecting data from CellMen
 * Can be used before we have information about what CellMan is in what position during precollection
 * @param address The address of the I2C Device/CellMan
 * @param index The index for where the data will be stored - Not relevant in precollection mode
 * @param preCollect A boolean to decide if the method is collecting data in the beginning for processing or just gathering new data
 * @return unsigned* char returns data array sent from the I2C slave
 */
unsigned char* Core1::requestDataFromSlave(unsigned char address, uint8_t index, bool preCollect) {
    uint8_t physicalODAddress = physicalLocationFromSortedArray(index);
    if(!preCollect) toggleCellManLED(address, true);

    Wire.requestFrom((int) address, REQUEST_LENGTH); // 24 is the max data length expected in bytes
    if (DEBUG) {
        Serial.print("Requesting data from CellMan on Address: ");
        Serial.println(address);
    }

    if (Wire.available()) {
        if (DEBUG) Serial.println("Wire Available!");
        if(cellFaults[physicalODAddress] == 9 && !preCollect){
            if(DEBUG) Serial.println("CellMan I2C Re-established!");
            cellFaults[physicalODAddress] = 0;    // If the Cellman comes back online, set the fault back to 0
        } 

        for (int i = 0; i < REQUEST_LENGTH; i++) {
            *(cellDs + i) = Wire.read();                     // Append the read character (byte) to our cellData array
            if (DEBUG) {
                Serial.println(cellDs[i], HEX);              // Print the character (byte) in HEX
                Serial.println(cellDs[i], DEC);              // Print the character (byte) in DEC
            }
        }
    }else{
        if(!preCollect && cellFaults[physicalODAddress] != 9){
            if(DEBUG) Serial.println("Got a fault!");
            cellFaults[physicalODAddress] = 9;
        }
    }

    if(!preCollect) toggleCellManLED(address, false);
    // Only update the faults if its 0 or 9, otherwise it negates the timer's purpose - Gets called a lot, maybe we can reduce the OD usage here sometime
    if(!preCollect){
        if(cellFaults[physicalODAddress] == 9 || cellFaults[physicalODAddress] == 0){
            CO_LOCK_OD();
            OD_warning[physicalODAddress] = cellFaults[physicalODAddress];
            CO_UNLOCK_OD();
        }
    }

    return cellDs;
}

// Process the celldata into our private arrays to be stored into the OD later
/**
 * @brief Process the collected cellData array from the CellMan into its respective arrays in the class
 * Since we get data from each CellMan at a time, but in the Object Dictionary it is stored by Data and then CellMan
 * We have to preprocess the data so it is ready to be inserted in the OD with minimal locking time
 * @param cellData Pass in the CellData array of bytes obtained from requestDataFromSlave
 * @param cellPhysicalLocation Pass in the physical location of the cell so it matches in the array, e.g. physical location 8 is 7 in the array
 */
void Core1::processCellData(unsigned char* cellData, uint8_t cellPhysicalLocation) {
    cellPositions[cellPhysicalLocation]          = cellPhysicalLocation;
    cellVoltages[cellPhysicalLocation]           = (uint16_t)((cellData[2] << 8) + cellData[1]); // Shift MSBs over 8 bits, then add the LSBs to the first 8 bits and cast as a uint16_t
    cellTemperatures[cellPhysicalLocation]       = (uint16_t)((cellData[4] << 8) + cellData[3]);
    minusTerminalVoltages[cellPhysicalLocation]  = (uint16_t)((cellData[6] << 8) + cellData[5]);
    cellBalanceCurrents[cellPhysicalLocation]    = (uint16_t)((cellData[8] << 8) + cellData[7]);


    // If we are in I2C Debug Mode
    if (DEBUG) {
        LEDStatuses[cellPhysicalLocation]          = (bool)cellData[9];
        balanceStatuses[cellPhysicalLocation]      = (bool)cellData[10];
        balanceDutyCycles[cellPhysicalLocation]    = (uint8_t)cellData[10];
        balanceFrequencies[cellPhysicalLocation]   = (uint16_t)((cellData[13] << 8) + cellData[12]);
        temperatureSlopes[cellPhysicalLocation]    = (uint16_t)((cellData[15] << 8) + cellData[14]);
        temperatureOffsets[cellPhysicalLocation]   = (uint16_t)((cellData[17] << 8) + cellData[16]);
        balanceCurrentSlopes[cellPhysicalLocation] = (uint16_t)((cellData[19] << 8) + cellData[18]);
        balanceVoltageSlopes[cellPhysicalLocation] = (uint16_t)((cellData[21] << 8) + cellData[20]);
    }
}

// Check the safety of our measurements, trigger warnings and safetyloop here
/**
 * @brief Checks the safety our of CellMen Data in the Object Dictionary
 * Looks through the number of CellMen we have detected and checks their values for
 * Undervoltage, Overvoltage, Overtemperature, and I2C disconnect/error
 * Starts/Stops appropriate warning and fault timers
 * @param numberOfDiscoveredCellMen Integer with the number of detected CellMan so we don't check bad data
 */
void Core1::checkSafety(uint8_t numberOfDiscoveredCellMen){
    tempUV = false;
    tempOV = false;
    tempOT = false;
    int i;
    int newIndex;

    if (DEBUG) printf("Core 1: Checking Safety of Data in Object Dictionary");
    CO_LOCK_OD();
    if (DEBUG) printf("Core 1: Obtained Object Dictionary Lock");
    for(i = 0; i < numberOfDiscoveredCellMen; i++){
        // Because we have everything stored in physical locations in the array. e.g. 0 goes to Cell 0, 1 goes to Cell 8 in the array since they are in different segments
        newIndex = physicalLocationFromSortedArray(i);
        
        // Voltages are below the threshold trigger the tempValue to symbolise at least one voltage low
        if(cellVoltages[newIndex] < OD_minCellVoltage[newIndex] && cellFaults[newIndex] != 9){
            cellFaults[newIndex] = 2;
            tempUV = true;
        } else if(cellVoltages[newIndex] > OD_maxCellVoltage[newIndex] && cellFaults[newIndex] != 9){
            cellFaults[newIndex] = 1;
            tempOV = true;
        } else if(cellTemperatures[newIndex] > OD_maxCellTemp[newIndex] && cellFaults[newIndex] != 9){
            cellFaults[newIndex] = 3;
            tempOT = true;

        /* Reset values here so that we get rid of warnings and reset timers
           Except if it is 9, since this means the cellman is not on I2C
           and the data is old and therefore not trustworthy*/
        }else if(cellFaults[newIndex] != 9){
            cellFaults[newIndex] = 0;
            OD_warning[newIndex] = 0;
            OD_fault[newIndex] = 0;
        }
    }
    CO_UNLOCK_OD();
    if (DEBUG) printf("Core 1: Released Object Dictionary Lock");

    // At least 1 cell was found below the voltage threshold - start undervoltage counter
    if(tempUV){
        if(xTimerIsTimerActive(underVoltageTimer) == pdFALSE){
            if(xTimerStart(underVoltageTimer, 0) != pdPASS ){
                /* The timer could not be set into the Active state. */
                if(DEBUG) Serial.println("Core 1: Could not start underVoltage Timer");
            }else{
                if(DEBUG) Serial.println("Core 1: underVoltage Timer as begun!");
            }
        }
        if(xTimerIsTimerActive(underVoltageWarningTimer) == pdFALSE){
            if(xTimerStart(underVoltageWarningTimer, 0) != pdPASS ){
                /* The timer could not be set into the Active state. */
                if(DEBUG) Serial.println("Core 1: Could not start underVoltage Warning Timer");
            }else{
                if(DEBUG) Serial.println("Core 1: underVoltage Warning Timer as begun!");
            }
        }
    // No cells were found below the minimum voltage - stop and reset both counters
    }else{
        // Reset puts the timer in an active state - Resets time, but need to stop it to put it in a dormant state
        xTimerReset(underVoltageTimer, 0);
        xTimerReset(underVoltageWarningTimer, 0);

        // Stop puts timer in a dormant state which lets us use xTimerIsTimerActive
        xTimerStop(underVoltageTimer, 0);
        xTimerStop(underVoltageWarningTimer, 0);
    }

    // At least 1 cell was found above the voltage threshold - start overvoltage counter
    if(tempOV){
        if(xTimerIsTimerActive(overVoltageTimer) == pdFALSE){ // Check to see if the timer has not been started yet, we don't want to start an already started timer
            if(xTimerStart(overVoltageTimer, 0) != pdPASS ){
                /* The timer could not be set into the Active state. */
                if(DEBUG) Serial.println("Core 1: Could not start overVoltage Timer");
            }else{
                if(DEBUG) Serial.println("Core 1: overVoltage Timer as begun!");
            }
            if(xTimerIsTimerActive(overVoltageWarningTimer) == pdFALSE){ // Check to see if the timer has not been started yet, we don't want to start an already started timer
                if(xTimerStart(overVoltageWarningTimer, 0) != pdPASS ){
                    /* The timer could not be set into the Active state. */
                    if(DEBUG) Serial.println("Core 1: Could not start overVoltage Warning Timer");
                }else{
                    if(DEBUG) Serial.println("Core 1: overVoltage Timer warning as begun!");
                }
            }
        }
    // No cells were found above the maximum voltage - stop and reset counters
    }else{
        xTimerReset(overVoltageTimer, 0);
        xTimerReset(overVoltageWarningTimer, 0);

        xTimerStop(overVoltageTimer, 0);
        xTimerStop(overVoltageWarningTimer, 0);
    }

    // At least 1 cell temp was found above the temp threshold - start overTemperature counter
    if(tempOT){
        if(xTimerIsTimerActive(overTemperatureTimer) == pdFALSE){
            if(xTimerStart(overTemperatureTimer, 0) != pdPASS ){
                /* The timer could not be set into the Active state. */
                if(DEBUG) Serial.println("Core 1: Could not start overTemperature Timer");
            }else{
                if(DEBUG) Serial.println("Core 1: overTemperature Timer as begun!");
            }
        }
        if(xTimerIsTimerActive(overTemperatureWarningTimer) == pdFALSE){
            if(xTimerStart(overTemperatureWarningTimer, 0) != pdPASS ){
                /* The timer could not be set into the Active state. */
                if(DEBUG) Serial.println("Core 1: Could not start overTemperature Warning Timer");
            }else{
                if (DEBUG) Serial.println("Core 1: overTemperature Timer Warning as begun!");
            }
        }
    // No cell temps were found above the maximum temp - stop and reset counter
    }else{
        xTimerReset(overTemperatureTimer, 0);
        xTimerReset(overTemperatureWarningTimer, 0);

        xTimerStop(overTemperatureTimer, 0);
        xTimerStop(overTemperatureWarningTimer, 0);
    }
}

// Maps the arrayIndex to a physical cell location in the packs (since we can't tell between segments right now) by saying the second instance of a same voltage potential cell is in the other segment
/**
 * @brief Maps an arrayIndex into the physical Cell Location in the Packs
 * This method is using a work around since the PacMan hardware this firmware was developed on has a flaw
 * We cannot differentiate between segments since they have their own grounds.
 * A PacMan board revision should have been made to fix this but this method randomises in which seg a cell gets placed 
 * This way you have a 50% chance of knowing what cell the data is about by looking in the Packs, this is better than otherwise
 * This function will need to be rewritten or swapped for something else once segment detection works
 * Segment detection will eventually be done by reseting a known segment and performing a CellMen discovery 
 * @param arrayIndex Integer of the array's Index (not physical location!)
 * @return uint8_t Returns an integer of the physical location which can be used as an index for physically indexed arrays (e.g OD)
 */
uint8_t Core1::physicalLocationFromSortedArray(uint8_t arrayIndex) {
    uint8_t physicalAddress;
    if (arrayIndex % 2 == 0) { // If Even
        physicalAddress = arrayIndex / 2;
    } else {                  // If Odd
        physicalAddress = ((arrayIndex - 1) / 2) + 8;
    }

    // return physicalAddress;
    return arrayIndex;
}

// Simple voltage-based SOC - Very inaccurate
/**
 * @brief Calculates a simple Voltage based State of Charge
 * This shouldn't be used in production and should be replaced
 * A simple EKF based model would be best here
 * Columb counting is the easier and better soltion than voltage but worse than EKF models
 */
void Core1::calculateTotalPackSOC() {
    int SOCTotal = 0;

    for (int index = 0; index < 16; index++) {
        //SOCTotal += privateCells[index].SOC;  // Sum up all of our SOCs from all the cells to get an average for the 16 cells in a pack
    }

    packSOC = (float)(SOCTotal / 16); // Return the average SOC from the cells
}

void Core1::toggleCellManLED(unsigned char address, bool state){
    // If on
    if(state){
        Wire.beginTransmission(address);
        Wire.write(0x23); // 0x23 is the LED register
        Wire.write(0x00); // MSB
        Wire.write(0x01); // LSB
        Wire.endTransmission();
    }else{ //If off
        Wire.beginTransmission(address);
        Wire.write(0x23); // 0x23 is the LED register
        Wire.write(0x00); // MSB
        Wire.write(0x00); // LSB
        Wire.endTransmission();
    }
}

// Toggle LEDs on CellMen in segment position order to indicate that they are properly connected and communicating
// addressVoltages should be sorted at this point
// Will need to modify when the segments are distinguishable so that the order is correct
/**
 * @brief Flashes CellMen LEDs in order of detection
 * Follows the order of flashing each CellMan and then all of them at once
 */
void Core1::indicateCellMen(){
    if(DEBUG){
        Serial.print("Flashing CellMen LEDs at time ");
        Serial.println(millis());
    }
    // Light up each LED incrementally every 250ms
    for (int i = 0; i < numberOfDiscoveredCellMen; i++)
    {
        toggleCellManLED(addressVoltages[i].address, true);
        delay(250);
    }

    // Blink the LEDs off and on 4 times
    for (int j = 0; j < 4; j++)
    {
        for (int i = 0; i < numberOfDiscoveredCellMen; i++)
        {
            toggleCellManLED(addressVoltages[i].address, false);
        }
        delay(250);
        for (int i = 0; i < numberOfDiscoveredCellMen; i++)
        {
            toggleCellManLED(addressVoltages[i].address, true);
        }
        delay(250);
    }

    // Turn each LED off
    for (int i = 0; i < numberOfDiscoveredCellMen; i++)
    {
        toggleCellManLED(addressVoltages[i].address, false);
    }
    if(DEBUG){
        Serial.print("Done flashing CellMen LEDs at time ");
        Serial.println(millis());
    }
}

/**
 * @brief Updates the Object Dictionary with udpated local CellMan data that was collected
 * Goes through the detected CellMen and collects, processes, and checks safety of CellMen Data
 * Locks the Object Dictionary
 * Updates all of the data from the warnings, faults, and measurements into the OD
 * Unlocks the OD
 */
void Core1::updateCellMenData(){
    //Collect data from all the CellMen & Update Object Dictionary Interrupt
    if (DEBUG) printf("Core 1: Attempting to take I2C Interrupt Semaphore");
    if (xSemaphoreTake(I2C_InterrupterSemaphore, 0) == pdTRUE) {
        if (DEBUG) printf("Core 1: Took I2C Interrupt Semaphore");
        
        // Update CellMan Code
        for (int i = 0; i < numberOfDiscoveredCellMen; i++) {
            if (DEBUG) printf("Core 1: Requesting Data From CellMan");
            unsigned char* celldata = requestDataFromSlave(addressVoltages[i].address, i, false);
            if (DEBUG) printf("Core 1: Processing Collected Data");
            processCellData(celldata, physicalLocationFromSortedArray(i)); // Process data retrieved from each cellman and is inerted based off of physicalAddress
            if (DEBUG) printf("Core 1: Checking Safety of Collected Data");
            checkSafety(numberOfDiscoveredCellMen);
        }
        if (DEBUG) printf("Core 1: Finishing Collecting CellMen Data");

        // Update the Object Dictionary Here
        if (DEBUG) printf("Core 1: Updating Object Dictionary");
        CO_LOCK_OD();
        if (DEBUG) printf("Core 1: Obtained Object Dictionary Lock");
        // index i here will go through the array, but in this array the physical stuff is already set - I hate how confusing the indexes are depending on where you are in the code
        for (int i = 0; i < 16; i++) {
            // If this cell is disconnected, set all its useful data to 0 prior to changing the O
            if(minusTerminalVoltages[i] != 0)  // TODO: Examine this, the first cells should be at zero -- Removing this causes shit values so gotta fix this
            {
                if(cellFaults[i] == 9)
                {
                    cellVoltages[i] = 0;
                    cellTemperatures[i] = 0;
                    cellBalancingEnabled[i] = 0;
                    cellBalanceCurrents[i] = 0;
                }
                
                OD_cellPosition[i]         = cellPositions[i];
                OD_cellVoltage[i]          = cellVoltages[i];
                OD_cellTemperature[i]      = cellTemperatures[i];
                OD_minusTerminalVoltage[i] = minusTerminalVoltages[i];
                OD_cellBalancingEnabled[i] = cellBalancingEnabled[i];
                OD_cellBalancingCurrent[i] = cellBalanceCurrents[i];
                maxCellVoltage[i]          = OD_maxCellVoltage[i];
                maxCellTemp[i]             = OD_maxCellTemp[i];
            }
        }
        CO_UNLOCK_OD();
        if (DEBUG) printf("Core 1: Updated Object Dictionary and Released OD Lock");
    }
}

/**
 * @brief Handles the charging interrupt and relays
 * Checks to make sure all cells are within acceptable voltage ranges
 * Trys to take the interrupt semaphore for the charge connector
 * Adjusts the state of the relay based on the safety and state of charge connector
 * This provides some hysteresis until the connector is replugged in if a cell goes out of safety spec
 * @bug Can get the state inverted in certain cases (Relay on when plug out, and vice-versa), some additional logic should be put in to fix this
 */
void Core1::handleCharging(){
    charge = true;
    int newIndex; // Gets us the actual physical location which is how the array in defined

    // Check all cells are within spec -- This might cause issue for balancing if 1 cell becomes too high it'll turn off the relay, voltage will drop, and we will have relay oscillations
    // UPDATE: ^ Above statement fixed by haiving the currentlyCharging variable which gives us hysteresis until the plug is taken out and put back in
    for (int i = 0; i < numberOfDiscoveredCellMen; i++) {
        newIndex = physicalLocationFromSortedArray(i);
        if(cellVoltages[newIndex] > maxCellVoltage[newIndex] || cellTemperatures[newIndex] > maxCellTemp[newIndex]){
            charge = false;
        }
    }

    // Charge detect interrupt
    if (xSemaphoreTake(chargeDetectSemaphore, 0) == pdTRUE) {
        if(DEBUG) Serial.println("Detected Charging thing!");

        // TODO: Prevent inverted state from occuring when lowering voltage when connector is in and then unplugging
        if(charge == true){
            if(digitalRead(PIN_CHRG_EN) == LOW && currentlyCharging == false){ // It's not already on, e.g. we've plugged the cable in
                digitalWrite(PIN_CHRG_EN, HIGH);
                OD_chargingEnabled = true;
                currentlyCharging = true;

            }else{ // The state changed because we removed the connector
                digitalWrite(PIN_CHRG_EN, LOW);
                OD_chargingEnabled = false;
                currentlyCharging = false;
            }
        }
    }

    if(charge == false){
        digitalWrite(PIN_CHRG_EN, LOW);
        OD_chargingEnabled = false;
        //currentlyCharging = false; -- Don't do this here, this gives us hysteresis without this and fixes bug that when we lower voltage and remove plug it turns on
    }
}

// Start main loop for thread
/**
 * @brief The main loop for Core 1 thread
 * Initialises various variables
 * Discovers CellMen
 * Quick Sorts discovered CellMen into voltage ascending over
 * Begins main loop of:
 * Updating CellMen Data
 * Handling Charging
 * Short delay to avoid triggering the Watchdog built into the ESP due to overloading Core1 
 */
void Core1::start() {
    if (DEBUG) printf("Core 1: Starting");
    ///// Initial Functions
    unsigned char* tempCellData;
    /* We know this because in the PacMan.ino file we start
     the ESP32 with charging false and this occurs before we start checking */
    currentlyCharging = false; 

    // Not really needed to zero the array of structures but why not
    for (int i = 0; i < 16; i++) {
        addressVoltages[i].address              = 0x00;
        addressVoltages[i].addressMinusVoltage  = 0;
        cellPositions[i]=i;
        minusTerminalVoltages[i]=0;
    }
    if (DEBUG) printf("Core 1: Initalised & Zeroed AddressVoltages Array");

    // Get all CellMan Addresses - loop discovering until we get some devices to prevent crashing of the CPU
    numberOfDiscoveredCellMen = 0;
    if (DEBUG) printf("Core 1: Beginning Discovery of CellMen on I2C Bus");
    while (numberOfDiscoveredCellMen == 0) {
        if(DEBUG) Serial.println("In the while loop, looking for CellMen");
        numberOfDiscoveredCellMen = discoverCellMen();
    }
    if (DEBUG) printf("Core 1: Finished Discoverying CellMen");

    if (DEBUG) {
        Serial.print("Core 1: The number of address found: ");
        Serial.println(numberOfDiscoveredCellMen);
    }


    // Put together addressVoltages array by requesting data from each cellman
    if (DEBUG) printf("Core 1: Collecting Initial Data from CellMen for Location Calculation");
    for (int i = 0; i < numberOfDiscoveredCellMen; i++) {
        tempCellData = requestDataFromSlave(addresses[i], i, true);

        addressVoltages[i].address = addresses[i];
        addressVoltages[i].addressMinusVoltage = (uint16_t)((tempCellData[6] << 8) + tempCellData[5]);
        if(DEBUG){
            Serial.print("Cell 1: Address minus voltage for address ");
            Serial.print(addressVoltages[i].address);
            Serial.print(": ");
            Serial.println(addressVoltages[i].addressMinusVoltage);
        }
    }

    if (DEBUG) printf("Core 1: Quicksorting the AddressVoltage Array");
    // for (int i = 0; i < numberOfDiscoveredCellMen; i++)
    // {
    //     Serial.print("Index ");
    //     Serial.print(i);
    //     Serial.print(": ");
    //     Serial.print(addressVoltages[i].address);
    //     Serial.print(", ");
    //     Serial.println(addressVoltages[i].addressMinusVoltage);
    // }

    // Sort the addressVoltages by ascending voltages - Wow this bug fix took FOREVER, forgot the -1 (haha jouny) after the numberOfDiscoveredCellMen oof
    addressVoltageQuickSort(addressVoltages, 0, numberOfDiscoveredCellMen - 1);

    // Serial.println("After quicksort:");
    // for (int i = 0; i < numberOfDiscoveredCellMen; i++)
    // {
    //     Serial.print("Index ");
    //     Serial.print(i);
    //     Serial.print(": ");
    //     Serial.print(addressVoltages[i].address);
    //     Serial.print(", ");
    //     Serial.println(addressVoltages[i].addressMinusVoltage);
    // }

    // Flash the LEDs on each CellMen in position order
    if (DEBUG) printf("Core 1: Flashing CellMen in Detected Order");
    indicateCellMen();
    
    ///// Main Loop
    if (DEBUG) printf("Core 1: Entering Main Loop");
    for (;;) {
        // Interrupt based updating of CellMen data and OD
        updateCellMenData();

        // Interrupt based charging detect method
        handleCharging();

        // High Priority Main Loop Code Here -- If empty put a fucking delay you faff
        delay(10);
    }
}
