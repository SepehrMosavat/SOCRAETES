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

typedef struct emulation_s
{
    int number_curves;
    int emu_voltage;
    int emu_current;
    float emu_duration;
} emulation_t;

extern MCP4822 dac;

extern emulation_t emu_parameters;

extern int calculateDACvalueForOCVoltageEmulation(int);

extern int calculateDACvalueForSCCurrentEmulation(int);

extern void emulateVoltageAndCurrent(int, int);

extern void initializeOutputToZero();

extern uint8_t setupSD();

extern void updateEmulationValues(void);

#endif /* FUNCTIONS_H_ */
