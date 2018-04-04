#!/bin/bash
cd ./bin

./dbServer
echo "restart dbServer ..."

sleep 1
./hallServer
echo "restart hallServer ..."
