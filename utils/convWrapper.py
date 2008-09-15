#!/usr/bin/python


import os
import sys

path = sys.argv[1]

fileName = os.path.basename(path)
fileName = fileName.rstrip(".h") + "Wrapper.cpp"

command = "./write_wrapper.sh " + path + " " + fileName
os.system(command)


#os.system("mv soundDeviceWrapper.cpp ../../soundLibrary")
#os.system("mv *cpp ../../webMain")

