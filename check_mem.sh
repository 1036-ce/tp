#!/bin/bash


# PARAM="-s
#         --log-file=log.log
#         --tool=memcheck
#         --leak-check=full
#         --show-leak-kinds=all"
PARAM="-s
		--tool=memcheck
		--leak-check=full
		--show-leak-kinds=all"
BIN_PATH="./build/src/main"

if [ "$1" != "" ]
then
	BIN_PATH=$1
fi

echo $BIN_PATH
valgrind $PARAM $BIN_PATH
