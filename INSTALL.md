# Building on Windows with CMake and MinGW

Follow these steps (tested on Windows 8.1):

1. Download and execute the [CMake Win32 Installer](http://www.cmake.org/download/). Make sure to set the PATH variable during installation.

2. Download and install [mingw-w64](http://mingw-w64.yaxm.org/doku.php/download/mingw-builds). The default options would work.

3. Add the mingw-w64 programs to the system PATH variable (eg. append this string `C:\Program Files (x86)\mingw-w64\i686-4.9.2-posix-dwarf-rt_v4-rev2\mingw32\bin`).

4. Download **UBoat** using `git` or by downloading a zip archive and unzipping it.

5. Open a *Windows PowerShell* and navigate to the folder where you just put **UBoat**

6. Execute the following commands:
```
mkdir build
cd build
cmake .. -G "MinGW Makefiles"
mingw32-make
```
If all goes well you should now have the `boat.exe` executable.

# Building on macOS with CMake and Clang

1. Download **UBoat** using `git` or by downloading a zip archive and unzipping it.

2. Open a *Terminal* and navigate to the folder where you just put **UBoat**

3. Execute the following commands:
```
mkdir build
cd build
cmake ..
make
```
