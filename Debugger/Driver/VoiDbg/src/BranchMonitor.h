/* VoiDbg - The VOID Debugger
 * A Debugger based on the BranchMonitor framework
 * Marcus Botacin - 2017
 */

/* avoid multiple includes */
#pragma once

/* MSR defs*/
#include<intrin.h>

/* Driver defs */
#define NATIVE_NAME L"\\Device\\VoiDbgDriver"
#define USER_NAME L"\\Global??\\VoiDbgDriver"

/* Thread defs */
#define BTS_CORE 3
#define DELAY_INTERVAL -10000000*5

/* Interrupt defs */
#define APIC_INT_VALUE 0x400
#define BTS_INTERRUPT_FLAGS 0x3C0

/* Struct definitions */
typedef struct st_branch
{
	UINT64 PID,TO,FROM;
}BRANCHDATA,*PBRANCHDATA;

typedef struct st_list_data
{
	BRANCHDATA data;
	LIST_ENTRY list;
}LISTDATA,*PLISTDATA;

typedef struct st_BTSBUFFER
{
	UINT64 FROM,TO,MISC;

}TBTS_BUFFER,*PTBTS_BUFFER;

typedef struct st_DSBASE
{
	PTBTS_BUFFER BUFFER_BASE,BTS_INDEX,MAXIMUM,THRESHOLD;
}TDS_BASE,*PTDS_BASE;

/* Function definitions */
void set_apic(INT32 value);