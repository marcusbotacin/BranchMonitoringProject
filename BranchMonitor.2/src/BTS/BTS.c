/* Branch Monitor
 * Marcus Botacin
 * 2017
 */

#include "BTS.h"
#include "dbg\debug.h"

/* More global variables due to IRQL issues
 * Locally defined data is not accessible to ISR, only global ones
 */

PTDS_BASE DS_BASE;
PTBTS_BUFFER BTS_BUFFER;
extern PDRIVER_OBJECT drv;

void bts_cleanup()
{
	pdevice_ext ext;
	debug("Cleaning up BTS config");

	/* stop bts */
	__writemsr(MSR_DEBUGCTL,0);

	ext=(pdevice_ext)drv->DeviceObject->DeviceExtension;
	
	ExFreePool(ext->BTS_BUFFER);
	ExFreePool(ext->DS_BASE);
}

void setup_bts()
{
	LARGE_INTEGER pa;
	UINT32* APIC;
	pdevice_ext ext;
	debug("Configuring BTS");
	
	ext=(pdevice_ext)drv->DeviceObject->DeviceExtension;

	DS_BASE=(PTDS_BASE)ExAllocatePool(NonPagedPool,sizeof(TDS_BASE));
	BTS_BUFFER=(PTBTS_BUFFER)ExAllocatePool(NonPagedPool,sizeof(TBTS_BUFFER)*SIZE_BTS_BUFFER);

	ext->DS_BASE=DS_BASE;
	ext->BTS_BUFFER=BTS_BUFFER;

	FILL_DS_WITH_BUFFER(DS_BASE,BTS_BUFFER);

	/* change APIC */

	pa.QuadPart=PERF_COUNTER_APIC;
	APIC=(UINT32*)MmMapIoSpace(pa,sizeof(UINT32),MmNonCached);
	*APIC=ORIGINAL_APIC_VALUE;
	MmUnmapIoSpace(APIC,sizeof(APIC));

	/* set ds area register */
	__writemsr(MSR_DS_AREA,(UINT_PTR)DS_BASE);


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