FROM ubuntu:24.04

MAINTAINER Dmitrii Snimshchikov <dmitrii.snimshchikov13@gmail.com>

ENV TZ=Europe/Moscow

RUN apt-get update && apt-get install -y wget g++ gcc cmake ninja-build clang-format python3 && \ 
apt-get clean && rm -rf /var/lib/apt/lists/* && \
wget https://github.com/google/googletest/archive/refs/tags/release-1.11.0.tar.gz && \
tar xf release-1.11.0.tar.gz && \
cmake -S googletest-release-1.11.0/ -B _builds -G Ninja -D CMAKE_BUILD_TYPE=Release -D CMAKE_INSTALL_PREFIX=${HOME}/opt/googletest && \
cmake --build _builds --target install && \
rm -rf googletest-release-1.11.0/ _builds/
