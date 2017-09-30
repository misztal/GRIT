#!/usr/bin/python

import sys
import os

import matplotlib.pyplot as plt
import matplotlib.tri as tri
import numpy as np

def read_float_file(f):
  lines = f.readlines();

  data = []
  
  for line in lines:
    tokens = line.split()
    x = float(tokens[0])
    y = float(tokens[1])
    data.append((x,y))
  return data

def main():
  filetype = "png"

  if len(sys.argv) != 2:
    print "Usage: speedup.py input"
    return

  filename = sys.argv[1]
  
  if not os.path.exists(filename):
    print "Error: file", filename, "not found"
    return
  
  f = open(filename, 'r')
  
  print "Input file:", filename
  
  data = read_float_file(f)
  
  f.close()
  
  xy = np.asarray(data)

  x = xy[:,0]
  y = xy[:,1]

  fig = plt.figure()
  
  ax = plt.subplot(111)
  ax.get_yaxis().set_major_locator(plt.MaxNLocator(integer=True))
  ax.get_xaxis().set_major_locator(plt.MaxNLocator(integer=True))
  
  plt.ylabel('Speedup factor')
  plt.xlabel('Number of processes')
  plt.plot(x, y, 'r.-')
  
  plt.savefig(filename + '.' + filetype, format=filetype)
  plt.show()
  
if __name__ == "__main__":
  main()