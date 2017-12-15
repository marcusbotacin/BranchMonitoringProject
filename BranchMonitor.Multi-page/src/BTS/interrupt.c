/* Branch Monitor
 * Marcus Botacin
 * 2017
 */

#include "BTS.h"
#include "dbg\debug.h"
#include "list\list.h"
#include <stdio.h>

extern PTDS_BASE DS_BASE;
extern PTBTS_BUFFER BTS_BUFFER[MAX_BUFFERS];
extern int current_buffer;
extern INT64 ready_buffer;
extern PDEVICE_OBJECT drv;

void PMI(__in struct _KINTERRUPT *Interrupt, __in PVOID ServiceContext) 
{
	LARGE_INTEGER pa;
	UINT32* APIC;

	/*clear interrupt flag */
	pa.QuadPart=PERF_COUNTER_APIC;
	APIC=(UINT32*)MmMapIoSpace(pa,sizeof(UINT32),MmNonCached);
	*APIC=ORIGINAL_APIC_VALUE;

	/* notify a new OS page is available */
	ready_buffer=ready_buffer+1;
	
	/* page offsets are restored externally */

	/* then, just reenable BTS */
	__writemsr(MSR_DEBUGCTL,BTS_OFF_USR|BTINT|BTS|TR);	
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