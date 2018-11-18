#!/bin/sh
rm -f binario.bin
g++ -g -O3 -ggdb -Wall -fpermissive test.cpp
./a.out
wxHexEditor binario.bin

