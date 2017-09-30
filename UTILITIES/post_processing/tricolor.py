#!/usr/bin/python

import sys
import os

import matplotlib.pyplot as plt
import matplotlib.tri as tri
import numpy as np

def read_tri_file(f):
  lines = f.readlines();
  
  vertices = []
  triangles = []
  labels = []
  data = []
  
  for line in lines:
    tokens = line.split()
    type = tokens[0]
    
    if type == "v":
      v1 = float(tokens[1])
      v2 = float(tokens[2])
      
      v  = (v1, v2)
    
      vertices.append(v)
    elif type == "t":
      v1 = int(tokens[1])
      v2 = int(tokens[2])
      v3 = int(tokens[3])
      
      t  = (v1, v2, v3)
      
      triangles.append(t)
    elif type == "l":
      v1 = int(tokens[1])
      v2 = int(tokens[2])
      v3 = int(tokens[3])
      lb = int(tokens[4])
      
      l  = (v1, v2, v3, lb)
      
      labels.append(l)
    elif type == "d":
      v  = int(tokens[1])
      d = float(tokens[2])
      
      data.append(d)
  return (vertices, triangles, labels, data)

def main():
  filetype = "png"

  if len(sys.argv) != 2:
    print "Usage: tricolor.py input"
    return

  filename = sys.argv[1]
  
  if not os.path.exists(filename):
    print "Error: file", filename, "not found"
    return
  
  f = open(filename, 'r')
  
  print "Input file:", filename
  
  (vertices, triangles, labels, data) = read_tri_file(f)
  
  f.close()
  
  xy = np.asarray(vertices)
  ts = np.asarray(triangles)

  x = xy[:,0]
  y = xy[:,1]
  
  d = np.asarray(data)
  
  plt.figure()
  plt.gca().set_aspect('equal')
  #plt.tripcolor(x, y, ts, d, edgecolors='k')
  plt.tripcolor(x, y, ts, d, shading='gouraud', edgecolors='k')
  
  r = 0.7333 # start_radius + step_size * steps * time_step
  circle = plt.Circle((0,0), r, color = 'black', fill = False)
  plt.gca().add_artist(circle)
  
  plt.savefig(filename + '.' + filetype, format=filetype)
  plt.show()
  
if __name__ == "__main__":
  main()