/*
 * HelperFunctions.cpp
 *
 *  Created on: 02.09.2020
 *      Author: Sayedsepehr Mosavat
 *      University of Duisburg-Essen
 *      Networked Embedded Systems (NES)
 */

/////////////////////////////////////////////////////////////INCLUDES///////////////////////////////////////////////////////////

#include "Definitions.h"
#include "Functions.h"
/* This library is project local and contains a bug fix
 */
#include "MCP48xx.h"

#include <SPI.h>
#include <ADC.h>
#include <SD.h>
#include <math.h>

/////////////////////////////////////////////////////////////DEFINES///////////////////////////////////////////////////////////
//
// Teensy 4.1 Board Pin Definitions

#define HARVESTER_VOLTAGE_ADC_PIN A15
#define HARVESTER_CURRENT_ADC_PIN A14

#define NUM_OF_CONFIGLINES 7
#define DAC_MAX ((1ul << 12) - 1) // 12-BIT DAC
                                  
// Values defined with testing
#define SCALE_CURVE 0.95	 // Scale factor for the curve
#define CURVE_STEEPNESS 0.02 // Steepness of the curve
#define DEFAULT_MIDPOINT 2800		 // Default midpoint of the curve

#define ADC_RESOLUTION_BITS 12
#define ADC_REFERENCE_VOLTAGE 3.3

static ADC *adc = new ADC();
static double adcMaxValue;

static MCP4822 dac(34);

// start of data used for sd mode only

static char filename[128];

static String harvesting_info[NUM_OF_CONFIGLINES];

static uint16_t lastindex;

// Initialize this value unequal to zero. It is set in readConfigFile()
static time_t fileRecDuration_s = 60;

time_t endFileRecord_s;

uint16_t mosfetValues[NUMBER_OF_CAPTURED_POINTS_IN_CURVE];

/////////////////////////////////////////////////////////////FUNCTIONS///////////////////////////////////////////////////////////

static void resetFunc(void)
{
  SCB_AIRCR = 0x05FA0004;
}

uint32_t getVoltageFromAdcValue_uV(void)
{
  double returnValue;
  int _adcValue = adc->analogRead(HARVESTER_VOLTAGE_ADC_PIN, ADC_1);


  if (_adcValue < 0)
  {
    return 0;
  }
  else
  {

    // Merged voltage divider value
    returnValue = (2.0 * (double)_adcValue *  ADC_REFERENCE_VOLTAGE) * (1000000.0 / adcMaxValue);

  }
#ifdef DEBUG_MODE
//  Serial.printf("adcValueVoltage: %d, voltageValue_uV %lf\n", _adcValue, returnValue);
#endif

  return (uint32_t)returnValue;
}

uint32_t getCurrentFromAdcValue_uA(void)
{
  double returnValue;
  int _adcValue = adc->analogRead(HARVESTER_CURRENT_ADC_PIN, ADC_1);


  if (_adcValue < 0)
  {
    return 0;
  }
  else
  {
    returnValue = ((double)_adcValue *  ADC_REFERENCE_VOLTAGE) * (1000000.0 / (CURRENT_SENSE_AMPLIFIER_GAIN * CURRENT_SENSE_SHUNT_RESISTOR_VALUE) / adcMaxValue);

  }

#ifdef DEBUG_MODE
  //Serial.printf("adcValueCurrent: %d, voltageValue_uA %lf\n", _adcValue, returnValue);
#endif

  return (uint32_t)returnValue;
}

/**
 * Saves the values of _sequence_number, _voltage and current to a byte array
 * @param _sequence_number: Sequence number of point in curve
 * @param _voltage: Voltage in uV
 * @param _current: Current in uA
 * @param _buffer: Byte array to store the values
 */
void transmitValuesAsByteArray(uint8_t SeqNo, uint32_t voltage, uint32_t current)
{
	uint8_t buffer[11];
	buffer[0] = 0xaa; // Start byte
	buffer[1] = SeqNo; 

	buffer[2] = (voltage >> 0) & 0xff;
	buffer[3] = (voltage >> 8) & 0xff;
	buffer[4] = (voltage >> 16) & 0xff;
	buffer[5] = (voltage >> 24) & 0xff;

	buffer[6] = (current >> 0) & 0xff;
	buffer[7] = (current >> 8) & 0xff;
	buffer[8] = (current >> 16) & 0xff;
	buffer[9] = (current >> 24) & 0xff;

	buffer[10] = 0x55; // Finish byte

	for(int i = 0; i < 11; i++)
	{
		Serial.write(buffer[i]);
	}
}

void initializeADC()
{
  pinMode(HARVESTER_VOLTAGE_ADC_PIN, INPUT); // Harvester Voltage ADC Input
  pinMode(HARVESTER_CURRENT_ADC_PIN, INPUT); // Harvester Current in uA-Range ADC Input
                                             
  // Initialize the current-sense and voltage-sense ADC
  adc->adc1->setReference(ADC_REFERENCE::REF_3V3);
  adc->adc1->setAveraging(4);					   // set number of averages
  adc->adc1->setResolution(ADC_RESOLUTION_BITS); // set bits of resolution
  adc->adc1->setConversionSpeed(ADC_CONVERSION_SPEED::LOW_SPEED);
  adc->adc1->setSamplingSpeed(ADC_SAMPLING_SPEED::LOW_SPEED);
  adcMaxValue = adc->adc1->getMaxValue();

}

void updateHarvesterLoad(uint8_t SeqNo)
{
  // Serial.printf("mosfetValues[%u]: %u\n", SeqNo, mosfetValues[SeqNo]);
  dac.setVoltageA(mosfetValues[SeqNo]);
  dac.updateDAC();
}

void startupDelay()
{
  //digitalWrite(PIN_ERROR_LED, HIGH);
  for (int i = 0; i < 20; i += 1)
  {
    digitalToggle(PIN_STATUS_LED);
    //digitalToggle(PIN_ERROR_LED);
    delay(200);
  }
  digitalWrite(PIN_STATUS_LED, LOW);
  digitalWrite(PIN_ERROR_LED, LOW);
}

int setupSD()
{
  const int chipSelect = BUILTIN_SDCARD;
  // Check if a SD card is available
  if (!SD.begin(chipSelect))
  {
#ifdef DEBUG_MODE
    Serial.println("Card failed, or not present");
#endif
    digitalWrite(PIN_ERROR_LED, HIGH);
    return -1;
  }

  // Create directory for storing data
  if (!SD.exists("/recording_data"))
    SD.mkdir("/recording_data");

  // Create file name which isn't used already
  for (unsigned int i = 0; i < UINT_MAX; i++)
  {
    sprintf(filename, "/recording_data/measurement%u.csv", i);

    if (!SD.exists(filename))
    {
      lastindex = i;
      break;
    }
  }

  return 0; }

int readConfigFile(void)
{
  // read harvesting information from configuration file
  File config_file = SD.open("configuration_file.txt", FILE_READ);
  if (!config_file)
  {
#ifdef DEBUG_MODE
    Serial.println("The configuration file couldn't be found");
#endif
    digitalToggle(PIN_ERROR_LED);
    return -1;
  }

  for (int i = 0; i < NUM_OF_CONFIGLINES; i++)
  {
    String harvesting_info_temp = config_file.readStringUntil('\n');
    int index_of_sc = harvesting_info_temp.indexOf('=');
    // Store everything after the = sign in harvesting_info_temp
    // and check for DOS or unix line endings

    int strLen = harvesting_info_temp.length();

    // harvesting_info_temp.charAt(strLen) is the null terminator '\0'
    if (harvesting_info_temp.charAt(strLen - 1) == '\r')
    {
      harvesting_info[i] = harvesting_info_temp.substring(index_of_sc + 1, strLen - 1);
    }
    else
    {
      harvesting_info[i] = harvesting_info_temp.substring(index_of_sc + 1, strLen);
    }
  }

#ifdef DEBUG_MODE
  for (int i = 0; i < NUM_OF_CONFIGLINES; i++)
  {
    Serial.println(harvesting_info[i]);
  }
#endif
  fileRecDuration_s = harvesting_info[0].toInt();
  //	harvesting_info[0] = duration_s
  //	harvesting_info[1] = Indoor/Outdoor
  //	harvesting_info[2] = Lux
  //	harvesting_info[3] = Season
  //	harvesting_info[4] = Country
  //	harvesting_info[5] = City
  // 	harvesting_info[6] = Source

  config_file.close();
  return 0;
}

time_t createNewFile(void)
{
  sprintf(filename, "/recording_data/measurement%u.csv", lastindex++);

  File rec_file = SD.open(filename, FILE_WRITE);

  char header_info[255];

  // convert duration to end date
  time_t duration = fileRecDuration_s + now();

  time_t retval = duration;

  time_t end_day = duration / 86400;
  duration = duration - (86400 * end_day);
  time_t end_hour = duration / 3600;
  duration = duration - (end_hour * 3600);
  time_t end_minutes = duration / 60;
  duration = duration - (end_minutes * 60);
  time_t end_seconds = duration;

  // Store harvesting information in recording file
  sprintf(header_info, "%02d.%02d.%4d;%02d:%02d:%02d;%02lu:%02lu:%02lu;%s;%s;%s;%s;%s;%s", day(),
      month(), year(), hour(), minute(), second(), end_hour, end_minutes, end_seconds,
      harvesting_info[1].c_str(), harvesting_info[2].c_str(),
      harvesting_info[3].c_str(), harvesting_info[4].c_str(),
      harvesting_info[5].c_str(), harvesting_info[6].c_str());
  rec_file.println(header_info);
  rec_file.close();

  return retval;
}

void writeDataToSD(uint8_t _sequence_number, uint32_t _voltage, uint32_t _current)
{
  if (!SD.mediaPresent())
  {
#ifdef DEBUG_MODE
  Serial.printf("SD Card not found");
  delay(50);
#endif
    while (!SD.mediaPresent())
    {
      delay(500);
    }
    resetFunc();
  }

  // 1 + 2 * sizeof(uint32_t) + 2 * ';' + '\n'
  // Writing around 1 + 2 * 4 + 2 * 1 + 1 = 12 byte
  File rec_file = SD.open(filename, FILE_WRITE);
  rec_file.print(_sequence_number);
  rec_file.print(";");
  rec_file.print(_voltage);
  rec_file.print(";");
  rec_file.println(_current);
  rec_file.close();
}

/**
 * Returns the current time in seconds
 * @return Current time in seconds
 */
time_t getTeensy3Time()
{
  return Teensy3Clock.get();
}

void setupTime()
{

  setSyncProvider(getTeensy3Time);

#ifdef DEBUG_MODE
//  if (timeStatus() != timeSet)
//  {
//    Serial.println("Unable to sync with the RTC");
//  }
//  else
//  {
//    Serial.println("RTC has set the system time");
//  }
#endif
}

/**
 * Measures the open circuit voltage and the short circuit voltage
 */
void measureLimits(uint32_t *ocVoltage_uV, uint32_t *scVoltage_uV)
{
  mosfetValues[0] = 0;
  updateHarvesterLoad(0);
  delay(5);												 // wait for voltage to settle
  *ocVoltage_uV = getVoltageFromAdcValue_uV(); // Save open circuit voltage

#ifdef DEBUG_MODE
  Serial.printf("OC voltage: %lu\n", *ocVoltage_uV);
  delay(10);
#endif

  // Minimum load for the solar cell
  mosfetValues[NUMBER_OF_CAPTURED_POINTS_IN_CURVE - 1] = DAC_MAX;
  updateHarvesterLoad(NUMBER_OF_CAPTURED_POINTS_IN_CURVE - 1);
  delay(5);												 // wait for voltage to settle
  *scVoltage_uV = getVoltageFromAdcValue_uV(); // Save short circuit voltage

#ifdef DEBUG_MODE
  Serial.printf("SC voltage: %lu\n", *scVoltage_uV);
  delay(10);
#endif
  updateHarvesterLoad(0);
}

void findMidpoint(uint32_t *ocVoltage_uV, uint16_t *midpoint)
{

  double currError_mV = 0.0;
  double voltage_mV;
  uint8_t counter = 0;
  const double midVoltage_mV = (double)(*ocVoltage_uV >> 1)/1000.0;

  //const double tau = 0.15; 


  /* Find midpoint */
  do {

    //*midpoint = *midpoint + tau * currError_mV;
    if ( currError_mV > 0.0 )
    {
      (*midpoint)++;
    }
    else if ( currError_mV < 0.0 )
    {
      (*midpoint)--;
    }
    else
    {

    }

    dac.setVoltageA(*midpoint);
    dac.updateDAC();
    delay(5);												 // wait for voltage to settle
    voltage_mV = (double)getVoltageFromAdcValue_uV() / 1000.0;
    currError_mV = (voltage_mV - midVoltage_mV);
#ifdef DEBUG_MODE
    Serial.printf("currError_mV: %lf, midpoint %u, midVoltage_mV %lf, voltage_mV %lf\n", currError_mV, *midpoint, midVoltage_mV, voltage_mV);
#endif
    if (counter >= 200)
    {
      //*midpoint = DEFAULT_MIDPOINT;
      break;
    }
    counter++;
  } while (abs(currError_mV) > 100.0);
  updateHarvesterLoad(0);
}

void calcCurve(void)
{
  // Curve parameters
  static uint16_t midpoint = DEFAULT_MIDPOINT;
  uint32_t ocVoltage_uV; // Open circuit voltage
  uint32_t scVoltage_uV; // Open circuit voltage
                             
  measureLimits(&ocVoltage_uV, &scVoltage_uV);
  findMidpoint(&ocVoltage_uV, &midpoint);

  // Define parameters for voltage value calculation
  uint32_t stepsize = (ocVoltage_uV - scVoltage_uV) / (NUMBER_OF_CAPTURED_POINTS_IN_CURVE - 1); // Calculate stepsize
  uint32_t voltageSizes[NUMBER_OF_CAPTURED_POINTS_IN_CURVE];							  // Array for storing the voltage values
  voltageSizes[0] = ocVoltage_uV;														  // Set first value to open circuit voltage
  voltageSizes[NUMBER_OF_CAPTURED_POINTS_IN_CURVE - 1] = scVoltage_uV;						  // set last value to short circuit voltage (~0V)

#ifdef DEBUG_MODE
  Serial.printf("Offset: %d, Upper limit: %d, Step size: %d\n", scVoltage_uV, ocVoltage_uV, stepsize);
  delay(10);
#endif

  // iterate over the array and calculate the voltage values; The array contains voltages in descending order
  for (uint8_t i = (NUMBER_OF_CAPTURED_POINTS_IN_CURVE - 2); i != 0; i--)
  {
    voltageSizes[i] = voltageSizes[i + 1] + stepsize;
  }

//#ifdef DEBUG_MODE
//  for (uint8_t i = 0; i < NUMBER_OF_CAPTURED_POINTS_IN_CURVE; i++)
//  {
//    Serial.printf("voltageSizes[%u] = %u\n", i, voltageSizes[i]);
//    delay(10);
//  }
//#endif

  // calculate the mosfet values for the curve
  for (uint8_t i = 1; i < (NUMBER_OF_CAPTURED_POINTS_IN_CURVE - 1); i++)
  {
    double arg = ((double)ocVoltage_uV / (((double)voltageSizes[i] / SCALE_CURVE) - (double)scVoltage_uV)) - 1.0;

    arg = arg > 0.0 ? arg : 1e-10;

    //Serial.printf("arg[%u] = %lf\n", i, arg);

     double calc = (double)midpoint + (log(arg) / CURVE_STEEPNESS);

    //if ((uint16_t)calc <= DAC_MAX && calc > 0.0)
    if (calc <= 0.0)
    {
        mosfetValues[i] = 0;
    }
    else if ((uint16_t)calc > DAC_MAX)
    {
      mosfetValues[i] = DAC_MAX;
    }
    else
    {
      if ( i >= (NUMBER_OF_CAPTURED_POINTS_IN_CURVE >> 1) && i < (NUMBER_OF_CAPTURED_POINTS_IN_CURVE - 2))
      {
        mosfetValues[i] = mosfetValues[i-1] + 2;
      }
      else
      {
        mosfetValues[i] = (uint16_t)calc;
      }
    }
  }

#ifdef DEBUG_MODE
//  for (uint8_t i = 0; i < NUMBER_OF_CAPTURED_POINTS_IN_CURVE; i++)
//  {
//    Serial.printf("voltageSizes[%u] = %u ", i, voltageSizes[i]);
//    Serial.printf("mosfetValues[%i] = %u\n", i, mosfetValues[i]);
//    delay(10);
//  }
#endif

}

void initDAC(void)
{
  SPI.begin();
  delay(10);
  dac.init();
  dac.turnOnChannelA();
  dac.shutdownChannelB();
  dac.setGainA(MCP4822::High);
}

void turnOffDAC(void)
{
  dac.shutdownChannelA();
}

void turnOnDAC(void)
{
  dac.turnOnChannelA();
  dac.shutdownChannelB();
  dac.setGainA(MCP4822::High);
}
