# std-capturer

C++11 stdout and stderr capture

## Requirements

- CMake v3.8+
- A c++ compiler (default to g++ or cl)
- make (Linux) or MS2013+ (Windows)

## Configure

~~~
mkdir build
cd build
cmake ..
~~~

## Build

### Build on Linux or WSL

~~~
make
~~~

If the default version of CMake is lower than v3.8+ (typically in Windows Subsystem for Linux), do the following commands:
~~~
wget https://cmake.org/files/v3.15/cmake-3.15.2-Linux-x86_64.sh
sudo mkdir build
sudo sh cmake-3.15.2-Linux-x86_64.sh --prefix=build
sudo PATH=$PWD/cmake-3.15.2-Linux-x86_64/bin:$PATH
~~~

### Build On Windows

In Visual Studio Command Prompt:
~~~
msbuild std-capturer.sln
~~~

## Test

~~~
./bin/<Debug>/Test<.exe>
~~~
