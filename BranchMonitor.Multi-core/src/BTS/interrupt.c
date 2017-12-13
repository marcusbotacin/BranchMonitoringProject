/* Branch Monitor - Multi-Core version
 * Marcus Botacin - 2017
 * Federal University of Paraná (UFPR)
 */

#include "BTS.h"
#include "dbg\debug.h"
#include "list\list.h"
#include <stdio.h>



extern PTDS_BASE DS_BASE[MAX_CORES];
extern PTBTS_BUFFER BTS_BUFFER[MAX_CORES];
extern PDEVICE_OBJECT drv;
extern ULONG PID;
extern UINT32* APIC[MAX_CORES];

void PMI(__in struct _KINTERRUPT *Interrupt, __in PVOID ServiceContext) 
{
	/* APIC addr */
	LARGE_INTEGER pa;
	/* current core */
	ULONG core;
	/* current process */
	ULONG _PID;
	
	/* identify current core */
	core=KeGetCurrentProcessorNumber();
	/* identify current process */
	_PID = (ULONG)PsGetCurrentProcessId();

	/* if current PID is being monitored */
	if(_PID==PID)
	{
		/* insert captured data into list */
		/* zeroed for test purposes */
		Insert_List(0,0,core);
	}

	/*clear interrupt flag */
	/* keep APUC mapped on each core */
	*APIC[core]=ORIGINAL_APIC_VALUE;

	/* fill ds again*/
	DS_BASE[core]->BTS_INDEX=BTS_BUFFER[core];

	/* reenable bts */
	__writemsr(MSR_DEBUGCTL,BTS_OFF_OS|BTINT|BTS|TR);
	
} 


void *perfmon_hook = PMI;
void *restore_hook = NULL;

void unhook_handler()
{
	NTSTATUS status;
	status = HalSetSystemInformation(HalProfileSourceInterruptHandler,sizeof(PVOID*),&restore_hook);
	emit_status(status);
}

void hook_handler()
{
	NTSTATUS status;
	status = HalSetSystemInformation(HalProfileSourceInterruptHandler,sizeof(PVOID*),&perfmon_hook);
	emit_status(status);
}
