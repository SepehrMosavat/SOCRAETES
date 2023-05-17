#include "Arduino.h"
#include "Definitions.h"
#include "Functions.h"
#include <TimeLib.h>



void setup()
{
	Serial.begin(115200);
	analogWriteResolution(12);
	pinMode(LED_BUILTIN, OUTPUT);
	digitalWrite(LED_BUILTIN, HIGH);
	initializeOutputToZero();

	#ifdef STAND_ALONE
		while( setupSD() != 0 )
		{
			delay(500);
		}
	#endif
}

int voltage, current = 0;
//byte incomingByteArray[4];
byte byteCounter = 0;
int incommingVoltageAndCurrentBuffer = 0;
bool isReceivingData = false;
bool isEmulating = true;
unsigned long emuDuration_s;

void loop()
{
	//Serial.println(millis());
	static int counter;
#ifdef STAND_ALONE
	if (isEmulating)
	{

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
				digitalToggle(LED_BUILTIN);
			}
			else
			{
			digitalWrite(LED_BUILTIN, HIGH);
			initializeOutputToZero();
			Serial.println("finished");
			delay(1000);
			}
		}
		
		
			
		
	}

#else
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
//				Serial.printf("Emulating: V: %d uV, I: %d uA\r\n", voltage, current);
				emulateVoltageAndCurrent(voltage, current);
				digitalToggle(LED_BUILTIN);
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
	#endif
	delay(1);
}	
