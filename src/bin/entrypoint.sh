#!/usr/bin/env bash

rm -rf build
mkdir -p build
cd build
cmake ../
make
./test/HWC1_TEST
