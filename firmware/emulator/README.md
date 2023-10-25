## Emulation via SD-card
For emulation via SD-card the emulator reads a file named `emulator_file.txt` placed in a folder named `emulating_data` on your SD-card.
The emulator reads the file line by line and restarts, when the number of lines are read. 

The format needs to be as follows:
```
3               // Number of curves
5               // Time between each curve
3443561;3074    // Voltage in uV; Current in uV
3443964;3083
3445273;3058
```
Take care of the delimiter `;` and don't put any comments in the file. 
An example file can be found in the folder `example`

