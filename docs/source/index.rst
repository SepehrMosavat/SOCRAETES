Welcome to the documentation of SOCRAETES!
===========================================

**Sayedsepehr Mosavat, Matteo Zella, Pedro José Marrón**
**Networked Embedded Systems Group (NES) - Universität Duisburg-Essen**

SOCRAETES is a tool for supporting the design and evaluation of energy harvesting systems. The core idea behind the development of SOCRAETES has been to enable easy replication of the required hardware components, therefore decreasing the entry-barrier into this field of research, even for researchers and hobbyists that have little prior experience with electronics. This is achieved by simplifying the hardware design as much as possible, and also by employing only through-hole components in the required hardware.

The hardware design of SOCRAETES provides a high level of flexibility in terms of the range of measurements and emulations. Since there is a tradeoff between this range and the overall accuracy of the system, the users have the possibility of modifying the underlying hardware by using different passive components. By doing so, each user can target the best overall range of measurements and emulation, while also achieving a high level of accuracy.

Please refer to our demo abstract, presented at EWSN 2021, for more information regarding the design and implementation of SOCRAETES.

To use SOCRAETES, the following prerequisites exist:

    * SOCRAETES hardware, either assembled on a prototyping board, or optionally, using the PCB found in the hardware directory. Please note that the full functionality of SOCRAETES can be realized using prototyping boards, but the highest reliability and accuracy will be achieved only if the PCB is used. Please refer to the hardware directory for instructions regarding preparing the hardware
    * SOCRAETES firmware, which will be running on a Teensy 3.6. Please refer to the firmware directory for instructions on flashing the Teensy board.
    * SOCRAETES software, which will be running on a host PC and used for acquiring data and emulating traces. The current implementation of SOCRAETES is using Python code, but we are working on extensions of this functionality for further data visualization and analysis.

Contact us
-----------

In case you experience any issues while using SOCRAETES, or have any questions or ideas for further improvements, please do not hesitate to contact us via e-mail! You can find our contact information on our web page.
