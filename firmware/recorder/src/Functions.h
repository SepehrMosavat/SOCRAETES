/*
 * HelperFunctions.h
 *
 *  Created on: 02.09.2020
 *      Author: Sayedsepehr Mosavat
 *      University of Duisburg-Essen
 *      Networked Embedded Systems (NES)
 */

#ifndef FUNCTIONS_H_
#define FUNCTIONS_H_

/////////////////////////////////////////////////////////////INCLUDES///////////////////////////////////////////////////////////

#include "Definitions.h"
#include <Arduino.h>

/////////////////////////////////////////////////////////////DEFINES////////////////////////////////////////////////////////////

#define PIN_STATUS_LED 1
#define PIN_ERROR_LED 2

/////////////////////////////////////////////////////////////FUNCTIONS//////////////////////////////////////////////////////////

extern uint16_t mosfetValues[NUMBER_OF_CAPTURED_POINTS_IN_CURVE];
/**
 * Returns the voltage in uV
 * @return voltage in uV
 */
uint32_t getVoltageFromAdcValue_uV(void);

/**
 * Returns the current in uA
 * @return current in uA
 */
uint32_t getCurrentFromAdcValue_uA(void);

/**
 * Transmits sequence number, voltage and current as a byte array via serial
 * @param SeqNo: Sequence number of point in curve
 * @param voltage: Voltage in uV
 * @param current: Current in uA
 */
void transmitValuesAsByteArray(uint8_t SeqNo, uint32_t voltage, uint32_t current);

/**
 * Initializes the ADC
 */
void initializeADC(void);

/**
 * Function to set the output voltage of the DAC
 * @param SeqNo: Sequence number, to look up the corresponding DAC value in mosfetValues[]
 */
void updateHarvesterLoad(uint8_t SeqNo);

/**
 * Function to toggle LED's if in callibration mode
 */
void startupDelay(void);

/////////////////////////////////////////////////////////////SD FUNCTIONS//////////////////////////////////////////////////////

/**
 * Function to setup SD and folder structure
 * @return 0 if successful, -1 if not
 */
int setupSD(void);

/**
 * Function to read the configuration file
 * @return 0 if successful, -1 if not
 */
int readConfigFile(void);

/**
 * Function to create a new file
 * @return Time when new file needs to be created => current time + defined recording time
 */
time_t createNewFile(void);

/**
 * Function to write data to SD
 * @param _sequence_number: Sequence number of point in curve
 * @param _voltage: Voltage in uV
 * @param _current: Current in uA
 */
void writeDataToSD(uint8_t _sequence_number, uint32_t _voltage, uint32_t _current);

/**
 * Sync system time with RTC
 */
void setupTime(void);

/**
 * Calculate the mosfet values for the curve of the harvester
 * uses a sigmoid function to calculate the values
 */
void calcCurve(void);

/**
 */
void initDAC(void);

void turnOffDAC(void);

#endif /* FUNCTIONS_H_ */
