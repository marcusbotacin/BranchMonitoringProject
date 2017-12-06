/* Branch Monitor
 * Marcus Botacin
 * 2017
 */

#include "dbg\debug.h"
#include "checks\checks.h"

/* This file contains all cpu-related checks */

/* check if it is Intel */
int CPU_check()
{
	/* Vendor String */
	char vendor[CPUID_MAX_STRING+1];
	/* ABI registers */
	int regs[CPUID_NUM_REGS];

	debug("Checking CPU Vendor");

	/* CPUID for vendor info*/
	__cpuid(regs,CPUID_VENDOR_INFO);
	
	/* building string -- weird reg order on ABI */
	memcpy(&vendor[0],&regs[REG_EBX],sizeof(int));
	memcpy(&vendor[4],&regs[REG_EDX],sizeof(int));
	memcpy(&vendor[8],&regs[REG_ECX],sizeof(int));
	vendor[CPUID_MAX_STRING]='\0';

	debug(vendor);

	/* Check Intel */
	if(strcmp(vendor,"GenuineIntel")==0)
	{
		return NO_ERROR;
	}

	return CPU_IS_NOT_INTEL;
}

/* Check for specific bit values 
 * TODO: Could be a macro
 */
INT64 CHECK_BIT(INT64 var,int pos)
{
	return ((var) & (1LL<<(pos)))>>(pos);
}

/* check if DS structure is 64 bits */

int check_is_64()
{
	int regs[CPUID_NUM_REGS];

	debug("Checking x64 CPU");

	/* cpuid processor features - eax = 1 */
	__cpuid(regs,CPUID_PROC_FEATURES);
	/* 64 bits DS = ECX[2]=1 */
	if(CHECK_BIT(regs[REG_ECX],x64_BITS_OFFSET)==BIT_ENABLED)
	{
		return NO_ERROR;
	}
	return CPU_IS_NOT_64;
}

/* Check if debug store capabilitie is present */
int check_has_ds()
{
	int regs[CPUID_NUM_REGS];

	debug("Check DS support");

	__cpuid(regs,CPUID_PROC_FEATURES);
	/* EDX[21]=1 */
	if(CHECK_BIT(regs[REG_EDX],DS_BIT_OFFSET)==BIT_ENABLED)
	{
		return NO_ERROR;
	}
	return CPU_HAS_NO_DS;
}

/* Check if processor has BTS capabilities */
int check_has_bts()
{
	INT64 msr;

	debug("Check BTS support");

	/* IA32_MISC_MSR - 416 */
	msr=__readmsr(MSR_MISC);
	/* MSR[11]=1 */
	if(CHECK_BIT(msr,BTS_BIT_OFFSET)==BIT_ENABLED)
	{
		return CPU_HAS_NO_BTS;
	}
	return NO_ERROR;
}