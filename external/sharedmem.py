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

def readmem(key):
  mem = sysv_ipc.SharedMemory(key)
  dlen = 32

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

  return data,n,shape,stride
