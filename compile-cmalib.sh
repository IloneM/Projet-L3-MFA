#!/bin/sh

g++ -fopenmp -std=gnu++11 -I/usr/include/eigen3/ -I/usr/local/include/libcmaes/ -L/usr/local/lib/ -lcmaes $*

