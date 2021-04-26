#include "Arduino.h"
#include "Definitions.h"
#include "Functions.h"



void setup()
{
	Serial.begin(115200);
	analogWriteResolution(12);

	pinMode(LED_BUILTIN, OUTPUT);

	initializeOutputToZero();
}

int voltage, current = 0;
//byte incomingByteArray[4];
byte byteCounter = 0;
int incommingVoltageAndCurrentBuffer = 0;
bool isReceivingData = false;


void loop()
{
	if(Serial.available() > 0)
	{
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
//			Serial.printf("Emulating: V: %d uV, I: %d uA\r\n", voltage, current);
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
}
