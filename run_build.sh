#!/bin/bash

set -e

./check_format.py --force
cmake -S . -B _builds -G Ninja -D CMAKE_BUILD_TYPE=Debug -D CMAKE_VERBOSE_MAKEFILE=ON -D CMAKE_PREFIX_PATH=${HOME}/opt/googletest
cmake --build _builds
ctest --test-dir _builds -VV -C Debug
valgrind --leak-check=full -s _builds/subprocess_ut
