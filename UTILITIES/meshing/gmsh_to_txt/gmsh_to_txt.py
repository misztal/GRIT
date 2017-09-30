# -*- coding: utf-8 -*-
"""
Created on Wed Jan  6 21:08:29 2016

@author: tobias

Script to convert a 2D gmsh generated mesh files .msh to .txt to be read
by the MOM2D load_from_txt.

Usage: python gmsh_to_txt.py <gmsh file>
"""

from gmsh import Mesh
import sys
from os import path
import matplotlib.pyplot as plt
import numpy as np


def vert_to_string(*args):
    """Converts list of vertices to string"""
    return "v %10.8f %10.8f\n" % tuple(args)


def elem_to_string(*args):
    """Converts list of elements to string"""
    return "t %d %d %d\n" % tuple(args)


def write_txt(outfile, mesh):
    """Writes the converted txt file"""
    with open(outfile, 'w') as file:
        for vert in mesh.Verts:
            # Write vertice with z-coordinate removed by indexing (2D)
            file.write("v {0} {1}\n".format(*vert[:2]))

        mesh.Elmts[2][1][:] += 1
        for elem in mesh.Elmts[2][1]:
            file.write("t {0} {1} {2}\n".format(*elem))

        label = 0
        old_key = mesh.Elmts[2][0][0]
        for key, elem in zip(*mesh.Elmts[2]):
            if key != old_key:
                label += 1
                old_key = key
            file.write("l {0} {1} {2} ".format(*elem) + str(label) + "\n")


def ccw_order(mesh):
    """
    Converts to CCW-ordered faces.

    Let v0, v1, v2 be the coordinates of idx0, idx1, idx2 then the
    requirement for CCW order is

        (0,0,1) dot ((v2-v1)  X (v0-v1))  > 0

    If test fails then swap for instance idx1 with idx2 and then you got
    CCW order.
    """
    for i in range(mesh.nel):
        v0, v1, v2 = mesh.Verts[mesh.Elmts[2][1][i],:]
        cond = np.array([0,0,1]).dot(np.cross((v2-v1),(v0-v1)))
        if cond < 0:
            mesh.Elmts[2][1][i][1], mesh.Elmts[2][1][i][2] = \
                mesh.Elmts[2][1][i][2], mesh.Elmts[2][1][i][1]
            print "To ensure CCW ordering, indices were swapped for triangle", i

    return mesh


if __name__ == '__main__':
    try:
        infile = sys.argv[1]
    except IndexError as err:
        print "\nUsage: python {0} <gmsh file>\n".format(sys.argv[0])
        raise IndexError("No filename given.")

    # Load mesh from file as Mesh object from gmsh.py
    mesh = Mesh()
    mesh.read_msh(infile)
    #mesh = ccw_order(mesh)

    # Name of outfile is same as mesh file with different extension (.txt)
    (head, tail) = path.split(infile)
    (root, ext) = path.splitext(tail)
    outfile = path.join(head, root + '.txt')

    write_txt(outfile, mesh)

    # plt.triplot(mesh.Verts[:,0], mesh.Verts[:,1], mesh.Elmts[2][1])
