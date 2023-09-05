###########################
Getting started / Tutorial
###########################

In this section you can find useful examples, how to work with with the
SOCRAETES-Tool. In each section you will find a work-along example, so that
you will learn to work with the basics of this tool. As a prerequisite for the 
whole section, you will need the hardware, described on the hardware section and
an teensyboard 4.1. Furthermore you have to download/clone our
`Github project <https://github.com/SepehrMosavat/SOCRAETES>`_.
All the other software prerequisites, that you will need, are 
described in the corresponding sub-section on this site.


Recorder
#########

Firmware
*********

Prerequisites
==============

For the firmware you will need Teensyduino.
We highly recommend to install it via `PlatformIO <https://docs.platformio.org/en/latest/index.html>`_
on  `Visual Studio Code <https://code.visualstudio.com/>`_ ,
because in our opinion its the most powerful and user friendly framework to work with on such projects.
After that you are ready to begin. In this tutorial the procedure is described using platformIO.

Uploading to board
===================

First, you will have to decide, which mode you want to use for the recording.

You can use a stand-alone mode, where the teensyboard with the hardware
can be disconnected after the upload from the computer and can be powered via the micro-usb port from
an extern power supply like a powerbank. This might be useful, if you want to collect data outside.

Attention: Powering the teensyboard via VIN doesn't seem to work properly with some attached hardware,
like in our project!

If you choose the pc-mode, the teensyboard with the hardware has to be connected via serial 
with the pc also after the upload.
Advantages of this mode are, that you can easily check the data that is collected and 
this even in real time with no lags, because the firmware waits, until your computer has processed a curve
of data points. 

In this tutorial we will start using the pc-mode, so you can get a first overview, what data is collected and how
easy it is to use SOCRAETES.

Before uploading the firmware to your teensyboard please plug a jumper on the hardware for using pc mode. This is very importing
because the firmware just checks relatively on the beginning which mode to use.

Now we can get to the actual uploading. Just open the subfolder firmware/recorder of our project with VS code.
PlatformIO will automatically begin to activate and initialize. After a short it is ready to use. Plug the teensyboard with
our hardware on the pc and click uploading in VS code (on the bottom left corner(or just press ctrl+alt+u)). The firmware is 
uploading to your teensyboard.


Software
*********

Prerequisites
==============

For the software you will need `python <https://www.python.org/>`_ and some python packages. To install the necessary
packages, open up a terminal , go to the subfolder software/recorder of our project and run the following command:

.. code-block:: console

    pip install -r requirements.txt

After that all the necessary packages are installed.

Running the software is as easy as running the firmware. For a detailed view, what you can do, just look in our
software section.

In this example we are using just the 3d plot mode. To run the software with 3D mode just type in the following command
in your terminal in the same subfolder as the prerequisites.

.. code-block:: console 

    record.py COM7 --mode "plot-surface"


The code will run forever until you press ctrl+c. If it's very cloudy, the curves could be very flat.
To rise the curve you can use a flashlight to see how the curve would be in a more sunny environment.
To limit the runtime of the code to 20 seconds we type:

.. code-block:: console 

    record.py COM7 --mode "plot-surface" --duration 20


Recorder (stand-alone)
#######################

Prerequisites
**************
The software prerequisites are the same as in the tutorial before. As a hardware
prerequisite you need also a sd card.

Firmware
********

After the successful execution of the recorder in combination with the pc mode, we are now going to use the recorder in stand-alone mode.
For this mode you will have to prepare a micro sd card. Create a new file with the name configuration_file.txt.
The content of the configuration_file.txt has to look like:

.. code-block:: text
    
    duration=10
    Indoor/Outdoor=Indoor
    Lux=150
    weather=sunny
    Country=Germany
    City=Krefeld
    Harvesting Source=solar cell

It is important, that you don't change the use of each content in each line. Everything after the equal sign gets read in from the 
firmware. In the above example each measurement has a duration of 10 seconds. We are recording indoor with a light intensity
of approximately 150 lux. The weather is sunny in the beautiful city Krefeld in Germany. As a harvesting source we are using a solar cell.
One thing you have to pay attention to is not using spaces between the equal sign and your content.

After modifying the configuration file, copy it to the root directory of your sd card and insert the sd card in your teensyboard.
Change the jumper on the hardware from P/C mode to stand-alone mode. Upload the firmware in the same way as in the tutorial above
and everything for the firmware is done. Every measurement is saved in an own CSV-file.

With the CSV file you can do what you want, but you can also use SOCRAETES to view the recorded data or transform the data to a hdf-file,
a file format for storing big data sets. 

Software
*********

There are in principle the same options for viewing or storing the data as if you were using pc mode. 
The only thing you have to do is copying the new created folder recording_data from your sd card to the directory
of the recorder software (software/recorder)
If you would like to see the curve of one measurement (one CSV file) in 3D plot surface mode, enter
for example for file measurement3.csv the following command in the subfolder software/recorder:

.. code-block:: console 

    python record.py None --mode "plot-surface" --read_from_sd "one-file" --doc_name "measurement3.csv" 

If you want to convert the CSV files to hdf5 files just type:

.. code-block:: console

    python record.py None --read_from_sd "all"

The hdf5 files are stored in the directory "captured_traces".



Emulator
#########

Firmware
*********

Prerequisites
==============

For the firmware you will need Teensyduino.
We highly recommend to install it via `PlatformIO <https://docs.platformio.org/en/latest/index.html>`_
on  `Visual Studio Code <https://code.visualstudio.com/>`_ ,
because in our opinion its the most powerful and user friendly framework to work with on such projects.
After that you are ready to begin. In this tutorial the procedure is described using platformIO.

Uploading to board
===================

First, you will have to decide, which mode you want to use for emulating similar to the recorder.

You can use a stand-alone mode, where the teensyboard with the hardware
can be disconnected after the upload from the computer and can be powered via the micro-usb port from
an extern power supply like a powerbank. This might be useful, if you want to emulate data outside.

Attention: Powering the teensyboard via VIN doesn't seem to work properly with some attached hardware,
like in our project!

If you choose the pc-mode, the teensyboard with the hardware has to be connected via serial 
with the pc also after the upload.

In this tutorial we will using the pc-mode.

Before uploading the firmware to your teensyboard please plug a jumper on the hardware for using pc mode. This is very importing
because the firmware just checks relatively on the beginning which mode to use.

Now we can get to the actual uploading. Just open the subfolder firmware/emulator of our project with VS code.
PlatformIO will automatically begin to activate and initialize. After a short it is ready to use. Plug the teensyboard with
our hardware on the pc and click uploading in VS code (on the bottom left corner(or just press ctrl+alt+u)). The firmware is 
uploading to your teensyboard.


Software
*********

Prerequisites
==============

For the software you will need `python <https://www.python.org/>`_ and some python packages. To install the necessary
packages, open up a terminal , go to the subfolder software/emulator of our project and run the following command:

.. code-block:: console

    pip install -r requirements.txt

After that all the necessary packages are installed.

Running the software is as easy as running the firmware. For a detailed view, what you can do, just look in our
software section.

In this example we are using 

.. code-block:: console 

    python emulate.py COM7 -f trace_3.hdf


The code will run forever until you press ctrl+c. 

