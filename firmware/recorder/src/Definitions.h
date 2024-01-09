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
//#define DEBUG_MODE

// System-wide Parameter Definitions

#define NUMBER_OF_CAPTURED_POINTS_IN_CURVE 40

#define CURRENT_SENSE_SHUNT_RESISTOR_VALUE 2.0 // For modifying the measurement range

//#define CURRENT_SENSE_AMPLIFIER_GAIN (1.0 + (50000.0 / CURRENT_SENSE_AMPLIFIER_GAIN_RESISTOR))
//#define CURRENT_SENSE_AMPLIFIER_GAIN 1003.0 // R_G = 49.9
//#define CURRENT_SENSE_AMPLIFIER_GAIN 501.0  // R_G = 100.0
//#define CURRENT_SENSE_AMPLIFIER_GAIN 200.0  // R_G = 249.0
//#define CURRENT_SENSE_AMPLIFIER_GAIN 99.0   // R_G = 511.0
//#define CURRENT_SENSE_AMPLIFIER_GAIN 50.0   // R_G = 1020.0
#define CURRENT_SENSE_AMPLIFIER_GAIN 25.0   // R_G = 2100.0

#endif /* DEFINITIONS_H_ */
