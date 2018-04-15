#!/bin/bash
rm -rf server
mkdir server
rm -rf bin
mkdir bin

cd ./server
cmake ../../server
make

cd ./dbServer
cp ./dbServer ../../bin/

cd ../hallServer
cp ./hallServer ../../bin/

cd ../../
