#include <pybind11/pybind11.h>

#include <grit.h>
#include <glue.h>
#include <util.h>

namespace py = pybind11;

PYBIND11_MODULE( pygrit, m ) 
{
  //using grit::param_type;
  //using grit::engine2d_type;

  using util::ConfigFile;


  py::class_<ConfigFile>(m, "ConfigFile")
    .def(py::init<>())
  //.def("get_values", &ConfigFile::get_values)
    .def("get_value" , (std::string (ConfigFile::*)( std::string const &) const) &ConfigFile::get_value)
    .def("get_value" , (std::string (ConfigFile::*)( std::string const &
                                                   , std::string const &) const) &ConfigFile::get_value)
    .def("get_value" , (std::string (ConfigFile::*)( std::string const &
                                                   , std::string const &
                                                   , std::string const &) const) &ConfigFile::get_value)
    .def("get_value" , (std::string (ConfigFile::*)( std::string const &
                                                   , std::string const &
                                                   , std::string const &
                                                   , std::string const &) const) &ConfigFile::get_value)
    .def("exist"     , &ConfigFile::exist)
    .def("clear"     , &ConfigFile::clear)
    .def("load"      , &ConfigFile::load, py::arg("filename"), py::arg("throw_on_errors") = true);

  // 2018-04-23 Marek: just adding a couple of get/set functions to test if this thing works.
  //                   Not sure if they'll be needed in the first release (as we should stick
  //                   to reading the parameters from the cfg files), but it should be rather
  //                   simple to add them later.
  /*py::class_<param_type>(m, "Parameters")
    .def(py::init([]() { return new param_type(); }) )
    .def("get_upper_threshold", &param_type::get_upper_threshold)
    .def("get_lower_threshold", &param_type::get_lower_threshold);*/
}

