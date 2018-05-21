#ifndef PYGRIT_TYPES_H
#define PYGRIT_TYPES_H

#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>

namespace py = pybind11;

namespace pygrit
{
  typedef py::array_t<double, py::array::c_style | py::array::forcecast>  py_dense_array_double;
}

// PYGRIT_TYPES_H
#endif
