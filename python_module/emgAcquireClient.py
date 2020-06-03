#!/usr/bin/env python

import ctypes
import numpy as np

lib = ctypes.cdll.LoadLibrary('..\\lib\\win32\\x64\\emgAcquireClient.dll')

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
        lib.client_getSignals.restype = ctypes.POINTER( ctypes.POINTER(ctypes.c_double) )

        self.obj = lib.new_Client(svrIP.encode(), svrPort, freq, nb_channels)

    def initialize(self):
        return lib.client_initialize(self.obj)
    
    def start(self):
        lib.client_start(self.obj)
    
    def stop(self):
        lib.client_stop(self.obj)
    
    def shutdown(self):
        lib.client_shutdown(self.obj)
    
    def getSignals(self):
        tt = lib.client_getSignals(self.obj)

