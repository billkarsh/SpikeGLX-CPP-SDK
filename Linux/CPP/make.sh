#!/bin/sh

# Assuming the headers and lib are installed in the API folder,
# get copies here.
#
cp ../API/SglxApi.h ./
cp ../API/SglxCppClient.* ./
cp ../API/libSglxApi.so ./

g++ main.cpp DemoRemoteAPI.cpp SglxCppClient.cpp -L./ -lSglxApi

