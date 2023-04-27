/*
 * HelperFunctions.cpp
 *
 *  Created on: 02.09.2020
 *      Author: Sayedsepehr Mosavat
 *      University of Duisburg-Essen
 *      Networked Embedded Systems (NES)
 */

#include <SPI.h>
#include <Encoder.h>
#include <ADC.h>
#include <ADC_util.h>
#include "Functions.h"
#include <SD.h>
#include <TimeLib.h>
#include <Definitions.h>

#define NUM_OF_CONFIGLINES 7
// See: https://forum.pjrc.com/threads/60696-Teensy-4-software-reset
static void resetFunc(void)
{
	SCB_AIRCR = 0x05FA0004;
}

ADC *adc = new ADC();

uint8_t ivCurveSequenceNumber = 0;

static char filename[80];

static String harvesting_info[NUM_OF_CONFIGLINES];

static unsigned int lastindex;

// Initialize this value unequal to zero. It is set in readConfigFile()
static time_t fileRecDuration_s = 60;

float shortToVoltage(short _voltage)
{
	float returnValue;
	returnValue = (float)_voltage/255;
	return returnValue*VCC_VOLTAGE;
}

int getVoltageFromAdcValue(int _adcValue)
{
	double returnValue;
	if (_adcValue < 0)
	{
		returnValue = 0;
	}
	else
	{
		returnValue = (double)_adcValue / 65535; // 16-bit ADC resolution
		returnValue *= ADC_REFERENCE_VOLTAGE;

#ifdef ADC_VOLTAGE_DIVIDER_USED
		returnValue *= ADC_VOLTAGE_DIVIDER_CONVERSION_FACTOR;
#endif

		returnValue *= 1000000; // Value in uV
	}
	return (int)returnValue;
}

int getCurrentFromAdcValue(int _adcValue)
{
	double returnValue;
	if(_adcValue < 0)
	{
		returnValue = 0;
	}
	else
	{
		returnValue = (double)_adcValue / 65535; // 16-bit ADC resolution
		returnValue *= ADC_REFERENCE_VOLTAGE; // VACC
		returnValue *= 1000000; // Value in uA

		returnValue /= CURRENT_SENSE_AMPLIFIER_GAIN;
		returnValue /= CURRENT_SENSE_SHUNT_RESISTOR_VALUE;
	}

	returnValue = (int)(returnValue * CURRENT_SENSE_CALIBRATION_FACTOR) + CURRENT_SENSE_CALIBRATION_OFFSET;
	return (returnValue < 0) ? 0 : returnValue;
}

void convertIntValuesToByteArrays(uint8_t _sequence_number, int _voltage, int _current, byte* _buffer)
{
	_buffer[0] = 0xaa; // Start byte
	_buffer[1] = _sequence_number; // TODO: IV curve point sequence number
	_buffer[10] = 0x55; // Finish byte

	_buffer[2] = (_voltage >> 0) & 0xff;
	_buffer[3] = (_voltage >> 8) & 0xff;
	_buffer[4] = (_voltage >> 16) & 0xff;
	_buffer[5] = (_voltage >> 24) & 0xff;

	_buffer[6] = (_current >> 0) & 0xff;
	_buffer[7] = (_current >> 8) & 0xff;
	_buffer[8] = (_current >> 16) & 0xff;
	_buffer[9] = (_current >> 24) & 0xff;
}

void initializeADC()
{
	// Initialize the current-sense ADC
	adc->adc0->setAveraging(32); // set number of averages
	adc->adc0->setResolution(16); // set bits of resolution
	adc->adc0->setConversionSpeed(ADC_CONVERSION_SPEED::VERY_HIGH_SPEED); // change the conversion speed
	adc->adc0->setSamplingSpeed(ADC_SAMPLING_SPEED::VERY_HIGH_SPEED); // change the sampling speed
	adc->adc0->setReference(ADC_REFERENCE::REF_3V3);

	// Initialize the voltage-sense ADC
	adc->adc1->setAveraging(32); // set number of averages
	adc->adc1->setResolution(16); // set bits of resolution
	adc->adc1->setConversionSpeed(ADC_CONVERSION_SPEED::VERY_HIGH_SPEED); // change the conversion speed
	adc->adc1->setSamplingSpeed(ADC_SAMPLING_SPEED::VERY_HIGH_SPEED); // change the sampling speed
	adc->adc1->setReference(ADC_REFERENCE::REF_3V3);
}

void updateHarvesterLoad()
{
#ifdef CALIBRATION_MODE
	analogWrite(LOAD_MOSFET_DAC_PIN, CALIBRATION_MODE_LOAD_MOSFET_VALUE);
#else
	ivCurveSequenceNumber++;
	if( ivCurveSequenceNumber >= NUMBER_OF_CAPTURED_POINTS_IN_CURVE )
	{
		ivCurveSequenceNumber = 0;
	}
	analogWrite(LOAD_MOSFET_DAC_PIN, LOAD_MOSFET_DAC_VALUES_LOOKUP_TABLE[ivCurveSequenceNumber] + LOAD_MOSFET_DAC_VALUES_LUT_OFFSET);
#endif
}

void startupDelay()
{
	for(int i = 0; i < 30; i++)
	{
		digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
		delay(500);
	}
}
#if STAND_ALONE

int setup_SD()
{
	const int chipSelect = BUILTIN_SDCARD;
	//Check if a SD card is available
	if (!SD.begin(chipSelect))
	{
	    Serial.println("Card failed, or not present");
	    return -1;
	}

	//Create directory for storing data
	if (!SD.exists("/recording_data"))
		SD.mkdir("/recording_data");
	
	//Create file name which isn't used already
	for (unsigned int i = 0; i < UINT_MAX; i++)
	{

		sprintf(filename,"/recording_data/measurement%u.csv",i);

		if (!SD.exists(filename))
		{
			lastindex = i;
			break;
		}
	}

	return 0;
}

int readConfigFile(void)
{
	//read harvesting information from configuration file
	File config_file = SD.open("configuration_file.txt", FILE_READ);
	if (!config_file)
	{
	    Serial.print("The configuration file couldn't be found");
	    return -1;
	}

	for(int i = 0; i < NUM_OF_CONFIGLINES; i++)
	{
		String harvesting_info_temp = config_file.readStringUntil('\n');
		int index_of_sc = harvesting_info_temp.indexOf('=');
		// Store everything after the = sign in harvesting_info_temp
		// and check for DOS or unix line endings

		int strLen = harvesting_info_temp.length();
		
		// harvesting_info_temp.charAt(strLen) is the null terminator '\0'
		if ( harvesting_info_temp.charAt(strLen - 1) == '\r' )
		{
			harvesting_info[i] = harvesting_info_temp.substring(index_of_sc + 1, strLen - 1);
		}
		else
		{
			harvesting_info[i] = harvesting_info_temp.substring(index_of_sc + 1, strLen);
		}

	}

	for(int i = 0; i < NUM_OF_CONFIGLINES; i++)
	{
		Serial.println(harvesting_info[i]);
	}
	fileRecDuration_s = harvesting_info[0].toInt();
	//	harvesting_info[0] = duration 
	//	harvesting_info[1] = Indoor/Outdoor
	//	harvesting_info[2] = Lux
	//	harvesting_info[3] = weather
	//	harvesting_info[4] = Country
	//	harvesting_info[5] = City
	// 	harvesting_info[6] = harvesting source

	config_file.close();
	return 0;

}

time_t createNewFile(void)
{
	sprintf(filename,"/recording_data/measurement%u.csv", lastindex++ );

	File rec_file = SD.open(filename, FILE_WRITE);

	char header_info[800];

	// convert duration to end date
	time_t duration = fileRecDuration_s + now();

	time_t retval = duration;

	int end_day = duration / 86400;
	duration = duration - (86400 * end_day);
	int end_hour = duration / 3600;
	duration = duration - (end_hour * 3600);
	int end_minutes = duration / 60;
	duration = duration - (end_minutes * 60);
	int end_seconds = duration;

	// Store harvesting information in recording file
	sprintf(header_info, "%02d.%02d.%4d;%02d:%02d:%02d;%02d:%02d:%02d;%s;%s;%s;%s;%s;%s", day(), 
			month(), year(), hour(), minute(), second(), end_hour, end_minutes, end_seconds, 
			harvesting_info[1].c_str(), harvesting_info[2].c_str(),
			harvesting_info[3].c_str(), harvesting_info[4].c_str(),
			harvesting_info[5].c_str(), harvesting_info[6].c_str() );
	rec_file.println(header_info);
	rec_file.close();

	return retval;

}

void write_data_to_SD(uint8_t _sequence_number, int _voltage, int _current)
{
	if ( !SD.mediaPresent() )
	{
		while ( !SD.mediaPresent() )
		{
			delay(500);
		}
		resetFunc();	
	}

	// 1 + 2 * sizeof(int) + 2 * ';' + '\n'
	// Writing around 1 + 2 * 4 + 2 * 1 + 1 = 12 byte
	File rec_file = SD.open(filename, FILE_WRITE);
	rec_file.print(_sequence_number);
	rec_file.print(";");
	rec_file.print(_voltage);
	rec_file.print(";");
	rec_file.println(_current);
	rec_file.close();
}

time_t getTeensy3Time()
{
  return Teensy3Clock.get();
}

void setup_time()
{

	setSyncProvider(getTeensy3Time);

	if (timeStatus()!= timeSet) {
	    Serial.println("Unable to sync with the RTC");
	  } else {
	    Serial.println("RTC has set the system time");
	  }
}

#endif
