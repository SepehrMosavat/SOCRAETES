/////////////////////////////////////////////////////////INCLUDES///////////////////////////////////////////////////////////////

#include <Arduino.h>
#include <Encoder.h>
#include <ADC.h>
#include <SPI.h>
#include "MCP48xx.h"

#include <TimeLib.h>
#include <elapsedMillis.h>

//#include <Snooze.h>
#include <SnoozeTimer.h>
#include <hal.h>

#include "Definitions.h"
#include "Functions.h"

///////////////////////////////////////////////////////////DEFINES////////////////////////////////////////////////////////////

// Cycle time for measuring points of curve
static const uint32_t innerCycleTime_ms = 5;

static elapsedMillis innerElapsedMillis;

time_t endFileRecord_s;

uint32_t outerCycleTime_ms;

//static elapsedMillis outerElapsedMillis;

static uint8_t ivCurveSequenceNumber;

static int voltageArray[NUMBER_OF_CAPTURED_POINTS_IN_CURVE];

static int currentArray[NUMBER_OF_CAPTURED_POINTS_IN_CURVE];

static int mode = MODE_SD;

MCP4822 dac(34);

SnoozeTimer timer;
//SnoozeAlarm alarm;

//SnoozeBlock config_teensy40(timer);

///////////////////////////////////////////////////////////FUNCTIONS//////////////////////////////////////////////////////////

void setup()
{
	Serial.begin(0);
	SPI.begin();

	dac.init();
	dac.turnOnChannelA();
	dac.setGainA(MCP4822::High);

	pinMode(HARVESTER_VOLTAGE_ADC_PIN, INPUT); // Harvester Voltage ADC Input
	pinMode(HARVESTER_CURRENT_ADC_PIN, INPUT); // Harvester Current in uA-Range ADC Input

	pinMode(STATUS_LED, OUTPUT);
	pinMode(ERROR_LED, OUTPUT);

	pinMode(MODE_JUMPER, INPUT);
	startupDelay();
	digitalWrite(STATUS_LED, LOW);
	digitalWrite(ERROR_LED, LOW);
	initializeADC();

	ivCurveSequenceNumber = 0;

	// Set DACs for first measurement
	// 1 means PC, 0 means SD;
	mode = digitalRead(MODE_JUMPER);
	outerCycleTime_ms = modeSelection(mode);

	calcCurve();
	updateHarvesterLoad(ivCurveSequenceNumber);

#ifdef DEBUG_MODE
	delay(10000);
// #else
// 	if (mode == 1)
// 	{
// 		while (1)
// 		{
// 			char _input = (char)Serial.read();
// 			if (_input == 's')
// 				break;
// 		}
// 	}
#endif

  //Serial.printf("Set timer\n");
	// Set up Snooze
	timer.setTimer(5);			// sleep for 5 seconds
    //alarm.setRtcTimer( 0, 0, 5);
  //Serial.printf("Timer set\n");

  hal_initialize( timer.clearIsrFlags );
  timer.enableDriver(2);
}

void toggle()
{
	digitalToggle(STATUS_LED);
}

void loop()
{
	// Uncomment to measure maximum inner cycle time
	// static unsigned long innerMaxTaskTime_ms = 0;
	// Uncomment to measure maximum outer cycle time
	// static unsigned long outerMaxTaskTime_ms = 0;
	// outerElapsedMillis = 0;

  //Snooze.deepSleep(config_teensy40);
  //Serial.printf("Going to sleep\n");
  //delay(100);
  hal_deepSleep();
  // Set time again as somehow it is lost after sleeping
  setupTime();
  calcCurve();

#ifdef DEBUG_MODE
  Serial.printf("mode: %s\n", mode == MODE_SD ? "SD":"PC");
  delay(50);
#endif
  //Serial.printf("Waking up\n");
  //delay(100);
  //timer.disableDriver(2);

	// digitalToggle(STATUS_LED);

	digitalWrite(STATUS_LED, HIGH);

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
		Serial.printf("Seq. No.: %d, V: %d, I: %d\n ", ivCurveSequenceNumber, voltageArray[ivCurveSequenceNumber], currentArray[ivCurveSequenceNumber]);
#endif
#endif

		ivCurveSequenceNumber++;

		if (ivCurveSequenceNumber >= NUMBER_OF_CAPTURED_POINTS_IN_CURVE)
		{
			ivCurveSequenceNumber = 0;
		}

		// Set new harvester load
		updateHarvesterLoad(ivCurveSequenceNumber);

#ifdef DEBUG_MODE
		// Add some extra delay when in debug mode
		delay(2);
#endif

		// Uncomment to measure maximum inner cycle time
		//		if ( innerMaxTaskTime_ms < innerElapsedMillis )
		//		{
		//			innerMaxTaskTime_ms = innerElapsedMillis;
		//			Serial.printf("innerMaxTaskTime_ms: %lu\n", innerMaxTaskTime_ms);
		//		}

		// Wait a bit that new harvester load can settle
		while (innerElapsedMillis < innerCycleTime_ms)
		{
			;
		}
	}

	// Store measure curve on SD card

	if (mode == MODE_SD)
	{
    uint8_t counter;
    for(counter = 0; counter < NUMBER_OF_CAPTURED_POINTS_IN_CURVE; counter++)
    {
      writeDataToSD(counter, voltageArray[counter], currentArray[counter]);
      delay(5);
    }
		// Create new file if required
    Serial.printf("now: %lu\n", now());
    if (now() > endFileRecord_s)
		{
			Serial.printf("new recording\n");
			endFileRecord_s = createNewFile();
			// Serial.printf("endFileRecord_s: %d \n", endFileRecord_s);
		}

		// Uncomment to measure maximum outer cycle time
		//	if ( outerMaxTaskTime_ms < outerElapsedMillis )
		//	{
		//		outerMaxTaskTime_ms = outerElapsedMillis;
		//		Serial.printf("outerMaxTaskTime_ms: %lu\n", outerMaxTaskTime_ms);
		//	}
	}
	digitalWrite(STATUS_LED, LOW);
}
