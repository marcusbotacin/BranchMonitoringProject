/* Branch Monitor
 * Marcus Botacin
 * 2017
 */

#include "BTS.h"
#include "dbg\debug.h"
#include "list\list.h"
#include <stdio.h>



extern PTDS_BASE DS_BASE;
extern PTBTS_BUFFER BTS_BUFFER;
extern PDEVICE_OBJECT drv;

void PMI(__in struct _KINTERRUPT *Interrupt, __in PVOID ServiceContext) 
{
	LARGE_INTEGER pa;
	UINT32* APIC;
	int i;

	/* collect data */

	for(i=0;i<THRESHOLD_BTS_BUFFER;i++)
	{
		if(BTS_BUFFER[i].FROM!=0 || BTS_BUFFER[i].TO!=0)
		{
			/* push data into list */
			Insert_List(BTS_BUFFER[i].FROM,BTS_BUFFER[i].TO);
		}
	}

	/*clear interrupt flag */
	pa.QuadPart=PERF_COUNTER_APIC;
	APIC=(UINT32*)MmMapIoSpace(pa,sizeof(UINT32),MmNonCached);
	*APIC=ORIGINAL_APIC_VALUE;
	MmUnmapIoSpace(APIC,sizeof(UINT32));		

	/* fill ds again*/
	FILL_DS_WITH_BUFFER(DS_BASE,BTS_BUFFER);

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