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

// global variable
emulation_t emu_parameters;

static String emulating_info[NUM_OF_CONFIGLINES];

static File config_file;

static int calculateDACvalueForOCVoltageEmulation(int _OCVoltageForEmulation_uV);
static int calculateDACvalueForSCCurrentEmulation(int _SCCurrentForEmulation);

extern void emulateVoltageAndCurrent(int _OCVoltage_uV, int _SCCurrent_uA)
{
	dac.setVoltageA(calculateDACvalueForOCVoltageEmulation(_OCVoltage_uV));
	dac.setVoltageB(calculateDACvalueForSCCurrentEmulation(_SCCurrent_uA));
	dac.updateDAC();

}

extern void initializeOutputToZero(void)
{
	dac.setVoltageA(0);
	dac.setVoltageB(0);
	dac.updateDAC();
}

extern uint8_t setupSD(void)
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
		Serial.println("SD card not configured properly (no directory with name *emulating_data* present)");
		return 0;
	}

	if (config_file)
	{
		config_file.close();
	}
    File config_dir = SD.open("/emulating_data");

    // Read emulator_file
	config_file = config_dir.openNextFile(FILE_READ);
    //SD.open(filename, FILE_READ);

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
    // Give a little startup boost
    emulateVoltageAndCurrent(2500000ul, 1500ul);
    delay(10);
	// emulating_info[0] = number of curves
	// emulating_info[1] = duration between each curve
	emu_parameters.number_curves = (emulating_info[0].toInt());
#ifdef DEBUG
	Serial.println("number_curves " + String(emu_parameters.number_curves));
#endif
	emu_parameters.emu_duration = (emulating_info[1].toFloat()) * 1000.0;
#ifdef DEBUG
	Serial.println("duration " + String(emu_parameters.emu_duration) + "ms");
#endif

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

#ifdef DEBUG
            Serial.print("Read: ");
            Serial.println(emulating_info_temp);
#endif
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

    // Look for separator
	index_of_sc = emulating_info_temp.indexOf(';');

    if (index_of_sc < 0)
    {
        // Try with comma
        index_of_sc = emulating_info_temp.indexOf(',');
    }

    if (index_of_sc < 0)
    {
			digitalWrite(ERROR_LED, HIGH);
            emu_parameters.emu_voltage = 0;
            emu_parameters.emu_current = 0;
#ifdef DEBUG
				Serial.println("Couldn't read from file");
#endif
            return;
    }

	emu_parameters.emu_voltage = (emulating_info_temp.substring(0, index_of_sc)).toInt();

	emu_parameters.emu_current = (emulating_info_temp.substring(index_of_sc + 1, emulating_info_temp.length())).toInt();
}

static int calculateDACvalueForOCVoltageEmulation(int _OCVoltageForEmulation_uV)
{
	double dacVoltage = (double)_OCVoltageForEmulation_uV / 3000.0; // Factor 3 is produced by voltage divider in circuit
	 // dacVoltage /= 1000.0;									// Convert to mV
	//	dacVoltage /= MAXIMUM_DAC_VOLTAGE;
	//	Serial.println(dacVoltage*4095);
	//	return (int)(dacVoltage * 4095);
	return (int)dacVoltage;
}

static int calculateDACvalueForSCCurrentEmulation(int _SCCurrentForEmulation)
{
	double dacVoltage = (double)_SCCurrentForEmulation * (double)CURRENT_EMULATION_RANGE_RESISTOR;
	dacVoltage = (double) dacVoltage / 1000.0;
	//	dacVoltage /= MAXIMUM_DAC_VOLTAGE;
	//	return (int)(dacVoltage * 4095) + CURREN_EMULATION_DAC_OFFSET;
	return (int)dacVoltage + CURRENT_EMULATION_DAC_OFFSET;
}
