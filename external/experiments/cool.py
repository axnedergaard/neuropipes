import numpy as np
import matplotlib.pyplot as plt
from sharedmem import readmem
from scipy import signal

data1,n1,shape1,stride1 = readmem(42)
y1 = data1[:shape1[1]] 
y2 = data1[shape1[1]:shape1[1]*2]
y3 = data1[shape1[1]*2:shape1[1]*3]
x = np.linspace(0,len(y1),len(y1))

plt.ion()

line1, = plt.plot(x, y1,'r')
line2, = plt.plot(x, y2,'g')
line3, = plt.plot(x, y3,'b')
ax = plt.gca()

while True:
  data1,n1,shape1,stride1 = readmem(42)
  y1 = data1[:shape1[1]] 
  y2 = data1[shape1[1]:shape1[1]*2]
  y3 = data1[shape1[1]*2:shape1[1]*3]
  y4 = data1[shape1[1]*3:shape1[1]*4]
  line1.set_ydata(y1)
  line2.set_ydata(y2)
  line3.set_ydata(y3)
  plt.draw()
  plt.pause(0.000001)
