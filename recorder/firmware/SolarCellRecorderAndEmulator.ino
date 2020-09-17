#include <Arduino.h>
#include <Encoder.h>
#include <ADC.h>
#include <SPI.h>

#include "Definitions.h"
#include "HelperFunctions.h"



extern byte uartByteArray[11];
extern ADC *adc;

void setup() {
  Serial.begin(115200);
  SPI.begin();

  pinMode(DIGITAL_POT_CS_PIN, OUTPUT);
  pinMode(DAC_OUTPUT_PIN, OUTPUT);
  pinMode(HARVESTER_CAPTURING_STATUS_PIN, OUTPUT);

  pinMode(A10, INPUT); //Diff Channel 0 Positive
  pinMode(A11, INPUT); //Diff Channel 0 Negative

  initializeADC();
}

int digitalPotValue = 0;
bool isCapturingHarvester = false;

void loop() {
  // Read encoder and update DAC voltage accordingly
  updateDacOutputVoltage();

  // Read ADC inputs for voltage and current calculations
  int currentSenseAdcValue = adc->analogReadDifferential(A10, A11, ADC_0);
  int voltageAdcValue = adc->analogRead(A2, ADC_1);

  int voltage = getVoltageFromAdcValue(voltageAdcValue, 1);
  int current = getCurrentFromAdcValue(currentSenseAdcValue, CURRENT_SENSE_PGA_GAIN);

  convertIntValuesToByteArrays(digitalPotValue, voltage, current, uartByteArray);

#ifdef DEBUG_MODE
  Serial.printf("Seq. No.: %d, V: %d, I: %d\n", uartByteArray[1], voltage, current);
#else
  for(int i = 0; i < 11; i++)
  {
    Serial.write(uartByteArray[i]);
  }
#endif

  writeToDigitalPot(digitalPotValue);
  digitalPotValue++;
  if(digitalPotValue > NUMBER_OF_CAPUTURED_POINTS_IN_CURVE)
  {
	  digitalWrite(HARVESTER_CAPTURING_STATUS_PIN, LOW);
	  digitalPotValue = 0;
	  writeToDigitalPot(digitalPotValue);
	  delay(10);
	  digitalWrite(HARVESTER_CAPTURING_STATUS_PIN, HIGH);
  }

//  if(isCapturingHarvester)
//  {
//    digitalWrite(HARVESTER_CAPTURING_STATUS_PIN, HIGH);
//
//    // Update the digital potentioemeter's value
//    if(digitalPotValue > 126)
//    {
//      isCapturingHarvester = false;
//    }
//    writeToDigitalPot(digitalPotValue++);
//    delay(10);
//  }
//  else
//  {
//    digitalWrite(HARVESTER_CAPTURING_STATUS_PIN, LOW);
//    delay(10);
//    digitalPotValue = 0;
//    isCapturingHarvester = true;
//  }
  //delay(2000);
}
