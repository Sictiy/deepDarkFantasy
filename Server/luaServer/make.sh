#!/bin/bash
cd ./build
cmake ../
make

rm -f ../hallServer/App
cp ./baseServer/App ../hallServer
cp ./baseServer/App ../dbServer
