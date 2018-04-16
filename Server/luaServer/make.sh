#!/bin/bash
cd ./build
cmake ../
make

rm -f ../hallServer/App
cp ./app/App ../hallServer
cp ./app/App ../dbServer
