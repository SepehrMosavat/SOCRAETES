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

#ifdef DEBUG_MODE
#define CALIBRATION_MODE
#define CALIBRATION_MODE_LOAD_MOSFET_VALUE 4095
#endif

#define STATUS_LED 1

// Teensy 4.1 Board Pin Definitions
#define HARVESTER_VOLTAGE_ADC_PIN A15
#define HARVESTER_CURRENT_ADC_PIN A14

// System-wide Parameter Definitions
#define NUMBER_OF_CAPUTURED_POINTS_IN_CURVE 40
#define LOAD_MOSFET_DAC_VALUES_LOOKUP_TABLE loadMosfetDACValuesLUT_40_points_0

#define VCC_VOLTAGE 3.326
#define ADC_REFERENCE_VOLTAGE 3.326

// For choosing the suitable gain for the current sense amplifier
//#define CURRENT_SENSE_AMPLIFIER_GAIN_RESISTOR 49.9 // Gain = 1003
//#define CURRENT_SENSE_AMPLIFIER_GAIN_RESISTOR 100 // Gain = 501
//#define CURRENT_SENSE_AMPLIFIER_GAIN_RESISTOR 249 // Gain = 200
#define CURRENT_SENSE_AMPLIFIER_GAIN_RESISTOR 511 // Gain = 99
//#define CURRENT_SENSE_AMPLIFIER_GAIN_RESISTOR 1020 // Gain = 50
//#define CURRENT_SENSE_AMPLIFIER_GAIN_RESISTOR 2100 // Gain = 25

#define CURRENT_SENSE_SHUNT_RESISTOR_VALUE 2 // For modifying the measurement range

#define CURRENT_SENSE_AMPLIFIER_GAIN (1 + ((double)50000 / CURRENT_SENSE_AMPLIFIER_GAIN_RESISTOR))

/* These values are meant for accounting for component tolerances of the current sensing circuit.
 * Care must be taken that the current readings do not get negative as a result of these values.
 */
#define CURRENT_SENSE_CALIBRATION_FACTOR 1 //0.95
#define CURRENT_SENSE_CALIBRATION_OFFSET 0 //(-800) //(20)

#define ADC_VOLTAGE_DIVIDER_USED
#define ADC_VOLTAGE_DIVIDER_R1 1800 // High-side resistor
#define ADC_VOLTAGE_DIVIDER_R2 1800 // Low-side resistor
#define ADC_VOLTAGE_DIVIDER_CONVERSION_FACTOR (((double)(ADC_VOLTAGE_DIVIDER_R2 + ADC_VOLTAGE_DIVIDER_R1) / (ADC_VOLTAGE_DIVIDER_R2)))

const int loadMosfetDACValuesLUT_40_points_0[] = {
		1100, 1112, 1124, 1136, 1148, 1160, 1172, 1184,
		1196, 1208, 1220, 1232, 1244, 1256, 1268, 1280,
		1292, 1304, 1316, 1328, 1340, 1352, 1364, 1376,
		1388, 1400, 1412, 1424, 1436, 1448, 1460, 1472,
		1484, 1496, 1508, 1520, 1532, 1550, 1570, 1600
};


#define LOAD_MOSFET_DAC_VALUES_LUT_OFFSET 50 // This offset is used for fine-tuning the gate voltage of the MOSFETs

#endif /* DEFINITIONS_H_ */
