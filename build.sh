#!/bin/bash

set -e

SCRIPT_DIR=$(dirname "$0")

cd $SCRIPT_DIR

if [ ! -d "build" ]; then
    mkdir build
fi
cd build
cmake -G "MinGW Makefiles" ../ .
mingw32-make
cd ..

python3 src/trans-chip.py
if [ ! -f "data/cover-BGM-71" ]; then
    ./build/src/gen_cover
fi
./build/src/gen_sols
