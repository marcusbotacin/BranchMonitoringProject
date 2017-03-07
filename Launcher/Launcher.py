# Branch Monitor Launcher
# Marcus Botacin, 2017
# A tool to ease passing lib args to the monitor

from bs4 import BeautifulSoup #xml parsing
import sys #binary name as argument
import subprocess #launch tools
from ConfigParser import SafeConfigParser #config file                                     
import codecs # handle UTF8

# use sysinternals to retrieve lib images addresses


def vmmap_dump(bin_name,vmmap,file):
	# Shell=True is not safe
	# TODO: Change to a formatted string
	s=subprocess.Popen(vmmap+" -p "+bin_name+" "+file,shell=True)
	s.wait()

# parse XML file generated from Sysinternal's vmmap
def parse_dump(file):

	file=open(file+".mmp","r").read()


	# Parse XML
	xml=BeautifulSoup(file,"lxml-xml")

	pid=xml.root['PID']

	nodes=xml.Snapshots.findAll('Region')


	args=""
	for i in nodes:

	    name=i['Details']

	    add=i['Address']

	    sz=i['Size']

		# Case Binary
	    if(".exe" in name):
		args="-bin "+name+" "+pid+" "+add+" "+sz

		# Case library
	    if(".dll" in name):

		args=args+" -lib "+name+" "+add+" "+sz


	return args

# Start Monitor by passing args
def launch_monitor(monitor,cmd):
	s=subprocess.Popen(monitor+" "+cmd,shell=True)

def main(argv):
	
	# Read config file to retrieve paths	
	with codecs.open('config.ini', 'r', encoding='utf-8') as f:
		parser = SafeConfigParser()
		parser.readfp(f)
		vmmap = parser.get('vmmap', 'exe')
		output = parser.get('vmmap','file')
		monitor = parser.get('client','exe')
	
	#Get addresses, parse, launch monitor with parsed data
	vmmap_dump(argv[1],vmmap,output)
	cmd=parse_dump(output)
	launch_monitor(monitor,cmd)

# No module import allowed yet
if __name__ == "__main__":
	# Assuming binary name passed as argument
	if(len(sys.argv)==2):
	    main(sys.argv)