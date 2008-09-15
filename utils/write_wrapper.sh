#!/bin/sh

if [ $# -ne 2 ]; then
	echo "usage: ./write_wrapper.sh <input-file> <output-file>"
	exit 1
fi


./gen-ff-interfaces.py $1 $2 && ./gen-ff-config.py $2 "wrapper" >> $2

