#include "Arduino.h"
#include "Definitions.h"
#include "Functions.h"

MCP4822 dac(34);

void setup()
{
	Serial.begin(0);
	SPI.begin();

	dac.init();
	dac.turnOnChannelA();
	dac.turnOnChannelB();
	dac.setGainA(MCP4822::High);
	dac.setGainB(MCP4822::High);

	initializeOutputToZero();

	pinMode(STATUS_LED, OUTPUT);
	pinMode(ERROR_LED, OUTPUT);
	pinMode(MODE_JUMPER, INPUT_PULLUP);

	digitalWrite(STATUS_LED, LOW);
	digitalWrite(ERROR_LED, LOW);	

	delay(50); // Initial delay to let the jumper input pull-up settle
}

// *************** Experiment Configuration ***************
unsigned int traceDuration_s = 60;
unsigned int onCycleDuration_ms = 20;
// *************** Experiment Configuration ***************

unsigned int emulationVoltage_uv = 3000000; // VCC: 3 V
unsigned int emulationCurrent_ua = 45000;   // Maximum current: 45 mA

void loop()
{
	while(digitalRead(MODE_JUMPER)) {
		initializeOutputToZero();
		digitalWrite(ERROR_LED, HIGH);
		delay(1000);
	}
	digitalWrite(ERROR_LED, LOW);

	signed long timerCounter = (traceDuration_s * 1000) / (2 * onCycleDuration_ms);
	while(timerCounter-- > 0) {
		emulateVoltageAndCurrent(emulationVoltage_uv, emulationCurrent_ua);
		digitalWrite(STATUS_LED, HIGH);
		delay(onCycleDuration_ms);

		emulateVoltageAndCurrent(0, 0);
		digitalWrite(STATUS_LED, LOW);
		delay(onCycleDuration_ms);
	}

	while(!digitalRead(MODE_JUMPER)) {
		initializeOutputToZero();
		digitalWrite(ERROR_LED, HIGH);
		delay(1000);
	}
}
