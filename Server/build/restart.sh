#!/bin/sh
make
sleep 1
kill -9 `ps ax | grep ./Deepdf | grep -v grep | awk '{print $1}'`
echo "shutdown Deepdf ..."
sleep 1
./Deepdf
echo "restart Deepdf ..."
