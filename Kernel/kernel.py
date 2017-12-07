# Kernel introspection module to enrich branch collected data
# This code is part of BranchMonitoring framework
# Written by: Marcus Botacin - 2017
# Federal University of Paraná (UFPR)

from xml.etree.ElementTree import ElementTree   # Parse XML
import subprocess                               # Run dump tools
import win32file as w                           # Use windows API
import time                                     # Wait for data
import signal                                   # Interrupt endless loop

# Monitoring class - retrieves branch data
class Monitor():

    # class instantiation
    def __init__(self,app_name=None,lib_name=None):
        self.mods = None                                    # No introspection data at this point
        signal.signal(signal.SIGINT,self.signal_handler)    # Installing signal handler
        # debug print
        if __debug__:
            print("Starting Monitor")

    # open driver handle
    def __open_driver_handler(self):
        self.hdevice=w.CreateFile("\\\\.\\BranchMonitor",0x80000000|0x40000000,0,None,3,0x00000080,None)

    # close driver handle
    def __close_driver_handler(self):
        w.CloseHandle(self.hdevice)

    # get branch data from driver handle
    def __get_branch_data(self):
        # read bytes and string itself
        tam,string = w.ReadFile(self.hdevice,200,None)
        # if no data, return
        if len(string)==0:
            return None
        # case having data
        else:
            # interpret string as hex address
            branch=int(string[8:15][::-1].encode('hex'),16)
            return branch

    # signal handler
    def signal_handler(self,signal,frame):
        self.run=False

    # get offset from a given function address
    # mod: module to look into
    # offset: offset to look for
    def offset_to_func(self,mod,offset):
        # get pointer to given module
        funcs = self.exports[mod]
        # default: no offset found
        last_offset = 0
        last_fname = "Unknown"
        # search whole exported symbols
        for f in funcs:
            name = f[0]     # function name
            addr = f[1]     # function offset
            rel_addr = f[2] # relative function offset

            # if we are looking for such offset
            if offset == addr or offset == rel_addr:
                # immediately returns
                return name

            # in case of a jump inside a given function
            # consider the closest exported symbol
            if offset > addr and addr > last_offset:
                last_offset = addr
                last_fname = name

        # return "unknown" or the closest symbol
        return last_fname

    # identifies to which module a given address refers
    def addr_to_module(self,branch):
        # consider only the meaningful bytes
        branch = branch & 0xFFFFFFFF
        # look into all loaded modules
        for m in self.mods:
            start_addr = mods[m][0] # lowest addr
            end_addr = mods[m][1]   # highestaddr
            # if branch is inside
            if branch >= start_addr and branch <= end_addr:
                # if there are exported symbols, dig into it
                if(self.exports is not None):
                    # return base_module+function_at_offset
                    return m+"+"+self.offset_to_func(m,branch-start_addr)
                # otherwise, return just the name
                return m
        # nothing found
        return "Unknown"

    # polling loop
    def loop(self,mods=None,exports=None):
        # default definitions
        last = ""
        self.mods = mods
        self.exports = exports
        self.run = True

        # debug print
        if __debug__:
            print("Starting looping")

        # open handler
        self.__open_driver_handler()
        try:
            # infinite loop
            while(self.run):
                # try to get a branch tuple
                branch=self.__get_branch_data()
                # check if got
                if branch is not None:
                    # no introspection, just print
                    if self.mods is None:
                        print("%x" % branch)
                    else:
                        # if there's introspection data, dig into it
                        module_string = self.addr_to_module(branch)
                        # do not print repeated entries
                        if module_string != last:
                            print("%x <%s>" % (branch,module_string))
                            last = module_string
                else:
                    # no data, sleep
                    time.sleep(1)
        # signal received
        finally:
            # cleanup
            self.__close_driver_handler()

# Dumper: the introspection class
class Dumper():
    # instantiation
    def __init__(self):
        # set parser configs
        self.parse()

    # set parser configs
    def parse(self):
        # External tools are required
        # DriverView used to enumerate modules
        # DriverView binary path
        self.drvview_path = "C:\driverview-x64\DriverView.exe"
        # DriverView Output file
        self.drvview_output = "C:\driverview-x64\drv.xml"
        # DllView used to map function to offsets
        # DllView binary path
        self.dllview_path = "C:\dllexp-x64\dllexp.exe"
        # DllView output
        self.dllview_output = "C:\dllexp-x64\dll.xml"

    # enumerate loaded modules
    def dump_modules(self):
        # using DriverView
        s = subprocess.Popen([self.drvview_path,"/sxml",self.drvview_output])
        s.wait()

    # get offsets
    def dump_exports(self,bin):
        # using DllView
        s = subprocess.Popen([self.dllview_path,"/from_files",bin,"/sxml",self.dllview_output])
        s.wait()

    # parse exported symbols
    def parse_exports(self):
        exp = []
        self.dtree = ElementTree()
        x = self.dtree.parse(self.dllview_output)
        for p in self.dtree.findall("item"):
            # get function name
            fname = p.find('function_name').text
            # try to get address
            try:
                # address
                addr = int(p.find('address').text,16)
            except:
                # error, no meaningful address
                addr = 0xFFFFFFFF
            # also get relative addr
            rel_addr = int(p.find('relative_address').text,16)
            # add tuple to list
            exp.append((fname,addr,rel_addr))
        # return list
        return exp

    # get offsets and parse
    def get_exports(self,bin):
        self.dump_exports(bin)
        return self.parse_exports()

    # parse loaded modules/drivers
    def parse_modules(self):
        mods = dict()
        exports = dict()
        self.dtree = ElementTree()
        self.dtree.parse(self.drvview_output)
        for p in self.dtree.findall("item"):
            # module name
            mod_name = p.find('driver_name').text
            # initial addr
            mod_addr = int(p.find('address').text.replace("`",""),16)
            # end addr
            mod_end_addr = int(p.find('end_address').text.replace("`",""),16)
            # add to dict - no repeated modules
            mods[mod_name]=(mod_addr,mod_end_addr)
            # try to get exports for the module
            # returns a list
            exp = self.get_exports(p.find('filename').text)
            # map module to export list
            exports[mod_name] = exp
        # return module dict and exports dict
        return mods, exports

# "main"
if __name__ == '__main__':
    # introspect first
    d = Dumper()
    d.dump_modules()
    mods, exports = d.parse_modules()
    # then monitor
    m=Monitor()
    # infinite loop
    # introspected data as parameter to the monitor
    m.loop(mods,exports)
# no module import
else:
    print("No module import support yet!")
