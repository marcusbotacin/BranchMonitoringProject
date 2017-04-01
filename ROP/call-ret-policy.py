#!/usr/bin/env python
# -*- coding: utf-8 -*-

# Marcus Botacin
# UNICAMP - 2017
# Branch Monitoring Project
# ROP CALL-RET policy matcher
# Post-Analysis

import sys # Receives argv
import re # Regular expression, parsing the trace

# Function Call Stack
stack=[]

# Process data
def process(data):
    # parsing data
    action=data[0]
    instr=data[2]
    instr_type=instr.split(" ")[0].strip()

    # Each code block refers to a given action

    # Action: CALL lib
    if("called lib" in action):
	# extract names
        source=re.search('Binary (.*?) at',action)
        target=re.search('\((.*?)\)',action)
	# each call is added to the stack
        stack.append([instr_type,source.group(1),target.group(1)])
	# print stack each step
        print("CURRENT STACK ",stack)

    # Action: Changing inside libs
    if("Same lib" in action or "Code swapping" in action):
	# Exclude JMPs
        if("call" in instr_type or "ret" in instr_type):
	    # extract name
            match=re.findall('\((.*?)\)',action)
            stack.append([instr_type,match[0].split("+")[0],match[1].split("+")[0]])
	    # print stack each step
            print("CURRENT STACK ",stack)

    # Action: RETurn -- each RET pop values from the stack
    if("returned to Binary" in action):
	# get names
        target=re.search('Binary (.*?) at',action)
        source=re.search('\((.*?)\)',action)
        stack.append([instr_type,source.group(1).split("+")[0],target.group(1).split("+")[0]])
	# print stack each step
        print("CURRENT STACK ",stack)

    # removal procedure -- executed at the end of each step


    run=1 # will run until no matches 
    while(run==1):
	# (near-)empty stack
        if(len(stack)<2):
            run=0 # will not run anymore
        else:
	    # get the two previous values from the stack
            previous1=stack[-1]
            previous2=stack[-2]

	    # if a call RET match
	    # previous2[0] == call and previous1[0] == ret
	    # previous2[2] == previous1[1] -- source at first, target on the last
	    # previous2[1] == previous1[2] -- the same
	
	    # OR: border cases -- handling exception
	    # SetLast and RtlRestore

            if(previous1[0]=="ret" and previous2[0]=="call" and
                   ( (previous1[1]==previous2[2] and previous1[2]==previous2[1]) or (
                       "RtlRestore" in previous1[1] and "SetLast" in previous2[2]
                       and previous1[2]==previous2[1]
                       ))):
		# Inform a call-ret match was found
                print("CALL-RET MATCH, REMOVING")
		# pop the two matching values
                stack.pop()
                stack.pop()
		# print the current stack at the end
                print("CURRENT STACK ",stack)
            else:
		# will not run anymore
                run=0
    
# Main
def main(args):
	# File read
	f=open(sys.argv[1],"r")
	# Group lines 3 by 3 (action+address+disasm)
	lines=[]
	for line in f:
		# consider only non-null lines
		if(len(line)>2):
		    lines.append(line)
		    if(len(lines)==3):
			# process full block
			process(lines)
			# clear line block
			lines=[]

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
