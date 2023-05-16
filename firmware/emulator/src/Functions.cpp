/*
 * Definitions.h
 *
 *  Created on: 15.12.2020
 *      Author: Sayedsepehr Mosavat
 *      University of Duisburg-Essen
 *      Networked Embedded Systems (NES)
 */

#include "Arduino.h"
#include "Functions.h"

extern MCP4822 dac;

int calculateDACvalueForOCVoltageEmulation(int _OCVoltageForEmulation)
{
	double dacVoltage = (double)_OCVoltageForEmulation / 3; // TODO magic number
	dacVoltage /= 1000.;
//	dacVoltage /= MAXIMUM_DAC_VOLTAGE;
//	Serial.println(dacVoltage*4095);
//	return (int)(dacVoltage * 4095);
	return (int)dacVoltage;
}

int calculateDACvalueForSCCurrentEmulation(int _SCCurrentForEmulation)
{
	double dacVoltage = (double)_SCCurrentForEmulation / 1000.;
	dacVoltage *= (double)CURRENT_EMULATION_RANGE_RESISTOR;
//	dacVoltage /= MAXIMUM_DAC_VOLTAGE;
//	return (int)(dacVoltage * 4095) + CURREN_EMULATION_DAC_OFFSET;
	return (int)dacVoltage + CURREN_EMULATION_DAC_OFFSET;

}

void emulateVoltageAndCurrent(int _OCVoltage, int _SCCurrent)
{
	dac.setVoltageA(calculateDACvalueForOCVoltageEmulation(_OCVoltage));
	dac.setVoltageB(calculateDACvalueForSCCurrentEmulation(_SCCurrent));
	dac.updateDAC();

//	analogWrite(OC_VOLTAGE_EMULATION_DAC_PIN, calculateDACvalueForOCVoltageEmulation(_OCVoltage));
//	analogWrite(SC_CURRENT_EMULATION_DAC_PIN, calculateDACvalueForSCCurrentEmulation(_SCCurrent));

}

void initializeOutputToZero()
{
	dac.setVoltageA(0);
	dac.setVoltageB(0);
	dac.updateDAC();

//	analogWrite(OC_VOLTAGE_EMULATION_DAC_PIN, 0);
//	analogWrite(SC_CURRENT_EMULATION_DAC_PIN, 0);
}


