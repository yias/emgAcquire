#!/usr/bin/env python

import os
import sys
import pathlib
import numpy as np
import time

import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation



# directory of the emgAcquireClient python_module 
if sys.platform == 'linux':
    emgAquire_dir = str(pathlib.Path().absolute()) + "/../python_module"
else:
    emgAquire_dir = str(pathlib.Path().absolute()) + "\\..\\python_module"

# append the path including the directory of the python_module
sys.path.append(emgAquire_dir)

# import the module
import emgAcquireClient


# define number of channels to acquire
nb_ch = 6

global tm_point, max_points, all_Data, isfirst

tm_point = 0

max_points = 2500

title = 'EMG signals'

################################################################################

# fig = plt.figure()

fig, m_axes = plt.subplots(nb_ch,1)

fig.suptitle(title, fontsize=12)

key_chars = 'ch'
m_lines = []
idx = 0
for ax_i in m_axes:
    # ax_i.set_xlim(0, 4)
    ax_i.set_ylim([-500, 500])
    ax_i.set_xlim([0, max_points])
    ax_i.set_ylabel(key_chars + str(idx + 1) + "mV", rotation=0)
    ax_i.grid(True)
    if(idx == nb_ch-1):
        ax_i.set_xlabel('time')
    if(idx != nb_ch-1):
        ax_i.xaxis.set_major_formatter(plt.NullFormatter())
    
    tmp_line, = ax_i.plot([], [], lw=3)
    m_lines += [tmp_line]
    
    idx += 1

##################################################

# create an emgClient object for acquiring the data
emgClient = emgAcquireClient.emgAcquireClient(svrIP='128.179.163.218', nb_channels=nb_ch)

# create a numpy array to contain all the data
all_Data = np.array([], dtype=np.float32).reshape(6,0)

# initialize the node
init_test = emgClient.initialize()

if init_test<0:
    print("unable to initialize")
    exit()



def animate(i):
    global tm_point, max_points, all_Data, isfirst

    if isfirst:
        emgClient.start()
        isfirst = False

    if all_Data.shape[1] > max_points:
        all_Data = np.array([], dtype=np.float32).reshape(6,0)
    
    # acquire the signals from the buffer
    emg_data = emgClient.getSignals()

    # append the array with the new data
    all_Data = np.hstack((all_Data, emg_data))

    # print(all_Data.shape)
    
    
    cnt = 0
    for ln in m_lines:
        
        y = all_Data[cnt,:]
        # print(y.shape)
        x = np.linspace(0,len(y)-1, num=len(y) )
        ln.set_data(x, y)
        ln.set_linewidth(2)
        cnt += 1
    
    return m_lines

# start the acquisition
# emgClient.start()
isfirst = True
anim = FuncAnimation(plt.gcf(), animate, interval=1, blit=True) # init_func=init

plt.show()

emgClient.shutdown()


