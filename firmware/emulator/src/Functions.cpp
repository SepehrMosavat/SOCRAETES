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
#include <SD.h>
#include <TimeLib.h>
#define NUM_OF_CONFIGLINES 2

emulation_t emu_parameters;

static String emulating_info[NUM_OF_CONFIGLINES];

static File config_file;

int calculateDACvalueForOCVoltageEmulation(int _OCVoltageForEmulation)
{
	double dacVoltage = (double)_OCVoltageForEmulation / 3; // Factor 3 is produced by voltage divider in circuit
	dacVoltage /= 1000.;									// Convert to mV
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
	return (int)dacVoltage + CURRENT_EMULATION_DAC_OFFSET;
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

uint8_t setupSD()
{
	const int chipSelect = BUILTIN_SDCARD;
	// Check if a SD card is available
	if (!SD.begin(chipSelect))
	{
		Serial.println("Card failed, or not present");
		return 0;
	}
	// Check if directory with emulated data exists
	if (!SD.exists("/emulating_data"))
	{
		Serial.println("SD card not configured properly (no directory with correct name present)");
		return 0;
	}

	const char *filename = "emulating_data/emulator_file.txt";

	if (config_file)
	{
		config_file.close();
	}

	config_file = SD.open(filename, FILE_READ);

	if (!config_file)
	{
		Serial.println("The emulator file couldn't be found");
		return 0;
	}

	for (int i = 0; i < NUM_OF_CONFIGLINES; i++)
	{
		String emulating_info_temp = config_file.readStringUntil('\n');
		int index_of_eq = emulating_info_temp.indexOf('=');

		// Store everything after the = sign in emulating_info_temp
		// and check for DOS or unix line endings

		int strLen = emulating_info_temp.length();

		// harvesting_info_temp.charAt(strLen) is the null terminator '\0'
		if (emulating_info_temp.charAt(strLen - 1) == '\r')
		{
			emulating_info[i] = emulating_info_temp.substring(index_of_eq + 1, strLen - 1);
		}
		else
		{
			emulating_info[i] = emulating_info_temp.substring(index_of_eq + 1, strLen);
		}
	}
	// emulating_info[0] = number of curves
	// emulating_info[1] = duration between each curve
	emu_parameters.number_curves = (emulating_info[0].toInt());
	Serial.println("number_curves " + String(emu_parameters.number_curves));
	emu_parameters.emu_duration = (emulating_info[1].toFloat()) * 1000.0;
	Serial.println("duration " + String(emu_parameters.emu_duration) + "ms");

	return 1;
}

extern void updateEmulationValues(void)
{
	int index_of_sc;

	if (!config_file)
	{
		return;
	}
	// Read one line of the file and update voltage and current
	String emulating_info_temp = config_file.readStringUntil('\n');

	// Check for DOS or unix line endings

	int strLen = emulating_info_temp.length();

	// harvesting_info_temp.charAt(strLen) is the null terminator '\0'
	if (emulating_info_temp.charAt(strLen - 1) == '\r')
	{
		emulating_info_temp = emulating_info_temp.substring(0, strLen - 1);
	}
	else
	{
		emulating_info_temp = emulating_info_temp.substring(0, strLen);
	}

	index_of_sc = emulating_info_temp.indexOf(';');

	emu_parameters.emu_voltage = (emulating_info_temp.substring(0, index_of_sc)).toInt();

	emu_parameters.emu_current = (emulating_info_temp.substring(index_of_sc + 1, emulating_info_temp.length())).toInt();
}
