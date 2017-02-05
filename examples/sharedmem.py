import sysv_ipc
import binascii
import re

#extract double from shared memory reading
def extract_double(s):
  ss = str(s)
  begin_pos = 0
  end_pos = 0
  for i,si in enumerate(ss):
    if si == "'":
      begin_pos = i+1
      break
  for i,si in enumerate(ss):
    if si == "\\":
      end_pos = i
      break
  st = ss[begin_pos:end_pos]
  return float(st)

mem = sysv_ipc.SharedMemory(6667)
dlen = 32

#print(extract_double(mem.read(dlen,0)))

offset = 0

#read n, shape, stride
n = int(extract_double(mem.read(dlen, offset)))
offset += dlen
shape = []
stride = []
for i in range(n):
  shape += [int(extract_double(mem.read(dlen, offset)))]
  offset += dlen
for i in range(n):
  stride += [int(extract_double(mem.read(dlen, offset)))]
  offset += dlen

data = []
#read data
reading = 0
while offset < mem.size:
  data += [float(extract_double(mem.read(dlen,offset)))]
  offset += dlen 

#print
print_i = 0
for d in data:
  print(d, end=' ')
  print_i += 1
  if print_i % shape[1] == 0:
    print()
