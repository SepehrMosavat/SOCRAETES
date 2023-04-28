## SOCRAETES: SOlar Cells Recorded And EmulaTed EaSily

This document provides instructions for compiling and flashing the firmware
on the MCU of SOCRAETES. 
- The current SOCRAETES hardware utilizes a
[Teensy 3.6](https://www.pjrc.com/store/teensy36.html) development board. 
- Compiling and flashing the Teensy board works with the Plugin
[PlatformIO](https://docs.platformio.org/en/latest/what-is-platformio.html).
- The recorder is designed to measure the voltage/current curve of a connected solar cell. 
- The measured curve can be either stored on a SD-card or sent to the PC via the USB-connection (serial). 
- For evaluating the data use the python tool provided in `SOCRAETES/software/recorder`

