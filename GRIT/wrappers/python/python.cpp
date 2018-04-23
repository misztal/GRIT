#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>

#include <grit.h>
#include <glue.h>
#include <util.h>

#include <memory>

//--- This is needed to use STL containers in Python
PYBIND11_MAKE_OPAQUE(std::vector<double>);

namespace py = pybind11;
  
PYBIND11_MODULE( pygrit, m ) 
{
  using grit::param_type;
  using grit::engine2d_type;

  using util::ConfigFile;

  using glue::Phase;

  //--- Binding STL containers

  py::bind_vector<std::vector<double>>(m, "VectorDouble")
    .def("size"  , &std::vector<double>::size);

  //--- Binding classes from util, grit and glue

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
  py::class_<param_type>(m, "Parameters")
    .def(py::init([]() { return new param_type(); }) )
    //.def("number_of_subdomains", (unsigned int const & (param_type::*)() const) &param_type::number_of_subdomains)
    //.def("number_of_subdomains", (unsigned int & (param_type::*)()) &param_type::number_of_subdomains)
    //.def("get_upper_threshold", &param_type::get_upper_threshold)
    //.def("get_lower_threshold", &param_type::get_lower_threshold);

  py::class_<engine2d_type>(m, "Engine2D")
    .def(py::init<>())
    .def("update", &engine2d_type::update);

  py::class_<Phase>(m, "Phase")
    .def(py::init<>());

  //--- Binding functions from util, grit and glue

  m.def("get_data_file_path"              , &util::get_data_file_path                                   );
  m.def("generate_filename"               , &util::generate_filename                                    );
  
  m.def("make_parameters_from_config_file", &grit::make_parameters_from_config_file                     );
  m.def("init_engine_with_mesh_file"      , &grit::init_engine_with_mesh_file<grit::default_grit_types> );

  m.def("svg_draw"                        , &glue::svg_draw 
                                          , py::arg("filename")
                                          , py::arg("engine")
                                          , py::arg("parameters")
                                          , py::arg("attribute_name_x") = "current"
                                          , py::arg("attribute_name_y") = "current"                     );

  m.def("make_phase"                      , (Phase (*)(engine2d_type const &, unsigned int const &)) &glue::make_phase );
  m.def("get_sub_range_current"           , &glue::get_sub_range_current                                );
  m.def("set_sub_range_target"            , &glue::set_sub_range_target                                 
                                          , py::arg("engine")
                                          , py::arg("phase")
                                          , py::arg("x")
                                          , py::arg("y")
                                          , py::arg("using_partial_data") = false                       );


}