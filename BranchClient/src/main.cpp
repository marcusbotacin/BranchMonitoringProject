/* Branch Monitor 
 * Introspection Client
 * Marcus Botacin, 2017
 */

#include "config.h"
#include "addresses.h"
#include "parse.h"
#include "Threads.h"
#include "signal.h"
#include "BTS.h"

/* global thread vars */
HANDLE proc;
BOOL run=TRUE;
std::mutex queue_lock;
std::queue<BRANCHDATA> q;
imgs binary;
imgs libs[MAX_LIBS];
int n_libs=0;

int main(int argc, char *argv[])
{
	HANDLE thread1,thread2;
	int status;
	int PID;

	/* Register Signal Handler */
	/* Signal handler required since threads perform infinite loops */
	signal(SIGINT, SignalHandler);

	/* Parse Data */
	if((status=parse(&binary,libs,argc,argv,&n_libs,&PID))==FOUND_NO_ERROR)
	{
		printf("Loaded Binary: %s (PID: %d) and %d libraries\n",binary.name,PID,n_libs);
	}else{
		printf("%s\n",enum_string[status]);
	}
		
	/* Open Process */

	proc=OpenProcess(PROCESS_VM_READ,FALSE,PID);
	if(proc!=NULL)
	{
		printf("Process Opened\n");
	}else{
		printf("OpenProcess has failed\n");
		return 0;
	}

	/* Launch Threads */

	/* Launch thread1 - the looper */
	thread1=CreateThread(NULL,0,ThreadProducer,NULL,0,NULL);
	if(thread1==NULL)
	{
		printf("Producer thread error: %d\n",GetLastError());
	}
	/* launch thread2, the memory dumper */
	thread2=CreateThread(NULL,0,ThreadConsumer,NULL,0,NULL);
	if(thread2==NULL)
	{
		printf("Consumer thread error: %d\n",GetLastError());
	}

	/* when thread finish, finish the program */
	WaitForSingleObject(thread1,INFINITE);
	WaitForSingleObject(thread2,INFINITE);

	printf("Finishing Application\n");
	CloseHandle(proc);

	return 0;
}