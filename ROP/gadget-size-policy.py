#!/usr/bin/env python
# -*- coding: utf-8 -*-

# Marcus Botacin
# UNICAMP - 2017
# Branch Monitoring Project
# ROP gadget-size policy matcher
# Post-Analysis

import sys # Receives argv

# LAST value for the first step
last=int(0)

# Empty list
list=[]

# processing itself
# build last,current tuples
# add to the list for further processing
def process(data):
    global last
    action=data[0]
    # same lib
    # Only possible when branching inside the same block of code
    if("Same lib" in action):
	# extract values
        addr_from=int(action.split("from")[1].split(":")[0].strip(),16)
        addr_to = int(action.split("TO")[1].split(":")[0].strip(),16)	
	# ignore the first execution -- no previous value
        if(last>0):
            size=addr_from-last # using byte block size as an indicator for the number of instruction
				# reasonable assumption for pop+ret gadgets
				# we could parse the disasm itself for more precision
	# Ignore spurious values -- a problem on the previous script version
            if(size>0 and size < 1000):
                list.append(size)
	# update last seen value
        last=addr_to

# Main
def main(args):
	f=open(sys.argv[1],"r")

	# group data 3 by 3 -- action,address,disasm
	# empty line block
	lines=[]
	for line in f:
	    # if non empty lines
	    if(len(line)>2):
		    # building blocks
		    lines.append(line)
		    # identified block
		    if(len(lines)==3):
			# process
			process(lines)
			# clear the block
			lines=[]

	# configuration variables
	# change these values for distinct policies
	window_size=10
	large_enough=5
	repetition_number=3

	# print the results
	print("***************************")
	print("Max : ",max(list))
	print("Min : ",min(list))
	print("Average : ", sum(list) / len(list))
	print("Applying Window of: ",window_size)
	print("Considering large enough: ",large_enough)
	print("Considering number of repetitions: ",repetition_number)
	print("***************************")
	# Iterate over all values (building distinct windows)
	for i in xrange(0,len(list)-window_size):
	    vals=[]
	    # a given window
	    for j in xrange(i,i+window_size):
		vals.append(list[j])
	    # how many values satisfie the criteria
	    if(sum(k <= large_enough for k in vals) >= repetition_number):
		print("Detected in",vals)
	print("***************************")

# Usage information
def usage(bin_name):
	print("Usage: python %s <trace>" % bin_name)

# Entry Point
if __name__ == "__main__":
	# check if trace file was supplied
	if(len(sys.argv)==2):
		main(sys.argv)
	# parser error, display usage
	else:
		usage(sys.argv[0])

# Only standalone use
else:
	print("No module import support yet")
