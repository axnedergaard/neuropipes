import numpy as np
import matplotlib.pyplot as plt
from sharedmem import readmem
from scipy import signal

rate = 128

data1,n1,shape1,stride1 = readmem(42)
data2,n2,shape2,stride2 = readmem(43)
y1 = data2[:shape2[1]] #tmp
y2 = data2[:shape2[1]]
x = np.linspace(0, rate/2, len(y2))

plt.ion()

line1, = plt.plot(x, y1,'ro')
line2, = plt.plot(x, y2,'b')
ax = plt.gca()

while True:
  data1,n1,shape1,stride1 = readmem(42)
  data2,n2,shape2,stride2 = readmem(43)
  data1 = data1[:shape1[1]] #only do for first channel...
  data1 = np.abs(np.fft.fft(data1))
  y1 = data1[:shape2[1]]
  y2 = data2[:shape2[1]]
  line1.set_ydata(y1)
  line2.set_ydata(y2)
  plt.draw()
  plt.pause(0.000001)
