#!/usr/bin/env python3
# -*- coding: utf-8 -*-

__author__ = "Vadym Stupakov"
__email__ = "vadim.stupakov@gmail.com"

import logging
from concurrent.futures import ThreadPoolExecutor, as_completed
from functools import cached_property
from pprint import pformat

import serial
from serial.tools import list_ports

logger = logging.getLogger(__name__)


class FocuserController:
    _BAUDRATES = (300, 600, 1200, 2400, 4800, 9600, 14400, 19200, 28800, 38400, 57600, 115200)

    def __init__(self):
        self._found = False

    @cached_property
    def ports(self):
        ports = list_ports.comports()
        logger.info(f"PORTS: {[p.description for p in ports]}")
        return ports[::-1]

    def _find_focuser_port_worker(self, port, baudrates):
        logger.debug(f"Trying port: {port.device}")
        handshake_req = "who are you?"
        handshake_resp = "focuser"

        for baudrate in baudrates:
            if self._found:
                return

            # Avoid arduino board from resetting on Serial connection
            ser = serial.Serial()
            ser.baudrate = baudrate
            ser.port = port.device
            ser.timeout = 2
            ser.write_timeout = 2
            ser.setDTR(False)
            ser.open()

            ser.write(handshake_req.encode("utf8"))
            res = ser.readline().decode().strip()

            if res == handshake_resp:
                cfg = ser.get_settings()
                cfg = {port.device: cfg}
                logger.info(f"Found port: {pformat(cfg)}")
                self._found = True
                return ser
            else:
                ser.close()

    def find_focuser_port(self):
        with ThreadPoolExecutor() as e:
            futures = []
            for port in self.ports:
                f = e.submit(self._find_focuser_port_worker, port, self._BAUDRATES)
                futures.append(f)

            for f in as_completed(futures):
                res = f.result()
                if res is not None:
                    e.shutdown(wait=False, cancel_futures=True)
                    return res


if __name__ == '__main__':
    logging.basicConfig(level=logging.DEBUG)
    f = FocuserController()
    f.find_focuser_port()
