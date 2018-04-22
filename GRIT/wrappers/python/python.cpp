#include <pybind11/pybind11.h>

struct Pet
{
  std::string m_name;

  Pet()
    : m_name("no name")
  { }

  Pet(std::string const & name)
    : m_name(name)
  { }

  void set_name(std::string const & value)
  {
    m_name = value;
  }

  std::string const & get_name() const
  {
    return m_name;
  }

};

namespace py = pybind11;

PYBIND11_MODULE( pygrit, m ) 
{
  py::class_<Pet>(m, "Pet")
        .def(py::init<std::string const &>())
        .def("set_name", &Pet::set_name)
        .def("get_name", &Pet::get_name);
}

