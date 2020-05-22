#!/usr/bin/env python

import emgAcquireClient
import numpy as np


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