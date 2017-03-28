import numpy as np
import matplotlib.pyplot as plt
from sharedmem import readmem
from scipy import signal

rate = 128

data1,n1,shape1,stride1 = readmem(42)
y1 = data1[-shape1[1]*2:-shape1[1]] #O2
y2 = data1[-shape1[1]*3:-shape1[1]*2] #O1
x = np.linspace(0, rate/2, len(y1))

plt.ion()

line1, = plt.plot(x, y1,'b')
line2, = plt.plot(x, y2, 'g')
ax = plt.gca()
ax.set_ylim([0,100])

while True:
  data1,n1,shape1,stride1 = readmem(42)
  y1 = data1[-shape1[1]*2:-shape1[1]] #O2
  y2 = data1[-shape1[1]*3:-shape1[1]*2] #O1
  line1.set_ydata(y1)
  line2.set_ydata(y2)
  plt.draw()
  plt.pause(0.000001)
