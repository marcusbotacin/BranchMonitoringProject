// Self Modifying Code
// SMC detector based on PEBS counter overflow
// Marcus Botacin - 2017
// Federal University of Paraná (UFPR)
// Implementation based on VoiDbg Driver

/* avoid multiple includes */
#pragma once

/* Data definitions */
//#include "SMChecker.h"

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
#define PERF_COUNTER_APIC 0xFEE00340
#define ORIGINAL_APIC_VALUE 254
#define MSR_IA32_PERFCTR0  0xC1
#define MSR_IA32_EVNTSEL0  0x186
#define MSR_IA32_PERF_CABABILITIES  0x345
#define MSR_IA32_GLOBAL_STATUS 		0x38E
#define MSR_IA32_GLOBAL_CTRL 		0x38F
#define MSR_IA32_GLOBAL_OVF_CTRL 	0x390
#define MSR_IA32_PEBS_ENABLE		0x3F1
#define MSR_DEBUGCTL 473
#define MSR_DS_AREA 1536
#define EVTSEL_USR 1<<16
#define EVTSEL_OS  1<<17
#define EVTSEL_INT 1<<20
#define EVTSEL_EN  1<<22
// Limit
#define PERIOD 10000
// MACHINE_CLEAR.SMC event
#define pebs_event 0x1C2
#define DISABLE_PEBS 0
#define ENABLE_PEBS 1
void map_apic(int core);
void enable_pebs();
void disable_pebs();