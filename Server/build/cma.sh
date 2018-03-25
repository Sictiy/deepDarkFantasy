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
