#!/usr/bin/python

import re
import sys
import os
import string

cppClassFile = True      # is this a cpp header file?
cppClassType = 'mouseCollision*' # the type of the class
cppClassName = 'mCol'  # what you want to call the local class *
cppClassPtr = 'mouseCollision-ptr'
writingHeader = ''
output = ''
LmallocList = []



# arg regex's
starRegex = re.compile('(\S+)\s*\*\s*(\S+).*')
charRegex = re.compile('.*char\s*(\S+).*')
charPtrRegex = re.compile('.*char\s*?\*\s*?(\S+).*')
shortRegex = re.compile('.*short\s*(\S+).*')
longRegex = re.compile('.*long\s*(\S+).*')
intRegex = re.compile('.*int\s*(\S+).*')
ucharRegex = re.compile('.*unsigned char\s*(\S+).*')
ushortRegex = re.compile('.*unsigned short\s*(\S+).*')
ulongRegex = re.compile('.*unsigned long\s*(\S+).*')
uintRegex = re.compile('.*unsigned int\s*(\S+).*')
floatRegex = re.compile('.*float\s*(\S+).*')
doubleRegex = re.compile('.*double\s*(\S+).*')
boolRegex = re.compile('.*bool\s*(\S+).*')


# return regex's
voidRegex = re.compile('void *[^\*]')
boolReturnRegex = re.compile('bool *[^\*]')
charPtrReturnRegex = re.compile('char *?\*')
dataPtrReturnRegex = re.compile('data_t *?\*')

funcNum = 0

currentHeader = ''

def writeHeader():
	global output, writingHeader 
	output += 'extern "C" {\n\n'
	output += '#include "ForeignMacros.h"\n\n'
	output += '}\n\n'
	output += '#include "%s"\n\n' % writingHeader 

def addError(type, errorString):
	global output
	output += '    if(!%s)\n' % type
	output += '\tFF_RET_ERROR(L"%s");\n\n' % (errorString)

def addCustomError(type, errorString):
	global output
	output += '    if(%s)\n' % type
	output += '\tFF_RET_ERROR(L"%s");\n\n' % (errorString)

def pickoutExcludeFunc(totalLines):
	excludeList = []
	print 'exclude list'
	for l in totalLines:
		if l.find('//exclude:') != -1:
			#print l + ' ->'
			remvTxt = l.replace('//exclude:','')
			#print '\t' + remvTxt
			remvTxt = string.rstrip(remvTxt,'\n')
			print '\t' + remvTxt
			excludeList.append( remvTxt )

	return excludeList

def pickoutLargeMalloc(totalLines):
	mallocList = []
	for l in totalLines:
		if l.find('//largeMalloc:') != -1:
			#print l + ' ->'
			remvTxt = l.replace('//largeMalloc:','')
			#print '\t' + remvTxt
			remvTxt = string.rstrip(remvTxt,'\n')
			#print '\t' + remvTxt
			mallocList.append( remvTxt )

	return mallocList

def writeEntrySecFunc(ret, func, args):
	funcArgList = ''
	
	global output, funcNum
	global cppClassType
	global cppClassName
	global cppClassPtr
	global writingHeader 
	splitArgs = args.split(',')

	# count non-null args
	argCount = 0
	for arg in splitArgs:
		if arg != '':
			argCount += 1
	
	if cppClassFile:
		# name mangling if c++
		output += 'FOREIGN_FUNCTION(%s_func_%d_%d) {\n' % (func, argCount, funcNum)
		funcNum += 1
	else:
		output += 'FOREIGN_FUNCTION(%s_func_%d) {\n' % (func, argCount)
		
	output += '    CHECK_ARITY(%d);\n\n' % argCount

	output += '    FF_START;\n\n'

	firstArg = True
	argNum = 0

	if cppClassFile:
		output += '    %s* %s = (%s*)env_get(GlobalEnv, (wchar_t*)L"%s", true)->data.ptr;\n\n' % (cppClassType, cppClassName, cppClassType, cppClassPtr)

	for arg in splitArgs:
		if arg.strip() == '':
			continue
		
		argNum += 1
											
		if not firstArg:
			output += '    FF_GET_NEXT_ARG;\n\n'
		
		firstArg = False
		if charPtrRegex.match(arg.strip()):
			match = charPtrRegex.match(arg.strip())
			addError('FF_IS_STRING', 'arg %d to %s must be a string' % (argNum, func))
			output += '    char *%s = NULL;\n' % match.group(1)

			try:
				if LmallocList.index(func):
					pass

				output += '    char %s_tmp[4096];\n' % match.group(1)
			except:
				output += '    char %s_tmp[256];\n' % match.group(1)

			output += '    {\n    const wchar_t *tmp_str = arg->data.text;\n'
			output += '    int tmp_str_len = (int)wcslen(tmp_str);\n'

			try:
				if LmallocList.index(func):
					pass
				output += '    if(tmp_str_len > 4096) FF_RET_ERROR(L"arg 1 longer than 4096");\n'
			except:
				output += '    if(tmp_str_len > 255) FF_RET_ERROR(L"arg 1 longer than 255");\n'

			#output += '	%s = (char*)malloc(tmp_str_len*4+1); // TODO: free this\n' % match.group(1)
			output += '    %s = (char *)%s_tmp;\n' % (match.group(1), match.group(1))
			output += '    wcsrtombs(%s, &tmp_str, (tmp_str_len+1)*4, NULL);\n    }\n\n' % match.group(1)
			if funcArgList == '':
				funcArgList = match.group(1)
			else:
				funcArgList += ', ' + match.group(1)
		elif starRegex.match(arg.strip()):
			match = starRegex.match(arg.strip())
			addError('FF_IS_POINTER', 'arg %d to %s must be a pointer' % (argNum, func))
			output += '    %s* %s = (%s *)arg->data.ptr;\n\n' % (match.group(1), match.group(2), match.group(1))
			if funcArgList == '':
				funcArgList = match.group(2)
			else:
				funcArgList += ', ' + match.group(2)
		elif uintRegex.match(arg.strip()):
			match = uintRegex.match(arg.strip())
			addError('FF_IS_NUMBER', 'arg %d to %s must be a number' % (argNum, func))
			output += '    unsigned int %s = (unsigned int)arg->data.number;\n\n' % match.group(1)
			if funcArgList == '':
				funcArgList = match.group(1)
			else:
				funcArgList += ', ' + match.group(1)
		elif ulongRegex.match(arg.strip()):
			match = ulongRegex.match(arg.strip())
			addError('FF_IS_NUMBER', 'arg %d to %s must be a number' % (argNum, func))
			output += '    unsigned long %s = (unsigned long)arg->data.number;\n\n' % match.group(1)
			if funcArgList == '':
				funcArgList = match.group(1)
			else:
				funcArgList += ', ' + match.group(1)
		elif ucharRegex.match(arg.strip()):
			match = ucharRegex.match(arg.strip())
			addError('FF_IS_NUMBER', 'arg %d to %s must be a number' % (argNum, func))
			output += '    unsigned char %s = (unsigned char)arg->data.number;\n\n' % match.group(1)
			if funcArgList == '':
				funcArgList = match.group(1)
			else:
				funcArgList += ', ' + match.group(1)
		elif ushortRegex.match(arg.strip()):
			match = ushortRegex.match(arg.strip())
			addError('FF_IS_NUMBER', 'arg %d to %s must be a number' % (argNum, func))
			output += '    unsigned short %s = (unsigned short)arg->data.number;\n\n' % match.group(1)
			if funcArgList == '':
				funcArgList = match.group(1)
			else:
				funcArgList += ', ' + match.group(1)
		elif intRegex.match(arg.strip()):
			match = intRegex.match(arg.strip())
			addError('FF_IS_NUMBER', 'arg %d to %s must be a number' % (argNum, func))
			output += '    int %s = (int)arg->data.number;\n\n' % match.group(1)
			if funcArgList == '':
				funcArgList = match.group(1)
			else:
				funcArgList += ', ' + match.group(1)
		elif longRegex.match(arg.strip()):
			match = longRegex.match(arg.strip())
			addError('FF_IS_NUMBER', 'arg %d to %s must be a number' % (argNum, func))
			output += '    long %s = (long)data_get_integer(arg);\n\n' % match.group(1)
			if funcArgList == '':
				funcArgList = match.group(1)
			else:
				funcArgList += ', ' + match.group(1)
		elif charRegex.match(arg.strip()):
			match = charRegex.match(arg.strip())
			addError('FF_IS_NUMBER', 'arg %d to %s must be a number' % (argNum, func))
			output += '    char %s = (char)arg->data.number;\n\n' % match.group(1)
			if funcArgList == '':
				funcArgList = match.group(1)
			else:
				funcArgList += ', ' + match.group(1)
		elif shortRegex.match(arg.strip()):
			match = shortRegex.match(arg.strip())
			addError('FF_IS_NUMBER', 'arg %d to %s must be a number' % (argNum, func))
			output += '    short %s = (short)arg->data.number;\n\n' % match.group(1)
			if funcArgList == '':
				funcArgList = match.group(1)
			else:
				funcArgList += ', ' + match.group(1)		
		elif floatRegex.match(arg.strip()):
			match = floatRegex.match(arg.strip())
			addError('FF_IS_NUMBER', 'arg %d to %s must be a number' % (argNum, func))
			output += '    float %s = (float)arg->data.number;\n' % match.group(1)
			if funcArgList == '':
				funcArgList = match.group(1)
			else:
				funcArgList += ', ' + match.group(1)
		elif doubleRegex.match(arg.strip()):
			match = doubleRegex.match(arg.strip())
			addError('FF_IS_NUMBER', 'arg %d to %s must be a number' % (argNum, func))
			output += '    double %s = (double)arg->data.number;\n' % match.group(1)
			if funcArgList == '':
				funcArgList = match.group(1)
			else:
				funcArgList += ', ' + match.group(1)
		elif boolRegex.match(arg.strip()):
			match = boolRegex.match(arg.strip())
			addCustomError('!FF_IS_TRUE && !FF_IS_FALSE', 'arg %d to %s must be true or NIL' % (argNum, func))
			output += '    bool %s;\n' % match.group(1)
			output += '    if(FF_IS_NIL)\n'
			output += '	%s = false;\n' % match.group(1)
			output += '    else\n'
			output += '	%s = true;\n' % match.group(1)
			
			if funcArgList == '':
				funcArgList = match.group(1)
			else:
				funcArgList += ', ' + match.group(1)
		else:
			print '  found unknown:', arg.strip()

		firstArg = False


	if voidRegex.match(ret):
		if(cppClassFile):
			output += '    %s->%s(%s);\n\n' % (cppClassName, func, funcArgList)
		else:
			output += '    %s(%s);\n\n' % (func, funcArgList)
		output += '    FF_RET_TRUE;\n}\n\n'
	else:
		if(cppClassFile):
			output += '    %s result = (%s)%s->%s(%s);\n\n' % (ret.strip(), ret.strip(), cppClassName, func, funcArgList)
		else:
			output += '    %s result = %s(%s);\n\n' % (ret.strip(), func, funcArgList)

		if floatRegex.match(ret) or doubleRegex.match(ret):
			output += '    FF_RET_REAL(result);\n}\n\n'
		elif boolReturnRegex.match(ret):
			output += '    if(result)\n'
			output += '	FF_RET_TRUE\n'
			output += '    else\n'
			output += '	FF_RET_FALSE\n}\n\n'
		elif charPtrReturnRegex.match(ret):
			try:
				if LmallocList.index(func):
					pass
				output += '    wchar_t tmp_str_arr[4096];\n'
			except:
				output += '    wchar_t tmp_str_arr[256];\n'

			output += '    wchar_t *tmp_str = tmp_str_arr;\n'

			try:
				if LmallocList.index(func):
					pass
				output += '    mbsrtowcs(tmp_str, (const char **)&result, 4096, NULL);\n'
			except:
				output += '    mbsrtowcs(tmp_str, (const char **)&result, 256, NULL);\n'



			output += '    FF_RET_STRING(tmp_str);\n}\n\n'
		elif dataPtrReturnRegex.match(ret):
			output += '    return result;\n}\n\n'
		else:
			output += '    FF_RET_NUMBER(result);\n}\n\n'

			
if __name__ == '__main__':
	if(len(sys.argv) < 3):
		print "usage: ./gen-ff-interfaces.py <header-file> <output-file>"
		sys.exit(1)
	elif len(sys.argv) == 4:
		pass
	else:
		cppClassType = os.path.split( sys.argv[1] )[1]
		cppClassType = os.path.splitext(cppClassType)[0]
		cppClassName = cppClassType + 'Ptr'
		cppClassPtr = cppClassType + '-ptr'


	currentHeader = sys.argv[1]
	writingHeader = os.path.split( sys.argv[1] )[1]

	funcRegex = re.compile(' (u?n?s?i?g?n?e?d?\s*[:\w]*\s?\*?\s\*?)([a-zA-Z_][:a-zA-Z_0-9]*)\(([^\)]*)\)')

	f = open(currentHeader, 'r')
	lines = f.read()
	f.seek(0)
	lineList = f.readlines()
	f.close()
	#print lineList
	excludeL = pickoutExcludeFunc(lineList)
	LmallocList = pickoutLargeMalloc(lineList)


	# write header
	writeHeader()

	linesNoNL = re.sub('[\n\t\s]+', ' ', lines)

	# look for function defs
	matches = funcRegex.findall(linesNoNL)

	duplicatList = []
	functionLists = []
	for mat in matches:
		functionLists.append( mat[1] )

	for i in range(len(functionLists)-1, -1, -1):
		if functionLists[i] in functionLists[:i]:
			duplicatList.append(functionLists[i])

	for i in range(len(duplicatList)-1, -1, -1):
		if duplicatList[i] in duplicatList[:i]:
			del duplicatList[i]

	if duplicatList != []:
		print "warning::::: there are duplicated functions in here"
		for mat in duplicatList:
			print '\t' + mat
		print ''

	for mat in matches:
		try:
			if excludeL.index( mat[1] ):
				pass
		except:
			functionName = mat[1]
			extra = ''
			if(cppClassFile):
				extra = ' as c++ file'
			writeEntrySecFunc(mat[0], mat[1], mat[2])


	f = open(sys.argv[2], 'w')
	f.write(output)
	f.close()
