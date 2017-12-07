# BranchMonitoring framework
# A tool to detect application misbehavior based on BTS data
# Marcus Botacin, 2017
# Federal University of Paraná (UFPR)


# Import Block
import sys                                      # binary name as argument
import codecs                                   # handle UTF8
from ConfigParser import SafeConfigParser       # config file
import pickle                                   # load/store database
import win32file as w                           # use windows API
import time                                     # wait for data
from bs4 import BeautifulSoup                   # parse xml
import signal                                   # interrupt endless loop
import ctypes                                   # display msgbox
import win32con                                 # msgbox codes
import os                                       # kill processes

# The monitoring class
class Monitor():
    def __init__(self,app_name=None,lib_name=None):

        # define message header
        self.class_name="BranchCluster"

        # user message
        if __debug__:
            print("[%s] Creating Monitor" % self.class_name)

        # test application name supplied
        if app_name is None:
            print("[%s] No application to monitor" % self.class_name)
            sys.exit(0)

        # get application name
        self.app_name=app_name
        self.lib_name=lib_name

        # parse config file
        self.vmmap_bin,self.vmmap_output,self.database_file,self.monitor_window,self.monitor_threshold = self.__parse()

        # get image address
        self.addr, self.addr_size, self.PID = self.__get_image_section()
        print("%x %x" % (self.addr,self.addr_size))

        # branch window, in case of
        self.window = []

        # installing signal handler
        signal.signal(signal.SIGINT,self.signal_handler)

    # get DB from pickle-encoded data
    def __get_db(self):
        try:
            f=open(self.database_file,'rb')
            return pickle.load(f)
        except:
            return dict()

    # save data in DB
    def __set_db(self):
        # using pickle to serialize
        f=open(self.database_file,'wb')
        pickle.dump(self.db,f,protocol=pickle.HIGHEST_PROTOCOL)

    # get image sectin in case of
    def __get_image_section(self):
        # uncoment to do that on demand
        #s=subprocess.Popen(self.vmmap_bin+" -p "+self.app_name+" "+self.vmmap_output,shell=True)
	#s.wait()
	# Parse XML
	file=open(self.vmmap_output+".mmp","r").read()

	xml=BeautifulSoup(file,"lxml-xml")

        # get PID
        pid=int(xml.root['PID'],10)

	nodes=xml.Snapshots.findAll('Region')

	for i in nodes:

	    name=i['Details']   # image name
	    addr=i['Address']   # image address
	    size=i['Size']      # image size

	    # Case Binary
	    if(self.lib_name is not None and self.lib_name in name.lower()):
                return int(addr,10),int(size,10),pid
            # Case Library
	    if(self.lib_name is None and ".exe" in name):
                return int(addr,10),int(size,10),pid

    # parse configuration file
    def __parse(self,config_file='config.conf'):
        # Read config file to retrieve paths
        with codecs.open(config_file, 'r', encoding='utf-8') as f:
                parser = SafeConfigParser()
                parser.readfp(f)
                # vmmap binary
                vmmap_bin = parser.get('vmmap', 'exe')
                # vmmap output
                vmmap_output = parser.get('vmmap','file')
                # database file
                database_file = parser.get('database','path')
                # monitor window
                # if win size = 1, no window is used in practice
                monitor_window = parser.get('monitor','window')
                # if threshold = 1, no threshold is used in practice
                monitor_threshold = parser.get('monitor','threshold')
        return vmmap_bin,vmmap_output,database_file,int(monitor_window,10),int(monitor_threshold,10)

    # add to window and test the new window
    def __add_and_test_window(self,item):
        # append all times
        self.window.append(item)

        # if windows exceeded, cut to fit
        if len(self.window)>self.monitor_window:
            self.window=self.window[1:]
        # window is not filled yet, go away
        else:
            return True, False

        # count failures (unexpected branches)
        count_failures=0
        for _item in self.window:
            if _item == False:
                count_failures+=1

        # check if the number of failures exceeds threshold
        if count_failures >= self.monitor_threshold:
            # violation
            # console message
            print("Violation on %d of last %d branches" % (count_failures,self.monitor_window))
            # display message
            opt = ctypes.windll.user32.MessageBoxA(0, "Consider as normal ?","Application misbehavior detected", 4)
            # consider as normal ?
            if opt==win32con.IDYES:
                # consider as normal
                print("Violation considered as normal behavior")
                # return NOT-OK (False) but considered OK (True)
                return False, True
            else:
                # consider as violation
                print("True violation detected")
                # return Detected (False) and considered Bad (False)
                return False, False
        else:
            # no violation
            # return Good (true), thus not harmful (false)
            return True, False

    # open driver handle
    def __open_driver_handler(self):
        self.hdevice=w.CreateFile("\\\\.\\BranchMonitor",0x80000000|0x40000000,0,None,3,0x00000080,None)

    # close driver handle
    def __close_driver_handler(self):
        w.CloseHandle(self.hdevice)

    # try to get branch data
    def __get_branch_data(self):
        # size, data
        tam,string = w.ReadFile(self.hdevice,200,None)
        # no data, size is zero
        if len(string)==0:
            return None
        # having data
        else:
            # branch as hex data
            branch=int(string[8:15][::-1].encode('hex'),16)
            return branch

    # check branch is of interest, since the mechanism is system-wide
    def __filter_branch(self,branch):
        # branch within delimited module
        if branch > self.addr and branch < self.addr+self.addr_size:
            return branch-self.addr_size
        # if outside, return no data
        else:
            return None

    # insert branch tuple in DB
    def __insert_db(self,addr_from,addr_to):
        # try to insert on a existing entry
        try:
            self.db[addr_from].add(addr_to)
        # else, create a new entry
        except:
            self.db[addr_from]=set([addr_to])

    # check tuple exists in DB
    def __check_db(self,addr_from,addr_to):
        # try to get
        try:
            return addr_to in self.db[addr_from]
        # if failed, return nothing
        except:
            return False

    # signal handler
    def signal_handler(self,signal,frame):
        # disable infinite loop
        self.run=False

    # monitoring function
    def loop(self,train=False):
        # defaults
        dump=False
        self.run = True
        previous=None

        # debug print
        if __debug__:
            print("[%s] Starting looping" % self.class_name)

        # if training mode, create DB (dict)
        if train:
            self.db = dict()
        else:
        # if matching mode, load DB
            self.db=self.__get_db()

        # get driver handle
        self.__open_driver_handler()

        # infinite loop until signal
        try:
            while(self.run):
                # try to get branch
                branch=self.__get_branch_data()

                # if got data
                if branch is not None:
                    # check if branch is of our interest
                    target=self.__filter_branch(branch)

                    if target is not None:
                        # if yes
                        if previous is not None:
                            # check this branch was expected
                            preview = self.__check_db(previous,target)

                            # debug print
                            if __debug__:
                                print("<%x,%x> (%d)" % (previous,target,preview))

                            # if training, just insert into DB
                            if train:
                                self.__insert_db(previous,target)
                            # if matching mode
                            else:
                                # check window
                                expected, should_update = self.__add_and_test_window(preview)
                                # if misbehavior
                                if not expected:
                                    # if should update (considered as normal)
                                    if should_update:
                                        # insert DB
                                        self.__insert_db(previous,target)
                                        # tell DB should be updated at the end
                                        dump=True
                                    else:
                                    # if should not be updated -- true violation
                                    # kill application
                                        print("Should kill %d" % self.PID)
                                        os.kill(self.PID,signal.SIGTERM)
                                    # stop running
                                        self.run=False

                        # update data to build the tuple
                        previous=target
                else:
                    # no data, wait more
                    time.sleep(1)

        finally:
            # handling stop signal
            # clean up
            # close driver handle
            self.__close_driver_handler()

            # debug message
            if __debug__:
                print("[%s] Stopping looping" % self.class_name)

            # check db needs to be updated
            # new things learned during this run
            if train or dump:
                self.__set_db()

# Direct call
if __name__ == "__main__":

        # parse what will be monitored
        # this can be improved

        # defaults: binary, matching mode, no library
        binary = sys.argv[1]
        train=False
        library=None

        # case arguments passed
        if(len(sys.argv)>2):
            # training argument
            if("-train" in sys.argv[2]):
                train = True
            # library argument
            else:
                library = sys.argv[2]

        mon = Monitor(binary,library)

        # after parsing, start monitoring
        mon.loop(train=train)

# module import not supported
else:
    print("Module import not supported yet!")
