#!/bin/sh
rm -f binario.bin
gcc server.cpp -o server -lpthread
gcc client.cpp -o client 
./server
sleep 1
gnome-terminal -e wxHexEditor binario.bin


