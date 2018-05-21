# GRIT

[![VIDEO](https://img.youtube.com/vi/gmyX38hCkho/0.jpg)](https://www.youtube.com/watch?v=gmyX38hCkho)

## General Robust Interface Tracking meshes in 2D

Please see `introduction.pdf` for a detailed description of the method, installation process and usage.

## Principal developers

* *Marek Krzysztof Misztal*, Niels Bohr Institute, University of Copenhagen, marek.misztal(at)gmail(dot)com

* *Kenny Erleben*, Department of Computer Science, University of Copenhagen, kenny(at)di(dot)ku(dot)dk

## Contributors

* Mark Jensen

* Tobias Hildebrandt

* Ulrik Bonde

* Danni Dromi

* Ask Neve Gamby

## Python Interface

**Linux**

First you must run CMake on GRIT, then open up your project files, and build the target pygrit. You can also just build all targets, but if you just want to play with Python then pygrit is sufficient.

After you have build the pygrit target in GRIT then your bin folder should contain a file named pygrit.cpython-36m-darwin.so or similar. Now one can use the python wrapper immediately as follows
```
cd  GRIT/bin
python tutorial_vortex_in_a_box.py 
```
The python script file tutorial_vortex_in_a_box.py  shows how to get started immediately writing a python "simulator" that uses GRIT.

**Windows / Visual Studio** 

First you must run CMake on GRIT to create the .sln file, then open it in Visual Studio, and build the target pygrit. You can also just build all targets, but if you just want to play with Python then pygrit is sufficient.

After you have build the pygrit target in GRIT then your build/wrappers/python/Release (or Debug) folder should contain a file named pygrit.cp37-win32.pyg or similar. Copy this file to your Python site-packages directory (e.g. C:\Python37-32\Lib\site-packages). 
 
You can then execute the tutorial_vortex_in_a_box.py script by double-clicking on it.

**Note to developers**

In order to enable the GLUE sub-range functionality, the pybind11 library had to be modified by overloading `operator[]` inside `array_t` class, see lines 807-819 in GRIT/wrappers/python/pybind11/include/pybind11/numpy.h:
```
template <typename T, ...> class array_t : public array {
...
public:

    T const & operator[](unsigned int const & index) const
    {
      T const * ptr = (T const *) this->data();
      return ptr[index];
    }

    T & operator[](unsigned int const & index)
    {
      T * ptr = (T *) this->data();
      return ptr[index];
    }
...
};
```
In case of future updates of the pybind11 library, the numpy.h will have to be updated accordingly.

**Numpy arrays**

The functions which operate on numpy.array objects rely on an assumption that all arrays have been pre-allocated to the correct size. Current pygrit interface offers little in terms of input numpy.array validity checks, and might produce Segmentation Fault type of errors if the arrays are not pre-allocated correctly.
