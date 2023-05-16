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

ADC *adc = new ADC();
byte uartByteArray[11];
int ivCurveSequenceNumber = 0;

extern MCP4822 dac;

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
		returnValue = (double)_adcValue / (pow(2, ADC_RESOLUTION_BITS) - 1);
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
		returnValue = (double)_adcValue / (pow(2, ADC_RESOLUTION_BITS) - 1);
		returnValue *= ADC_REFERENCE_VOLTAGE; // VACC
		returnValue *= 1000000; // Value in uA

		returnValue /= CURRENT_SENSE_AMPLIFIER_GAIN;
		returnValue /= CURRENT_SENSE_SHUNT_RESISTOR_VALUE;
	}

	returnValue = (int)(returnValue * CURRENT_SENSE_CALIBRATION_FACTOR) + CURRENT_SENSE_CALIBRATION_OFFSET;
	return (returnValue < 0) ? 0 : returnValue;
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

void initializeADC()
{
	// Initialize the current-sense and voltage-sense ADC
	adc->adc1->setAveraging(0); // set number of averages
	adc->adc1->setResolution(ADC_RESOLUTION_BITS); // set bits of resolution
	adc->adc1->setConversionSpeed(ADC_CONVERSION_SPEED::VERY_LOW_SPEED);
	adc->adc1->setSamplingSpeed(ADC_SAMPLING_SPEED::VERY_LOW_SPEED);
}

int i = 1100;
int first=0, second=0;
bool calibrated = 0;
void updateHarvesterLoad()
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
	//	Serial.printf("i: %d, first: %d, second: %d, diff: %d -> ", i, first, second, second-first);
	//	second=first;
	if(i > 1600) {
		i = 1100;
	}
	 */
#else
	dac.setVoltageA(LOAD_MOSFET_DAC_VALUES_LOOKUP_TABLE[ivCurveSequenceNumber] + LOAD_MOSFET_DAC_VALUES_LUT_OFFSET);
	dac.updateDAC();

	ivCurveSequenceNumber++;
	if(ivCurveSequenceNumber > NUMBER_OF_CAPUTURED_POINTS_IN_CURVE - 1)
	{
		ivCurveSequenceNumber = 0;
		dac.setVoltageA(LOAD_MOSFET_DAC_VALUES_LOOKUP_TABLE[ivCurveSequenceNumber] + LOAD_MOSFET_DAC_VALUES_LUT_OFFSET);
		dac.updateDAC();
		delay(10);
	}
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
