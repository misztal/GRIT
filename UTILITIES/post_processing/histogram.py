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
    d = float(line)
    data.append(d)
  return data

def main():
  filetype = "png"

  if len(sys.argv) != 3:
    print "Usage: histogram.py input xlabel"
    return

  filename = sys.argv[1]
  
  xlabel = sys.argv[2]
  
  if not os.path.exists(filename):
    print "Error: file", filename, "not found"
    return
  
  f = open(filename, 'r')
  
  print "Input file:", filename
  
  data = read_float_file(f)
  
  f.close()
  
  x = np.asarray(data)

  fig = plt.figure()
  
  ax = plt.subplot(111)
  #ax.get_yaxis().set_major_locator(plt.MaxNLocator(integer=True))
  #ax.get_xaxis().set_major_locator(plt.MaxNLocator(integer=True))
  
  plt.ylabel('Frequency')
  plt.xlabel(xlabel)

  n, bins, patches = plt.hist(x, 10, normed=False, facecolor='#ffaaaa', alpha=1.0, edgecolor='#d40000')
  
  plt.savefig(filename + '.' + filetype, format=filetype)
  plt.show()
  
if __name__ == "__main__":
  main()