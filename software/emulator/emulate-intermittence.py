import logging
import queue
import signal
import sys
import threading
import time

import fire
import numpy as np

from curve_emulation import emulate_intermittence
from serial_communication import serial_port_handling

serial_port = None
emulation_timing = None

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


def cli(port, timing=[1, 1]):
    """
    :param port: The serial port used for communication with the hardware.
    :param timing: An array in the form of [<POWER ON PERIOD (s)>,<POWER OFF PERIOD (s)>]. Using this array the user\n
    can adjust the behavior of the intermittent power supply being emulated for powering the load.
    Default value: [1,1]
    :return:
    """
    global serial_port
    global emulation_timing
    # global trace_emulation_source
    # if source == 'array':
    #     curve_emulation_method = CurveEmulationMethod.EMULATE_TRACE_FROM_ARRAY
    #     trace_emulation_source = trace_emulation_array = np.array(array)  # [[delay(s), 0], []]
    # else:
    #     curve_emulation_method = CurveEmulationMethod.EMULATE_TRACE_FROM_FILE
    #     trace_emulation_source = trace_emulation_file = file
    serial_port = port
    emulation_timing = timing


if __name__ == '__main__':
    signal.signal(signal.SIGINT, interrupt_signal_handler)
    fire.Fire(cli)
    serial_port_handling_thread = threading.Thread(target=serial_port_handling, args=[trace_emulation_queue,
                                                                                      stop_thread_event, serial_port])
    intermittence_emulation_thread = threading.Thread(target=emulate_intermittence,
                                                      args=[emulation_timing, trace_emulation_queue, stop_thread_event])
    serial_port_handling_thread.daemon = True
    intermittence_emulation_thread.daemon = True
    serial_port_handling_thread.start()
    intermittence_emulation_thread.start()
    while True:
        if stop_thread_event.isSet():
            logger.info("Emulation finished. Exiting...")
            time.sleep(1)
            sys.exit()
        time.sleep(1)
