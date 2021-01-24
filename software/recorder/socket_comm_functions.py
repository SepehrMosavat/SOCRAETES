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

sio = socketio.Client()


@sio.event
def connect():
    logger.info("Connected to Server with ID " + sio.sid)


@sio.event
def disconnect():
    logger.info("Disconnected from Server with ID " + sio.sid)


sio.connect('http://localhost:5000')
