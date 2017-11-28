#!/usr/bin/env bash

cd ..
mkdir -p build
cd build
cmake ../code
make
./test/HWC1_TEST
