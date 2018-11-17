#!/bin/sh
rm -f binario.bin
g++ -O3 -ggdb -std=c++11 test.cpp
./a.out
wxHexEditor binario.bin

