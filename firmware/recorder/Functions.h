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

int getVoltageFromAdcValue(int);

int getCurrentFromAdcValue(int);

void convertIntValuesToByteArrays(unsigned short, int, int, byte*);

void initializeADC();

void updateHarvesterLoad();

void startupDelay();

int setup_SD();

void write_data_to_SD(unsigned short, int, int);

void setup_time();


#endif /* FUNCTIONS_H_ */
