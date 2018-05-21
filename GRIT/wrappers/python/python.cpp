#include <configuration.h>

#include <pybind11/numpy.h>
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

typedef py::array_t<double, py::array::c_style | py::array::forcecast>  py_dense_array_double;

namespace pygrit
{
  void get_sub_range_current(
                             grit::engine2d_type     const & engine
                             , glue::Phase           const & phase
                             //, unsigned int          const & array_size
                             , py_dense_array_double       & x
                             , py_dense_array_double       & y
                             )
  {
    if( phase.m_vertices.size() != x.size() )
    {
      util::Log log;
      log << "get_sub_range_target(): The number of vertices must be equal to the size of x array" << util::Log::newline();
      throw std::invalid_argument("Vertices and x-values must be of same size");
    }
    
    if( phase.m_vertices.size() != y.size() )
    {
      util::Log log;
      log << "get_sub_range_target(): The number of vertices must be equal to the size of y array" << util::Log::newline();
      throw std::invalid_argument("Vertices and y-values must be of same size");
    }

    glue::get_sub_range_current( engine, phase, x, y );
  }

  void set_sub_range_target(
                            grit::engine2d_type           & engine
                            , glue::Phase           const & phase
                            //, unsigned int          const & array_size
                            , py_dense_array_double const & x
                            , py_dense_array_double const & y
                            , bool                  const & using_partial_data = false
                            )
  {
    if( phase.m_vertices.size() != x.size() )
    {
      util::Log log;
      log << "set_sub_range_target(): The number of vertices must be equal to the size of x array" << util::Log::newline();
      throw std::invalid_argument("Vertices and x-values must be of same size");
    }

    if( phase.m_vertices.size() != y.size() )
    {
      util::Log log;
      log << "set_sub_range_target(): The number of vertices must be equal to the size of y array" << util::Log::newline();
      throw std::invalid_argument("Vertices and y-values must be of same size");
    }

    glue::set_sub_range_target( engine, phase, x, y, using_partial_data );
  }
} // namespace pygrit

PYBIND11_MODULE( pygrit, m ) 
{

  /**
   * A generic buffer class.
   * Is used to expose buffer like data from C++ to Python.
   */
  class BufData
  {
  public:

    void *_ptr = nullptr;         ///< Pointer to the underlying storage
    size_t _itemsize = 0;         ///< Size of individual items in bytes
    std::string _format;          ///< For homogeneous buffers, see https://docs.python.org/3/library/struct.html#format-characters
    size_t _ndim = 0;             ///< Number of dimensions
    std::vector<size_t> _shape;   ///< Shape of the buffer (1 entry per dimension)
    std::vector<size_t> _strides; ///< Number of entries between adjacent entries (for each per dimension)

  public:

    BufData(
            void * ptr
            , size_t itemsize
            , std::string const & format
            , size_t ndim
            , std::vector<size_t> const & shape
            , std::vector<size_t> const & strides
            )
    : _ptr(ptr)
    , _itemsize(itemsize)
    , _format(format)
    , _ndim(ndim)
    , _shape(shape)
    , _strides(strides)
    {
    }

    BufData(
            void * ptr
            , size_t itemsize
            , std::string const & format
            , size_t size
            )
    : BufData(ptr
              , itemsize
              , format
              , 1
              , std::vector<size_t> { size }
              , std::vector<size_t> { itemsize }
              )
    {
    }

    /***
     * Specialized constructor that creates 2D arrays of size rows x cols and each entry is of type format.
     */
    BufData(
            void * ptr                   // Raw data stored in row-major format.
            , std::string const & format // The data type of each element
            , size_t rows                // Number of rows
            , size_t cols                // The number of columns
            )
    : BufData(
              ptr
              , sizeof(float)
              , format
              , 2
              , std::vector<size_t> { rows, cols }
              , std::vector<size_t> { sizeof(float)*cols, sizeof(float) }
              )
    {
    }

  };

  py::class_<BufData> BufData_py(m, "BufData", py::buffer_protocol());
  BufData_py.def_buffer([](BufData& self)
                        { return py::buffer_info(
                                                 self._ptr,
                                                 self._itemsize,
                                                 self._format,
                                                 self._ndim,
                                                 self._shape,
                                                 self._strides); }
                        );





  using grit::param_type;
  using grit::engine2d_type;

  using util::ConfigFile;

  using glue::Phase;

  //--- Binding STL containers

  //py::bind_vector<std::vector<double>>(m, "VectorDouble")
  //  .def("size"  , &std::vector<double>::size);

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
  .def(py::init([]() { return new param_type(); }) );
    //.def("number_of_subdomains", (unsigned int const & (param_type::*)() const) &param_type::number_of_subdomains)
    //.def("number_of_subdomains", (unsigned int & (param_type::*)()) &param_type::number_of_subdomains)
    //.def("get_upper_threshold", &param_type::get_upper_threshold)
    //.def("get_lower_threshold", &param_type::get_lower_threshold);

  py::class_<engine2d_type>(m, "Engine2D")
    .def(py::init<>())
    .def("update", &engine2d_type::update);

  py::class_<Phase>(m, "Phase")
    .def(py::init<>())
    .def("get_vertices", [](glue::Phase & self) -> BufData
       {
         return BufData(
                        const_cast<grit::Simplex0*>(self.m_vertices.data())  //Raw pointer to vertices
                        , sizeof(grit::Simplex0)     // Number of bytes for 1 Simplex0 type
                        , std::string("@L")       // 1 unsigned long
                        , self.m_vertices.size()   // Number of vertices
                        );

       }, py::keep_alive<0, 1>())
    .def("get_triangles", [](glue::Phase & self) -> BufData
       {
         return BufData(
                        const_cast<glue::Triplet*>(self.m_triangles.data())  //Raw pointer to triangles
                        , sizeof(glue::Triplet)     // Number of bytes for one Triple type
                        , std::string("@III")       // 3 unsigned ints
                        , self.m_triangles.size()   // Number of triangles
                        );

       }, py::keep_alive<0, 1>());


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
  m.def("get_sub_range_current"           , &pygrit::get_sub_range_current                              );
  m.def("set_sub_range_target"            , &pygrit::set_sub_range_target  
                                          , py::arg("engine")
                                          , py::arg("phase")
                                          , py::arg("array_size")
                                          , py::arg("x")
                                          //, py::arg("y")
                                          , py::arg("using_partial_data") = false                       );


}
