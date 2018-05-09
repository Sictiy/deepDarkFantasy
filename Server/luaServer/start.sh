#!/bin/bash

cd ./dbServer
../bin/App& 
echo "start dbServer ..."

cd ../hallServer
../bin/App&
echo "start hallServer ..."
