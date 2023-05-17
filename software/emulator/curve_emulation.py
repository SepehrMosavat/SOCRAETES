import logging
import queue
import time
from datetime import datetime
from threading import Event

import h5py
import numpy as np

from definitions import EmulationParameters, CurveEmulationMethod, zero_output_emulation_parameters

logger = logging.getLogger(__name__)
logger.setLevel(logging.DEBUG)
console_log_handler = logging.StreamHandler()
console_log_formatter = logging.Formatter('%(levelname)s - %(message)s')
console_log_handler.setFormatter(console_log_formatter)
logger.addHandler(console_log_handler)


def get_number_of_curves_in_trace(_trace_emulation_file) -> int:
    # len -2 because of the last curve which could not be full in sd_mode recording and because of harvesting infos 
    return len(_trace_emulation_file.keys()) - 2


def get_sleep_time_between_curves(_trace_emulation_file) -> float:
    number_of_curves_in_trace = get_number_of_curves_in_trace(_trace_emulation_file)
    harvesting_conditions_dataset = _trace_emulation_file.get('harvesting conditions')
    harvesting_conditions_dataset_as_array = np.array(harvesting_conditions_dataset)
    capture_start_time = datetime.strptime(harvesting_conditions_dataset_as_array[1, 1].decode('UTF-8'), '%H:%M:%S:%f')
    capture_stop_time = datetime.strptime(harvesting_conditions_dataset_as_array[1, 2].decode('UTF-8'), '%H:%M:%S:%f')
    capture_duration = capture_stop_time - capture_start_time
    capture_duration_in_seconds = capture_duration.total_seconds()
    sleep_time_between_curves = (capture_duration_in_seconds / number_of_curves_in_trace)
    return sleep_time_between_curves


def get_voltage_and_current_of_curve(_emulated_curve_dataset) -> EmulationParameters:
    curve_dataset_as_array = np.array(_emulated_curve_dataset)
    curve_emulation_parameters = EmulationParameters
    curve_emulation_parameters.open_circuit_voltage = curve_dataset_as_array[0, 0] * 1000000  # In uV
    curve_emulation_parameters.short_circuit_current = curve_dataset_as_array[1, curve_dataset_as_array.shape[1] - 1]
    return curve_emulation_parameters


def file_handling(_trace_emulation_file_name):
    try:
        trace_emulation_file = h5py.File(_trace_emulation_file_name, 'r')
    except OSError:
        logger.error("Could not open the specified file")
        return None
    # trace_emulation_file.close() # TODO Close the trace file after finishing with it
    return trace_emulation_file


def get_curve_parameters_for_emulation(_trace_emulation_file, _curve_number):
    curve_name = 'curve' + str(_curve_number)
    curve_dataset = _trace_emulation_file.get(curve_name)
    curve_emulation_parameters = get_voltage_and_current_of_curve(curve_dataset)
    return curve_emulation_parameters


def emulate_curve(_trace_emulation_source, _trace_emulation_queue: queue.Queue, _emulation_method: CurveEmulationMethod,
                  _stop_thread_event: Event):
    if _emulation_method is CurveEmulationMethod.EMULATE_TRACE_FROM_FILE:
        trace_emulation_file_name = _trace_emulation_source
        trace_emulation_file = file_handling(trace_emulation_file_name)
        if trace_emulation_file is None:
            _stop_thread_event.set()
            return
        sleep_time_between_curves = get_sleep_time_between_curves(trace_emulation_file)
        number_of_curves_in_trace = get_number_of_curves_in_trace(trace_emulation_file)
        logger.info('Dynamic curve emulation started for file name \'' + str(trace_emulation_file_name) +
              '\' with ' + str(number_of_curves_in_trace) + ' curves in the trace and with ' +
              str(sleep_time_between_curves) + ' seconds of delay between each curve')
        current_emulated_curve_number = 0
        curve_parameters_for_emulation = EmulationParameters
        while True:
            if _stop_thread_event.isSet():
                _trace_emulation_queue.put(zero_output_emulation_parameters)
                break
            while current_emulated_curve_number < number_of_curves_in_trace and not _stop_thread_event.isSet():
                curve_parameters_for_emulation = get_curve_parameters_for_emulation(trace_emulation_file,
                                                                                    current_emulated_curve_number)
                time.sleep(sleep_time_between_curves)
                _trace_emulation_queue.put(curve_parameters_for_emulation)
                current_emulated_curve_number = current_emulated_curve_number + 1
            current_emulated_curve_number = 0
    else:
        trace_emulation_array = _trace_emulation_source
        sleep_time_between_curves = trace_emulation_array[0, 0]
        number_of_curves_in_trace = len(trace_emulation_array) - 1
        current_emulated_curve_number = 0
        curve_parameters_for_emulation = EmulationParameters
        while True:
            if _stop_thread_event.isSet():
                _trace_emulation_queue.put(zero_output_emulation_parameters)
                break
            while current_emulated_curve_number < number_of_curves_in_trace:
                curve_parameters_for_emulation.open_circuit_voltage = trace_emulation_array[
                                                                          current_emulated_curve_number + 1, 0] * 1000000  # In uV
                curve_parameters_for_emulation.short_circuit_current = trace_emulation_array[
                    current_emulated_curve_number + 1, 1]
                _trace_emulation_queue.put(curve_parameters_for_emulation)
                time.sleep(sleep_time_between_curves)
                current_emulated_curve_number = current_emulated_curve_number + 1
            current_emulated_curve_number = 0


def emulate_intermittence(_timing_array: int, _trace_emulation_queue: queue.Queue, _stop_thread_event: Event):
    power_on_period = _timing_array[0]
    power_off_period = _timing_array[1]
    curve_parameters_for_emulation = EmulationParameters
    curve_parameters_for_emulation.short_circuit_current = 50000 # I_max_load = 50 mA
    curve_parameters_for_emulation.open_circuit_voltage = 3300000 # V_load = 3.3 V
    is_load_powered = False
    while True:
        if _stop_thread_event.isSet():
            _trace_emulation_queue.put(zero_output_emulation_parameters)
            break
        else:
            if is_load_powered:
                _trace_emulation_queue.put(curve_parameters_for_emulation)
                time.sleep(power_on_period)
                is_load_powered = False
                continue
            else: # Load is not powered
                _trace_emulation_queue.put(zero_output_emulation_parameters)
                time.sleep(power_off_period)
                is_load_powered = True
                continue

def write_curve_info_to_sd(_trace_emulation_source, _stop_thread_event: Event):
    trace_emulation_file_name = _trace_emulation_source
    trace_emulation_file = file_handling(trace_emulation_file_name)
    if trace_emulation_file is None:
        _stop_thread_event.set()
        return
    sleep_time_between_curves = get_sleep_time_between_curves(trace_emulation_file)
    number_of_curves_in_trace = get_number_of_curves_in_trace(trace_emulation_file)
    try:
        emulating_file = open( trace_emulation_file_name + ".txt", "x")
    except:
        emulating_file = open(trace_emulation_file_name + ".txt", "w")

    emulating_file.write("number of curves =" + str(number_of_curves_in_trace) + "\n")
    emulating_file.write("duration per curve =" + str(sleep_time_between_curves) + "\n")

    _voltage_values = []
    _current_values = []
    curve_parameters_for_emulation = EmulationParameters

    for _curve_number in range(1,number_of_curves_in_trace +1):
        curve_parameter_for_emulation = get_curve_parameters_for_emulation(trace_emulation_file, _curve_number)
        _voltage_values.append( (curve_parameter_for_emulation.open_circuit_voltage ) )
        _current_values.append( (curve_parameter_for_emulation.short_circuit_current))
    emulating_file.write("open circuit voltage curve points /V =")
    for line in _voltage_values:
        emulating_file.write(str(line) +";")
    emulating_file.write("\n")
    emulating_file.write("short circuit current curve points /uA =")
    for line in _current_values:
        emulating_file.write(str(line) +";")
    emulating_file.write("\n")
    emulating_file.close()
    trace_emulation_file.close()


    


