/* Branch Monitor - Multi-Core version
 * Marcus Botacin - 2017
 * Federal University of Paraná (UFPR)
 */

#include "BTS.h"
#include "dbg\debug.h"

/* More global variables due to IRQL issues
 * Locally defined data is not accessible to ISR, only global ones
 */

/* DS and BTS buffer for each core */
PTDS_BASE DS_BASE[MAX_CORES];
PTBTS_BUFFER BTS_BUFFER[MAX_CORES];
extern PDRIVER_OBJECT drv;

void bts_cleanup(ULONG core)
{
	debug("Cleaning up BTS config");

	/* stop bts */
	__writemsr(MSR_DEBUGCTL,0);
	
	ExFreePool(BTS_BUFFER[core]);
	ExFreePool(DS_BASE[core]);
}

/* one APIC for each core */
UINT32* APIC[MAX_CORES];

void setup_bts(ULONG core)
{
	LARGE_INTEGER pa;
	
	debug("Configuring BTS");

	/* allocate for this core */
	DS_BASE[core]=(PTDS_BASE)ExAllocatePool(NonPagedPool,sizeof(TDS_BASE));
	BTS_BUFFER[core]=(PTBTS_BUFFER)ExAllocatePool(NonPagedPool,sizeof(TBTS_BUFFER)*SIZE_BTS_BUFFER);

	FILL_DS_WITH_BUFFER(DS_BASE[core],BTS_BUFFER[core]);

	/* change APIC */

	pa.QuadPart=PERF_COUNTER_APIC;
	APIC[core]=(UINT32*)MmMapIoSpace(pa,sizeof(UINT32),MmNonCached);
	*APIC[core]=ORIGINAL_APIC_VALUE;
	//MmUnmapIoSpace(APIC,sizeof(APIC));

	/* set ds area register */
	__writemsr(MSR_DS_AREA,(UINT_PTR)DS_BASE[core]);

}

/* Configure offsets and threshold on DS_AREA */
void FILL_DS_WITH_BUFFER(PTDS_BASE DS_BASE,PTBTS_BUFFER BTS_BUFFER)
{
	debug("Filling DS with bts addresses");
	/* BTS BUFFER BASE */
	DS_BASE->BUFFER_BASE=BTS_BUFFER;
	/* BTS INDEX */
	DS_BASE->BTS_INDEX=BTS_BUFFER;
	/* BTS MAX */
	DS_BASE->MAXIMUM=(PTBTS_BUFFER)(((UINT_PTR)BTS_BUFFER)+(SIZE_BTS_BUFFER*sizeof(TBTS_BUFFER)));
	/* BTS Threshold */
	DS_BASE->THRESHOLD=(PTBTS_BUFFER)(((UINT_PTR)BTS_BUFFER)+(THRESHOLD_BTS_BUFFER*sizeof(TBTS_BUFFER)));
}