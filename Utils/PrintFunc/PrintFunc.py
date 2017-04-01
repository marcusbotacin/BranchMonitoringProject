#!/usr/bin/env python
# -*- coding: utf-8 -*-

# Marcus Botacin
# UNICAMP - 2017
# Branch Monitoring Project
# Print all called functions on a given trace
# Post-Analysis

import sys # Receives argv
import re # Regular expression, parsing the trace

# Main
def main(trace,offsets):
	# File read
	f=open(trace,"r")
	# for each line
	for line in f:
		# match function names
		matches=re.findall('\((.*?)\)',line)
		# for each match
		for match in matches:
			# if offsets should be removed
			if(offsets==False):
				match=match.split("+")[0]
			# print
			print(match)

# Usage information
def usage(bin_name):
	print("Usage: python %s <trace> --remove-offsets" % bin_name)

# Entry Point
if __name__ == "__main__":
	# check if trace file was supplied
	if(len(sys.argv)==2):
		main(sys.argv[1],True)
	# check if additional flags were supplied
	elif(len(sys.argv)==3):
		if("--remove-offsets" in sys.argv[2]):
			main(sys.argv[1],False)
		else:
			main(sys.argv[1],True)
	# parser error, display usage
	else:
		usage(sys.argv[0])

# Only standalone use
else:
	print("No module import support yet")
