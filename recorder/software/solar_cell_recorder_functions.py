import serial
import queue
import time
import signal

from iv_curves_definitions import IvCurve, CurvePoint


class ResourceCleanup:
    kill_now = False

    def __init__(self):
        signal.signal(signal.SIGABRT, self.perform_cleanup)
        signal.signal(signal.SIGTERM, self.perform_cleanup)

    def perform_cleanup(self, signum, frame):
        self.kill_now = True
        print("Signal received" + str(signum))


# Function for reading the COM port values
def read_byte_array_from_serial_port(raw_serial_data_queue: queue.Queue):
    ser = serial.Serial(
        port='COM15',
        baudrate=115200,
        parity=serial.PARITY_NONE,
        stopbits=serial.STOPBITS_ONE,
        bytesize=serial.EIGHTBITS
    )

    ser.close()
    ser.open()
    if ser.isOpen():
        print("Serial port open")
    else:
        print("Serial port could not be opened")

    while True:
        serial_bytes_received = ser.read(11)
        serial_bytes_received_as_bytearray = bytearray(serial_bytes_received)

        if serial_bytes_received_as_bytearray[0] == 170 or serial_bytes_received_as_bytearray[10] == 85:  # 0xAA
            raw_serial_data_queue.put(serial_bytes_received_as_bytearray)


# Function for processing received byte arrays and extract the IV curves from them
def process_received_serial_data(raw_serial_data_queue: queue.Queue, captured_curves_queue: queue.Queue):
    is_iv_curve_being_captured = False
    curve_number_counter = 0
    while True:
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
                #print("Capturing")

            if is_iv_curve_being_captured is True:
                if point_sequence_number == 128:
                    # Stop capturing IV curve
                    is_iv_curve_being_captured = False
                    captured_curves_queue.put(captured_curve)
                    curve_number_counter += 1
                    print("Curve captured: " + str(captured_curve.curve_number))

                captured_curve_point = CurvePoint(point_sequence_number, voltage / 1000000, current)
                captured_curve.add_point_to_curve(captured_curve_point)

            time.sleep(0.001)
