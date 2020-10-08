/*
 * Definitions.h
 *
 *  Created on: 02.09.2020
 *      Author: Sayedsepehr Mosavat
 *      University of Duisburg-Essen
 *      Networked Embedded Systems (NES)
 */

#ifndef DEFINITIONS_H_
#define DEFINITIONS_H_

//#define DEBUG_MODE

#define NUMBER_OF_CAPUTURED_POINTS_IN_CURVE 40
#define DIGITAL_POT_VALUE_LOOKUP_TABLE digitalPotValueLookupTable_40_points



#define DIGITAL_POT_CS_PIN 9
#define DIGITAL_POT_WIPER0_ADDRESS 0

#define HARVESTER_CAPTURING_STATUS_PIN 6

#define VCC_VOLTAGE 3.33
#define ADC_REFERENCE_VOLTAGE 3.33
#define CURRENT_SENSE_PGA_GAIN 4
#define CURRENT_SENSE_RESISTOR_VALUE 820

#define ADC_VOLTAGE_DIVIDER_USED
#define ADC_VOLTAGE_DIVIDER_R1 13000 // High-side resistor
#define ADC_VOLTAGE_DIVIDER_R2 11000 // Low-side resistor
#define ADC_VOLTAGE_DIVIDER_CONVERSION_FACTOR (((double)(ADC_VOLTAGE_DIVIDER_R2 + ADC_VOLTAGE_DIVIDER_R1) / (ADC_VOLTAGE_DIVIDER_R2)))

const short digitalPotValueLookupTable_40_points[] = {
		0, 1, 2, 3, 4, 5, 6, 7,
		8, 10, 12, 14, 16, 18, 20, 22,
		24, 28, 32, 36, 40, 44, 48, 52,
		54, 56, 60, 65, 70, 75, 80, 85,
		90, 95, 100, 105, 110, 115, 120, 128
};


#endif /* DEFINITIONS_H_ */
