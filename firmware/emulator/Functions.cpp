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
#define NUM_OF_CONFIGLINES 4
static String emulating_info[NUM_OF_CONFIGLINES];
emulation_t emu_parameters;

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
	dacVoltage *= CURRENT_EMULATION_RANGE_RESISTOR;
	dacVoltage /= MAXIMUM_DAC_VOLTAGE;
	return (int)(dacVoltage * 4095) + CURREN_EMULATION_DAC_OFFSET;

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

#ifdef STAND_ALONE

	int setupSD()
	{
		const int chipSelect = BUILTIN_SDCARD;
		//Check if a SD card is available
		if (!SD.begin(chipSelect))
		{
		    Serial.println("Card failed, or not present");
		    return -1;
		}


		//Check if directory with emulated data exists
		if (!SD.exists("/emulating_data"))
		{
			Serial.println("SD card not configured properly (no directory with correct name present)");
			return -1;
		}
		char filename[] ="emulating_data/configuration_file.txt";

		File config_file = SD.open(filename, FILE_READ);
		if (!config_file)
		{
		    Serial.println("The configuration file couldn't be found");
		    return -1;
		}
		 
		for(int i = 0; i < NUM_OF_CONFIGLINES; i++)
		{
			String emulating_info_temp = config_file.readStringUntil('\n');
			int index_of_eq = emulating_info_temp.indexOf('=');

			
			// Store everything after the = sign in emulating_info_temp
			// and check for DOS or unix line endings

			int strLen = emulating_info_temp.length();

			// harvesting_info_temp.charAt(strLen) is the null terminator '\0'
			if ( emulating_info_temp.charAt(strLen - 1) == '\r' )
			{
				emulating_info[i] = emulating_info_temp.substring(index_of_eq + 1, strLen - 1);
			}
			else
			{
				emulating_info[i] = emulating_info_temp.substring(index_of_eq + 1, strLen);
			}

		}
		for(int i = 0; i < NUM_OF_CONFIGLINES; i++)
		{
			Serial.println(emulating_info[i]);
		}
		//  emulating_info[0] = number of curves
		//	emulating_info[1] = duration between each curve
		//	emulating_info[2] = open-circuit voltages
		//	emulating_info[3] = short-circuit currents
		emu_parameters.number_curves = (emulating_info[0].toInt()); 
		emu_parameters.emu_duration = (emulating_info[1].toFloat()) * 1000;
		Serial.println("duration " + String(emu_parameters.emu_duration));
		emu_parameters.emu_voltage = (int*) malloc(emulating_info[0].toInt() *sizeof(int));
		emu_parameters.emu_current = (int*) malloc(emulating_info[0].toInt() *sizeof(int));
		int index_of_sc_vol,index_of_sc_cu;
		int temp_voltage, temp_current;
		for( int i =0; i< emu_parameters.number_curves; i++)
		{
			index_of_sc_vol = emulating_info[2].indexOf(';');
			temp_voltage= (emulating_info[2].substring(0, index_of_sc_vol)).toInt();
			emulating_info[2] = emulating_info[2].substring(index_of_sc_vol + 1);
			index_of_sc_cu = emulating_info[3].indexOf(';');
			temp_current= (emulating_info[3].substring(0, index_of_sc_vol)).toInt();
			emulating_info[3] = emulating_info[3].substring(index_of_sc_cu + 1);
			emu_parameters.emu_voltage[i] =temp_voltage;
			emu_parameters.emu_current[i] = temp_current;

		}
		config_file.close();
		
		return 0;
	} 
#endif


