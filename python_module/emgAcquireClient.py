#!/usr/bin/env python

import sys
import ctypes
import numpy as np

if sys.platform == 'linux':
    lib = ctypes.cdll.LoadLibrary('../lib/linux/shared/x64/libemgAcquireClient.so')
else:
    lib = ctypes.cdll.LoadLibrary('..\\lib\\win32\\dll\\x64\\emgAcquireClient.dll')

class emgAcquireClient(object):
    def __init__(self, svrIP="localhost", svrPort=10352, freq=20, nb_channels=16):
        lib.new_Client.argtypes = [ctypes.c_char_p, ctypes.c_int, ctypes.c_float, ctypes.c_int]
        lib.new_Client.restype = ctypes.c_void_p

        lib.client_initialize.argtypes = [ctypes.c_void_p]
        lib.client_initialize.restype = ctypes.c_int

        lib.client_start.argtypes = [ctypes.c_void_p]
        lib.client_start.restype = ctypes.c_void_p

        lib.client_stop.argtypes = [ctypes.c_void_p]
        lib.client_stop.restype = ctypes.c_void_p

        lib.client_shutdown.argtypes = [ctypes.c_void_p]
        lib.client_shutdown.restype = ctypes.c_void_p

        lib.client_getSignals.argtypes = [ctypes.c_void_p]
        lib.client_getSignals.restype = ctypes.POINTER(ctypes.c_double)

        self.nb_ch = nb_channels
        self.frequency = freq
        self.obj = lib.new_Client(svrIP.encode('utf-8'), svrPort, freq, nb_channels)
        self.SR = 1000
        self.nb_samples = self.nb_ch * self.SR / self.frequency
        self.nb_samples = int(self.nb_samples)
        self.data = np.empty((self.nb_samples,), dtype=np.float32)

    def initialize(self):
        return lib.client_initialize(self.obj)
    
    def start(self):
        lib.client_start(self.obj)
    
    def stop(self):
        lib.client_stop(self.obj)
    
    def shutdown(self):
        lib.client_shutdown(self.obj)
    
    def getSignals(self):
        returned_data = np.empty((self.nb_samples,), dtype=np.float64)

        tt = lib.client_getSignals(self.obj)

        ctypes.memmove(returned_data.ctypes.data, tt, (self.nb_samples) * np.dtype(np.float64).itemsize)

        returned_data = np.reshape (returned_data, (self.nb_ch, int(self.SR / self.frequency)))

        return returned_data


