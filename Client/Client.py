# Branch Monitor Simple Client
# Marcus Botacin, 2017
# Only prints FROM and TO addresses

import win32file as w #use windows API
import time

# HEAD
print("Monitor Starting")

# Open Driver as a File Device
hdevice=w.CreateFile("\\\\.\\BranchMonitor",0x80000000|0x40000000,0,None,3,0x00000080,None)

# wait until some data is collected
time.sleep(1)

try:
	# polling
	while(1):
		tam,string = w.ReadFile(hdevice,200,None)
		# manually calculating 64bit entries
		# should use struct instead
		data_from=string[0:7][::-1].encode('hex')
		data_to=string[8:15][::-1].encode('hex')
		print(data_from, data_to)
finally:
	# nothing more to be done, get out!
	w.CloseHandle(hdevice)