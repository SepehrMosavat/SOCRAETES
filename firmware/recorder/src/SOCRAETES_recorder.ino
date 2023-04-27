#include <Arduino.h>
#include <Encoder.h>
#include <ADC.h>
#include <SPI.h>
#include "MCP48xx/src/MCP48xx.h"

#include "Definitions.h"
#include "Functions.h"

extern byte uartByteArray[11];
extern ADC *adc;
extern int ivCurveSequenceNumber;

MCP4822 dac(34);

void setup() {
	Serial.begin(0);
	SPI.begin();

	dac.init();
	dac.turnOnChannelA();
	dac.setGainA(MCP4822::High);

	pinMode(HARVESTER_VOLTAGE_ADC_PIN, INPUT); // Harvester Voltage ADC Input
	pinMode(HARVESTER_CURRENT_ADC_PIN, INPUT); // Harvester Current in uA-Range ADC Input

	pinMode(STATUS_LED, OUTPUT);

	startupDelay();
	digitalWrite(STATUS_LED, LOW);

	initializeADC();
}

void loop() {
	// Read harvester voltage and current from ADC lines

	int currentSenseAdcValue = adc->analogRead(HARVESTER_CURRENT_ADC_PIN, ADC_1);
	int voltageAdcValue = adc->analogRead(HARVESTER_VOLTAGE_ADC_PIN, ADC_1);

	// Calculate harvester voltage and current from raw ADC values
	int current = getCurrentFromAdcValue(currentSenseAdcValue);
	int voltage = getVoltageFromAdcValue(voltageAdcValue);

	convertIntValuesToByteArrays(ivCurveSequenceNumber, voltage, current, uartByteArray);

	updateHarvesterLoad();

#ifdef DEBUG_MODE
#ifdef CALIBRATION_MODE
	Serial.printf("Calibration mode: V: %d, I: %d\n", voltage, current);
	delay(20);
#else
	Serial.printf("Seq. No.: %d, V: %d, I: %d\n", uartByteArray[1], voltage, current);
	delay(20);
#endif
#else
	for(int i = 0; i < 11; i++)
	{
		Serial.write(uartByteArray[i]);
	}
#endif


	if(uartByteArray[1] == NUMBER_OF_CAPUTURED_POINTS_IN_CURVE - 1){
		digitalToggle(STATUS_LED);
		delay(500);
	}


	delay(3);
}
