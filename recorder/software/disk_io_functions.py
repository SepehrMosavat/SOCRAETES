import h5py
import numpy as np
import queue
import threading

from iv_curves_definitions import HarvestingCondition

harvesting_condition = HarvestingCondition('indoor', "8", 'sunny', 'germany', 'essen')


def write_iv_curves_to_disk(_iv_curves_queue: queue.Queue, _stop_thread_event: threading.Event):
    curve_counter = 0
    data_array_buffer = []

    while True:
        if _stop_thread_event.isSet():
            print("Committing curve data to the hard disk...")
            with h5py.File('captured_traces\\output.hdf5', 'a') as f:
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
                with h5py.File('captured_traces\\output.hdf5', 'a') as f:
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







