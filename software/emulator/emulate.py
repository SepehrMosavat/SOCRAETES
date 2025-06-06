#!/usr/bin/env python3
import logging
import queue
import signal
import sys
import threading
import time

import fire
import numpy as np

from curve_emulation import emulate_curve, write_curve_info_to_sd
from definitions import CurveEmulationMethod
from serial_communication import serial_port_handling

serial_port = None
trace_emulation_source = None
curve_emulation_method = None
sdMode = False

trace_emulation_queue = queue.Queue()
stop_thread_event = threading.Event()

logger = logging.getLogger(__name__)
logger.setLevel(logging.DEBUG)
console_log_handler = logging.StreamHandler()
console_log_formatter = logging.Formatter('%(levelname)s - %(message)s')
console_log_handler.setFormatter(console_log_formatter)
logger.addHandler(console_log_handler)


def interrupt_signal_handler(_signal, _frame):
    stop_thread_event.set()


def cli(port, source='array', file='trace_0.hdf5', sd_mode=False, array=[[1, 0], [1, 1000]]):
    """
    :param port: The serial port used for communication with the hardware.
    :param source: The source of emulation data:\n
    array: A user-specified array containing the emulation parameters. (see array below)
    file: An HDF5 file containing the traces previously recorded.
    Default value: array
    :param file: The file name of the HDF5 file used for emulation from a file.\n
    Default value: trace_0.hdf5
    :param sd_mode: Turn to true if you want to create a txt file, which will then work on a stand alone emulator.\n
    Default value: False
    :param array: An array in form of [[<DELAY BETWEEN CURVES (s)>,0],[OPEN CIRCUIT VOLTAGE (V)>,<SHORT CIRCUIT CURRENT (uA)>],...]\n
    Default value: [[1,0],[1,1000]], which will result in the following parameters: OC voltage: 1v,
    SC current: 1000uA = 1mA, and 1 second of delay between each curve.
    :return:
    """
    global serial_port
    global curve_emulation_method
    global trace_emulation_source
    global sdMode
    if source == 'array':
        curve_emulation_method = CurveEmulationMethod.EMULATE_TRACE_FROM_ARRAY
        trace_emulation_source = trace_emulation_array = np.array(array)  # [[delay(s), 0], []]
    else:
        curve_emulation_method = CurveEmulationMethod.EMULATE_TRACE_FROM_FILE
        trace_emulation_source = trace_emulation_file = file
        sdMode = sd_mode
    serial_port = port


if __name__ == '__main__':
    signal.signal(signal.SIGINT, interrupt_signal_handler)
    fire.Fire(cli)
    if sdMode:
        write_curve_info_to_sd(trace_emulation_source, stop_thread_event)

    else:
        serial_port_handling_thread = threading.Thread(target=serial_port_handling, args=[trace_emulation_queue,
                                                                                      stop_thread_event, serial_port])
        curve_emulation_thread = threading.Thread(target=emulate_curve, args=[trace_emulation_source, trace_emulation_queue,
                                                                          curve_emulation_method, stop_thread_event])
        serial_port_handling_thread.daemon = True
        curve_emulation_thread.daemon = True
        serial_port_handling_thread.start()
        curve_emulation_thread.start()
        while True:
            if stop_thread_event.isSet():
                logger.info("Emulation finished. Exiting...")
                time.sleep(1)
                sys.exit()
        time.sleep(1)
