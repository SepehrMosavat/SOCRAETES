#include <Arduino.h>
#include <Encoder.h>
#include <ADC.h>
#include <SPI.h>

#include "Definitions.h"
#include "HelperFunctions.h"



extern byte uartByteArray[11];
extern ADC *adc;
extern int ivCurveSequenceNumber;

void setup() {
  Serial.begin(115200);
  SPI.begin();

  pinMode(HARVESTER_CAPTURING_STATUS_PIN, OUTPUT);

  pinMode(A10, INPUT); //Diff Channel 0 Positive
  pinMode(A11, INPUT); //Diff Channel 0 Negative
	analogWriteResolution(12);

  initializeADC();
}

bool isCapturingHarvester = false;

void loop() {
  // Read ADC inputs for voltage and current calculations
  int currentSenseAdcValue = adc->analogReadDifferential(A10, A11, ADC_0);
  int voltageAdcValue = adc->analogRead(A2, ADC_1);

  int voltage = getVoltageFromAdcValue(voltageAdcValue, 1);
  int current = getCurrentFromAdcValue(currentSenseAdcValue, CURRENT_SENSE_PGA_GAIN);

  convertIntValuesToByteArrays(digitalPotValue, voltage, current, uartByteArray);
	updateHarvesterLoad();

#ifdef DEBUG_MODE
  Serial.printf("Seq. No.: %d, V: %d, I: %d\n", uartByteArray[1], voltage, current);
#else
  for(int i = 0; i < 11; i++)
  {
    Serial.write(uartByteArray[i]);
  }
#endif

}
