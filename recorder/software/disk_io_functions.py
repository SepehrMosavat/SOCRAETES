import h5py
import numpy as np
import queue
import threading


def write_iv_curves_to_disk(_iv_curves_queue: queue.Queue, _stop_thread_event: threading.Event):
    curve_counter = 0
    data_array_buffer = []

    while True:
        if _stop_thread_event.isSet():
            print("Committing curve data to the hard disk...")
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







