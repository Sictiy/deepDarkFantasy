#!/bin/bash

kill -9 `ps ax | grep ./App | grep -v grep | awk '{print $1}'`
echo "shutdown success ..."
