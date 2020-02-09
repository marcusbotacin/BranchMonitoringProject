/* Branch Monitor - RetMonitor version
 * Marcus Botacin - 2018
 * Federal University of Paraná (UFPR)
 */

#include<fltKernel.h>
#include<LBR.h>
#include<intrin.h>

#define LBR_TOS 0x1C9
#define LBR_ENABLE 1
#define LBR_DISABLE 1
#define LBR_MSR 473

void enable_lbr()
{
	ULONG mask;
	mask =	LBR_DISABLE_JCC				|
			LBR_DISABLE_NEAR_IND_JMP	|
			LBR_DISABLE_NEAR_REL_JMP	|
			LBR_DISABLE_FAR_BRANCH		|
			LBR_DISABLE_NEAR_RET		|
			LBR_DISABLE_KERNEL;
	__writemsr(MSR_LBR_SELECT,mask);
	__writemsr(LBR_MSR,LBR_ENABLE);
}

void disable_lbr()
{
	__writemsr(LBR_MSR,LBR_DISABLE);
}

UINT64 get_lbr_tos()
{
	UINT64 tos = __readmsr(LBR_TOS) & LBR_BITMASK;
	DbgPrint("TOS ptr %x", tos);
	return __readmsr(LBR_BASE_FROM+tos); //+tos
}