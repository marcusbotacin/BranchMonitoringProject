# Branch Monitor Simple Client
# Marcus Botacin, 2017
# Only prints FROM and TO addresses

import win32file as w #use windows API
import time

# HEAD
print("Monitor Starting")

# Open Driver as a File Device
hdevice=w.CreateFile("\\\\.\\RetMonitor",0x80000000|0x40000000,0,None,3,0x00000080,None)

# wait until some data is collected
time.sleep(10)

# nothing more to be done, get out!
w.CloseHandle(hdevice)
