#include <Arduino.h>
#include <Encoder.h>
#include <ADC.h>
#include <SPI.h>
#include <TimeLib.h>

#include "Definitions.h"
#include "Functions.h"

extern ADC *adc;

extern uint8_t ivCurveSequenceNumber;

#if ! defined(DEBUG_MODE) && ! defined(STAND_ALONE)
static byte uartByteArray[11];
#endif

#if defined(STAND_ALONE)
static time_t endFileRecord_s;
#endif

// Cycle time for measuring points of curve
static const uint32_t innerCycleTime_ms = 5;

static uint32_t innerTimeStamp_ms;

#if defined(STAND_ALONE)
static const uint32_t outerCycleTime_ms = 2000;
#else
static const uint32_t outerCycleTime_ms = 500;
#endif

static uint32_t outerTimeStamp_ms;

static int voltageArray[NUMBER_OF_CAPTURED_POINTS_IN_CURVE];

static int currentArray[NUMBER_OF_CAPTURED_POINTS_IN_CURVE];

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
	while( setup_SD() != 0 )
	{
		delay(500);
	}
	while( readConfigFile() != 0 )
	{
		delay(500);
	}
	endFileRecord_s = createNewFile();
	ivCurveSequenceNumber = NUMBER_OF_CAPTURED_POINTS_IN_CURVE; 
#endif
	// Set DACs for first measurement
	updateHarvesterLoad();	
}

void loop() {
	// Uncomment to measure maximum inner cycle time
//	static time_t innerMaxCycleTime_ms = -1;
	// Uncomment to measure maximum outer cycle time
//	static time_t outerMaxCycleTime_ms = -1;

	outerTimeStamp_ms = millis();

	digitalToggle(LED_BUILTIN);

	for (uint8_t Counter = 0; Counter < NUMBER_OF_CAPTURED_POINTS_IN_CURVE; Counter++)
	{
		innerTimeStamp_ms = millis();
		// Read harvester voltage and current from ADC lines
		int currentSenseAdcValue = adc->analogRead(HARVESTER_CURRENT_ADC_PIN, ADC_0);
		int voltageAdcValue = adc->analogRead(HARVESTER_VOLTAGE_ADC_PIN, ADC_1);

		// Calculate harvester voltage and current from raw ADC values
		int voltage = getVoltageFromAdcValue(voltageAdcValue);
		int current = getCurrentFromAdcValue(currentSenseAdcValue);

		// Store measured point in an array
		voltageArray[ivCurveSequenceNumber] = voltage;
		currentArray[ivCurveSequenceNumber] = current;
#ifdef DEBUG_MODE
		Serial.printf("Seq. No.: %d, V: %d, I: %d\n", ivCurveSequenceNumber, voltageArray[ivCurveSequenceNumber], currentArray[ivCurveSequenceNumber]);
		delay(200);
#endif

		updateHarvesterLoad();  
		// Uncomment to measure maximum inner cycle time
//		time_t innerCurrCycleTime_ms = millis() - innerTimeStamp_ms;
//		if ( innerMaxCycleTime_ms < innerCurrCycleTime_ms ) 
//		{ 
//			innerMaxCycleTime_ms = innerCurrCycleTime_ms;
//			Serial.printf("innerMaxCycleTime_ms: %d\n", innerMaxCycleTime_ms);
//		}

		//Wait a bit that new harvester load can settle
		while ( (millis() - innerTimeStamp_ms) < innerCycleTime_ms )
		{
			;
		}

	}
	// Transmit the measured curve or store it on SD card
	for (uint8_t Counter = 0; Counter < NUMBER_OF_CAPTURED_POINTS_IN_CURVE; Counter++)
	{
#if defined(STAND_ALONE)
		write_data_to_SD(Counter, voltageArray[Counter], currentArray[Counter]);
#elif ! defined(DEBUG_MODE)
		convertIntValuesToByteArrays(Counter, voltageArray[Counter], currentArray[Counter], uartByteArray);
		for(int i = 0; i < 11; i++)
		{
			Serial.write(uartByteArray[i]);
		}
#endif
		delay(5);
	}

#ifdef STAND_ALONE
	// Create new file if required
	if (now() > endFileRecord_s)
	{
		Serial.printf("new recording\n");
		ivCurveSequenceNumber = NUMBER_OF_CAPTURED_POINTS_IN_CURVE;
		endFileRecord_s = createNewFile();
		Serial.printf("endFileRecord_s: %d \n", endFileRecord_s);
	}
#endif

	// Uncomment to measure maximum outer cycle time
//	time_t outerCurrCycleTime_ms = millis() - outerTimeStamp_ms;
//	if ( outerMaxCycleTime_ms < outerCurrCycleTime_ms ) 
//	{
//		outerMaxCycleTime_ms = outerCurrCycleTime_ms; 
//		Serial.printf("outerMaxCycleTime_ms: %d\n", outerMaxCycleTime_ms);
//	}
	while ( (millis() - outerTimeStamp_ms) < outerCycleTime_ms )
	{
		;
	}
}
