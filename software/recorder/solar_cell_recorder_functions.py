import logging
import queue
import sys
import threading
import time

import serial

from iv_curves_definitions import IvCurve, CurvePoint

logger = logging.getLogger(__name__)
logger.setLevel(logging.DEBUG)
console_log_handler = logging.StreamHandler()
console_log_formatter = logging.Formatter('%(levelname)s - %(message)s')
console_log_handler.setFormatter(console_log_formatter)
logger.addHandler(console_log_handler)


# Function for reading the COM port values
def read_byte_array_from_serial_port(raw_serial_data_queue: queue.Queue, _port, _stop_thread_event: threading.Event):
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
    try:
        ser.open()
    except serial.SerialException:
        print(_port)
        logger.error('Could not open the specified serial port')
        _stop_thread_event.set()
        return None
    if ser.isOpen():
        logger.info("Serial port open")
    else:
        logger.error("Serial port could not be opened")

    while True:
        if _stop_thread_event.isSet():
            ser.close()
            sys.exit()
        serial_bytes_received = ser.read(11)
        serial_bytes_received_as_bytearray = bytearray(serial_bytes_received)

        if serial_bytes_received_as_bytearray[0] == 170 or serial_bytes_received_as_bytearray[10] == 85:  # 0xAA
            raw_serial_data_queue.put(serial_bytes_received_as_bytearray)


# Function for processing received byte arrays and extract the IV curves from them
def process_received_serial_data(raw_serial_data_queue: queue.Queue, captured_curves_queue: queue.Queue,
                                 _stop_thread_event: threading.Event):
    is_iv_curve_being_captured = False
    curve_number_counter = 0
    while True:
        if _stop_thread_event.isSet():
            sys.exit()
        if not raw_serial_data_queue.empty():
            serial_bytes_received_as_bytearray = raw_serial_data_queue.get()

            voltage_bytes = serial_bytes_received_as_bytearray[2:6]
            current_bytes = serial_bytes_received_as_bytearray[6:10]

            point_sequence_number = serial_bytes_received_as_bytearray[1]

            voltage = int.from_bytes(voltage_bytes, byteorder='little', signed=False)
            current = int.from_bytes(current_bytes, byteorder='little', signed=False)

            if point_sequence_number == 1 and is_iv_curve_being_captured is False:
                # Start capturing IV curve
                is_iv_curve_being_captured = True
                captured_curve = IvCurve(curve_number_counter)

            if is_iv_curve_being_captured is True:
                if point_sequence_number == captured_curve.number_of_points_in_curve:
                    # Stop capturing IV curve
                    is_iv_curve_being_captured = False
                    captured_curves_queue.put(captured_curve)
                    curve_number_counter += 1
                    logger.info("Curve captured: " + str(captured_curve.curve_number))

                captured_curve_point = CurvePoint(point_sequence_number, voltage / 1000000, current)
                captured_curve.add_point_to_curve(captured_curve_point)

            time.sleep(0.001)
