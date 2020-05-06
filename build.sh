#!/bin/bash

set -e

SCRIPT_DIR=$(dirname "$0")

cd $SCRIPT_DIR

if [ ! -d "build" ]; then
    mkdir build
fi
cd build
if command -v mingw32-make; then
    cmake -G "MinGW Makefiles" -S ../ -B .
    mingw32-make
else
    cmake -S ../ -B .
    make
fi
cd ..

python3 src/trans-chip.py
if [ ! -f "data/cover-BGM-71" ]; then
    ./build/src/gen_cover
fi
./build/src/gen_sols
