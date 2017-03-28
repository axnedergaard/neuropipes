import numpy as np
import matplotlib.pyplot as plt
from sharedmem import readmem
from scipy import signal

data1,n1,shape1,stride1 = readmem(42)
y1 = data1[:shape1[1]] #tmp
x = np.linspace(0,len(y1),len(y1))

plt.ion()

line1, = plt.plot(x, y1,'b')
ax = plt.gca()

while True:
  data1,n1,shape1,stride1 = readmem(42)
  y1 = data1[:shape1[1]]
  line1.set_ydata(y1)
  plt.draw()
  plt.pause(0.000001)
