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
