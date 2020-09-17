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

#define NUMBER_OF_CAPUTURED_POINTS 129

#define DAC_OUTPUT_PIN A14

#define channelA 2
#define channelB 3


#define DIGITAL_POT_CS_PIN 9
#define DIGITAL_POT_WIPER0_ADDRESS 0

#define HARVESTER_CAPTURING_STATUS_PIN 6

#define VCC_VOLTAGE 3.33
#define ADC_REFERENCE_VOLTAGE 3.33
#define CURRENT_SENSE_RESISTOR_VALUE 820
#define DAC_VOLTAGE_STEP 0.05

#define ADC_VOLTAGE_DIVIDER_USED
#define ADC_VOLTAGE_DIVIDER_R1 13000 // High-side resistor
#define ADC_VOLTAGE_DIVIDER_R2 11000 // Low-side resistor
#define ADC_VOLTAGE_DIVIDER_CONVERSION_FACTOR (((double)(ADC_VOLTAGE_DIVIDER_R2 + ADC_VOLTAGE_DIVIDER_R1) / (ADC_VOLTAGE_DIVIDER_R2)))

const short digitalPotValueLookupTable[] = {
		0, 2
};


#endif /* DEFINITIONS_H_ */
