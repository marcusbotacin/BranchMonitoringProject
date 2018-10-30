/* Branch Monitor - Multi-Core version
 * Marcus Botacin - 2017
 * Federal University of Paraná (UFPR)
 */

#include "driver.h"
#include "debug.h"
#include "checks.h"
#include "BTS.h"
 
/* Perform APIC check
 * This driver works when APIC is not relocated
 * In case it was relocated, an error is raised
 */

/* one APIC for each core */
UINT32* APIC[MAX_CORES];

void map_apic(int core)
{
	LARGE_INTEGER pa;
	pa.QuadPart=PERF_COUNTER_APIC;
	APIC[core]=(UINT32*)MmMapIoSpace(pa,sizeof(UINT32),MmNonCached);
	*APIC[core]=ORIGINAL_APIC_VALUE;
	debug("Mapping APIC");
}

int check_apic()
{
	UINT64 apic_register;
	UINT64 apic_base;
	int bit10,bit11;
	debug("Checking APIC");
	/* read APIC MSR */
	apic_register = __readmsr(MSR_APIC_BASE);
	/* check APIC base address remaping */
	apic_base=(apic_register & APIC_MASK)>>APIC_MASK_OFFSET;
	/* check the default value */
	if(apic_base!=APIC_BASE_VALUE)
	{
		return APIC_WAS_REMAPPED;
		/* TODO: support apic remap
		 * Save apic_base value for further usage
		 * APIC value is located at the +0x340 offset
		 */
	}
	
	/* check APIC operation mode */
	bit10 = BIT(apic_register,10);
	bit11 = BIT(apic_register,11);
	
	/* APIC disabled */
	if(!bit10 && !bit11)
	{
		return APIC_DISABLED;
	}
	/* invalid */
	if(bit10 && !bit11)
	{
		return APIC_INVALID;
	}
	/* x2apic */
	if(bit10 && bit11)
	{
		return APIC_X2APIC;
	}
	// xapic is supported
	return NO_ERROR;

}