/*
 * Definitions.h
 *
 *  Created on: 15.12.2020
 *      Author: Sayedsepehr Mosavat
 *      University of Duisburg-Essen
 *      Networked Embedded Systems (NES)
 */

#ifndef DEFINITIONS_H_
#define DEFINITIONS_H_

#define DEBUG

#define STATUS_LED 1
#define ERROR_LED 2
#define MODE_JUMPER 32

#define MAXIMUM_DAC_VOLTAGE 3.3

#define CURRENT_EMULATION_RANGE_RESISTOR 49.9 // For setting the maximum emulation current

/*
 * This offset accounts for the DC bias of the DAC output used for the SC current emulation.
 * At the moment this value needs to be figured out experimentally for each piece of emulator
 * hardware due to the component tolerances.
 */
#define CURRENT_EMULATION_DAC_OFFSET 0

enum SignalCode
{
	START_OF_CURVE_DATA = 0xAA,
	END_OF_CURVE_DATA = 0x55,
};

#endif /* DEFINITIONS_H_ */
