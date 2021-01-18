import queue
from threading import Event
import sys

import serial

from system_configurations import DEBUG_MODE


def serial_port_handling(_trace_emulation_queue: queue.Queue, _stop_thread_event: Event, _port):
    if DEBUG_MODE is False:
        try:
            ser = serial.Serial(
                port=_port,
                baudrate=115200,
                parity=serial.PARITY_NONE,
                stopbits=serial.STOPBITS_ONE,
                bytesize=serial.EIGHTBITS
            )
        except serial.SerialException:
            print("Could not open the specified serial port")
            _stop_thread_event.set()
            return None

        ser.close()
        ser.open()

        if ser.isOpen():
            print("Serial port opened successfully")
        else:
            print("Serial port could not be opened")

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

            if DEBUG_MODE is False:
                ser.write(outgoing_serial_byte_array)
                # time.sleep(0.5)
                serial_bytes_received = ser.readline()
                print(serial_bytes_received.decode('UTF-8'))
            else:
                print('OC voltage: ' +
                      str(int(curve_emulation_parameters.open_circuit_voltage)) +
                      ' uV, SC current: ' +
                      str(int(curve_emulation_parameters.short_circuit_current)) +
                      ' uA')
