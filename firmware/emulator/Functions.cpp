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


int calculateDACvalueForOCVoltageEmulation(int _OCVoltageForEmulation)
{
	double dacVoltage = (double)_OCVoltageForEmulation / 3; // TODO magic number
	dacVoltage /= 1000000.;
	dacVoltage /= MAXIMUM_DAC_VOLTAGE;
//	Serial.println(dacVoltage*4095);
	return (int)(dacVoltage * 4095);
}

int calculateDACvalueForSCCurrentEmulation(int _SCCurrentForEmulation)
{
	double dacVoltage = (double)_SCCurrentForEmulation / 1000000;
	dacVoltage *= 51;
	dacVoltage /= MAXIMUM_DAC_VOLTAGE;
	return (int)(dacVoltage * 4095);

}

void emulateVoltageAndCurrent(int _OCVoltage, int _SCCurrent)
{
	analogWrite(OC_VOLTAGE_EMULATION_DAC_PIN, calculateDACvalueForOCVoltageEmulation(_OCVoltage));
	analogWrite(SC_CURRENT_EMULATION_DAC_PIN, calculateDACvalueForSCCurrentEmulation(_SCCurrent));

}

void initializeOutputToZero()
{
	analogWrite(OC_VOLTAGE_EMULATION_DAC_PIN, 0);
	analogWrite(SC_CURRENT_EMULATION_DAC_PIN, 0);
}


