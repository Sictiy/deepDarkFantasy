#!/bin/sh
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

cd ./bin/
pwd
kill -9 `ps ax | grep ./hallServer | grep -v grep | awk '{print $1}'`
echo "shutdown hallServer ..."

kill -9 `ps ax | grep ./dbServer | grep -v grep | awk '{print $1}'`
echo "shutdown dbServer ..."

sleep 1
./dbServer
echo "restart dbServer ..."

sleep 1
./hallServer
echo "restart hallServer ..."
