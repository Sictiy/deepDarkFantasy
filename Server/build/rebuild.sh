#!/bin/sh
./stop.sh
echo "stop success!"
sleep 1

rm -rf ./bin
mkdir bin

cd ./server
make 

cd ./dbServer
cp ./dbServer ../../bin/

cd ../hallServer
cp ./hallServer ../../bin/

cd ../../
echo "remake success!"

sleep 1
./start.sh
echo "restart success!"
