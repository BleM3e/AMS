#!/usr/bin/env python

import numpy as np
import matplotlib.pyplot as plt
import sys


if len(sys.argv) > 1:
  file = sys.argv[1]
else:
  print("Usage: python3 visualization.py file.csv")
  exit(1)

Y_UPPER = 1000

fig, ax = plt.subplots()
ax.set_ylabel("Access latency (cycles)")
ax.set_xlabel("Measurements")
ax.grid(True)

data = np.genfromtxt(file,delimiter=' ', dtype = int)
n = len(list(data))
x = np.arange(0, n)
y1 = [row if row < Y_UPPER else np.nan for row in data]

plt.plot(x,y1,'r+')
plt.show()

