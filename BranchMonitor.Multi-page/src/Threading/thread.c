/* Branch Monitor
 * Marcus Botacin
 * 2017
 */

#include "thread.h"
#include "dbg\debug.h"
#include "BTS\BTS.h"
#include "list\list.h"

extern PDRIVER_OBJECT drv;
extern BOOLEAN exit;
extern PTDS_BASE DS_BASE;
extern PTBTS_BUFFER BTS_BUFFER[MAX_BUFFERS];
extern UINT64 ready_buffer;

void delay()
{
	LARGE_INTEGER p;
	p.QuadPart=100000;
	KeDelayExecutionThread(KernelMode,FALSE,&p);
}

/* generic thread controller */
void control_thread(int action, int core)
{
	
	HANDLE thandle;
	debug("control_thread");
	switch(action)
	{
	case LOAD_BTS:
		PsCreateSystemThread(&thandle,GENERIC_ALL,NULL,NULL,NULL,ThreadStart,(void*)core);
		break;
	case UNLOAD_BTS:
		PsCreateSystemThread(&thandle,GENERIC_ALL,NULL,NULL,NULL,ThreadStop,(void*)core);
		break;
	case LOAD_WORKER:
		PsCreateSystemThread(&thandle,GENERIC_ALL,NULL,NULL,NULL,WorkerThread,NULL);
		break;
	case UNLOAD_WORKER:
		exit=TRUE;
		delay();
		break;
	default:
		debug("Invalid Option to control_thread");
		break;
		/* This function can be expanded here to include more thread actions*/
	}
}

/* avoid core migration by setting affinity*/
void thread_attach_to_core(unsigned id)
{
	KAFFINITY mask;
	debug("Attaching thread to core");
#pragma warning( disable : 4305 )
#pragma warning( disable : 4334 )
	mask=1<<id;
	KeSetSystemAffinityThread(mask);
}


VOID ThreadStart(_In_ PVOID StartContext)
{
	unsigned int id;
	debug("Starter Thread");
#pragma warning( disable : 4305 )
	id=(int)StartContext;
	thread_attach_to_core(id);

	/* set up bts */
	setup_bts();

	/* start bts */
	// userland
	//__writemsr(MSR_DEBUGCTL,BTS_OFF_OS|BTINT|BTS|TR);
	// kernel
	__writemsr(MSR_DEBUGCTL,BTS_OFF_USR|BTINT|BTS|TR);
	return;
}

VOID ThreadStop(_In_ PVOID StartContext)
{
	pdevice_ext ext;
	unsigned int id;
	debug("Stopper Thread");
	#pragma warning( disable : 4305 )
	id=(int)StartContext;
	thread_attach_to_core(id);

	__writemsr(MSR_DEBUGCTL,BTS_DISABLED);

	bts_cleanup();

	return;
}

/* Thread which will collect data */
VOID WorkerThread(_In_ PVOID StartContext)
{
	int i;
	INT64 r;
	UINT64 last=0, index;
	
	/* keep running */
	while(!exit)
	{
		/* get available page index */
		r = ready_buffer;
		/* if available */
		if(r>=0)
		{
			/* adjust counter to fit on the number of used pages */
			/* index = highest available page */
			index = r % MAX_BUFFERS;
			/* last = last collected page */
			last = last % MAX_BUFFERS;
			
			/* for each available page */
			for(;last<=index;last++)
			{
				/* for each entry on each page */
				for(i=0;i<THRESHOLD_BTS_BUFFER;i++)
				{
					/* for each non-zero entry */
					if(BTS_BUFFER[index][i].FROM!=0 || BTS_BUFFER[index][i].TO!=0)
					{
						/* push data into list */
						Insert_List(BTS_BUFFER[index][i].FROM,BTS_BUFFER[index][i].TO);
					}
				}
				/* renable BTS on this page */
				FILL_DS_WITH_BUFFER(DS_BASE,BTS_BUFFER[last]);
			}
		}
	}
}