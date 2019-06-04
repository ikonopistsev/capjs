#!/bin/bash

[ ! -d build ] && mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make
#cpack .. 
#make install