/* BranchMonitoring Project 
 * Debugger Server-side
 * Marcus Botacin
 * UNICAMP - 2017
 */

/* Avoid multiple includes */
#pragma once

/* General Includes */
#include "includes.h"

/* External/Undocumented Functions */
typedef LONG (NTAPI *NtSuspendProcess)(IN HANDLE ProcessHandle);
typedef LONG (NTAPI *NtResumeProcess)(IN HANDLE ProcessHandle);

/* arguments definitions */
#define PROC_NO_CMDLINE NULL
#define PROC_NO_ATTR NULL
#define THREAD_NO_ATTR NULL
#define PROC_NO_INHERIT FALSE
#define PROC_ENV NULL
#define PROC_DIR NULL
#define ALL_PIDS 0
#define PROC_IN_MEMORY_INIT 0
#define NTDLL_NAME "ntdll.dll"
#define UKN_MODULE "Unknown module"
#define FUNC_MODULE "function name"
/* Internal Functions */

int launch_suspended_process(char *path);
void resume2(DWORD processId);

/* Multiple ways of suspending a process */
void suspend(DWORD processId);
void suspend2(DWORD processId);