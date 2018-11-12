#include "driver.h"
#include "PEBS.h"

void enable_pebs()
{
	__writemsr(MSR_IA32_PEBS_ENABLE, DISABLE_PEBS);
	__writemsr(MSR_IA32_GLOBAL_CTRL, DISABLE_PEBS);
	__writemsr(MSR_IA32_PERFCTR0, -PERIOD);
	__writemsr(MSR_IA32_EVNTSEL0,pebs_event | EVTSEL_EN | EVTSEL_USR | EVTSEL_INT);
	__writemsr(MSR_IA32_PEBS_ENABLE, ENABLE_PEBS);
	__writemsr(MSR_IA32_GLOBAL_CTRL, ENABLE_PEBS);
	return;
}

void disable_pebs()
{
	__writemsr(MSR_IA32_PEBS_ENABLE, DISABLE_PEBS);
	__writemsr(MSR_IA32_GLOBAL_CTRL, DISABLE_PEBS);
}