#!/bin/bash
cd ./bin

./dbServer
echo "start dbServer ..."

sleep 1
./hallServer
echo "start hallServer ..."
