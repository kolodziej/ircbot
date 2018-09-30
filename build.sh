#!/bin/bash

mkdir -pv build
cd build
cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=True ..
