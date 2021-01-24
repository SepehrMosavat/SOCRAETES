import socketio
import coloredlogs
import logging

""" Initialize Logger

    Available Logging Levels:
    - debug (higher level needed for display, see 'level' attribute in coloredlogs.install)
    - info
    - warning
    - error
    - critical
"""
logger = logging.getLogger(__name__)

""" Set Logger config such as format or color """
coloredlogs.install(level='INFO', logger=logger,
                    fmt='%(asctime)s %(levelname)s \033[32mCLIENT:\x1b[0m %(message)s', datefmt='%H:%M:%S')

""" sio = socketio.Client(logger=True, engineio_logger=True) """
sio = socketio.Client()


def start_socket_transmission() -> None:
    sio.connect('http://localhost:5000')


def send_curve(curve) -> None:
    sio.emit('send_curve', curve)
    logger.info("Emitted Curve to Server with ID " + sio.sid)


@sio.event
def connect() -> None:
    logger.info("Connected to Server with ID " + sio.sid)


@sio.event
def disconnect() -> None:
    logger.info("Disconnected from Server with ID " + sio.sid)
