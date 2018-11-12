/* Branch Monitor - Multi-Core version
 * Marcus Botacin - 2017
 * Federal University of Paraná (UFPR)
 */

#include "BTS.h"
#include "debug.h"
#include "list.h"
#include <stdio.h>
#include "PEBS.h"
#include "LBR.h"

extern PTDS_BASE DS_BASE[MAX_CORES];
extern PTBTS_BUFFER BTS_BUFFER[MAX_CORES];
extern PDEVICE_OBJECT drv;
extern ULONG PID;
extern UINT32* APIC[MAX_CORES];

//#define LIMITED_DEBUG

#ifdef LIMITED_DEBUG
int count=0;
#endif

UINT64 last=0;

void PMI(__in struct _KINTERRUPT *Interrupt, __in PVOID ServiceContext) 
{
	UINT64 TOS;
	/* APIC addr */
	LARGE_INTEGER pa;
	/* current core */
	ULONG core;
	/* current process */
	ULONG _PID;
	
	/* identify current core */
	core=KeGetCurrentProcessorNumber();
	/* identify current process */
	//_PID = (ULONG)PsGetCurrentProcessId();

	//debug("Interrupted");
	DbgPrint("Interrupted core %d",core);

	/* if current PID is being monitored */
	//if(_PID==PID)
	//{
		/* insert captured data into list */
		/* zeroed for test purposes */
	//	Insert_List(0,0,core);
	//}

	/*clear interrupt flag */
	/* keep APIC mapped on each core */
	*APIC[core]=ORIGINAL_APIC_VALUE;

	disable_pebs();

	#ifdef LIMITED_DEBUG
	if(++count<3) {
		TOS = get_lbr_tos();
		DbgPrint("TOS %x",TOS);
		if(last != 0 && last==TOS)
		{
			DbgPrint("CFI violation");
		}
		last = TOS;
		enable_pebs();
	}
	#endif
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
