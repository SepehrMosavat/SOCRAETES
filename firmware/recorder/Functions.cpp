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

ADC *adc = new ADC();
byte uartByteArray[11];
int ivCurveSequenceNumber = 0;

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
	return (int)returnValue;
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
	analogWrite(LOAD_MOSFET_DAC_PIN, 4535);
#else
	analogWrite(LOAD_MOSFET_DAC_PIN, LOAD_MOSFET_DAC_VALUES_LOOKUP_TABLE[ivCurveSequenceNumber] + LOAD_MOSFET_DAC_VALUES_LUT_OFFSET);
	ivCurveSequenceNumber++;
	if(ivCurveSequenceNumber > NUMBER_OF_CAPUTURED_POINTS_IN_CURVE)
	{
		ivCurveSequenceNumber = 0;
		analogWrite(LOAD_MOSFET_DAC_PIN, LOAD_MOSFET_DAC_VALUES_LOOKUP_TABLE[ivCurveSequenceNumber] + LOAD_MOSFET_DAC_VALUES_LUT_OFFSET);
		delay(20);
	}
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
