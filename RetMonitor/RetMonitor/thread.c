/* Branch Monitor - Multi-Core version
 * Marcus Botacin - 2017
 * Federal University of Paraná (UFPR)
 */

#include "thread.h"
#include "debug.h"
#include "BTS.h"
#include "PEBS.h"

extern PDRIVER_OBJECT drv;

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
	map_apic(id);
	enable_pebs();
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
	disable_pebs();	
	return;
}