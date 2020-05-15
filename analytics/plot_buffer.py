import os
import sys
import pathlib

import numpy as np
import pandas as pd

import matplotlib.pyplot as plt

if sys.platform == "linux":
    data_dir = str(pathlib.Path().absolute()) + "/../logs/"
else:
    data_dir = str(pathlib.Path().absolute()) + "\\..\\logs\\"

fname = "emgAcquireClient_logs.csv"

buffer_data = pd.read_csv(data_dir+fname, sep=';')

print(buffer_data)

time = buffer_data.get('time').to_numpy()

buffer1 = buffer_data.get('buffer 1').to_numpy()

buffer_digital = buffer_data.get('buffer digital').to_numpy()

fig, ax = plt.subplots()

plt.plot(time/1000, buffer1, 'b', label='channel 1')
plt.plot(time/1000, buffer_digital, 'r', label='digital channel')

if 'buffer 2' in buffer_data:
    buffer2 = buffer_data.get('buffer 2').to_numpy()
    plt.plot(time/1000, buffer_digital, 'g', label='channel 8')

ax.set_ylim([0, 500])
ax.set_title('Buffer')
ax.set_ylabel('samples')
ax.set_xlabel('time (s)')
ax.grid()
ax.legend()

plt.savefig('buffer_evolution.png', bbox_inches='tight', dpi=300)

plt.show()
