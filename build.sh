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
# ./build/src/gen_cover
./build/src/gen_sols
