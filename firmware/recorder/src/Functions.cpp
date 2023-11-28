/*
 * HelperFunctions.cpp
 *
 *  Created on: 02.09.2020
 *      Author: Sayedsepehr Mosavat
 *      University of Duisburg-Essen
 *      Networked Embedded Systems (NES)
 */

/////////////////////////////////////////////////////////////INCLUDES///////////////////////////////////////////////////////////

#include <SPI.h>
#include <Encoder.h>
#include <ADC.h>
#include <ADC_util.h>
#include "Functions.h"
#include "MCP48xx.h"
#include "Definitions.h"
#include <SD.h>
#include <TimeLib.h>
#include <usb_serial.h>
#include <math.h>

/////////////////////////////////////////////////////////////DEFINES///////////////////////////////////////////////////////////

#define NUM_OF_CONFIGLINES 7

static ADC *adc = new ADC();

// start of data used for sd mode only

static char filename[128];

static String harvesting_info[NUM_OF_CONFIGLINES];

static unsigned int lastindex;

// Initialize this value unequal to zero. It is set in readConfigFile()
static time_t fileRecDuration_s = 60;

static int mosfetValues[NUMBER_OF_CAPTURED_POINTS_IN_CURVE];

extern MCP4822 dac;
extern time_t endFileRecord_s;

// voltage limit 185000 uV due to inaccurancy of the DAC, value found by testing
#define VOLTAGE_LIMIT_uV 185000
// Current limit 16 uA, value found by testing
#define CURRENT_LIMIT_uA 16

// Curve parameters

#define OC_VOLTAGE 0				   // Open circuit voltage index
#define SC_VOLTAGE 1				   // Short circuit voltage index
static uint voltageLimits[2] = {0, 0}; // Array for storing the voltage limits (OC and SC)

// Values defined with testing
#define SCALE_CURVE 0.95	 // Scale factor for the curve
#define CURVE_STEEPNESS 0.02 // Steepness of the curve
#define MIDPOINT 3200		 // Midpoint of the curve

/////////////////////////////////////////////////////////////FUNCTIONS///////////////////////////////////////////////////////////

static void resetFunc(void)
{
	SCB_AIRCR = 0x05FA0004;
}

// end data used for sd mode only

float shortToVoltage(short _voltage)
{
	float returnValue;
	returnValue = (float)_voltage / 255;
	return returnValue * VCC_VOLTAGE;
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
	// Serial.println(_adcValue);
	if (_adcValue < 0)
	{
		returnValue = 0;
	}
	else
	{
		returnValue = (double)_adcValue / (pow(2, ADC_RESOLUTION_BITS) - 1);
		returnValue *= ADC_REFERENCE_VOLTAGE; // VACC
		returnValue *= 1000000;				  // Value in uA

		returnValue /= CURRENT_SENSE_AMPLIFIER_GAIN;
		returnValue /= CURRENT_SENSE_SHUNT_RESISTOR_VALUE;
	}

	returnValue = (int)(returnValue * CURRENT_SENSE_CALIBRATION_FACTOR) + CURRENT_SENSE_CALIBRATION_OFFSET;
	return returnValue;
}

/**
 * Saves the values of _sequence_number, _voltage and current to a byte array
 * @param _sequence_number: Sequence number of point in curve
 * @param _voltage: Voltage in uV
 * @param _current: Current in uA
 * @param _buffer: Byte array to store the values
 */
void convertIntValuesToByteArrays(unsigned short _sequence_number, int _voltage, int _current, byte *_buffer)
{
	_buffer[0] = 0xaa;			   // Start byte
	_buffer[1] = _sequence_number; // TODO: IV curve point sequence number
	_buffer[10] = 0x55;			   // Finish byte

	_buffer[2] = (_voltage >> 0) & 0xff;
	_buffer[3] = (_voltage >> 8) & 0xff;
	_buffer[4] = (_voltage >> 16) & 0xff;
	_buffer[5] = (_voltage >> 24) & 0xff;

	_buffer[6] = (_current >> 0) & 0xff;
	_buffer[7] = (_current >> 8) & 0xff;
	_buffer[8] = (_current >> 16) & 0xff;
	_buffer[9] = (_current >> 24) & 0xff;
}

void transmitValuesAsByteArray(uint8_t SeqNo, int *voltage, int *current, unsigned int _transferredBytes)
{
	//_transferredBytes=6 for max speed, see https://www.pjrc.com/teensy/usb_serial.html
	byte buffer[66] = {0};
	for (unsigned i = 0; i < _transferredBytes; i += 1)
	{
		buffer[0 + 11 * i] = 0xaa; // Start byte
		buffer[1 + 11 * i] = SeqNo + i;
		buffer[10 + 11 * i] = 0x55; // Finish byte
		buffer[2 + 11 * i] = (voltage[SeqNo + i] >> 0) & 0xff;
		buffer[3 + 11 * i] = (voltage[SeqNo + i] >> 8) & 0xff;
		buffer[4 + 11 * i] = (voltage[SeqNo + i] >> 16) & 0xff;
		buffer[5 + 11 * i] = (voltage[SeqNo + i] >> 24) & 0xff;
		buffer[6 + 11 * i] = (current[SeqNo + i] >> 0) & 0xff;
		buffer[7 + 11 * i] = (current[SeqNo + i] >> 8) & 0xff;
		buffer[8 + 11 * i] = (current[SeqNo + i] >> 16) & 0xff;
		buffer[9 + 11 * i] = (current[SeqNo + i] >> 24) & 0xff;
	}
	usb_serial_write((void *)buffer, 66 * sizeof(byte));
}

void initializeADC()
{
	// Initialize the current-sense and voltage-sense ADC
	adc->adc1->setAveraging(0);					   // set number of averages
	adc->adc1->setResolution(ADC_RESOLUTION_BITS); // set bits of resolution
	adc->adc1->setConversionSpeed(ADC_CONVERSION_SPEED::VERY_LOW_SPEED);
	adc->adc1->setSamplingSpeed(ADC_SAMPLING_SPEED::VERY_LOW_SPEED);
}

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
	dac.setVoltageA(mosfetValues[SeqNo]);
	dac.updateDAC();

#endif
}

void startupDelay()
{
#ifndef CALIBRATION_MODE
	int ledDelay = 100;

	digitalWrite(ERROR_LED, HIGH);
	for (int i = 0; i < 50; i += 1)
	{
		digitalToggle(STATUS_LED);
		digitalToggle(ERROR_LED);
		delay(ledDelay);
	}
#endif
}

uint32_t modeSelection(int _mode)
{
	if (_mode == MODE_SD)
	{
		while (setupSD() != 0)
		{
			delay(500);
		}
		while (readConfigFile() != 0)
		{
			delay(500);
		}
		digitalWrite(ERROR_LED, LOW);
		setupTime();
		endFileRecord_s = createNewFile();
		return 5000u;
	}
	else
	{
		return 500u;
	}
}

// functions used for sd mode only

int setupSD()
{
	const int chipSelect = BUILTIN_SDCARD;
	// Check if a SD card is available
	if (!SD.begin(chipSelect))
	{
		Serial.println("Card failed, or not present");
		digitalWrite(ERROR_LED, HIGH);
		return -1;
	}

	// Create directory for storing data
	if (!SD.exists("/recording_data"))
		SD.mkdir("/recording_data");

	// Create file name which isn't used already
	for (unsigned int i = 0; i < UINT_MAX; i++)
	{

		sprintf(filename, "/recording_data/measurement%u.csv", i);

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
	// read harvesting information from configuration file
	File config_file = SD.open("configuration_file.txt", FILE_READ);
	if (!config_file)
	{
		Serial.println("The configuration file couldn't be found");
		digitalToggle(ERROR_LED);
		return -1;
	}

	for (int i = 0; i < NUM_OF_CONFIGLINES; i++)
	{
		String harvesting_info_temp = config_file.readStringUntil('\n');
		int index_of_sc = harvesting_info_temp.indexOf('=');
		// Store everything after the = sign in harvesting_info_temp
		// and check for DOS or unix line endings

		int strLen = harvesting_info_temp.length();

		// harvesting_info_temp.charAt(strLen) is the null terminator '\0'
		if (harvesting_info_temp.charAt(strLen - 1) == '\r')
		{
			harvesting_info[i] = harvesting_info_temp.substring(index_of_sc + 1, strLen - 1);
		}
		else
		{
			harvesting_info[i] = harvesting_info_temp.substring(index_of_sc + 1, strLen);
		}
	}

	for (int i = 0; i < NUM_OF_CONFIGLINES; i++)
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
	sprintf(filename, "/recording_data/measurement%u.csv", lastindex++);

	File rec_file = SD.open(filename, FILE_WRITE);

	char header_info[255];

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
			harvesting_info[5].c_str(), harvesting_info[6].c_str());
	rec_file.println(header_info);
	rec_file.close();

	return retval;
}

void writeDataToSD(uint8_t _sequence_number, int _voltage, int _current)
{
	if (!SD.mediaPresent())
	{
		while (!SD.mediaPresent())
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

/**
 * Returns the current time in seconds
 * @return Current time in seconds
 */
time_t getTeensy3Time()
{
	return Teensy3Clock.get();
}

void setupTime()
{

	setSyncProvider(getTeensy3Time);

	if (timeStatus() != timeSet)
	{
		Serial.println("Unable to sync with the RTC");
	}
	else
	{
		Serial.println("RTC has set the system time");
	}
}

/**
 * Finds the open circuit voltage and the short circuit voltage
 * and saves them in voltageLimits[]
 */
void findLimits(void)
{

	// determine open circuit voltage
	uint step = 1000;
	uint predCurrent;
	uint current;
	uint predMosfetValue = LOAD_MOSFET_DAC_VALUES_LUT_OFFSET;
	uint mosfetValue = LOAD_MOSFET_DAC_VALUES_LUT_OFFSET + step;

	// Set and upddate mosfet value
	dac.setVoltageA(predMosfetValue);
	dac.updateDAC();
	delay(1); // wait for voltage to settle

	predCurrent = getCurrentFromAdcValue();

	while (1)
	{

		dac.setVoltageA(mosfetValue);
		dac.updateDAC();
		delay(1);

		current = getCurrentFromAdcValue();

		if ((predCurrent <= CURRENT_LIMIT_uA) && (current <= CURRENT_LIMIT_uA))
		{
			predMosfetValue = mosfetValue;
			mosfetValue += step;
		}
		else if ((predCurrent <= CURRENT_LIMIT_uA) && (current > CURRENT_LIMIT_uA))
		{
			if (step < 3)
			{
				break;
			}
			else
			{
				step /= 2;
				predMosfetValue = mosfetValue;
				mosfetValue -= step;
				predCurrent = current;
			}
		}
		else if ((predCurrent > CURRENT_LIMIT_uA) && (current <= CURRENT_LIMIT_uA))
		{
			predMosfetValue = mosfetValue;
			step /= 2; // Changed step update location, to before the mosfetValue update
			mosfetValue += step;
			predCurrent = current;
		}
		else if ((predCurrent > CURRENT_LIMIT_uA) && (current > CURRENT_LIMIT_uA))
		{
			predMosfetValue = mosfetValue;
			mosfetValue -= step;
			predCurrent = current;
		}
	}

#ifdef DEBUG_MODE
	Serial.println("ende oc" + String(predMosfetValue));
#endif

	int oc_voltage = predMosfetValue;
	mosfetValues[0] = oc_voltage;
	voltageLimits[OC_VOLTAGE] = getVoltageFromAdcValue(); // Save open circuit voltage

#ifdef DEBUG_MODE
	Serial.printf("OC voltage: %d\n", voltageLimits[OC_VOLTAGE]);
	delay(100);
#endif

	// determine short circuit voltage
	step = 500;
	uint predVoltage;
	uint voltage;
	predMosfetValue = LOAD_MOSFET_DAC_VALUES_LUT_OFFSET;
	mosfetValue = LOAD_MOSFET_DAC_VALUES_LUT_OFFSET + step;

	dac.setVoltageA(predMosfetValue);
	dac.updateDAC();
	delay(1);

	predVoltage = getVoltageFromAdcValue();

	while (1)
	{
		dac.setVoltageA(mosfetValue);
		dac.updateDAC();
		delay(1);

		voltage = getVoltageFromAdcValue();

		if ((predVoltage <= VOLTAGE_LIMIT_uV) && (voltage <= VOLTAGE_LIMIT_uV))
		{
			predMosfetValue = mosfetValue;
			mosfetValue -= step;
			predVoltage = voltage; // Added this line
		}
		else if ((predVoltage <= VOLTAGE_LIMIT_uV) && (voltage > VOLTAGE_LIMIT_uV))
		{
			predMosfetValue = mosfetValue;
			step /= 2; // Changed step update location, to before the mosfetValue update
			mosfetValue += step;
			predVoltage = voltage;
		}
		else if ((predVoltage > VOLTAGE_LIMIT_uV) && (voltage <= VOLTAGE_LIMIT_uV))
		{
			if (step < 3)
			{
				break;
			}
			predMosfetValue = mosfetValue;
			step /= 2;
			mosfetValue -= step;
			predVoltage = voltage;
		}
		else if ((predVoltage > VOLTAGE_LIMIT_uV) && (voltage > VOLTAGE_LIMIT_uV))
		{
			predMosfetValue = mosfetValue;
			mosfetValue += step;
			predVoltage = voltage;
		}
	}

#ifdef DEBUG_MODE
	Serial.println("ende sc " + String(predMosfetValue));
	delay(100);
#endif

	int sc_voltage = predMosfetValue;
	mosfetValues[39] = sc_voltage;
	voltageLimits[SC_VOLTAGE] = getVoltageFromAdcValue(); // Save short circuit voltage

#ifdef DEBUG_MODE
	Serial.printf("SC voltage: %d\n", voltageLimits[SC_VOLTAGE]);
	delay(1000);
#endif

	return;
}

void calcCurve(void)
{
	findLimits();

	// define parameters of curve

	int offset = voltageLimits[SC_VOLTAGE];		// Set offset to short circuit voltage
	int upperLimit = voltageLimits[OC_VOLTAGE]; // Set upper limit to open circuit voltage

	// Define parameters for voltage value calculation
	uint stepsize = (voltageLimits[OC_VOLTAGE] - voltageLimits[SC_VOLTAGE]) / (NUMBER_OF_CAPTURED_POINTS_IN_CURVE - 1); // Calculate stepsize
	uint voltageSizes[NUMBER_OF_CAPTURED_POINTS_IN_CURVE] = {};															// Array for storing the voltage values
	voltageSizes[0] = voltageLimits[SC_VOLTAGE];																		// Set first value to smallest voltage
	voltageSizes[NUMBER_OF_CAPTURED_POINTS_IN_CURVE - 1] = voltageLimits[OC_VOLTAGE];									// set last value to highest voltage

#ifdef DEBUG_MODE
	Serial.printf("Offset: %d, Upper limit: %d, Step size: %d\n", offset, upperLimit, stepsize);
	delay(1000);
#endif
	// iterate over the array and calculate the voltage values
	for (uint8_t i = 1; i < NUMBER_OF_CAPTURED_POINTS_IN_CURVE; i++)
	{
		voltageSizes[i] = offset + (i * stepsize);
	}

	// calculate the mosfet values for the curve
	for (uint8_t i = 1; i < (NUMBER_OF_CAPTURED_POINTS_IN_CURVE); i++)
	{
		mosfetValues[i] = MIDPOINT + (log((upperLimit / ((voltageSizes[i] / SCALE_CURVE) - offset)) - 1) / CURVE_STEEPNESS);
	}

	return;
}