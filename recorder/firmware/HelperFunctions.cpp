/*
 * HelperFunctions.cpp
 *
 *  Created on: 02.09.2020
 *      Author: Sayedsepehr Mosavat
 *      University of Duisburg-Essen
 *      Networked Embedded Systems (NES)
 */

#include "HelperFunctions.h"
#include <SPI.h>
#include <Encoder.h>
#include <ADC.h>
#include <ADC_util.h>

Encoder rotaryEncoder(channelA, channelB);
ADC *adc = new ADC();

float DACvoltage = 0;
short encoderPosition = 0;
byte uartByteArray[11];


float shortToVoltage(short _voltage)
{
	float returnValue;
	returnValue = (float)_voltage/255;
	return returnValue*VCC_VOLTAGE;
}

int getVoltageFromAdcValue(int _adcValue, int _pgaGain)
{
	double returnValue;
	if (_adcValue < 0)
	{
		returnValue = 0;
	}
	else
	{
		returnValue = (double)_adcValue / 65535; // 16-bit ADC resolution
		returnValue /= _pgaGain;
		returnValue *= ADC_REFERENCE_VOLTAGE;

#ifdef ADC_VOLTAGE_DIVIDER_USED
		returnValue *= ADC_VOLTAGE_DIVIDER_CONVERSION_FACTOR;
#endif

		returnValue *= 1000000; // Value in uV
	}
	return (int)returnValue;
}

int getCurrentFromAdcValue(int _adcValue, int _pgaGain)
{
	double returnValue;
	if(_adcValue < 0)
	{
		returnValue = 0;
	}
	else
	{
		returnValue = (double)_adcValue / 65535; // 16-bit ADC resulution
		returnValue /= _pgaGain;
		returnValue *= ADC_REFERENCE_VOLTAGE; // VACC
		returnValue *= 1.2; // Scaling factor (magic number)

		returnValue *= 1000000; // Value in uA
		returnValue /= CURRENT_SENSE_RESISTOR_VALUE;
	}
	return (int)returnValue;
}

void writeToDigitalPot(byte _value)
{
	digitalWrite(DIGITAL_POT_CS_PIN, LOW);

	SPI.transfer(DIGITAL_POT_WIPER0_ADDRESS); // Volatile wiper0 register
	SPI.transfer(_value);

	digitalWrite(DIGITAL_POT_CS_PIN, HIGH);
	delay(4);
}

void updateDacOutputVoltage()
{
	short newEncoderPosition = rotaryEncoder.read();
	if(newEncoderPosition != encoderPosition)
	{
		if(newEncoderPosition < 0)
		{
			newEncoderPosition = 0;
			rotaryEncoder.write(newEncoderPosition);
		}
		else if(newEncoderPosition > 255)
		{
			newEncoderPosition = 255;
			rotaryEncoder.write(newEncoderPosition);
		}
		encoderPosition = newEncoderPosition;

		analogWrite(DAC_OUTPUT_PIN, encoderPosition);
	}
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
	adc->adc0->setAveraging(64); // set number of averages
	adc->adc0->setResolution(16); // set bits of resolution
	adc->adc0->setConversionSpeed(ADC_CONVERSION_SPEED::VERY_HIGH_SPEED); // change the conversion speed
	adc->adc0->setSamplingSpeed(ADC_SAMPLING_SPEED::VERY_HIGH_SPEED); // change the sampling speed
	adc->adc0->setReference(ADC_REFERENCE::REF_3V3);
	adc->adc0->enablePGA(CURRENT_SENSE_PGA_GAIN);

	adc->adc1->setAveraging(64); // set number of averages
	adc->adc1->setResolution(16); // set bits of resolution
	adc->adc1->setConversionSpeed(ADC_CONVERSION_SPEED::VERY_HIGH_SPEED); // change the conversion speed
	adc->adc1->setSamplingSpeed(ADC_SAMPLING_SPEED::VERY_HIGH_SPEED); // change the sampling speed
	adc->adc1->setReference(ADC_REFERENCE::REF_3V3);
}
