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


#include "Definitions.h"
#include <Arduino.h>

float shortToVoltage(short);

int getVoltageFromAdcValue(void);

int getCurrentFromAdcValue(void);

/* Transmits sequence number, voltage and current as a byte array 
 * via serial
 */
void transmitValuesAsByteArray(uint8_t SeqNo, int* voltage, int* current, unsigned int  _transferredBytes);

void initializeADC();

void updateHarvesterLoad(uint8_t SeqNo);

void startupDelay();

int setupSD();

void setupTime();

int readConfigFile(void);

/* Returns the timestamp when to stop recording to the created file */
time_t createNewFile(void);

void writeDataToSD(uint8_t _sequence_number, int _voltage, int _current);

uint32_t modeSelection(int _mode);

int  calculateMosfetValues();
#endif /* FUNCTIONS_H_ */
