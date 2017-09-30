#ifndef GLUE_H
#define GLUE_H


/**
 
 The GLUE library was created with the purpose of making it easy for application
 developers to get started using GRIT.
 
 The library name pretty much reflects how we think of this library. It is
 the convenience glue between GRIT and a Simulator that basically allows
 application developers to completely abstract away details of GRIT.
 
 The library is not written with performance in mind, but rather with
 simplicity and ease of use in mind. Hence the library consist of a set
 of simple object types (C++ classes). These objects are simple flat data
 containers that contains aggregated GRIT data. The data is often stored
 using STL containers holding primitive data types such as int or floats.
 
 The library also contains a number of almost c-style functions. The
 purpose of these functions are in most cases to extract aggregated
 data from GRIT and put this into a corresponding object.

 */


/**
 
 The objects in GLUE

 */

#include <glue_triplet.h>
#include <glue_tuple.h>
#include <glue_contact_point.h>
#include <glue_phase.h>
#include <glue_neighbors.h>

/**
 
 The functions in GLUE

 */

#include <glue_make_phase.h>
#include <glue_make_adjacency.h>
#include <glue_compute_contact_points.h>
#include <glue_compute_neighbors.h>
#include <glue_matlab_write_mesh.h>
#include <glue_sub_range.h>
#include <glue_copy_attribute.h>
#include <glue_clear_attribute.h>
#include <glue_svg_draw.h>

// GLUE_H
#endif
