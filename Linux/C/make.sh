#!/bin/sh

# Assuming the headers and lib are installed in the API folder,
# get copies here.
#
cp ../API/C_SglxApi.h ./
cp ../API/libSglxApi.so ./

gcc main.c DemoRemoteAPI.c -L./ -lSglxApi

