import logging
import os
import queue
import threading
from datetime import datetime
import time
import csv
import h5py
import numpy as np
import sys
from iv_curves_definitions import HarvestingCondition, IvCurve, CurvePoint

logger = logging.getLogger(__name__)
logger.setLevel(logging.DEBUG)
console_log_handler = logging.StreamHandler()
console_log_formatter = logging.Formatter('%(levelname)s - %(message)s')
console_log_handler.setFormatter(console_log_formatter)
logger.addHandler(console_log_handler)


def generate_filename(_file_name) -> str:

    if not os.path.exists('captured_traces'):
        os.mkdir('captured_traces')

    if _file_name == 'AUTO-GENERATE':
        for files in os.walk('captured_traces'):
            number_of_files_in_directory = len(files[2])
            if number_of_files_in_directory == 0:
                return 'trace_0.hdf5'
                # TODO Is this too hacky?
            list_of_file_names = files[2]
            highest_file_index = 0
            for file_name_without_extension in list_of_file_names:
                file_name_without_extension = file_name_without_extension.split('.')
                index_of_trace_file = file_name_without_extension[0].split('_')
                if int(index_of_trace_file[1]) > highest_file_index:
                    highest_file_index = int(index_of_trace_file[1])
            new_filename = 'trace_' + str(highest_file_index + 1) + '.hdf5'
            # TODO Add more error handling for the files already present in the directory
            return new_filename
    else:
        return _file_name


def write_iv_curves_to_disk(_iv_curves_queue: queue.Queue, _file_name, _harvesting_condition: HarvestingCondition,
                            _stop_thread_event: threading.Event, sd_mode = False):
    global start_time_string
    global start_date_string
    global end_time_string
    curve_counter = 0
    data_array_buffer = []

    new_filename = generate_filename(_file_name)
    new_filename = os.path.join('captured_traces', new_filename)
    if not sd_mode:
        start_time = datetime.now()
        start_time_string = str(start_time.hour) + ':' + str(start_time.minute) + ':' + str(start_time.second) + '.' +\
                            str(start_time.microsecond)
        start_date_string = str(start_time.day) + '.' + str(start_time.month) + '.' + str(start_time.year)

    while True:
        if _stop_thread_event.isSet():
            if not sd_mode:
                end_time = datetime.now()
                end_time_string = str(end_time.hour) + ':' + str(end_time.minute) + ':' + str(end_time.second) + '.' +\
                                  str(end_time.microsecond)

            logger.info("Committing curve data to the hard disk...")
            with h5py.File(new_filename, 'a') as f:
                harvesting_condition_list = [(np.string_('Date'),
                                              np.string_('Start Time (Local Timezone)'),
                                              np.string_('End Time (Local Timezone)'),
                                              np.string_('Indoor/Outdoor'),
                                              np.string_('Light Intensity (Lux)'),
                                              np.string_('Weather Condition'),
                                              np.string_('Country'),
                                              np.string_('City'),
                                              np.string_('Harvesting Source')),
                                             (np.string_(start_date_string),
                                              np.string_(start_time_string),
                                              np.string_(end_time_string),
                                              np.string_(_harvesting_condition.indoor_or_outdoor),
                                              np.string_(_harvesting_condition.light_intensity),
                                              np.string_(_harvesting_condition.weather_condition),
                                              np.string_(_harvesting_condition.country),
                                              np.string_(_harvesting_condition.city),
                                              np.string_(_harvesting_condition.source))]
                dataset = f.create_dataset('harvesting conditions', data=harvesting_condition_list)
            for arr in data_array_buffer:
                with h5py.File(new_filename, 'a') as f:
                    dataset = f.create_dataset('curve' + str(curve_counter), data=arr, dtype='f')
                curve_counter += 1
            if sd_mode:
                _stop_thread_event.clear()
            break

        if not _iv_curves_queue.empty():
            iv_curve = _iv_curves_queue.get()
            x_temp = []
            y_temp = []
            for c in iv_curve.curve_points_list:
                x_temp.append(c.x)
                y_temp.append(c.y)

            data_array = np.array([x_temp, y_temp], dtype=float)
            data_array_buffer.append(data_array)
        time.sleep(0.001)

def convert_csv_to_hdf5( captured_curves_queue: queue.Queue, _stop_thread_event: threading.Event, document ):

    is_iv_curve_being_captured = False
    curve_number_counter = 0
    for files in os.walk('recording_data'):
        list_of_file_names = files[2]
        if document in list_of_file_names:
            document = os.path.join('recording_data', document)
            with open(document, 'r') as csv_datei:
                reader = list(csv.reader(csv_datei, delimiter=';'))
                for i in range (1,len(reader)):
                    if _stop_thread_event.isSet():
                        sys.exit()
                    line= list(map(int, reader[i]))
                    point_sequence_number = line[0]
                    voltage = line[1]
                    current = line[2]

                    if point_sequence_number == 0 and is_iv_curve_being_captured is False:
                        # Start capturing IV curve
                        is_iv_curve_being_captured = True
                        captured_curve = IvCurve(curve_number_counter)

                    if is_iv_curve_being_captured is True:
                        captured_curve_point = CurvePoint(point_sequence_number, voltage / 1000000, current)
                        captured_curve.add_point_to_curve(captured_curve_point)

                        if len(captured_curve.curve_points_list) == IvCurve.number_of_points_in_curve:
                            is_iv_curve_being_captured = False
                            captured_curves_queue.put(captured_curve)
                            curve_number_counter += 1
                            logger.info("Curve read in: " + str(captured_curve.curve_number))
                while not captured_curves_queue.empty():
                    time.sleep(0.001)

                _stop_thread_event.set()
                continue

        time.sleep(0.001)




def read_harvesting_info_sd(document):

    global start_time_string
    global start_date_string
    global end_time_string
    for files in os.walk('recording_data'):
        list_of_file_names = files[2]
        if document in files[2]:
            document = os.path.join('recording_data', document)
            
            with open(document, 'r') as csv_datei:
                reader = list(csv.reader(csv_datei, delimiter=';'))
                start_date_string = reader[0][0]
                start_time_string = reader[0][1]
                end_time_string =reader [0][2]
                temp = HarvestingCondition( reader[0][3], reader[0][4], reader[0][5], reader[0][6], reader[0][7], reader[0][8])
                return temp

def get_file():
    for files in os.walk('recording_data'):
        list_of_file_names = files[2]
        return list_of_file_names


