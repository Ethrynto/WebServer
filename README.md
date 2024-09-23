## Description
This is a simple web server, processes the request and outputs the result. Works in asynchronous mode, so it is possible to process several requests simultaneously

## Installation
1) Download and install <a href="https://boost.org">Boost</a> (If not installed)
2) Download and install <a href="https://cmake.org/download/">CMake</a> (If not installed)
3) To set your Boost path at `BOOST_ROOT` in CMakeLists.txt. Example ```set(BOOST_ROOT "Your_path")```
4) Create a new directory ``` mkdir build && cd build ```
5) ```cmake ..```
5) ```cmake --build .```
