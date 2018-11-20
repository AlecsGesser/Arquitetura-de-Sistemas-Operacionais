#!/bin/sh
rm -f binario.bin
gcc server.cpp -o server -lpthread
gcc client.cpp -o client 
./server





