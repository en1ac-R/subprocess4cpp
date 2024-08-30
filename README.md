# Build tools

```bash
sudo apt -y install g++ gcc
sudo apt -y install cmake
sudo apt -y install ninja-build
sudo apt -y install clang-format>=18
```

# Google test
```bash
wget https://github.com/google/googletest/archive/refs/tags/release-1.11.0.tar.gz
tar xf release-1.11.0.tar.gz
cmake -S googletest-release-1.11.0/ -B _builds -G Ninja -D CMAKE_BUILD_TYPE=Release -D CMAKE_INSTALL_PREFIX=${HOME}/opt/googletest
cmake --build _builds --target install
rm release-1.11.0.tar.gz
rm -rf googletest-release-1.11.0/ _builds/
```
# Configure
```bash
cmake -S . -B _builds -G Ninja -D CMAKE_BUILD_TYPE=Release -D CMAKE_VERBOSE_MAKEFILE=ON -D CMAKE_PREFIX_PATH=${HOME}/opt/googletest
```
# Build
```bash
cmake --build _builds
```
# Test
```bash
ctest --test-dir _builds -VV -C Release
```
# Check formatting

```bash
./check_format.py
```
example result:
```
Configuration: /.../.clang-format
Executing: clang-format -style=file /.../main_ut.cpp
Executing: clang-format -style=file /.../subprocess_ut.cpp
Executing: clang-format -style=file /.../subprocess.h
Executing: clang-format -style=file /.../subprocess.cpp
Done in 0.018 sec
```
