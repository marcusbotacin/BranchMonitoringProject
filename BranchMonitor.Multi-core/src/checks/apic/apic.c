/* Branch Monitor - Multi-Core version
 * Marcus Botacin - 2017
 * Federal University of Paraná (UFPR)
 */

#include "dbg\debug.h"
#include "checks\checks.h"

/* Perform APIC check
 * This driver works when APIC is not relocated
 * In case it was relocated, an error is raised
 */

int check_apic()
{
	UINT64 apic_base;
	debug("Checking APIC");
	/* read APIC MSR */
	apic_base=(__readmsr(MSR_APIC_BASE) & (APIC_MASK));
	/* check the default value */
	if((apic_base>>APIC_MASK_OFFSET)!=APIC_BASE_VALUE)
	{
		return APIC_WAS_REMAPPED;
		/* TODO: support apic remap
		 * Save apic_base value for further usage
		 * APIC value is located at the +0x340 offset
		 */
	}
	
	return NO_ERROR;

}