
# glexperiment

A basic OpenGL based 3D world generator, renderer and editor.

## To compile on Windows:

### Prerequesits:
Download and install [TDM-GCC-64](https://jmeubank.github.io/tdm-gcc/)
Download and install [CMake](https://cmake.org/)
Download, compile and install [glfw3](https://github.com/glfw/glfw).
### Compile and run:
```
mkdir build
cd build
cmake -G "MinGW Makefiles" -Dglfw3_DIR=/path/to/compiled/glfw3/lib/cmake/glfw3/ ..
mingw32-make
cd ..
build/main.exe
```

## To compile on Mac 

### Prerequesits:
```
brew install gcc
brew install cmake
brew install glfw
```
### Compile and run:
```
mkdir build
cd build
cmake ..
cmake --build .
cd ..
build/main.exe
```

## To compile on Linux:



## Controls:

- **Mouse** - Look Around
- **Alt + Scroll Wheel** - Zoom
- **WSAD** - Forward, Backwards, Strafe Left, Strafe Right
- **RF** - Fly Up, Fly Down
- **Ctrl + M** - Generate Maze
- **Ctrl + F** - Generate Flat Land
- **Ctrl + V** - Load MagicaVoxel File (hard coded to assets/monu1.vox at the moment)
- **Ctrl + R** - Generate Random Spheres
-  **Shift (Held)** - Show Editing Cursor
- **+ Left Click** - 'Build'
- **+ Right Click** - 'Break' 
- **Esc** - Save World & Exit
