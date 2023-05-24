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
#include "MCP48xx/src/MCP48xx.h"
#include "Definitions.h"
#include <SD.h>
#include <TimeLib.h>

#define NUM_OF_CONFIGLINES 7

static ADC *adc = new ADC();

//start of data used for sd mode only


static char filename[80];

static String harvesting_info[NUM_OF_CONFIGLINES];

static unsigned int lastindex;

// Initialize this value unequal to zero. It is set in readConfigFile()
static time_t fileRecDuration_s = 60;

static void resetFunc(void)
{
	SCB_AIRCR = 0x05FA0004;
}

//end data used for sd mode only

extern MCP4822 dac;

float shortToVoltage(short _voltage)
{
	float returnValue;
	returnValue = (float)_voltage/255;
	return returnValue*VCC_VOLTAGE;
}

int getVoltageFromAdcValue(void)
{
	double returnValue;
	int _adcValue = adc->analogRead(HARVESTER_VOLTAGE_ADC_PIN, ADC_1);
	if (_adcValue < 0)
	{
		returnValue = 0;
	}
	else
	{
		returnValue = (double)_adcValue / (pow(2, ADC_RESOLUTION_BITS) - 1);
		returnValue *= ADC_REFERENCE_VOLTAGE;

#ifdef ADC_VOLTAGE_DIVIDER_USED
		returnValue *= ADC_VOLTAGE_DIVIDER_CONVERSION_FACTOR;
#endif

		returnValue *= 1000000; // Value in uV
	}
	return (int)returnValue;
}

int getCurrentFromAdcValue(void)
{
	double returnValue;
	int _adcValue = adc->analogRead(HARVESTER_CURRENT_ADC_PIN, ADC_1);
	//Serial.println(_adcValue);
	if(_adcValue < 0)
	{
		returnValue = 0;
	}
	else
	{
		returnValue = (double)_adcValue / (pow(2, ADC_RESOLUTION_BITS) - 1);
		returnValue *= ADC_REFERENCE_VOLTAGE; // VACC
		returnValue *= 1000000; // Value in uA

		returnValue /= CURRENT_SENSE_AMPLIFIER_GAIN;
		returnValue /= CURRENT_SENSE_SHUNT_RESISTOR_VALUE;
	}

	returnValue = (int)(returnValue * CURRENT_SENSE_CALIBRATION_FACTOR) + CURRENT_SENSE_CALIBRATION_OFFSET;
	return returnValue;
}

void convertIntValuesToByteArrays(unsigned short _sequence_number, int _voltage, int _current, byte* _buffer)
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

void transmitValuesAsByteArray(uint8_t SeqNo, int voltage, int current)
{
	byte buffer[11];
	buffer[0] = 0xaa; // Start byte
	buffer[1] = SeqNo; 
	buffer[10] = 0x55; // Finish byte

	buffer[2] = (voltage >> 0) & 0xff;
	buffer[3] = (voltage >> 8) & 0xff;
	buffer[4] = (voltage >> 16) & 0xff;
	buffer[5] = (voltage >> 24) & 0xff;

	buffer[6] = (current >> 0) & 0xff;
	buffer[7] = (current >> 8) & 0xff;
	buffer[8] = (current >> 16) & 0xff;
	buffer[9] = (current >> 24) & 0xff;

	for(int i = 0; i < 11; i++)
	{
		Serial.write(buffer[i]);
	}
}

void initializeADC()
{
	// Initialize the current-sense and voltage-sense ADC
	adc->adc1->setAveraging(0); // set number of averages
	adc->adc1->setResolution(ADC_RESOLUTION_BITS); // set bits of resolution
	adc->adc1->setConversionSpeed(ADC_CONVERSION_SPEED::VERY_LOW_SPEED);
	adc->adc1->setSamplingSpeed(ADC_SAMPLING_SPEED::VERY_LOW_SPEED);
}

int i = 1100;
int _first=0, _second=0;
bool calibrated = 0;

void updateHarvesterLoad(uint8_t SeqNo)
{
#ifdef CALIBRATION_MODE
	dac.setVoltageA(CALIBRATION_MODE_LOAD_MOSFET_VALUE);
	dac.updateDAC();
	delay(100);
	// analogWrite(LOAD_MOSFET_DAC_PIN, CALIBRATION_MODE_LOAD_MOSFET_VALUE);

	//		analogWrite(LOAD_MOSFET_DAC_PIN, i);

	/*
	int currentCalibrationValue = 0;

	long long sum = 0;
	if(!calibrated) {
		dac.setVoltageA(0);
		dac.updateDAC();
		delay(100);
		for(int i = 0; i < 500; i++) {
			int temp = adc->analogRead(HARVESTER_CURRENT_ADC_PIN, ADC_1);
			sum += temp;
		}
		currentCalibrationValue = sum / 500;
		Serial.printf("Calibration value: %d\n", currentCalibrationValue);
		calibrated = 1;
	}

	dac.setVoltageA(0);
	dac.updateDAC();
	delay(50);

	int currentSenseAdcValue = adc->analogRead(HARVESTER_CURRENT_ADC_PIN, ADC_1) - currentCalibrationValue;
	int voltageAdcValue = adc->analogRead(HARVESTER_VOLTAGE_ADC_PIN, ADC_1);

	int current = getCurrentFromAdcValue(currentSenseAdcValue);
	int voltage = getVoltageFromAdcValue(voltageAdcValue);

	//	Serial.printf("V: %d, I: %d\n", voltageAdcValue, currentSenseAdcValue);
	Serial.printf("V: %d, I: %d\n", voltage, current - 300);
	 */
	/*
	dac.setVoltageA(i);
	dac.updateDAC();
	delay(100);

	i += 15;
	//	Serial.printf("i: %d, _first: %d, _second: %d, diff: %d -> ", i, _first, _second, _second-_first);
	//	_second=_first;
	if(i > 1600) {
		i = 1100;
	}
	 */
#else
	dac.setVoltageA(LOAD_MOSFET_DAC_VALUES_LOOKUP_TABLE[SeqNo] + LOAD_MOSFET_DAC_VALUES_LUT_OFFSET);
	dac.updateDAC();

#endif
}

void startupDelay()
{
#ifndef CALIBRATION_MODE
	int ledDelay = 100;

	digitalWrite(ERROR_LED, HIGH);
	for(int i = 0; i < 50; i += 1)
	{
		digitalToggle(STATUS_LED);
		digitalToggle(ERROR_LED);
		delay(ledDelay);
	}
#endif
}

//functions used for sd mode only

int setupSD()
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

void writeDataToSD(uint8_t _sequence_number, int _voltage, int _current)
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

void setupTime()
{

	setSyncProvider(getTeensy3Time);

	if (timeStatus()!= timeSet) {
	    Serial.println("Unable to sync with the RTC");
	  } else {
	    Serial.println("RTC has set the system time");
	  }
}

