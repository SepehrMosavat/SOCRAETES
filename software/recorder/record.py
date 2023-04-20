import logging
import queue
import signal
import sys
import threading
import time

import fire

from definitions import PlotOrDiskCommit
from disk_io_functions import write_iv_curves_to_disk, convert_csv_to_hdf5, read_harvesting_info_sd,get_file
from iv_curve_visualization_functions import plot_iv_curve, plot_iv_surface
from iv_curves_definitions import HarvestingCondition
from solar_cell_recorder_functions import process_received_serial_data, read_byte_array_from_serial_port

logger = logging.getLogger(__name__)
logger.setLevel(logging.DEBUG)
console_log_handler = logging.StreamHandler()
console_log_formatter = logging.Formatter('%(levelname)s - %(message)s')
console_log_handler.setFormatter(console_log_formatter)
logger.addHandler(console_log_handler)


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
        country='N/A', city='N/A', read_from_sd = False, doc_name= 'N/A'):
    """
    :param port: The serial port used for communication with the hardware.
    :param mode: The data handling mode will determine the behavior of the application:\n
    plot-curve: Plots 2D I-V curves of the connected harvester over.\n
    plot-surface: Plots 3D I-V curves of the connected harvester over time.\n
    commit-to-file: Saves the I-V curves of the connected harvester to an HDF5 file.\n
    Default value: plot-curve
    :param file: The file name of the HDF5 file in commit-to-file mode. If not specified, the file name will be
    auto-generated.
    :param duration: The duration of capturing the trace in commit-to-file mode.\n
    Default value: 30 seconds
    :param environment: The environment in which energy harvesting is carried out. Only used in commit-to-file mode.\n
    Default value: indoor
    :param lux: The light intensity (in Lux) of the environment in which energy harvesting is carried out.
    Only used in commit-to-file mode.\n
    Default value: 50 Lux
    :param weather: The weather condition during energy harvesting. Only used in commit-to-file mode.\n
    Default value: sunny
    :param country: The country in which energy harvesting is carried out. Only used in commit-to-file mode.\n
    Default value: N\A
    :param city: The city in which energy harvesting is carried out. Only used in commit-to-file mode.\n
    Default value: N\A
    :param read_from_sd: reads the data from a csv-file. Set to "one-file" and set flag doc_no, if you want to convert one specific document.\n
    Set flag to "all" if you want to convert all csv-files to hdf-files.\n
    Default value: False
    :param doc_name: if flag read_from_sd is set to "one-file" pls set this flag to the file name you want to convert (e.g measurement3.csv)\n
    :return:
    """
    global serial_port
    global data_handling_mode
    global file_name_for_trace_saving
    global capture_duration
    global harvesting_conditions
    global sd_mode
    global csv_doc
    serial_port = port
    if mode == 'plot-curve':
        data_handling_mode = PlotOrDiskCommit.PLOT_CURVE
    elif mode == 'plot-surface':
        data_handling_mode = PlotOrDiskCommit.PLOT_SURFACE
    elif mode == 'commit-to-file':
        data_handling_mode = PlotOrDiskCommit.COMMIT_TRACE_TO_DISK
    file_name_for_trace_saving = file
    sd_mode = read_from_sd
    csv_doc = doc_name
    if not sd_mode:
        harvesting_conditions = HarvestingCondition(environment, str(lux), weather, country, city)
        capture_duration = duration
    elif sd_mode =="one-file":
        harvesting_conditions = read_harvesting_info_sd(csv_doc)
       
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
    


        
    if (sd_mode == 'one-file'):
        harvesting_conditions = read_harvesting_info_sd(csv_doc)
        read_sd_thread = threading.Thread(target=convert_csv_to_hdf5, args=(captured_curves_queue,stop_thread_event,csv_doc,))
        read_sd_thread.daemon = True
        read_sd_thread.start()

    elif(sd_mode == 'all'):
        while True:
            file_list = get_file()
            for files in file_list:
                    harvesting_conditions = read_harvesting_info_sd(files)
                    read_sd_thread = threading.Thread(target=convert_csv_to_hdf5, args=(captured_curves_queue,stop_thread_event,files,))
                    read_sd_thread.daemon = True
                    read_sd_thread.start()
                    write_iv_curves_to_disk(captured_curves_queue, file_name_for_trace_saving, harvesting_conditions,
                                    stop_thread_event, True)
            stop_thread_event.set()
            logger.info('Recording finished. Exiting...')
            time.sleep(1)
            sys.exit()
            
    else:
        process_serial_data_thread.start()
        read_byte_thread.start()
        timer_thread.start()
    
    if data_handling_mode == PlotOrDiskCommit.COMMIT_TRACE_TO_DISK:
                if sd_mode == 'one-file':
                    write_iv_curves_to_disk(captured_curves_queue, file_name_for_trace_saving, harvesting_conditions,
                                    stop_thread_event, True)
                    stop_thread_event.set()
                elif not(sd_mode):
                    write_iv_curves_to_disk(captured_curves_queue, file_name_for_trace_saving, harvesting_conditions,
                                    stop_thread_event)
    elif data_handling_mode == PlotOrDiskCommit.PLOT_CURVE:
        plot_iv_curve(captured_curves_queue, stop_thread_event)
    elif data_handling_mode == PlotOrDiskCommit.PLOT_SURFACE:
        plot_iv_surface(captured_curves_queue, stop_thread_event)
    while True:
        if stop_thread_event.isSet():
            logger.info('Recording finished. Exiting...')
            time.sleep(1)
            sys.exit()
