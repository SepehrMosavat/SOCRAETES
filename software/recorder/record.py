import queue
import signal
import sys
import threading
import time

import fire

from definitions import PlotOrDiskCommit
from disk_io_functions import write_iv_curves_to_disk
from iv_curve_visualization_functions import plot_iv_curve, plot_iv_surface
from iv_curves_definitions import HarvestingCondition
from solar_cell_recorder_functions import process_received_serial_data, read_byte_array_from_serial_port

serial_port = None
data_handling_mode = None
file_name_for_trace_saving = None
capture_duration = None
harvesting_conditions = None

raw_serial_data_queue = queue.Queue()
captured_curves_queue = queue.Queue()
stop_thread_event = threading.Event()


def timer_function(_time_duration):
    time.sleep(_time_duration)
    stop_thread_event.set()


def interrupt_signal_handler(_signal, _frame):
    stop_thread_event.set()


def cli(port, mode='plot-curve', file='AUTO-GENERATE', duration=30, environment='indoor', lux=50, weather='sunny',
        country='N/A', city='N/A'):
    global serial_port
    global data_handling_mode
    global file_name_for_trace_saving
    global capture_duration
    global harvesting_conditions
    serial_port = port
    if mode == 'plot-curve':
        data_handling_mode = PlotOrDiskCommit.PLOT_CURVE
    elif mode == 'plot-surface':
        data_handling_mode = PlotOrDiskCommit.PLOT_SURFACE
    elif mode == 'commit-to-file':
        data_handling_mode = PlotOrDiskCommit.COMMIT_TRACE_TO_DISK
    file_name_for_trace_saving = file
    capture_duration = duration
    harvesting_conditions = HarvestingCondition(environment, str(lux), weather, country, city)


if __name__ == '__main__':
    signal.signal(signal.SIGINT, interrupt_signal_handler)
    fire.Fire(cli)
    timer_thread = threading.Thread(target=timer_function, args=(capture_duration,))
    read_byte_thread = threading.Thread(target=read_byte_array_from_serial_port, args=(raw_serial_data_queue,
                                                                                       serial_port, stop_thread_event,))
    process_serial_data_thread = threading.Thread(target=process_received_serial_data, args=(raw_serial_data_queue,
                                                                                             captured_curves_queue,
                                                                                             stop_thread_event,))

    timer_thread.daemon = True
    read_byte_thread.daemon = True
    process_serial_data_thread.daemon = True

    process_serial_data_thread.start()
    read_byte_thread.start()
    if data_handling_mode == PlotOrDiskCommit.COMMIT_TRACE_TO_DISK:
        timer_thread.start()
        write_iv_curves_to_disk(captured_curves_queue, file_name_for_trace_saving, harvesting_conditions,
                                stop_thread_event)
    elif data_handling_mode == PlotOrDiskCommit.PLOT_CURVE:
        plot_iv_curve(captured_curves_queue, stop_thread_event)
    elif data_handling_mode == PlotOrDiskCommit.PLOT_SURFACE:
        plot_iv_surface(captured_curves_queue, stop_thread_event)
    while True:
        if stop_thread_event.isSet():
            print('Recording finished. Exiting...')
            time.sleep(1)
            sys.exit()