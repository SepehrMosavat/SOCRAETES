####################################################
Firmware
####################################################

This page provides instructions for compiling and flashing the firmware
on the MCU of SOCRAETES.
- The current SOCRAETES hardware utilizes a
`Teensy 4.1 <https://www.pjrc.com/store/teensy41.html>`_ development board. 
- Compiling and flashing the Teensy board works with
- the Plugin
`PlatformIO <https://docs.platformio.org/en/latest/what-is-platformio.html>`_.
- The recorder is designed to measure the voltage/current curve of an energy source, for example of a solar cell.  
- The measured curve can be either stored on a SD-card or sent to the PC via the USB-connection (serial). 
- For evaluating the data use the python tool provided in `SOCRAETES/software/recorder`

The main script of the firmware is running in the SOCRAETES_recorder.ino file.
The selection between the stand-alone mode and the mode connected via a serial
communication with a PC is made via a jumper on the hardware.  


Recorder firmware
##################

First of all everything gets initialized. After that, the range of the 
MOSFET-transistor gets determined automatically. This has to be done, because
the MOSFET range is depending on the temperature, which can vary due to the
location or because the recorder hardware is standing outside and undergoes
significant temperature changes due to sunny or cloudy weather. The MOSFET range
is automatically adjusted after each curve. 
After the initialization of the hardware components, the firmware is depending
on the chosen mode (stand-alone or PC). 

PC-mode
****************

If the PC-mode is running, the firmware 
is waiting for a starting signal from the corresponding software. If the 
starting signal is send, the firmware starts capturing traces. After a whole
trace (40 points), the curve values are send to the software, and the firmware
waits until the software has processed the curve and is sending back a signal.
This is going on until you stop the software. The signals are built-in to
assure that the curve points and their visualization are up to date and are not 
lagging behind because of buffers or a slower processing of the software.
 
Stand-alone mode
*****************

If you choose the stand-alone mode, the data of the curves is stored on a sd card.
The configuration, how long each measurement should run and where and under which
condition it was created, is read in from a configuration textfile on the sd-card.
The data of the measurement, is stored in an seperate CSV-file for each measurement.
A directory for these files will be created automatically, if it didn't existed before.
When the measurement started and stopped will be also automatically stored on the 
CSV-file of a measurement. the date will be correct, because of a built-in rtc-module,
that the teensyboard has and an super-capacitor, which is on the hardware and will
provide the clock for a period, even if the teensyboard is disconnected from a power supply.
The conversion from CSV to a graphical view or a storing 


Emulator firmware
#################






