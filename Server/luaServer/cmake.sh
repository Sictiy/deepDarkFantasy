#!/bin/bash

rm -rf build
mkdir build
cd ./build
cmake ../
make

rm -f ../hallServer/App
cp ./app/App ../hallServer
