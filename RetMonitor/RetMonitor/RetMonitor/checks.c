/* Branch Monitor - Multi-Core version
 * Marcus Botacin - 2017
 * Federal University of Paraná (UFPR)
 */

#include "debug.h"
#include "checks.h"

/* perform all checks at once */
int perform_checks()
{
	int ret_code;
	debug("Performing Checks");

	/* check if CPU is Intel
	 * AMD processor uses another kind of branch monitor
	 */
	if((ret_code=CPU_check())!=NO_ERROR)
	{
		return CPU_IS_NOT_INTEL;
	}
	/* check if APIC was changed */
	if((ret_code=check_apic())!=NO_ERROR)
	{
		return ret_code;
	}
	/* Check if CPU is x64
	 * BTS supports x86; the current implm. doesn't
	 * TODO: change structs to support UINT32
	 */
	if((ret_code=check_is_64())!=NO_ERROR)
	{
		return CPU_IS_NOT_64;
	}
	/* Check if CPU has DS support
	 * This is required for writing on O.S pages
	 * If it doesn't, use LBR instead BTS
	 */
	if((ret_code=check_has_ds())!=NO_ERROR)
	{
		return CPU_HAS_NO_DS;
	}
	/* check BTS support itself */
	if((ret_code=check_has_bts())!=NO_ERROR)
	{
		return CPU_HAS_NO_BTS;
	}

	/* Probably more checks are available
	 * TODO: Check for PERF_COUNT support
	 */

	return NO_ERROR;
}