/* Branch Monitor
 * Marcus Botacin
 * 2017
 */

#include "BTS.h"
#include "dbg\debug.h"
#include "list\list.h"

extern PTDS_BASE DS_BASE;
extern PTBTS_BUFFER BTS_BUFFER;
extern PDEVICE_OBJECT drv;

/* ISR */
BOOLEAN NmiCallback(IN PVOID Context,IN BOOLEAN Handled)
{
	int i;
	LARGE_INTEGER pa;
	UINT32* APIC;
	pdevice_ext ext;
	UNREFERENCED_PARAMETER(Context);
	UNREFERENCED_PARAMETER(Handled);
	ext=(pdevice_ext)drv->DriverObject->DriverExtension;
	
	debug("Interrupted");

	/*clear interrupt flag */
	pa.QuadPart=PERF_COUNTER_APIC;
	APIC=(UINT32*)MmMapIoSpace(pa,sizeof(UINT32),MmNonCached);
	*APIC=0x400;
	MmUnmapIoSpace(APIC,sizeof(UINT32));
	
	/* collect data */

	for(i=0;i<THRESHOLD_BTS_BUFFER;i++)
	{
		if(BTS_BUFFER[i].FROM!=0 || BTS_BUFFER[i].TO!=0)
		{
			/* push data into list */
			Insert_List(BTS_BUFFER[i].FROM,BTS_BUFFER[i].TO);
		}
	}

	/* fill ds again*/
	FILL_DS_WITH_BUFFER(DS_BASE,BTS_BUFFER);

	/* reenable bts */
	__writemsr(MSR_DEBUGCTL,BTS_OFF_OS|BTINT|BTS|TR);
	return TRUE;
}