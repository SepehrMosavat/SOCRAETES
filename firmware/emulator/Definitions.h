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


#define OC_VOLTAGE_EMULATION_DAC_PIN A22
#define SC_CURRENT_EMULATION_DAC_PIN A21

#define MAXIMUM_DAC_VOLTAGE 3.3

#define CURRENT_EMULATION_RANGE_RESISTOR 49.9 // For setting the maximum emulation current

enum SignalCode
{
	START_OF_CURVE_DATA = 0xAA,
	END_OF_CURVE_DATA = 0x55,
};


#endif /* DEFINITIONS_H_ */
