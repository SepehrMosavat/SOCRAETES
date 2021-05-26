import logging
import queue
from threading import Event

import serial

from definitions import DebugMode
from system_configurations import DEBUG_MODE

logger = logging.getLogger(__name__)
logger.setLevel(logging.DEBUG)
console_log_handler = logging.StreamHandler()
console_log_formatter = logging.Formatter('%(levelname)s - %(message)s')
console_log_handler.setFormatter(console_log_formatter)
logger.addHandler(console_log_handler)


def serial_port_handling(_trace_emulation_queue: queue.Queue, _stop_thread_event: Event, _port):
    if DEBUG_MODE is DebugMode.NO_DEBUG or DEBUG_MODE is DebugMode.DEBUG_WITH_HARDWARE:
        try:
            ser = serial.Serial(
                port=_port,
                baudrate=115200,
                parity=serial.PARITY_NONE,
                stopbits=serial.STOPBITS_ONE,
                bytesize=serial.EIGHTBITS
            )
        except serial.SerialException:
            logger.error('Could not open the specified serial port')
            _stop_thread_event.set()
            return None

        ser.close()
        ser.open()

        if ser.isOpen():
            logger.info('Serial port opened successfully')
        else:
            logger.error('Serial port could not be opened')

    logger.info('Emulation Started...')
    curve_counter = 0
    while True:
        if not _trace_emulation_queue.empty():
            curve_emulation_parameters = _trace_emulation_queue.get()
            outgoing_serial_byte_array = b'\xaa'  # Start of sequence
            outgoing_serial_byte_array = outgoing_serial_byte_array + \
                                         int.to_bytes(int(curve_emulation_parameters.open_circuit_voltage), 4,
                                                      byteorder='big', signed=False)  # OC voltage
            outgoing_serial_byte_array = outgoing_serial_byte_array + \
                                         int.to_bytes(int(curve_emulation_parameters.short_circuit_current), 4,
                                                      byteorder='big', signed=False)  # SC current
            outgoing_serial_byte_array = outgoing_serial_byte_array + b'\x55'  # End of sequence

            if DEBUG_MODE is DebugMode.NO_DEBUG or DEBUG_MODE is DebugMode.DEBUG_WITH_HARDWARE:
                ser.write(outgoing_serial_byte_array)
                logger.info(f'Curve {curve_counter:5}' +
                      f':   OC voltage: {int(curve_emulation_parameters.open_circuit_voltage):7}' +
                      f' uV, SC current: {int(curve_emulation_parameters.short_circuit_current):5}' +
                      ' uA')
                if DEBUG_MODE is DebugMode.DEBUG_WITH_HARDWARE:
                    serial_bytes_received = ser.readline()
                    logger.debug(serial_bytes_received.decode('UTF-8'))
            elif DEBUG_MODE is DebugMode.DEBUG_WITHOUT_HARDWARE:
                logger.debug(f'Curve {curve_counter:5}' +
                      f':   OC voltage: {int(curve_emulation_parameters.open_circuit_voltage):7}' +
                      f' uV, SC current: {int(curve_emulation_parameters.short_circuit_current):5}' +
                      ' uA')
            curve_counter = curve_counter + 1
