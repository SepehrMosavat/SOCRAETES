/*
 * Definitions.h
 *
 *  Created on: 15.12.2020
 *      Author: Sayedsepehr Mosavat
 *      University of Duisburg-Essen
 *      Networked Embedded Systems (NES)
 */

#ifndef FUNCTIONS_H_
#define FUNCTIONS_H_

#include "Definitions.h"
#include "MCP48xx.h"

int calculateDACvalueForOCVoltageEmulation(int);

int calculateDACvalueForSCCurrentEmulation(int);

void emulateVoltageAndCurrent(int, int);

void initializeOutputToZero();

int setupSD();

typedef struct emulation_s {
    int* emu_voltage;
    int* emu_current;
    float emu_duration;
    int number_curves; 
}emulation_t;

extern emulation_t emu_parameters;


#endif /* FUNCTIONS_H_ */
