#include <Arduino.h>
#include <Encoder.h>
#include <ADC.h>
#include <SPI.h>

#include "Definitions.h"
#include "Functions.h"

extern byte uartByteArray[11];
extern ADC *adc;
extern int ivCurveSequenceNumber;

void setup() {
	Serial.begin(0);
	SPI.begin();

	pinMode(HARVESTER_VOLTAGE_ADC_PIN, INPUT); // Harvester Voltage ADC Input
	pinMode(HARVESTER_CURRENT_ADC_PIN, INPUT); // Harvester Current in uA-Range ADC Input

	analogWriteResolution(12);

	pinMode(LED_BUILTIN, OUTPUT);

	startupDelay();
	digitalWrite(LED_BUILTIN, HIGH);

	initializeADC();
}

void loop() {
	// Read harvester voltage and current from ADC lines
	int currentSenseAdcValue = adc->analogRead(HARVESTER_CURRENT_ADC_PIN, ADC_0);
	int voltageAdcValue = adc->analogRead(HARVESTER_VOLTAGE_ADC_PIN, ADC_1);


	// Calculate harvester voltage and current from raw ADC values
	int voltage = getVoltageFromAdcValue(voltageAdcValue);
	int current = getCurrentFromAdcValue(currentSenseAdcValue);

	convertIntValuesToByteArrays(ivCurveSequenceNumber, voltage, current, uartByteArray);

	updateHarvesterLoad();

#ifdef DEBUG_MODE
	Serial.printf("Seq. No.: %d, V: %d, I: %d\n", uartByteArray[1], voltage, current);
	delay(200);
#else
	for(int i = 0; i < 11; i++)
	{
		Serial.write(uartByteArray[i]);
	}
#endif

	delay(3);
}
