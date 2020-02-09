/* Branch Monitor - Multi-Core version
 * Marcus Botacin - 2017
 * Federal University of Paraná (UFPR)
 */

#include "config.h"

/* CPUID */

#define CPUID_MAX_STRING 12
#define CPUID_NUM_REGS 4
#define CPUID_VENDOR_INFO 0
#define CPUID_PROC_FEATURES 1

/* MSR */

#define MSR_MISC 416

/* Registers */

#define REG_EAX 0
#define REG_EBX 1
#define REG_ECX 2
#define REG_EDX 3

/* Flags and Offsets */

#define DS_BIT_OFFSET 21
#define x64_BITS_OFFSET 2
#define BTS_BIT_OFFSET 11
#define BIT_ENABLED 1

#define MSR_APIC_BASE 27
#define APIC_MASK 0xFFFFF000
#define APIC_MASK_OFFSET 12
#define APIC_BASE_VALUE 0xfee00
#define PERF_COUNTER_APIC 0xFEE00340

/* Errors and messages */

/* Error messages associated to the following enum */
/* TODO: Find a way of sync */

static const char *enum_string[]={
	"Everything OK",
	"You are not running an Intel CPU",
	"You are not running an x64 CPU",
	"CPU has no DS AREA",
	"CPU has no BTS support",
	"CPU APIC was remapped",
	"APIC disabled",
	"Invalid APIC configuration",
	"X2APIC not supported"
};

/* error codes */

enum error_codes
{
	NO_ERROR, /* In case of no error */
	CPU_IS_NOT_INTEL, /* In case of CPU is not Intel */
	CPU_IS_NOT_64, /* In case of a 32-bit CPU */
	CPU_HAS_NO_DS, /* Only LBR support */
	CPU_HAS_NO_BTS, /* Missing BTS support */
	APIC_WAS_REMAPPED, /* APIC relocation not supported yet */
	APIC_DISABLED, 
	APIC_INVALID,
	APIC_X2APIC
};

/* Checks prototypes */

int perform_checks();
int CPU_check();
int check_apic();
int check_is_64();
int check_has_ds();
int check_has_bts();