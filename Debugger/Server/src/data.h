/* BranchMonitoring Project 
 * Debugger Server-side
 * Marcus Botacin
 * UNICAMP - 2017
 */

/* Avoid multiple includes */
#pragma once

/* Include general definitions */
#include "includes.h"

/* Inverted I/O Structure */
/* Passing Interrupt Info */
typedef struct _OVL_WRAPPER {
	OVERLAPPED  Overlapped;
	LONG        ReturnedSequence;	/* Interrupt Info */
} OVL_WRAPPER, *POVL_WRAPPER;

/* Branch Data Structure */
typedef struct BD
{
	UINT64 PID,TO,FROM;
}BRANCHDATA,*PBRANCHDATA;

/* Another I/O Structure */
/* Passing branch data */
typedef struct _OVL_WRAPPER2 {
	OVERLAPPED  Overlapped;
	BRANCHDATA bd;
} OVL_WRAPPER2, *POVL_WRAPPER2;

/* Structure to hold CPU Registers */
typedef struct st_reg
{
	UINT64 cr0,cr1,cr2,cr3,cr4;
}REGS,*PREGS;

/* Another I/O Structure */
/* Passing Register Data */
typedef struct _OVL_WRAPPER3 {
	OVERLAPPED  Overlapped;
	REGS data;
} OVL_WRAPPER3, *POVL_WRAPPER3;
