#!/bin/bash
rm -rf hallbuild
mkdir hallbuild
cd hallbuild
cmake ../../src/hallServer
cd ..

rm -rf dbbuild
mkdir dbbuild
cd dbbuild
cmake ../../src/dbServer
cd ..

rm -rf ./bin
mkdir ./bin

cd ./dbbuild
pwd
make
cp ./dbServer ../bin/
sleep 1
cd ../

cd ./hallbuild
pwd
make
cp ./hallServer ../bin/
sleep 1
cd ../
