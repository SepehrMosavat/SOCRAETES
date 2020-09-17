import time
import threading
import queue

from solar_cell_recorder_functions import process_received_serial_data, read_byte_array_from_serial_port
from iv_curve_visualization_functions import plot_iv_curve, plot_iv_surface
from disk_io_functions import write_iv_curves_to_disk
from solar_cell_recorder_functions import ResourceCleanup

visualization_method = 'Curve'
is_plotting_iv_data = True
is_writing_data_to_disk = False
capture_duration = 10

raw_serial_data_queue = queue.Queue()
captured_curves_queue = queue.Queue()
stop_thread_event = threading.Event()

read_byte_thread = threading.Thread(target=read_byte_array_from_serial_port, args=(raw_serial_data_queue,))
process_serial_data_thread = threading.Thread(target=process_received_serial_data, args=(raw_serial_data_queue,
                                                                                         captured_curves_queue,))
plot_iv_curve_thread = threading.Thread(target=plot_iv_curve, args=(captured_curves_queue,))
plot_iv_surface_thread = threading.Thread(target=plot_iv_surface, args=(captured_curves_queue,))
write_iv_curves_to_disk_thread = threading.Thread(target=write_iv_curves_to_disk, args=(captured_curves_queue,
                                                                                        stop_thread_event,))

process_serial_data_thread.start()
read_byte_thread.start()

if is_writing_data_to_disk is True:
    write_iv_curves_to_disk_thread.start()

if is_plotting_iv_data is True:
    if visualization_method == 'Curve':
        plot_iv_curve_thread.start()
    elif visualization_method == 'Surface':
        plot_iv_surface_thread.start()


if __name__ == '__main__':
    signal_handler = ResourceCleanup()
    counter = 0
    while True:
        time.sleep(1)
        counter += 1
        if counter == capture_duration:
            stop_thread_event.set()
            time.sleep(5)
            break;