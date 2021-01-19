import datetime
import queue
import threading
import time

import matplotlib.pyplot as plt

plt.rcParams['toolbar'] = 'None'


def plot_iv_curve(iv_curves_queue: queue.Queue, _stop_thread_event: threading.Event):
    while True:
        if not iv_curves_queue.empty():
            curve = iv_curves_queue.get()

            v_value_list = []
            i_value_list = []
            for i in curve.curve_points_list:
                v_value_list.append(i.x)
                i_value_list.append(i.y)

            plot_x_max = max(v_value_list) + 0.5
            plot_y_max = max(i_value_list) + 500

            plt.axis([0, plot_x_max, 0, plot_y_max])
            plt.xlabel("Solar Cell Voltage (V)")
            plt.ylabel("Solar Cell Current (uA)")
            plt.title("Solar Cell IV Characteristics")
            #plt.ion()
            #plt.show()

            plt.plot(v_value_list, i_value_list)
            plt.draw()
            plt.pause(0.001)
            plt.clf()

        time.sleep(0.01)
        if _stop_thread_event.isSet():
            break


def plot_iv_surface(iv_curves_queue: queue.Queue, _stop_thread_event: threading.Event):
    z_axis = 0
    start_time = datetime.datetime.now()
    fig = plt.figure()
    ax = plt.gca(projection="3d")

    # plt.ion()

    ax.set_xlabel('Solar Cell Voltage (V)')
    ax.set_ylabel('Solar Cell Current (uA)')
    ax.set_zlabel('Time (s)')

    ax.set_xlim3d(0, 4)
    ax.set_ylim3d(0, 1000)
    ax.set_zlim3d(0, 100)
    ax.view_init(130, 270)
    # plt.show()

    while True:
        if _stop_thread_event.isSet():
            break
        if not iv_curves_queue.empty():
            curve = iv_curves_queue.get()

            v_value_list = []
            i_value_list = []
            z_value_list = []
            for i in curve.curve_points_list:
                v_value_list.append(i.x)
                i_value_list.append(i.y)
                z_value_list.append(z_axis)

            current_time = datetime.datetime.now()
            time_difference = current_time - start_time
            z_axis = time_difference.microseconds/1000000 + time_difference.seconds

            ax.plot3D(v_value_list, i_value_list, z_axis)

            #plt.show()
            # plt.draw()
            plt.pause(0.0001)
            #plt.clf()
            z_axis += 1

        time.sleep(0.001)

