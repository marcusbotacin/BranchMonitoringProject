/* VoiDbg - The VOID Debugger
 * A Debugger based on the BranchMonitor framework
 * Marcus Botacin - 2017
 */

/* avoid multiple includes */
#pragma once

/* Data definitions */
#include "BranchMonitor.h"

#define MSR_MISC 416
#define MSR_LBR_SELECT 456
#define MSR_DEBUGCTL 473
#define MSR_TOS 457
#define MSR_DS_AREA 1536
#define MSR_APIC_BASE 27
#define APIC_MASK 0xFFFFF000
#define APIC_MASK_OFFSET 12
#define APIC_BASE_VALUE 0xfee00
#define APIC_PERFORMANCE_INTERRUPT 254
#define PERF_COUNTER_APIC 0xFEE00340
#define LBR_ACTIVATED 1
#define LBR_ACTIVATED_USERLAND 0x201
#define LBR_DISABLE 0
#define LBR_FILTER 0x381
#define LBR_FILTER2 0x281
#define REG_EAX 0
#define REG_EBX 1
#define REG_ECX 2
#define REG_EDX 3
#define CPUID_PROC_FEATURES 1
#define SIZE_BTS_BUFFER 30
#define POOL_TAG 'pRE'
#define BITMASK16 0xFFFF
#define BITMASK32 0xFFFFFFFF
#define MSR_FROM_BASE 1664
#define MSR_TO_BASE 1728
#define MAX_PROC 4

/* Values only for test purposes */
/* Do not forget to change it */
#define THRESHOLD_BTS_BUFFER 5
#define DBG_DATA_ENTRY 3