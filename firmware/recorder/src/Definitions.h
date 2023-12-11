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

// In debug mode every measured point of curve is printed via serial
// Cycle times are not taken into account
// #define DEBUG_MODE

#define MODE_SD 0

#ifdef DEBUG_MODE
// define CALIBRATION_MODE if required
// #define CALIBRATION_MODE
#define CALIBRATION_MODE_LOAD_MOSFET_VALUE 4095
#endif

#define STATUS_LED 1
#define ERROR_LED 2
#define MODE_JUMPER 32

// Teensy 4.1 Board Pin Definitions

#define HARVESTER_VOLTAGE_ADC_PIN A15
#define HARVESTER_CURRENT_ADC_PIN A14

// System-wide Parameter Definitions

#define NUMBER_OF_CAPTURED_POINTS_IN_CURVE 40
#define LOAD_MOSFET_DAC_VALUES_LOOKUP_TABLE loadMosfetDACValuesLUT_40_points_irf1404

#define VCC_VOLTAGE 3.326
#define ADC_REFERENCE_VOLTAGE 3.326

#define ADC_RESOLUTION_BITS 10
#define ADC_MAX 1023.0

// For choosing the suitable gain for the current sense amplifier
// #define CURRENT_SENSE_AMPLIFIER_GAIN_RESISTOR 49.9 // Gain = 1003
// #define CURRENT_SENSE_AMPLIFIER_GAIN_RESISTOR 100 // Gain = 501
// #define CURRENT_SENSE_AMPLIFIER_GAIN_RESISTOR 249 // Gain = 200
#define CURRENT_SENSE_AMPLIFIER_GAIN_RESISTOR 511.0 // Gain = 99
// #define CURRENT_SENSE_AMPLIFIER_GAIN_RESISTOR 1020 // Gain = 50
// #define CURRENT_SENSE_AMPLIFIER_GAIN_RESISTOR 2100 // Gain = 25

#define CURRENT_SENSE_SHUNT_RESISTOR_VALUE 2.0 // For modifying the measurement range

#define CURRENT_SENSE_AMPLIFIER_GAIN (1.0 + (50000.0 / CURRENT_SENSE_AMPLIFIER_GAIN_RESISTOR))

/* These values are meant for accounting for component tolerances of the current sensing circuit.
 * Care must be taken that the current readings do not get negative as a result of these values.
 */
#define CURRENT_SENSE_CALIBRATION_FACTOR 1 // 0.95
#define CURRENT_SENSE_CALIBRATION_OFFSET 0 //(-800) //(20)

#define ADC_VOLTAGE_DIVIDER_USED
#define ADC_VOLTAGE_DIVIDER_R1 1800 // High-side resistor
#define ADC_VOLTAGE_DIVIDER_R2 1800 // Low-side resistor
#define ADC_VOLTAGE_DIVIDER_CONVERSION_FACTOR (((double)(ADC_VOLTAGE_DIVIDER_R2 + ADC_VOLTAGE_DIVIDER_R1) / (ADC_VOLTAGE_DIVIDER_R2)))

#define LOAD_MOSFET_DAC_VALUES_LUT_OFFSET 50 // This offset is used for fine-tuning the gate voltage of the MOSFETs

#endif /* DEFINITIONS_H_ */
