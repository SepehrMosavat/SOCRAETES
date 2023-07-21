#include "Arduino.h"
#include "Definitions.h"
#include "Functions.h"

MCP4822 dac(34);

#define	MODE_PC  0
#define	MODE_SD  1

static int voltage = 0;
static int current = 0;
//byte incomingByteArray[4];
static byte byteCounter = 0;
static int incommingVoltageAndCurrentBuffer = 0;
static bool isReceivingData = false;
static bool isEmulating = true;
static unsigned long emuDuration_s;

static int mode = MODE_PC;

void setup()
{
	uint8_t counter;

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
	pinMode(MODE_JUMPER, INPUT_PULLDOWN);

	digitalWrite(STATUS_LED, LOW);
	digitalWrite(ERROR_LED, LOW);	

	for (counter = 0; counter < 10; counter++)
	{

		digitalToggle(STATUS_LED);
		delay(500);
	}

	mode =  digitalRead(MODE_JUMPER);

	Serial.printf("Starting in mode %s", mode ? "SD" : "PC");
}


void loop()
{
	static int counter;

	if (mode == MODE_SD)
	{
		if (isEmulating)
		{
			while( setupSD() != 0 )
			{	
				digitalWrite(ERROR_LED, HIGH);
				delay(500);
			}
			digitalWrite(ERROR_LED, LOW);

		emulateVoltageAndCurrent(emu_parameters.emu_voltage[0], emu_parameters.emu_current[0]);
		isEmulating = false;
		emuDuration_s = emu_parameters.emu_duration + millis();
		counter = 1;
		}
		else
		{
			if (millis() > emuDuration_s)
			{
				if (counter < emu_parameters.number_curves)
				{
				emulateVoltageAndCurrent(emu_parameters.emu_voltage[counter], emu_parameters.emu_current[counter]);
				emuDuration_s = emu_parameters.emu_duration + millis();
				Serial.println(emu_parameters.emu_voltage[counter]);
				Serial.println(emu_parameters.emu_current[counter]);
				counter++;
				digitalToggle(STATUS_LED);
				}
				else
				{
				digitalWrite(STATUS_LED, HIGH);
				initializeOutputToZero();
				Serial.println("finished");
				delay(1000);
				Serial.println("again");
				digitalWrite(STATUS_LED, LOW);
				isEmulating = true;
				}
			}	
		}
	}
	else
	{
		if(Serial.available() > 0)
		{	
			digitalWrite(ERROR_LED, LOW);
			byte lastReceivedByte = Serial.read();
			if(lastReceivedByte == START_OF_CURVE_DATA)
			{
				byteCounter = 0;
				voltage = 0;
				current = 0;
				isReceivingData = true;
			}
			else if(lastReceivedByte == END_OF_CURVE_DATA)
			{
				isReceivingData = false;
				Serial.printf("Emulating: V: %d uV, I: %d uA\r\n", voltage, current);
				emulateVoltageAndCurrent(voltage, current);
				digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN)); // Toggle LED on each curve change
			}
			else
			{
				if(isReceivingData)
				{
					incommingVoltageAndCurrentBuffer = incommingVoltageAndCurrentBuffer | lastReceivedByte;
					if(byteCounter == 3)
					{
						voltage = incommingVoltageAndCurrentBuffer;
					}
					else if(byteCounter == 7)
					{
						current = incommingVoltageAndCurrentBuffer;
					}
					incommingVoltageAndCurrentBuffer = incommingVoltageAndCurrentBuffer << 8;

					byteCounter++;
				}
			}
		}
		else
		{
			digitalWrite(ERROR_LED, HIGH);
		}
	}
	delay(1);
}
