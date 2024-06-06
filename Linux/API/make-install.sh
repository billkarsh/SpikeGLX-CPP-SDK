#!/bin/sh

# Edit these two paths:
# - out_api_path gets source files you need to include in your projects
# - out_lib_path gets the shared library (so) file
#
out_api_path=./
out_lib_path=$out_api_path

# -----------------
# DO NOT EDIT BELOW
# -----------------

INPATH=../../Build

# Copy existing API source files

CPFILES="C_SglxApi.h SglxApi.h SglxCppClient.h SglxCppClient.cpp"
for i in $CPFILES
do
    cp $INPATH/$i $out_api_path
done

# Build library

CFLAGS="-shared -fPIC -std=c++11 -O3 -fdata-sections -ffunction-sections"
LFLAGS="-Wl,--gc-sections -Wl,--strip-all"

g++ -o $out_lib_path/libSglxApi.so $CFLAGS $LFLAGS -I/$INPATH $INPATH/*.cpp

