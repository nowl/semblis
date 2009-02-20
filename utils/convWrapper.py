#!/usr/bin/python


import os
import sys

path = sys.argv[1]

fileName = os.path.basename(path)
print fileName

fileName = os.path.splitext(fileName)[0] + "Wrapper.cpp"

command = "./write_wrapper.sh " + path + " " + fileName
os.system(command)


