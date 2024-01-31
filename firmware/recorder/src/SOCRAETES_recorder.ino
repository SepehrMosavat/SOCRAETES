/////////////////////////////////////////////////////////INCLUDES///////////////////////////////////////////////////////////////

#include "Definitions.h"
#include "Functions.h"

/* These two libraries are project local and changed for the 
 * own purpose 
 */
#include "SnoozeTimer.h"
#include "hal.h"

#include <Arduino.h>
#include <SPI.h>
#include <elapsedMillis.h>



///////////////////////////////////////////////////////////DEFINES////////////////////////////////////////////////////////////

#define PIN_MODE_JUMPER 32
#define MODE_SD 0

// Cycle time for measuring points of curve
static const uint32_t innerCycleTime_ms = 10;

static elapsedMillis innerElapsedMillis;

uint32_t outerCycleTime_ms;

//static elapsedMillis outerElapsedMillis;

static uint32_t voltageArray_uV[NUMBER_OF_CAPTURED_POINTS_IN_CURVE];

static uint32_t currentArray_uA[NUMBER_OF_CAPTURED_POINTS_IN_CURVE];

static int mode = MODE_SD;

SnoozeTimer timer;

///////////////////////////////////////////////////////////FUNCTIONS//////////////////////////////////////////////////////////

void setup()
{

  pinMode(PIN_MODE_JUMPER, INPUT);

  pinMode(PIN_STATUS_LED, OUTPUT);
  pinMode(PIN_ERROR_LED, OUTPUT);

  startupDelay();
  initDAC();
  initializeADC();

  mode = digitalRead(PIN_MODE_JUMPER);

  if (mode == MODE_SD)
  {
#ifdef DEBUG_MODE
    Serial.begin(0);
#else
    Serial.end();
#endif
    while (setupSD() != 0)
    {
      delay(500);
    }
    while (readConfigFile() != 0)
    {
      delay(500);
    }
    setupTime();
    endFileRecord_s = createNewFile();
    outerCycleTime_ms =  5000u;
  }
  else
  {
    Serial.begin(0);
    outerCycleTime_ms = 2000u;
  }

  calcCurve();

  updateHarvesterLoad(0);

#ifdef DEBUG_MODE
  delay(1000);
#endif

  // Set up Snooze
  timer.setTimer( outerCycleTime_ms / 1000 );			// set cycletime of 5 seconds
  hal_initialize( timer.clearIsrFlags );
  timer.enableDriver(2);
}

void loop()
{
  // Uncomment to measure maximum inner cycle time
  // static unsigned long innerMaxTaskTime_ms = 0;
  // Uncomment to measure maximum outer cycle time
  // static unsigned long outerMaxTaskTime_ms = 0;
  // outerElapsedMillis = 0;

  digitalWrite(PIN_STATUS_LED, LOW);

  hal_deepSleep();

  // Turn on analog circuitry
  digitalWrite(PIN_STATUS_LED, HIGH);
  delay(5);
  // Set RTC time again as somehow it is lost after sleeping
  setupTime();
  delay(5);
  
  // Throw away first measurement
  getVoltageFromAdcValue_uV();
  getCurrentFromAdcValue_uA();

  calcCurve();
  updateHarvesterLoad(0);
  delay(5);

//#ifdef DEBUG_MODE
//  Serial.printf("mode: %s\n", mode == MODE_SD ? "SD":"PC");
//  delay(50);
//#endif

  for (uint8_t Counter = 0; Counter < NUMBER_OF_CAPTURED_POINTS_IN_CURVE; Counter++)
  {
    innerElapsedMillis = 0;

    // Read ADCs and convert to voltage and current values
    // Store measured point in an array
    voltageArray_uV[Counter] = getVoltageFromAdcValue_uV();
    currentArray_uA[Counter] = getCurrentFromAdcValue_uA();

    // Set new harvester load
    updateHarvesterLoad( (Counter + 1)%NUMBER_OF_CAPTURED_POINTS_IN_CURVE );

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
  

  // Store measured curve on SD card
  if (mode == MODE_SD)
  {
    uint8_t counter;
    for(counter = 0; counter < NUMBER_OF_CAPTURED_POINTS_IN_CURVE; counter++)
    {
#ifdef DEBUG_MODE
      Serial.printf("Seq. No.: %lu, V: %lu, I: %lu\n ", counter, voltageArray_uV[counter], currentArray_uA[counter]);
#endif
      writeDataToSD(counter, voltageArray_uV[counter], currentArray_uA[counter]);
      delay(5);
    }
    // Create new file if required
    //Serial.printf("now: %lu\n", now());
    if (now() > endFileRecord_s)
    {
      //Serial.printf("new recording\n");
      endFileRecord_s = createNewFile();
      // Serial.printf("endFileRecord_s: %d \n", endFileRecord_s);
    }

  }
  else
  {
    uint8_t counter;
    //for(counter = 0; counter < NUMBER_OF_CAPTURED_POINTS_IN_CURVE - 1; counter++)
    for(counter = 0; counter < NUMBER_OF_CAPTURED_POINTS_IN_CURVE; counter++)
    {
#ifndef DEBUG_MODE
      //transmitValuesAsByteArray(counter, 1000 * mosfetValues[counter], voltageArray_uV[counter]);
      transmitValuesAsByteArray(counter, voltageArray_uV[counter], currentArray_uA[counter]);
#endif
      delay(5);
    }

  }
}
