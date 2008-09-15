#!/usr/bin/python

import sys
import re
'''
if len(sys.argv) != 2:
	print "Usage: ./mem_trace_checker.py <input-file>"
	sys.exit(1)

infile = sys.argv[1]
'''
infile = 'debug'

fin = open(infile, 'r')
fin_lines = fin.readlines()
fin.close()

mems = {}
pattern = re.compile('^(\S+)\((\S+)\):(.*)')

for line in fin_lines:
	match = pattern.match(line)
	if not match:
		continue
		
	typ = match.group(1)
	addr = match.group(2)
	info = match.group(3)

	if typ == 'free':
		if addr not in mems.keys():
			print "trying to free and not malloc'd: %s : %s" % (addr, info)
			mems[addr] = [-1, info]
		mems[addr][0] -= 1
	elif typ == 'malloc':
		if addr not in mems.keys():
			mems[addr] = [1, info]
		else:
			mems[addr][0] += 1
			mems[addr][1] = info
	else:
		print 'error in trace 2'
		sys.exit(1)

print ''
# show non-freed memory
for key in mems.keys():
	if mems[key][0] != 0:
		print 'non-freed memory: %s : %s' % (key, mems[key][1])
