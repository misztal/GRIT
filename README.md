# GRIT

Area Maximization

[![VIDEO](https://img.youtube.com/vi/-PeN2YkvBz4/0.jpg)](https://www.youtube.com/watch?v=-PeN2YkvBz4)

Sliding Contact

[![VIDEO](https://img.youtube.com/vi/aiU6QEdkwSw/0.jpg)](https://www.youtube.com/watch?v=aiU6QEdkwSw)

Beam Material Space

[![VIDEO](https://img.youtube.com/vi/xSkG1H7c0WM/0.jpg)](https://www.youtube.com/watch?v=xSkG1H7c0WM)

Beam Spatial Space

[![VIDEO](https://img.youtube.com/vi/NMS0pkYMyxM/0.jpg)](https://www.youtube.com/watch?v=NMS0pkYMyxM)

Compression Material Space

[![VIDEO](https://img.youtube.com/vi/mTA-XLvBhs8/0.jpg)](https://www.youtube.com/watch?v=mTA-XLvBhs8)

Compression  Spatial Space

[![VIDEO](https://img.youtube.com/vi/IKSRQpPx3p0/0.jpg)](https://www.youtube.com/watch?v=IKSRQpPx3p0)

Squeeze Material Space

[![VIDEO](https://img.youtube.com/vi/a5DFJZB7bA0/0.jpg)](https://www.youtube.com/watch?v=a5DFJZB7bA0)

Squeeze Spatial Space

[![VIDEO](https://img.youtube.com/vi/QrZEPO6g1Xk/0.jpg)](https://www.youtube.com/watch?v=QrZEPO6g1Xk)

Stretch Material Space

[![VIDEO](https://img.youtube.com/vi/H_ZDGscTEwU/0.jpg)](https://www.youtube.com/watch?v=H_ZDGscTEwU)

Streatch Spatial Space

[![VIDEO](https://img.youtube.com/vi/vXNtuxYEdes/0.jpg)](https://www.youtube.com/watch?v=vXNtuxYEdes)

Erosion Space

[![VIDEO](https://img.youtube.com/vi/smA-GyVKdwY/0.jpg)](https://www.youtube.com/watch?v=smA-GyVKdwY)

Liquid Space

[![VIDEO](https://img.youtube.com/vi/VEg_2Pak7WU/0.jpg)](https://www.youtube.com/watch?v=VEg_2Pak7WU)

Stefan Problem

[![VIDEO](https://img.youtube.com/vi/uGHfS19SZSc/0.jpg)](https://www.youtube.com/watch?v=uGHfS19SZSc)

[![VIDEO](https://img.youtube.com/vi/mHiiumjjxbQ/0.jpg)](https://www.youtube.com/watch?v=mHiiumjjxbQ)

Smolarkiewicz

[![VIDEO](https://img.youtube.com/vi/TrT6LsgwygY/0.jpg)](https://www.youtube.com/watch?v=TrT6LsgwygY)

Vortex in Box

[![VIDEO](https://img.youtube.com/vi/UsTTK6jSZcs/0.jpg)](https://www.youtube.com/watch?v=UsTTK6jSZcs)

Zalesak Disk

[![VIDEO](https://img.youtube.com/vi/52NVEzAUX6o/0.jpg)](https://www.youtube.com/watch?v=52NVEzAUX6o)


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

The functions which operate on numpy.array objects rely on an assumption that all arrays have been pre-allocated to the correct size. 
