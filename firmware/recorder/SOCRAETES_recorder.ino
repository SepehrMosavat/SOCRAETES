#include <Arduino.h>
#include <Encoder.h>
#include <ADC.h>
#include <SPI.h>
#include <TimeLib.h>

#include "Definitions.h"
#include "Functions.h"

extern byte uartByteArray[11];
extern ADC *adc;
extern uint8_t ivCurveSequenceNumber;
static time_t end_timestamp_s;

static const uint32_t cycleTime_ms = 100;
static uint32_t timeStamp_ms;

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

#if STAND_ALONE
	setup_time();
	setup_SD();
	readConfigFile();
	end_timestamp_s = createNewFile();
	ivCurveSequenceNumber = NUMBER_OF_CAPTURED_POINTS_IN_CURVE; 
#endif
	// Set DACs for first measurement
	updateHarvesterLoad();	
}

void loop() {

	timeStamp_ms = millis();
	digitalToggle(LED_BUILTIN);
	// Read harvester voltage and current from ADC lines
	int currentSenseAdcValue = adc->analogRead(HARVESTER_CURRENT_ADC_PIN, ADC_0);
	int voltageAdcValue = adc->analogRead(HARVESTER_VOLTAGE_ADC_PIN, ADC_1);


	// Calculate harvester voltage and current from raw ADC values
	int voltage = getVoltageFromAdcValue(voltageAdcValue);
	int current = getCurrentFromAdcValue(currentSenseAdcValue);

#ifdef DEBUG_MODE
	Serial.printf("Seq. No.: %d, V: %d, I: %d\n", uartByteArray[1], voltage, current);
	delay(200);
#elif STAND_ALONE
	write_data_to_SD((ivCurveSequenceNumber),voltage, current);
	if (now() > end_timestamp_s)
	{
		Serial.printf("new recording\n");
		ivCurveSequenceNumber = NUMBER_OF_CAPTURED_POINTS_IN_CURVE;
		end_timestamp_s = createNewFile();
	}
#else
	convertIntValuesToByteArrays(ivCurveSequenceNumber, voltage, current, uartByteArray);
	for(int i = 0; i < 11; i++)
	{
		Serial.write(uartByteArray[i]);
	}
#endif

	updateHarvesterLoad();  

	while ( (millis() - timeStamp_ms) < cycleTime_ms )
	{
		;
	}
}
