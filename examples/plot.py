import numpy as np
import matplotlib.pyplot as plt
from sharedmem import readmem
from scipy import signal

rate = 128
nyquist = rate/2
lf = 8/nyquist
hf = 12/nyquist
b,a = signal.butter(4, [lf,hf], 'bandpass')

data1,n1,shape1,stride1 = readmem(42)
data2,n2,shape2,stride2 = readmem(43)
x = []
for i in range(shape1[1]):
  x += [i]
y1 = data1[:shape1[1]]
y2 = data2[:shape2[1]]

plt.ion()

line1, = plt.plot(y1,'b')
line2, = plt.plot(y2,'ro')
ax = plt.gca()
#ax.set_ylim([-2500,2500])

while True:
  data1,n1,shape1,stride1 = readmem(42)
  data2,n2,shape2,stride2 = readmem(43)
  data1 = signal.lfilter(b,a,data1[:shape1[1]]) #python filter
  y1 = data1[:shape1[1]]
  y2 = data2[:shape2[1]]
  line1.set_ydata(y1)
  line2.set_ydata(y2)
  plt.draw()
  plt.pause(1)
