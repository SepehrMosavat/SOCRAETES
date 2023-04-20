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
#define STAND_ALONE 1

#ifdef DEBUG_MODE
#define CALIBRATION_MODE
#define CALIBRATION_MODE_LOAD_MOSFET_VALUE 4095
#endif

// Teensy 3.6 Board Pin Definitions
#define HARVESTER_VOLTAGE_ADC_PIN A2
#define HARVESTER_CURRENT_ADC_PIN A3
#define LOAD_MOSFET_DAC_PIN A21

// System-wide Parameter Definitions
#define NUMBER_OF_CAPUTURED_POINTS_IN_CURVE 40
#define LOAD_MOSFET_DAC_VALUES_LOOKUP_TABLE loadMosfetDACValuesLUT_40_points_2

#define VCC_VOLTAGE 3.33
#define ADC_REFERENCE_VOLTAGE 3.3

// For choosing the suitable gain for the current sense amplifier
//#define CURRENT_SENSE_AMPLIFIER_GAIN_RESISTOR 49.9 // Gain = 1003
//#define CURRENT_SENSE_AMPLIFIER_GAIN_RESISTOR 100 // Gain = 501
#define CURRENT_SENSE_AMPLIFIER_GAIN_RESISTOR 511 // Gain = 99
//#define CURRENT_SENSE_AMPLIFIER_GAIN_RESISTOR 1020 // Gain = 50
//#define CURRENT_SENSE_AMPLIFIER_GAIN_RESISTOR 2610 // Gain = 20

#define CURRENT_SENSE_SHUNT_RESISTOR_VALUE 0.5 // For modifying the measurement range

#define CURRENT_SENSE_AMPLIFIER_GAIN (1 + (50000 / CURRENT_SENSE_AMPLIFIER_GAIN_RESISTOR))

/* These values are meant for accounting for component tolerances of the current sensing circuit.
 * Care must be taken that the current readings do not get negative as a result of these values.
 */
#define CURRENT_SENSE_CALIBRATION_FACTOR 0.95
#define CURRENT_SENSE_CALIBRATION_OFFSET 20

#define ADC_VOLTAGE_DIVIDER_USED
#define ADC_VOLTAGE_DIVIDER_R1 1800 // High-side resistor
#define ADC_VOLTAGE_DIVIDER_R2 1800 // Low-side resistor
#define ADC_VOLTAGE_DIVIDER_CONVERSION_FACTOR (((double)(ADC_VOLTAGE_DIVIDER_R2 + ADC_VOLTAGE_DIVIDER_R1) / (ADC_VOLTAGE_DIVIDER_R2)))

const int loadMosfetDACValuesLUT_40_points_0[] = {
		1500, 1525, 1550, 1575, 1600, 1625, 1650, 1675,
		1700, 1725, 1750, 1775, 1800, 1825, 1850, 1875,
		1900, 1925, 1950, 1975, 2000, 2025, 2050, 2075,
		2100, 2125, 2150, 2175, 2200, 2225, 2250, 2275,
		2300, 2325, 2350, 2375, 2400, 2425, 2450, 2475
};

const int loadMosfetDACValuesLUT_40_points_1[] = {
		1500, 1525, 1550, 1575, 1600, 1625, 1650, 1675,
		1700, 1725, 1750, 1800, 1850, 1900, 1950, 2000,
		2050, 2100, 2150, 2200, 2250, 2300, 2350, 2400,
		2450, 2500, 2550, 2600, 2650, 2700, 2750, 2800,
		2850, 2900, 2950, 3000, 3050, 3100, 3150, 3200
};

const int loadMosfetDACValuesLUT_40_points_2[] = {
		1630, 1660, 1690, 1720, 1750, 1780, 1810, 1840,
		1870, 1900, 1930, 1960, 1991, 2021, 2050, 2080,
		2110, 2140, 2170, 2200, 2230, 2260, 2290, 2310,
		2340, 2370, 2400, 2430, 2460, 2490, 2520, 2550,
		2580, 2610, 2640, 2670, 2700, 2730, 2765, 2800
};

#define LOAD_MOSFET_DAC_VALUES_LUT_OFFSET 50 // This offset is used for fine-tuning the gate voltage of the MOSFETs

#endif /* DEFINITIONS_H_ */
