#!/usr/bin/python

import re
import sys
import os
import string

# user defined vars
wrapperName = 'SemOperation'
# end of user defined vars


output = ''

cfuncRegex = re.compile('FOREIGN_FUNCTION\((\S+)\)')
funcRegex = re.compile('(\S+)_func')
numArgsRegex = re.compile('.*_func_(\d+)')

def print_usage_and_exit():
	print "usage: ./gen-ff-config.py <plugin/wrapper source file> <\"PLUGIN\" | \"WRAPPER\">\n"
	sys.exit(1)

def writePluginHeader():
	global output
	output += '[LibraryDefinition]\n'
	output += 'Module = <put module here>\n'
	output += 'File = <put dll file here>\n'

def writeWrapperHeader():
	global output
	output += 'void register%s() {\n' % wrapperName

def writePluginFooter():
	pass

def writeWrapperFooter():
	global output
	output += '}\n'

def writePluginEntry(func, cfunc, numArgs):
	# no longer using numArgs
	global output
	output += 'Definition = %s, %s\n' % (func, cfunc)

def writeWrapperEntry(func, cfunc, numArgs):
	global output
	output += '\tREGISTER_FUNC("foreign function", L"%s", %s);\n' % (func, cfunc)

if __name__ == '__main__':
	if(len(sys.argv) != 3):
		print_usage_and_exit()

	f = open(sys.argv[1], 'r')
	lines = f.read()
	f.close()

	wrapperName = string.strip( sys.argv[1], '.cpp' )

	outputType = string.lower(sys.argv[2])

	# write header	
	if outputType == "wrapper":
		writeWrapperHeader()
	elif outputType == "plugin":
		writePluginHeader()
	else:
		print_usage_and_exit()

	# look for function defs
	matches = cfuncRegex.findall(lines)

	for cfunc in matches:
		funcMatch = funcRegex.match(cfunc)
		func = funcMatch.group(1)
		numArgsMatch = numArgsRegex.match(cfunc)
		numArgs = numArgsMatch.group(1)
		
		if outputType == "wrapper":
			writeWrapperEntry(func, cfunc, numArgs)
		elif outputType == "plugin":
			writePluginEntry(func, cfunc, numArgs)


	# write footer
	if outputType == "wrapper":
		writeWrapperFooter()
	elif outputType == "plugin":
		writePluginFooter()

	print output
