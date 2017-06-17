/* BranchMonitoring Project 
 * Debugger Server-side
 * Marcus Botacin
 * UNICAMP - 2017
 */

/* Include Block */
#include "includes.h"
#include "debug.h"
#include "data.h"
#include "inspect.h"
#include "network.h"
#include "inverted_ioctl.h"
#include "process.h"
#include "string_utils.h"
#include "print.h"

/* Branch-by-branch step execution */
void next_branch(SOCKET ClientSocket, HANDLE hFile,int monitored_pid, HANDLE completionPortHandle, POVL_WRAPPER2 wrap2)
{
	POVL_WRAPPER wrap;	/* Inverted I/O overlap */
	DWORD code;			/* IOCTRL code */

/* debug message */
#ifdef ENABLE_DBG_PRINT
	printf("receive next\n");
#endif
		
/* debug message at each step */
#ifdef ENABLE_DBG_PRINT
	printf("sending ioctl\n");
#endif

	/* prepare request */
	wrap = (POVL_WRAPPER)malloc(sizeof(OVL_WRAPPER));
	memset(wrap, DEFAULT_IO_STRUCT_VALUE, sizeof(OVL_WRAPPER));

	/* send the branch request */
	code = SendTheIoctl(wrap,hFile);

#ifdef ENABLE_DBG_PRINT
	printf("IOCTL sent\n");
#endif

	/* check for any error */
	code = GetLastError();
	if(code != ERROR_IO_PENDING)  {
		console_message_with_code("DeviceIoControl failed", code);
		return;
	}

	/* Wait for an I/O notification */

	DWORD byteCount = DEFAULT_OVERLAP_BYTES;
	ULONG_PTR compKey = DEFAULT_OVERLAP_BYTES;
	OVERLAPPED* overlapped = DEFAULT_OVERLAP_VALUE;
	wrap = DEFAULT_OVERLAP_VALUE;
	overlapped = DEFAULT_OVERLAP_VALUE;

#ifdef ENABLE_DBG_PRINT
	printf("waiting for event\n");
#endif
	
	/* run the process which will be monitored and then generate the IO */
	resume2(monitored_pid);

	/* get completion I/O */
	BOOL worked = GetQueuedCompletionStatus(
					completionPortHandle,      // Completion port handle
					&byteCount,                // Bytes transferred
					&compKey,                  // Completion key... don't care
					&overlapped,               // OVERLAPPED structure
					INFINITE);                 // Notification time-out interval

	/* check I/O operation result */
	if (byteCount == IO_DATA_ERROR) {
		console_message_with_code("I/O Completion error",IO_DATA_ERROR);
	}

	if (overlapped == IO_OVERLAP_ERROR) {
		console_message("I/O Completion is NULL");
	}

	/* get IO data */
	wrap = (POVL_WRAPPER)overlapped;
	code = GetLastError();

#ifdef ENABLE_DBG_PRINT
	printf(">>> Notification received.  Sequence = %d\n",wrap->ReturnedSequence);
#endif

	/* As soon an interrupt was handled, suspend the running process */
	suspend2(monitored_pid);

	/* then get driver information */
	getdata(hFile,wrap2);

#ifdef ENABLE_DBG_PRINT
	printf("%x %x\n",wrap2->bd.FROM,wrap2->bd.TO);
#endif

/* then convert int to string */
	char from_string[MAX_STRING];
	char to_string[MAX_STRING];

	sprintf(from_string,"%x",wrap2->bd.FROM);
	sprintf(to_string,"%x",wrap2->bd.TO);

	/* and send this data to the driver client */

	Sleep(SLEEP_TIME);
	send(ClientSocket,from_string,string_size(from_string),SOCKET_FLAGS);
	Sleep(SLEEP_TIME);
	send(ClientSocket,to_string,string_size(to_string),SOCKET_FLAGS);
	Sleep(SLEEP_TIME);
}

/* Performs memory dump when requested */
void memdump(SOCKET ClientSocket,int monitored_pid)
{
	ULONG64 base_int,size_int;
	char base[MAX_STRING];
	char size[MAX_STRING];
	char memdump[MAX_DUMP];
	int effectively_dumped; /* how many bytes were dumped */
	/* debug print */
#ifdef ENABLE_DBG_PRINT
	printf("dump\n");
#endif
	/* receive base address */
	recv(ClientSocket,base,MAX_STRING,SOCKET_FLAGS);
	/* wait some time */
	Sleep(SLEEP_TIME);
	/* receive how many bytes to dump */
	recv(ClientSocket,size,MAX_STRING,SOCKET_FLAGS);
	/* debug print */
#ifdef ENABLE_DBG_PRINT
	printf("%s %s\n",base,size);
#endif
	/* convert from string to int */
	base_int=_strtoi64(base,NULL,HEX);
	size_int=_strtoi64(size,NULL,HEX);
	/* debug print */
#ifdef ENABLE_DBG_PRINT
	printf("%llx %llx\n",base_int,size_int);
#endif
	/* dump memory bytes from given address */
	effectively_dumped=ReadMem(monitored_pid,base_int,size_int,memdump);
	/* send result through the network */
	send(ClientSocket,memdump,effectively_dumped,SOCKET_FLAGS);
}

/* Retrieves system register values and send throgh the network */
void register_values(SOCKET ClientSocket,HANDLE hFile)
{
	/* register data*/
	REGS r;

	/* In this case, data is obtained from the kernel
	 * No PID is involved, since these are general registers
	 */

	DeviceIoControl(hFile,
		(DWORD)IOCTL_VOIDBG_GET_REG,	// Request Type = Registers
		DEFAULT_IO_IN_BUFFER,			// Input = Null
		DEFAULT_IO_IN_BUFFER_SIZE,		// Input Length = 0
		&r,								// Output = Registers
		sizeof(REGS),					// Output Length = sizeof(Registers)
		DEFAULT_IO_RESULTING_BYTES,                         
		DEFAULT_IO_NO_OVERLAP);

	/* Assuming it will succeed */

	/* debug print */
#ifdef ENABLE_DBG_PRINT
	printf("%x %x %x\n",r.cr0,r.cr1,r.cr2);
#endif

	/* serialize to string */
	char regs_serial[MAX_STRING];
	sprintf(regs_serial,"CR0: 0x%08x\n> CR2: 0x%08x\n> CR3: 0x%08x\n> CR4: 0x%08x\n> CR8: 0x%08x",r.cr0,r.cr1,r.cr2,r.cr3,r.cr4);
				
	/* then send through the network */
	send(ClientSocket,regs_serial,string_size(regs_serial),SOCKET_FLAGS);
}

/* Retrieves introspection information from the requested process
 * and send it through the network
 */
void introspection(SOCKET ClientSocket, int monitored_pid)
{
	/* Introspection address */
	UINT64 addr_int;
	/* will receive addr as string */
	char addr_str[MAX_STRING];
	/* debug print */
#ifdef ENABLE_DBG_PRINT
	printf("introspection\n");
#endif
	/* receive */
	recv(ClientSocket,addr_str,MAX_STRING,SOCKET_FLAGS);
	/* convert from string to int */
	addr_int=_strtoi64(addr_str,NULL,HEX);
	/* resulting variable */
	char intro_str[MAX_STRING];
	/* introspection into address */
	introspect(addr_int,intro_str,monitored_pid);
	/* send result through the network */
	send(ClientSocket,intro_str,string_size(intro_str),SOCKET_FLAGS);
}

/* Return the list of loaded modules inside the requested process */
void list_modules(SOCKET ClientSocket,int monitored_pid)
{
	/* modules as string */
	char modules[MAX_STRING];
	/* debug print */
#ifdef ENABLE_DBG_PRINT
	printf("list modules\n");
#endif
	/* Get modules locally */
	PrintModules(monitored_pid,modules);
	/* Send through the network */
	send(ClientSocket,modules,string_size(modules),SOCKET_FLAGS);
}

/* Returns process name given such request */
void getname(SOCKET ClientSocket,int monitored_pid)
{
	/* Process name */
	char pname[MAX_STRING];
	/* Clear string */
	memset(pname,EMPTY_STRING,MAX_STRING);
	/* Retrieves name locally */
	get_name(pname,monitored_pid);
	/* Send Through the network */
	send(ClientSocket,pname,string_size(pname),SOCKET_FLAGS);
}

/* Given a context requisition, return it */
void getcontext(SOCKET ClientSocket,int monitored_pid)
{
	/* Context as a string (serialization) */
	char context[MAX_STRING];
	/* Get Context locally */
	get_context(context,monitored_pid);
	/* Send result through the network */
	send(ClientSocket,context,string_size(context),SOCKET_FLAGS);
}