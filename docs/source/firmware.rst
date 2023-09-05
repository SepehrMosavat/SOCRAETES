####################################################
Firmware
####################################################

This page provides instructions for compiling and flashing the firmware
on the MCU of SOCRAETES.

- The current SOCRAETES hardware utilizes a
  `Teensy 4.1 <https://www.pjrc.com/store/teensy41.html>`_ development board. 

- Compiling and flashing the Teensy board works with the Plugin
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
condition it was created, is read in from a configuration text file on the sd-card.
The format needs to be as follows:

.. code-block:: text

    duration=20
    Indoor/Outdoor=Indoor
    Lux=50
    weather=sunny
    Country=N/A
    City=N/A
    Harvesting Source=solar cell

Please take care to not use any comments and write your specifications directly behind
the equal sign (no spaces). The configuration file must be placed in the root 
folder of your sd card with the name configuration_file.txt .



The data of the measurement, is stored in an separate CSV-file for each measurement.
A directory for these files will be created automatically, if it didn't existed before.
When the measurement started and stopped will be also automatically stored on the 
CSV-file of a measurement. the date will be correct, because of a built-in rtc-module,
that the teensyboard has and an super-capacitor, which is on the hardware and will
provide the clock for a period, even if the teensyboard is disconnected from a power supply.
The conversion from CSV to a graphical view or a conversion to a hdf5 file, is done by the software.


Emulator firmware
#################

The emulator firmware is used to emulate curves, which were captured with the recorder firmware.
It uses the short-circuit current and the open-loop voltage as an input and emulate these for the
duration of the original curve capturing time.


PC-mode
***********

If the pc mode is used, the software sends the firmware the data and the firmware is checking,
if the data was transmitted correctly. If so the data gets emulated, otherwise it is thrown away.


Stand-alone mode
*****************

For emulation via SD-card the emulator reads a file named `emulator_file.txt` placed in a folder named `emulating_data` on your SD-card.
The emulator reads the file line by line and restarts, when the number of lines are read. 

The format needs to be as follows:

.. code-block:: text

    3               // Number of curves
    5               // Time between each curve
    3443561;3074    // Voltage in uV; Current in uV
    3443964;3083
    3445273;3058


Take care of the delimiter `;` and don't put any comments in the file. 

After reading it, it emulates each curve for a duration of the time in line 2 of the 
configuration file (Time between each curve). 







