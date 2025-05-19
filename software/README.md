## SOCRAETES: SOlar Cells Recorded And EmulaTed EaSily

This document provides the instructions for setting up and using the
software of SOCRAETES. The software is meant for being run on a host system,
to which the SOCRAETES hardware is connected. The software, developed in Python,
will communicate with the hardware and provide a user interface for functionalities
such as data visualization and retention.

The current software provides the user a command line interface to either record or emulate
energy harvesting traces of a solar cell. Either functionality can be used
as follows.

#### Requirements
The dependencies of the Python code can be found in *requirements.txt*. The
dependencies can be installed using the following command:
```
pip install -r requirements.txt
```
#### Recording
The following examples demonstrate how to record traces in different modes.
To record a trace and visualize it using a 2D plot:
```
record.py --port <Serial Port>
```
The only necessary parameter is the serial port, on which the hardware is
communicating with the host system. Not providing further parameters will
record a trace in the default operation mode, which is 2D plotting of the trace.

The above example could also be run explicitly using the following command:
```
record.py --port <Serial Port> --mode plot-curve
```
Alternatively, the trace can be visualized with a 3D surface by using the following
command:
```
record.py --port <Serial Port> --mode plot-surface
```
Furthermore, a trace can be captured for 30 seconds and saved on the local
machine in an HDF5 file using the following command:
```
record.py --port <Serial Port> --mode commit-to-file --duration 30
```
The command above will automatically generate a unique file name for the output
file. However, the file name can be chosen explicitly by the user using the
following command:
```
record.py --port <Serial Port> --mode commit-to-file --file <File Name> --duration 30
```
Last but not least, the energy harvesting environment can be described by
parameters that will be appended to the output file as metadata. Such data can
be used for data analysis in later stages. For example, the following command
will capture 30 seconds of data in an outdoor situation on a sunny day, in
Berlin, Germany. The ambient light intensity of the environment is estimated
to be 150 Lux:
```
record.py --port <Serial Port> --mode commit-to-file --duration 30 --environment indoor
--lux 150 --weather sunny --country Germany --city Berlin
```
#### Emulation
The following examples demonstrate how to emulate energy harvesting traces using
SOCRAETES. The emulation can be performed either by using a file containing
previously-recorded traces or by using a user-defined array with arbitrary
data.

The following command will emulate a trace from a file:
```
emulate.py --port <Serial Port> --source file --file <File Name>
```
An example under Linux would be:
```
emulate.py --port /dev/ttyACM0 --source file --file recorded_trace.hdf5
```
On the other hand, the following command can be used for emulating a user-defined
set of operation parameters:
```
emulate.py --port <Serial Port> --source array --array [[0.5, 0], [1, 1000], [2, 2000]]
```
The user-defined array has the following format: ``[[<DELAY BETWEEN CURVES (s)>,0],[OPEN CIRCUIT VOLTAGE (V)>,<SHORT CIRCUIT CURRENT (uA)>],...]``
In the previous example two curves will be emulated with a delay of 0.5 seconds
between each. The first curve will have an open-circuit voltage of 1V and a
short-circuit current of 1000uA. The second curve will have an open-circuit
voltage of 2V and a short-circuit current of 2000uA.
