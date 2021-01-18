from enum import Enum


class EmulationParameters:
    def __init__(self, _open_circuit_voltage: int, _short_circuit_current: int):
        self.open_circuit_voltage = _open_circuit_voltage
        self.short_circuit_current = _short_circuit_current


class CurveEmulationMethod(Enum):
    EMULATE_TRACE_FROM_FILE = 0
    EMULATE_TRACE_FROM_ARRAY = 1


zero_output_emulation_parameters = EmulationParameters(0, 0)
