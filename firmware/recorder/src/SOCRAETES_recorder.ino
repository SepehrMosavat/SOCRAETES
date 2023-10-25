#include <Arduino.h>
#include <Encoder.h>
#include <ADC.h>
#include <SPI.h>

#include "MCP48xx.h"

#include <TimeLib.h>
#include <elapsedMillis.h>

#include "Definitions.h"
#include "Functions.h"

// Cycle time for measuring points of curve
static const uint32_t innerCycleTime_ms = 10;

static elapsedMillis innerElapsedMillis;

time_t endFileRecord_s;

uint32_t outerCycleTime_ms = 500;

static elapsedMillis outerElapsedMillis;

static uint8_t ivCurveSequenceNumber;

static int voltageArray[NUMBER_OF_CAPTURED_POINTS_IN_CURVE];

static int currentArray[NUMBER_OF_CAPTURED_POINTS_IN_CURVE];

static int mode =1;

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
	pinMode(ERROR_LED, OUTPUT);
	
	pinMode(MODE_JUMPER, INPUT_PULLUP);
	startupDelay();
	digitalWrite(STATUS_LED, LOW);
	digitalWrite(ERROR_LED, LOW);

	initializeADC();
	ivCurveSequenceNumber = 0; 
	// Set DACs for first measurement
	updateHarvesterLoad(ivCurveSequenceNumber);
	// 1 means PC, 0 means S/A;
	mode=digitalRead(MODE_JUMPER);
	outerCycleTime_ms = modeSelection(mode);

	
}

void loop() {
  // Uncomment to measure maximum inner cycle time
  //static unsigned long innerMaxTaskTime_ms = 0;
  // Uncomment to measure maximum outer cycle time
  //static unsigned long outerMaxTaskTime_ms = 0;
	outerElapsedMillis = 0;
  digitalToggle(STATUS_LED);

	for (uint8_t Counter = 0; Counter < NUMBER_OF_CAPTURED_POINTS_IN_CURVE; Counter++)
	{
		innerElapsedMillis = 0;

		// Read ADCs and convert to voltage and current values
		// Store measured point in an array
		voltageArray[ivCurveSequenceNumber] = getVoltageFromAdcValue();
		currentArray[ivCurveSequenceNumber] = getCurrentFromAdcValue();

#ifdef DEBUG_MODE
	#ifdef CALIBRATION_MODE
		Serial.printf("Calibration mode: V: %d, I: %d\n", voltageArray[ivCurveSequenceNumber], currentArray[ivCurveSequenceNumber]);
		delay(20);
	#else
		Serial.printf("Seq. No.: %d, V: %d, I: %d\n", ivCurveSequenceNumber, voltageArray[ivCurveSequenceNumber], currentArray[ivCurveSequenceNumber]);
	#endif
#endif


		ivCurveSequenceNumber++;

		if( ivCurveSequenceNumber >= NUMBER_OF_CAPTURED_POINTS_IN_CURVE )
		{
			ivCurveSequenceNumber = 0;
		}

		// Set new harvester load
		updateHarvesterLoad(ivCurveSequenceNumber);  

#ifdef DEBUG_MODE
		// Add some extra delay when in debug mode
		delay(200);
#endif

		// Uncomment to measure maximum inner cycle time
//		if ( innerMaxTaskTime_ms < innerElapsedMillis ) 
//		{ 
//			innerMaxTaskTime_ms = innerElapsedMillis;
//			Serial.printf("innerMaxTaskTime_ms: %lu\n", innerMaxTaskTime_ms);
//		}

		//Wait a bit that new harvester load can settle
		while ( innerElapsedMillis < innerCycleTime_ms )
		{
			;
		}
	}
	// Transmit the measured curve or store it on SD card
	for (uint8_t Counter = 0; Counter < NUMBER_OF_CAPTURED_POINTS_IN_CURVE; Counter++)
	{
		if (mode == 0) 
			writeDataToSD(Counter, voltageArray[Counter], currentArray[Counter]);
		else
			#if defined(DEBUG_MODE)
			;
			#else
			transmitValuesAsByteArray(Counter, voltageArray[Counter], currentArray[Counter]);
			#endif
		delay(5);
	}

if (mode == 0)
{
	// Create new file if required
	if (now() > endFileRecord_s)
	{
		Serial.printf("new recording\n");
		endFileRecord_s = createNewFile();
		// Serial.printf("endFileRecord_s: %d \n", endFileRecord_s);
	}
}

	// Uncomment to measure maximum outer cycle time
//	if ( outerMaxTaskTime_ms < outerElapsedMillis ) 
//	{
//		outerMaxTaskTime_ms = outerElapsedMillis; 
//		Serial.printf("outerMaxTaskTime_ms: %lu\n", outerMaxTaskTime_ms);
//	}
	while ( outerElapsedMillis < outerCycleTime_ms )
	{
		;
	}
}
