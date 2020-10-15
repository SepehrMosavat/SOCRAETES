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

// Teensy 3.2 Board Pin Definitions
#define HARVESTER_VOLTAGE_ADC_PIN A3
#define HARVESTER_CURRENT_ADC_PIN A1
#define LOAD_MOSFET_DAC_PIN A14

// System-wide Parameter Definitions
#define NUMBER_OF_CAPUTURED_POINTS_IN_CURVE 40
#define LOAD_MOSFET_DAC_VALUES_LOOKUP_TABLE loadMosfetDACValuesLUT_40_points_0

#define VCC_VOLTAGE 3.33
#define ADC_REFERENCE_VOLTAGE 3.3

#define CURRENT_SENSE_AMPLIFIER_GAIN 50
#define CURRENT_SENSE_SHUNT_RESISTOR_VALUE 1

#define ADC_VOLTAGE_DIVIDER_USED
#define ADC_VOLTAGE_DIVIDER_R1 2000000 // High-side resistor
#define ADC_VOLTAGE_DIVIDER_R2 2000000 // Low-side resistor
#define ADC_VOLTAGE_DIVIDER_CONVERSION_FACTOR (((double)(ADC_VOLTAGE_DIVIDER_R2 + ADC_VOLTAGE_DIVIDER_R1) / (ADC_VOLTAGE_DIVIDER_R2)))

const int loadMosfetDACValuesLUT_40_points_0[] = {
		1500, 1525, 1550, 1575, 1600, 1625, 1650, 1675,
		1700, 1725, 1750, 1775, 1800, 1825, 1850, 1875,
		1900, 1925, 1950, 1975, 2000, 2025, 2050, 2075,
		2100, 2125, 2150, 2175, 2200, 2225, 2250, 2275,
		2300, 2325, 2350, 2375, 2400, 2425, 2450, 2475
};

#define LOAD_MOSFET_DAC_VALUES_LUT_OFFSET 50 // This offset is used for fine-tuning the gate voltage of the MOSFETs

#endif /* DEFINITIONS_H_ */
