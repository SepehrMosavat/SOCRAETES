from enum import Enum


class PlotOrDiskCommit(Enum):
    PLOT_CURVE = 0
    PLOT_SURFACE = 1
    COMMIT_TRACE_TO_DISK = 2
