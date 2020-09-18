import h5py
import numpy as np
import queue
import threading
import os

from iv_curves_definitions import HarvestingCondition

harvesting_condition = HarvestingCondition('indoor', "5", 'sunny', 'germany', 'essen')


def generate_filename() -> str:
    for files in os.walk('captured_traces'):
        number_of_files_in_directory = len(files[2])
        highest_filename = files[2][number_of_files_in_directory-1]
        split_file_extension = highest_filename.split('.')
        highest_filename_without_extension = split_file_extension[0]
        parsed_filename = highest_filename_without_extension.split('_')
        if parsed_filename[0] == 'trace':
            new_filename = 'trace_' + str(int(parsed_filename[1]) + 1) + '.hdf5'
            # TODO Add more error handling for the files already present in the directory
        return new_filename


def write_iv_curves_to_disk(_iv_curves_queue: queue.Queue, _stop_thread_event: threading.Event):
    curve_counter = 0
    data_array_buffer = []

    new_filename = generate_filename()
    new_filename = 'captured_traces\\' + new_filename

    while True:
        if _stop_thread_event.isSet():
            print("Committing curve data to the hard disk...")
            with h5py.File(new_filename, 'a') as f:
                harvesting_condition_list = [(np.string_('Indoor/Outdoor'),
                                             np.string_('Light Intensity (out of 10)'),
                                             np.string_('Weather Condition'),
                                             np.string_('Country'),
                                             np.string_('City')),
                                             (np.string_(harvesting_condition.indoor_or_outdoor),
                                             np.string_(harvesting_condition.light_intensity),
                                             np.string_(harvesting_condition.weather_condition),
                                              np.string_(harvesting_condition.country),
                                            np.string_(harvesting_condition.city))]
                dataset = f.create_dataset('harvesting conditions', data=harvesting_condition_list)
            for arr in data_array_buffer:
                with h5py.File(new_filename, 'a') as f:
                    dataset = f.create_dataset('curve' + str(curve_counter), data=arr, dtype='f')
                curve_counter += 1
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







