#!/bin/bash
cd ./build
cmake ../
make

rm -rf ../bin/*
cp ./cppBase/App ../bin
