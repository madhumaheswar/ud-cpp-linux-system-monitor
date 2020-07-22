# System Monitor
## Background
* Coded this project as part of the Udacity's C++ Nano Degree Program
* This project displays the System and Process information on Linux machine(similar to top and htop)
* Executed on "Ubuntu 18.04.4 LTS"

Starter code for System Monitor Project in the Object Oriented Programming Course of the [Udacity C++ Nanodegree Program](https://www.udacity.com/course/c-plus-plus-nanodegree--nd213). 

![System Monitor](images/monitor.png)

## ncurses
* [ncurses](https://www.gnu.org/software/ncurses/) is a library that facilitates text-based graphical output in the terminal. This project relies on ncurses for display output.
* Install ncurses within your own Linux environment: `sudo apt install libncurses5-dev libncursesw5-dev`

## Make
This project uses [Make](https://www.gnu.org/software/make/). The Makefile has four targets:
* `build` compiles the source code and generates an executable
* `format` applies [ClangFormat](https://clang.llvm.org/docs/ClangFormat.html) to style the source code
* `debug` compiles the source code and generates an executable, including debugging symbols
* `clean` deletes the `build/` directory, including all of the build artifacts

## CMake
* This project uses cmake [CMake](https://cmake.org/).
* To install Cmake within you own Linux enviroment: `sudo apt-get install cmake`

## Clang-format
* This source code is formatted with clang-format
* To install clang-format within your own Linux environment: `sudo apt-get install clang-format`

## Instructions
1. Clone the project repository: [`git clone https://github.com/udacity/CppND-System-Monitor-Project-Updated.git`] -- Needs to be updated**
2. Build the project: `make build`
3. Run the resulting executable: `./build/monitor`
