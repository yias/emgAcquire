#!/usr/bin/env python

import os
import sys
import pathlib
import numpy as np


# directory of the emgAcquireClient python_module 
emgAquire_dir = str(pathlib.Path().absolute()) + "\\..\\python_module"
print(emgAquire_dir)

# append the path including the directory of the python_module
sys.path.append(emgAquire_dir)

# import the module
import emgAcquireClient



emgClient = emgAcquireClient.emgAcquireClient()

test = emgClient.initialize()

if test<0:
    print("unable to initialize")
    exit()

emgClient.start()

while True:

    try:
        emgClient.getSignals()
    
    except KeyboardInterrupt:
            break

emgClient.shutdown()