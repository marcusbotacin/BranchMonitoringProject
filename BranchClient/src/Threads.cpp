/* Branch Monitor 
 * Introspection Client
 * Marcus Botacin, 2017
 */

#include "config.h"
#include "Threads.h"
#include "BTS.h"
#include "introspection.h"

/* Global Variables */
/* TODO: Passing vars as ThreadContext */

extern BOOL run;
extern std::mutex queue_lock;
extern std::queue<BRANCHDATA> q;
extern imgs binary;

/* Collect data from the driver and stores on a queue */
DWORD WINAPI ThreadProducer(_In_ LPVOID lpParameter)
{
	printf("Producer Thread Started\n");
	BRANCHDATA bdata;
	HANDLE hFile;
	DWORD read;
	hFile=CreateFile(L"\\\\.\\BranchMonitor",GENERIC_READ|GENERIC_WRITE,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	if(hFile==INVALID_HANDLE_VALUE) /* Check */
	{
		printf("Open Driver Problem %d\n",GetLastError());
		printf("Finishing\n");
		return 0;
	}
	/* Polling */
	while(run)
	{
		/* Get data */
		if(ReadFile(hFile,&bdata,sizeof(bdata),&read,NULL)==TRUE && read>0)
		{
			/* enqueue */
			queue_lock.lock();
			q.push(bdata);
			queue_lock.unlock();
		}
	}

	printf("Finishing Producer\n");
	CloseHandle(hFile);
	return 0;
}

/* Get data and perform analysis */
DWORD WINAPI ThreadConsumer(_In_ LPVOID lpParameter)
{
	printf("Consumer Thread Started\n");
	BRANCHDATA bdata;
	/* polling */
	while(run)
	{
		queue_lock.lock();
		if(!q.empty())
		{
			/* get data */
			bdata=q.front();
			q.pop();
			queue_lock.unlock();
			/* data analysis */
			flow_analysis(bdata.FROM,bdata.TO);
		}else{
			/* unlock case empty */
			queue_lock.unlock();
		}
	}

	printf("Finishing Consumer\n");
	return 0;
}