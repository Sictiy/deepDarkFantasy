#!/bin/bash

kill -9 `ps ax | grep ./hallServer | grep -v grep | awk '{print $1}'`
echo "shutdown hallServer ..."

kill -9 `ps ax | grep ./dbServer | grep -v grep | awk '{print $1}'`
echo "shutdown dbServer ..."
